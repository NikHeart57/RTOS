/**
 * @note        Комментарий сгенерирован нейросетью deepseek 29.08.25
 *
 * @file        UART.h
 * @brief       Драйвер UART для AVR ATmega32 - низкоуровневая реализация
 * @details     Предоставляет полный набор функций для асинхронной последовательной связи
 *              с поддержкой кольцевых буферов и callback-функций.
 * 
 * @note        Особенности:
 *              - Поддержка скоростей до 115200 бод
 *              - Кольцевые буферы приема/передачи (64 байта)
 *              - Настраиваемые параметры связи (битность, четность, стоп-биты)
 *              - Режимы прерываний для приема/передачи
 *              - Callback-механизм для асинхронного приема
 * 
 * @warning     Ограничения:
 *              - Блокирующие функции приема (требуют обработки в основном цикле)
 *              - Ограниченный размер буферов (64 байта)
 * 
 * @attention   Для работы необходимо:
 *              - Подключение <avr/io.h> и <avr/interrupt.h>
 *              - Корректная настройка F_CPU
 *              - Включенные глобальные прерывания (sei())
 * 
 *
 *
 * @section     Конфигурационные define
 * 
 * @def         F_CPU
 *              Базовая частота микроконтроллера (Гц) - ОБЯЗАТЕЛЬНЫЙ ПАРАМЕТР
 * @note        Должна быть определена через флаг компилятора: -DF_CPU=16000000UL
 * @warning     Неправильное значение приведет к неверной скорости UART!
 *
 * @def         UART_RX_BUFFER_SIZE
 *              Размер буфера приема (64 байта)
 *
 * @def         UART_TX_BUFFER_SIZE  
 *              Размер буфера передачи (64 байта)
 * 
 *
 *
 * @section     Перечисления (enum)
 * 
 * @enum        UART_State
 *              Состояние UART (вкл/выкл)
 * @value       UART_DISABLED
 *              UART выключен (энергосбережение)
 * @value       UART_ENABLED
 *              UART включен и готов к работе
 * 
 * @enum        UART_DataBits
 *              Режимы битности данных
 * @value       UART_5_BITS
 *              5 бит данных (редко используется)
 * @value       UART_6_BITS
 *              6 бит данных
 * @value       UART_7_BITS  
 *              7 бит данных (для ASCII)
 * @value       UART_8_BITS
 *              8 бит данных (стандартный режим)
 * @value       UART_9_BITS
 *              9 бит данных (особый режим)
 * 
 * @enum        UART_StopBits
 *              Количество стоп-битов
 * @value       UART_1_STOP_BIT
 *              1 стоп-бит (стандарт)
 * @value       UART_2_STOP_BITS
 *              2 стоп-бита (для длинных линий)
 * 
 * @enum        UART_Parity
 *              Режимы контроля четности
 * @value       UART_PARITY_NONE
 *              Без контроля четности
 * @value       UART_PARITY_EVEN
 *              Четный паритет
 * @value       UART_PARITY_ODD
 *              Нечетный паритет
 * 
 * @enum        UART_Interrupt
 *              Режимы прерываний UART
 * @value       UART_INTERRUPT_DISABLED
 *              Прерывания отключены
 * @value       UART_INTERRUPT_RX_ENABLED
 *              Прерывание по приему
 * @value       UART_INTERRUPT_TX_ENABLED
 *              Прерывание по готовности передатчика
 * @value       UART_INTERRUPT_BOTH_ENABLED
 *              Оба прерывания включены
 * 
 *
 *
 * @section     Типы данных
 * 
 * @typedef     UART_RxCallback
 *              Тип callback-функции для обработки принятых данных
 * @param       c Принятый символ (1 байт данных)
 * @note        Функция вызывается из прерывания при получении каждого символа
 * @warning     Должна быть максимально краткой (не блокировать прерывания)
 * 
 *
 *
 * @section     Функции инициализации
 * 
 * @fn			  void	UART_Init(UART_State state, uint32_t baud, UART_DataBits dataBits, 
 *								  UART_StopBits stopBits, UART_Parity parity, 
 *								  UART_Interrupt interrupt)						Инициализация UART
 * @fn            void	UART_Init(UART_State state)								Базовая инициализация UART с настройками по умолчанию
 * @fn            void	UART_Enable(void)										Включение приемопередатчика UART
 * @fn            void	UART_Disable(void)										Выключение приемопередатчика UART
 * @fn            void	UART_SetBaudRate(uint32_t baud)							Установка скорости передачи (бодрейт)
 * 
 *
 *
 * @section     Функции передачи данных
 * 
 * @fn            void	UART_SendByte(uint8_t byte)								Синхронная передача одного байта
 * @fn            void	UART_SendBuffer(const uint8_t *buffer, uint16_t length)	Передача бинарного массива данных
 * 
 *
 *
 * @section     Функции приема данных
 * 
 * @fn            char	UART_ReceiveByte(void)									Синхронный прием одного символа
 * @fn         uint8_t	UART_ReceiveBuffer(uint8_t *buffer, uint16_t length)	Прием массива данных в буфер
 * @fn         uint8_t	UART_DataAvailable(void)								Проверка наличия данных в буфере приема
 * @fn            void	UART_FlushRxBuffer(void)								Очистка буфера приема
 * @fn            void	UART_SetRxCallback(UART_RxCallback callback)			Установка callback-функции для асинхронного приема
 * 
 *
 *
 * @section     Примеры использования
 * 
 * @example     Базовая инициализация:
 * @code
 * UART_Init(UART_ENABLED, 9600, UART_8_BITS, UART_1_STOP_BIT, 
 *          UART_PARITY_NONE, UART_INTERRUPT_RX_ENABLED);
 * UART_SendByte(12);
 * @endcode
 * 
 * @example     Использование callback:
 * @code
 * void my_callback(char data) {
 *     // Обработка полученного символа
 * }
 * 
 * UART_SetRxCallback(my_callback);
 * @endcode
 * 
 *
 *
 * @author      Николай Куркин
 * @date        2025-08-29
 * @version     1.0
 * 
 * @copyright   MIT License
 */


#pragma once
//////////////////////////////////////////////////////////////////////////
//  СИСТЕМНЫЕ ЗАГОЛОВОЧНЫЕ ФАЙЛЫ
//////////////////////////////////////////////////////////////////////////

#include <avr/io.h>          ///< Регистры ввода-вывода AVR
#include <avr/interrupt.h>   ///< Работа с прерываниями
#include <stddef.h>          ///< Стандартные определения размеров типов


//////////////////////////////////////////////////////////////////////////
//  ПРОВЕРКА ПОДДЕРЖКИ C++11
//////////////////////////////////////////////////////////////////////////

#if __cplusplus < 201103L
#error "C++11 support required! Add -std=c++11 to compiler flags"
#endif


//////////////////////////////////////////////////////////////////////////
//  ПРОВЕРКА ПОДДЕРЖКИ МИКРОКОНТРОЛЛЕРА (ATmega32 ONLY)
//////////////////////////////////////////////////////////////////////////

#ifndef __AVR_ATmega32__
#error "UART driver requires ATmega32 MCU! Check your target device."
#endif


//////////////////////////////////////////////////////////////////////////
//  ОСНОВНЫЕ НАСТРОЙКИ UART
//////////////////////////////////////////////////////////////////////////

/**
 * @brief Базовая частота микроконтроллера (Гц) - ДОЛЖНА БЫТЬ ОПРЕДЕЛЕНА
 * @details Частота CPU критична для расчета таймингов и делителей UART.
 * 
 * @note Определяется через флаг компилятора: -DF_CPU=16000000UL
 * @note Или явно перед включением этого заголовка: #define F_CPU 16000000UL
 * 
 * @warning Должна точно соответствовать реальной частоте кварца/генератора!
 * @warning Неправильное значение приведет к неверной скорости UART!
 * 
 * @example Правильное определение:
 * @code
 * // В настройках компилятора:
 * -DF_CPU=16000000UL
 * 
 * // Или в коде (перед #include "UART.h"):
 * #define F_CPU 16000000UL
 * @endcode
 * 
 * @error Компиляция прервется с сообщением, если F_CPU не определена
 */
#ifndef F_CPU
#error "UART.h - F_CPU must be defined! Add F_CPU=16000000UL to compiler flags"
#endif

/**
 * @brief Размер буфера приема (байт)
 * @note Рекомендуется степень двойки (64, 128, 256)
 * @warning Увеличивает потребление RAM
 */
#define UART_RX_BUFFER_SIZE 64

/**
 * @brief Размер буфера передачи (байт)
 * @note Рекомендуется степень двойки (64, 128, 256)
 * @warning Увеличивает потребление RAM
 */
#define UART_TX_BUFFER_SIZE 64

//////////////////////////////////////////////////////////////////////////
//  КОНФИГУРАЦИЯ UART (ПЕРЕЧИСЛЕНИЯ)
//////////////////////////////////////////////////////////////////////////

/**
 * @brief Состояние UART (вкл/выкл)
 * @note Управляется битами RXEN/TXEN в регистре UCSRB
 */
typedef enum {
    UART_DISABLED = 0,  ///< UART выключен (энергосбережение)
    UART_ENABLED         ///< UART включен и готов к работе
} UART_State;

/**
 * @brief Режимы битности данных
 * @note Для 9-битного режима требуется дополнительно установить бит UCSZ2
 */
typedef enum {
    UART_5_BITS = 0,  ///< 5 бит данных (редко используется)
    UART_6_BITS,      ///< 6 бит данных
    UART_7_BITS,      ///< 7 бит данных (для ASCII)
    UART_8_BITS,      ///< 8 бит данных (стандартный режим)
    UART_9_BITS = 7   ///< 9 бит данных (особый режим)
} UART_DataBits;

/**
 * @brief Количество стоп-битов
 * @note Большинство устройств используют 1 стоп-бит
 */
typedef enum {
    UART_1_STOP_BIT = 0,  ///< 1 стоп-бит (стандарт)
    UART_2_STOP_BITS       ///< 2 стоп-бита (для длинных линий)
} UART_StopBits;

/**
 * @brief Режимы контроля четности
 * @note Используется для обнаружения ошибок передачи
 */
typedef enum {
    UART_PARITY_NONE = 0,  ///< Без контроля четности
    UART_PARITY_EVEN = 2,   ///< Четный паритет
    UART_PARITY_ODD = 3     ///< Нечетный паритет
} UART_Parity;

/**
 * @brief Режимы прерываний UART
 * @note Позволяют обрабатывать события в фоновом режиме
 */
typedef enum {
    UART_INTERRUPT_DISABLED = 0,    ///< Прерывания отключены
    UART_INTERRUPT_RX_ENABLED,      ///< Прерывание по приему
    UART_INTERRUPT_TX_ENABLED,      ///< Прерывание по готовности передатчика
    UART_INTERRUPT_BOTH_ENABLED     ///< Оба прерывания включены
} UART_Interrupt;

//////////////////////////////////////////////////////////////////////////
//  CALLBACK-МЕХАНИЗМ ДЛЯ АСИНХРОННОГО ПРИЕМА
//////////////////////////////////////////////////////////////////////////

/**
 * @brief Тип callback-функции для обработки принятых данных
 * @param[in] c Принятый символ (1 байт данных)
 * @note Функция вызывается из прерывания при получении каждого символа
 * @warning Должна быть максимально краткой (не блокировать прерывания)
 * @example 
 * void my_callback(char data) {
 *     // Обработка полученного символа
 * }
 */
typedef void (*UART_RxCallback)(char c);

//////////////////////////////////////////////////////////////////////////
//  ОСНОВНОЙ ИНТЕРФЕЙС UART API
//////////////////////////////////////////////////////////////////////////

/**
 * @brief Расширенная инициализация UART
 * @param[in] state Состояние (UART_ENABLED/UART_DISABLED)
 * @param[in] baud Скорость в бодах (300-115200)
 * @param[in] dataBits Количество бит данных (5-9 бит)
 * @param[in] stopBits Количество стоп-битов (1 или 2)
 * @param[in] parity Контроль четности (NONE/EVEN/ODD)
 * @param[in] interrupt Режим прерываний (RX/TX/BOTH)
 * @note Для 9-битного режима автоматически устанавливается бит UCSZ2
 * @warning Неправильные настройки могут привести к ошибкам связи!
 */
void UART_Init(UART_State state, uint32_t baud, UART_DataBits dataBits, UART_StopBits stopBits, UART_Parity parity, UART_Interrupt interrupt);

/**
 * @brief Базовая инициализация UART с настройками по умолчанию
 * @param[in] state Состояние (UART_ENABLED/UART_DISABLED)
 * @details Используются параметры:
 * - 8 бит данных
 * - 1 стоп-бит
 * - Без контроля четности
 * - Прерывания приема включены
 * - Скорость 9600 бод
 */
void UART_Init(UART_State state);

/**
 * @brief Включение приемопередатчика UART
 * @note Активирует биты RXEN и TXEN в регистре UCSRB
 */
void UART_Enable(void);

/**
 * @brief Выключение приемопередатчика UART
 * @note Деактивирует биты RXEN и TXEN в регистре UCSRB
 * @warning Останавливает все текущие передачи
 */
void UART_Disable(void);

/**
 * @brief Установка скорости передачи (бодрейт)
 * @param[in] baud Скорость в бодах (300-115200)
 * @note Пересчитывает значение UBRR по формуле:
 *       UBRR = (F_CPU / (16 * baud)) - 1
 * @warning Не меняет текущие настройки формата кадра
 */
void UART_SetBaudRate(uint32_t baud);

//////////////////////////////////////////////////////////////////////////
//  ФУНКЦИИ ПЕРЕДАЧИ ДАННЫХ
//////////////////////////////////////////////////////////////////////////

/**
 * @brief Синхронная передача одного байта
 * @param[in] byte байт для передачи (1 байт)
 * @note Блокирует выполнение до готовности передатчика
 * @warning Для 9-битного режима требует отдельной обработки
 */
void UART_SendByte(uint8_t byte);

/**
 * @brief Передача бинарного массива данных
 * @param[in] buffer Указатель на буфер данных
 * @param[in] length Количество байт для передачи
 * @note Безопасно работает с бинарными данными (включая нулевые байты)
 */
void UART_SendBuffer(const uint8_t *buffer, uint16_t length);

//////////////////////////////////////////////////////////////////////////
//  ФУНКЦИИ ПРИЕМА ДАННЫХ
//////////////////////////////////////////////////////////////////////////

/**
 * @brief Синхронный прием одного байта
 * @return Принятый символ (1 байт)
 * @note Блокирует выполнение до получения данных
 * @warning Для 9-битного режима требует отдельной обработки
 */
char UART_ReceiveByte(void);

/**
 * @brief Прием массива данных в буфер
 * @param[out] buffer Буфер для записи принятых данных
 * @param[in] length Максимальное количество байт для приема
 * @return Количество фактически принятых байт
 * @note Читает все доступные данные (но не более length байт)
 */
uint8_t UART_ReceiveBuffer(uint8_t *buffer, uint16_t length);

/**
 * @brief Проверка наличия данных в буфере приема
 * @return Количество доступных для чтения байт
 * @note Неблокирующая функция
 */
uint8_t UART_DataAvailable(void);

/**
 * @brief Очистка буфера приема
 * @note Сбрасывает индексы головы и хвоста буфера
 * @warning Удаляет все непрочитанные данные!
 */
void UART_FlushRxBuffer(void);

/**
 * @brief Установка callback-функции для асинхронного приема
 * @param[in] callback Функция обратного вызова (NULL для отключения)
 * @note При установке callback стандартный буфер не используется
 * @warning Callback выполняется в контексте прерывания!
 */
void UART_SetRxCallback(UART_RxCallback callback);