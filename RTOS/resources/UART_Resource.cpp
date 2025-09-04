#include "UART_Resource.hpp"

/**
 * @brief Конструктор UART ресурса
 * @param[in] config Конфигурация UART (по умолчанию используются стандартные настройки)
 */
UART_Resource::UART_Resource(const Config& config) :
config_(config),				// Инициализация поля config_
isWorking_(false)				// Инициализация поля isWorking_
{
}

/**
 * @brief Деструктор UART ресурса
 * @note Автоматически отключает UART при уничтожении объекта
 */
UART_Resource::~UART_Resource()
{
	if (isWorking_) 
	{
		UART_Disable();
		isWorking_ = false;
	}
}

/**
 * @brief Запуск UART с текущими настройками
 */
bool UART_Resource::start()
{
	// Инициализация с текущими настройками config_
	UART_Init(this->config_.state, this->config_.baudRate, this->config_.dataBits,
	this->config_.stopBits, this->config_.parity, this->config_.interrupt);
	
	this->isWorking_ = (this->config_.state == UART_ENABLED);
	return this->isWorking_;
}


/**
 * @brief Запуск UART с указанной скоростью
 */
bool UART_Resource::start(uint32_t baudRate)
{
	this->config_.baudRate = baudRate;   // Обновляем конфигурацию
	return start();						// Вызываем версию без параметров
}


/**
 * @brief Запуск UART с указанной конфигурацией
 */
bool UART_Resource::start(const Config& config)
{
	this->config_ = config;				// Обновляем конфигурацию
	return start();						// Вызываем версию без параметров
}

/**
 * @brief Остановка UART 
 * @return true если UART успешно остановлен, false в случае ошибки
 */
bool UART_Resource::stop()
{
	if (!isWorking_) 
	{
		// UART уже остановлен
		return false;
	}
	
	// Отключаем UART
	UART_Disable();
	
	// Сбрасываем флаг работоспособности
	isWorking_ = false;
	
	// Обновляем состояние в конфигурации
	config_.state = UART_DISABLED;
	
	return true;
}

/**
 * @brief Проверка работоспособности UART
 */
bool UART_Resource::isWorking() const
{
	return this->isWorking_;
}


/**
 * @brief Получение количества доступных для чтения байт
 * @return Количество байт в буфере приема
 */
uint8_t UART_Resource::bytesAvailable() const
{
	if (!isWorking_) return 0;
	return UART_DataAvailable();
}

/**
 * @brief Чтение одного байта из буфера приема
 * @return Прочитанный байт или -1 если данных нет
 */
int UART_Resource::read()
{
	if (!this->isWorking_ || !bytesAvailable()) return -1;
	return UART_ReceiveByte();
}

/**
 * @brief Чтение массива данных из буфера приема
 * @param[out] buffer Буфер для записи данных
 * @param[in] size Размер буфера
 * @return Количество фактически прочитанных байт
 */
size_t UART_Resource::read(uint8_t* buffer, size_t size)
{
	if (!this->isWorking_ || !buffer || size == 0) return 0;
	
	uint8_t available_bytes = bytesAvailable();
	if (available_bytes == 0) return 0;
	
	size_t bytes_to_read = (size < available_bytes) ? size : available_bytes;
	return UART_ReceiveBuffer(buffer, bytes_to_read);
}


/**
     * @brief Запись одного байта в UART
     * @param[in] data Байт для передачи
     * @return true если запись успешна, false в случае ошибки
     */
bool UART_Resource::write(uint8_t data)
{
	if (!this->isWorking_) return false;
	
	UART_SendByte(data);
	return true;
}

/**
     * @brief Запись массива данных в UART
     * @param[in] data Указатель на данные для передачи
     * @param[in] size Размер данных в байтах
     * @return Количество фактически записанных байт
     */
size_t UART_Resource::write(const uint8_t* data, size_t size)
{
	if (!this->isWorking_ || !data || size == 0) return 0;
	
	UART_SendBuffer(data, size);
	
	return size;
}

/**
     * @brief Запись строки в UART
     * @param[in] str Нуль-терминированная строка
     * @return Количество записанных символов
     */
size_t UART_Resource::print(const char* str)
{
	if (!this->isWorking_ || !str) return 0;
	
	size_t length = 0;
	while (*str)
	{
		UART_SendByte(*str++);
		length++;
	}
	
	return length;
}

/**
     * @brief Запись строки с переводом строки
     * @param[in] str Нуль-терминированная строка
     * @return Количество записанных символов (включая \r\n)
     */
size_t UART_Resource::println(const char* str)
{
	size_t length = print(str);
	length += print("\r\n");
	return length;
}


/**
     * @brief Очистка буфера приема
     */
void UART_Resource::flush()
{
	if (this->isWorking_)
	{
		UART_FlushRxBuffer();
	}
}

/**
     * @brief Установка скорости передачи
     * @param[in] baudRate Скорость в бодах
     * @return true если скорость успешно изменена, false в случае ошибки
     */
bool UART_Resource::setBaudRate(uint32_t baudRate)
{
	if (!isWorking_) return false;
	
	// Если скорость не изменилась, ничего не делаем
	if (config_.baudRate == baudRate) 
	{
		return true;
	}
	
	config_.baudRate = baudRate;
	UART_SetBaudRate(baudRate);
	return true;
}

/**
     * @brief Получение текущей конфигурации UART
     * @return Текущая конфигурация
     */
UART_Resource::Config UART_Resource::getConfig() const
{
	return this->config_;
}

/**
     * @brief Установка callback-функции для асинхронного приема
     * @param[in] callback Функция обратного вызова
     */
void UART_Resource::setRxCallback(UART_RxCallback callback)
{
	if (this->isWorking_)
	{
		UART_SetRxCallback(callback);
	}
}
