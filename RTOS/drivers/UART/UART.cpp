#include "UART.h"

//////////////////////////////////////////////////////////////////////////
//  КОЛЬЦЕВЫЕ БУФЕРЫ ДЛЯ ПРИЕМА И ПЕРЕДАЧИ
//////////////////////////////////////////////////////////////////////////

/**
 * @brief Буфер приема данных (кольцевой)
 * @note Размер определяется UART_RX_BUFFER_SIZE
 * @warning Объявлен как volatile для работы в прерываниях
 */
static volatile uint8_t uart_rx_buffer[UART_RX_BUFFER_SIZE];

/**
 * @brief Буфер передачи данных (кольцевой)
 * @note Размер определяется UART_TX_BUFFER_SIZE
 * @warning Объявлен как volatile для работы в прерываниях
 */
static volatile uint8_t uart_tx_buffer[UART_TX_BUFFER_SIZE];

/**
 * @brief Индекс головы (записи) буфера приема
 * @note Указывает на следующую позицию для записи
 */
static volatile uint8_t uart_rx_head = 0;

/**
 * @brief Индекс хвоста (чтения) буфера приема
 * @note Указывает на следующую позицию для чтения
 */
static volatile uint8_t uart_rx_tail = 0;

/**
 * @brief Индекс головы (записи) буфера передачи
 * @note Указывает на следующую позицию для записи
 */
static volatile uint8_t uart_tx_head = 0;

/**
 * @brief Индекс хвоста (чтения) буфера передачи
 * @note Указывает на следующую позицию для чтения
 */
static volatile uint8_t uart_tx_tail = 0;

//////////////////////////////////////////////////////////////////////////
//  CALLBACK-ФУНКЦИЯ ДЛЯ АСИНХРОННОГО ПРИЕМА
//////////////////////////////////////////////////////////////////////////

/**
 * @brief Callback-функция для обработки принятых данных
 * @note Если установлена, вызывается вместо буферизации
 * @warning Должна быть краткой (выполняется в прерывании)
 */
static UART_RxCallback uart_rx_callback = NULL;

//////////////////////////////////////////////////////////////////////////
//  ОСНОВНЫЕ ФУНКЦИИ UART
//////////////////////////////////////////////////////////////////////////

/**
 * @brief Расширенная инициализация UART
 * @param state Состояние UART (вкл/выкл)
 * @param dataBits Количество бит данных
 * @param stopBits Количество стоп-битов
 * @param parity Контроль четности
 * @param interrupt Режим прерываний
 * 
 * @details Алгоритм инициализации:
 * 1. Расчет и установка делителя частоты (UBRR)
 * 2. Настройка формата кадра (UCSRC)
 * 3. Включение приемника/передатчика (UCSRB)
 * 4. Настройка прерываний согласно параметрам
 */
void UART_Init(UART_State state, uint32_t baud, UART_DataBits dataBits, UART_StopBits stopBits, UART_Parity parity, UART_Interrupt interrupt)
{
    // 1. Расчет и установка делителя частоты
    UART_SetBaudRate(baud);
    
    // 2. Настройка формата кадра (битность, стоп-биты, четность)
    UCSRC = (1 << URSEL) | (parity << UPM0) | (stopBits << USBS) | (dataBits << UCSZ0);
    
    // 3. Включение приемника и передатчика
    UCSRB = (1 << RXEN) | (1 << TXEN);
    
    // 4. Дополнительная настройка для 9-битного режима
    if(dataBits == UART_9_BITS)
    {
        UCSRB |= (1 << UCSZ2);
    }
    
    // 5. Настройка прерываний согласно выбранному режиму
    switch(interrupt) 
	{
        case UART_INTERRUPT_RX_ENABLED:
            UCSRB |= (1 << RXCIE);
            break;
        case UART_INTERRUPT_TX_ENABLED:
            UCSRB |= (1 << TXCIE);
            break;
        case UART_INTERRUPT_BOTH_ENABLED:
            UCSRB |= (1 << RXCIE) | (1 << TXCIE);
            break;
        default:
            // Прерывания отключены
            break;
    }
    
    // 6. Включение/выключение UART согласно параметру
    if(state == UART_DISABLED) {
        UART_Disable();
    }
}

/**
 * @brief Упрощенная инициализация UART
 * @param state Состояние UART (вкл/выкл)
 * @note Использует настройки по умолчанию:
 * - 8 бит данных, 1 стоп-бит, без контроля четности
 * - Скорость 9600 бод
 * - Прерывания приема включены
 */
void UART_Init(UART_State state)
{
    if(state == UART_ENABLED) 
	{
        // 9600 бод
        UART_SetBaudRate(9600);
        
        // Настройка формата кадра: 8N1
        UCSRC = (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0);
        
        // Включение приемника, передатчика и прерывания приема
        UCSRB = (1 << RXEN) | (1 << TXEN) | (1 << RXCIE);
    } 
	else 
	{
        // Полное отключение UART
        UCSRB = 0;  // Отключаем прием, передачу и прерывания
    }
}

/**
 * @brief Включение UART
 * @note Активирует биты RXEN и TXEN в регистре UCSRB
 */
void UART_Enable(void) 
{
    UCSRB |= (1 << RXEN) | (1 << TXEN);
}

/**
 * @brief Выключение UART
 * @note Деактивирует биты RXEN и TXEN в регистре UCSRB
 * @warning Прерывает все текущие операции передачи/приема
 */
void UART_Disable(void) 
{
    UCSRB &= ~((1 << RXEN) | (1 << TXEN));
}

/**
 * @brief Установка скорости передачи
 * @param baud Скорость в бодах (300-115200)
 * @note Пересчитывает значение UBRR для заданной скорости
 */
void UART_SetBaudRate(uint32_t baud) 
{
    uint16_t ubrr = (F_CPU / (16UL * baud)) - 1;
    UBRRH = (uint8_t)(ubrr >> 8);
    UBRRL = (uint8_t)ubrr;
}

//////////////////////////////////////////////////////////////////////////
//  ФУНКЦИИ ПЕРЕДАЧИ ДАННЫХ
//////////////////////////////////////////////////////////////////////////

/**
 * @brief Передача одного байта
 * @param byte Байт для передачи
 * @note Блокирует выполнение до готовности передатчика
 */
void UART_SendByte(uint8_t byte) 
{
    while(!(UCSRA & (1 << UDRE)));		// Ожидание готовности
    UDR = byte;							// Запись данных
}


/**
 * @brief Передача бинарного массива
 * @param buffer Указатель на данные
 * @param length Длина данных в байтах
 */
void UART_SendBuffer(const uint8_t *buffer, uint16_t length) 
{
    while(length--) {
	    UART_SendByte(*buffer++);
    }
}

//////////////////////////////////////////////////////////////////////////
//  ФУНКЦИИ ПРИЕМА ДАННЫХ
//////////////////////////////////////////////////////////////////////////

/**
 * @brief Синхронный прием одного байта
 * @return Принятый символ (1 байт)
 * @note Блокирует выполнение до получения данных
 * @warning Для 9-битного режима требует отдельной обработки
 */
char UART_ReceiveByte(void) 
{
    while(!UART_DataAvailable());  // Ожидание данных
    uint8_t data = uart_rx_buffer[uart_rx_tail];
    uart_rx_tail = (uart_rx_tail + 1) % UART_RX_BUFFER_SIZE;
    return data;
}

/**
 * @brief Прием массива данных
 * @param buffer Буфер для записи
 * @param length Максимальная длина приема
 * @return Количество фактически принятых байт
 */
uint8_t UART_ReceiveBuffer(uint8_t *buffer, uint16_t length) 
{
    uint16_t i;
    for(i = 0; i < length && UART_DataAvailable(); i++) {
        buffer[i] = uart_rx_buffer[uart_rx_tail];
        uart_rx_tail = (uart_rx_tail + 1) % UART_RX_BUFFER_SIZE;
    }
    return i;
}

/**
 * @brief Проверка наличия данных в буфере
 * @return Количество доступных байт (0 если буфер пуст)
 */
uint8_t UART_DataAvailable(void) 
{
    return (uart_rx_head != uart_rx_tail);
}

/**
 * @brief Очистка буфера приема
 * @note Сбрасывает индексы и удаляет все непрочитанные данные
 */
void UART_FlushRxBuffer(void) 
{
    uart_rx_head = uart_rx_tail = 0;
}

/**
 * @brief Установка callback-функции для приема
 * @param callback Функция обратного вызова
 * @note NULL отключает callback и включает буферизацию
 */
void UART_SetRxCallback(UART_RxCallback callback)
{
    uart_rx_callback = callback;
}

//////////////////////////////////////////////////////////////////////////
//  ОБРАБОТЧИКИ ПРЕРЫВАНИЙ
//////////////////////////////////////////////////////////////////////////

/**
 * @brief Обработчик прерывания приема
 * @note Вызывается при получении каждого байта
 */
ISR(USART_RXC_vect)
{
    char c = UDR;  // Чтение полученного байта
    
    if (uart_rx_callback != NULL)
    {
        // Вызов callback-функции (если установлена)
        uart_rx_callback(c);
    }
    else
    {
        // Стандартная обработка - запись в кольцевой буфер
        uint8_t next_head = (uart_rx_head + 1) % UART_RX_BUFFER_SIZE;
        if (next_head != uart_rx_tail)  // Проверка переполнения
        {
            uart_rx_buffer[uart_rx_head] = c;
            uart_rx_head = next_head;
        }
    }
}

/**
 * @brief Обработчик прерывания передачи
 * @note Вызывается при готовности передатчика
 */
ISR(USART_TXC_vect) 
{
    if (uart_tx_head != uart_tx_tail)
    {
        // Передача следующего байта из буфера
        UDR = uart_tx_buffer[uart_tx_tail];
        uart_tx_tail = (uart_tx_tail + 1) % UART_TX_BUFFER_SIZE;
    }
}