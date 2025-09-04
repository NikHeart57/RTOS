/**
 * @note        Комментарий сгенерирован нейросетью deepseek 29.08.25
 *
 * @file        UART_Resource.hpp
 * @brief       Ресурс UART - высокоуровневая абстракция над UART драйвером
 * @details     Предоставляет объектно-ориентированный интерфейс для работы с UART,
 *              инкапсулируя низкоуровневый драйвер и предоставляя удобный API
 *              в соответствии с принципами ООП.
 * 
 * @note        Особенности:
 *              - Полная инкапсуляция аппаратной реализации
 *              - Поддержка различных конфигураций UART
 *              - Единообразный интерфейс для чтения/записи
 *              - Callback-механизм для асинхронного приема
 * 
 * @warning     Для работы требует предварительной инициализации драйвера UART
 * 
 *
 *
 * @section     Структуры данных
 * 
 * @struct      UART_Resource::Config
 * @brief       Структура конфигурации параметров UART
 * @var         Config::state
 *              Состояние UART (включен/выключен)
 * @var         Config::dataBits
 *              Количество бит данных (5-9)
 * @var         Config::stopBits  
 *              Количество стоп-битов (1-2)
 * @var         Config::parity
 *              Контроль четности
 * @var         Config::interrupt
 *              Режим прерываний
 * @var         Config::baudRate
 *              Скорость передачи (бод)
 * 
 *
 *
 * @section     Публичные методы класса
 *
 * @fn					UART_Resource(const Config& config)				Конструктор с конфигурацией	
 * @fn				   ~UART_Resource()									Деструктор (автоматически отключает UART)
 *
 *
 *
 * @section     Методы включения/выключения
 *
 * @fn            bool	start()											Запуск UART с текущими настройками	
 * @fn            bool	start(uint32_t baudRate)						Запуск UART с указанной скоростью
 * @fn            bool	start(const Config& config)						Запуск UART с указанной конфигурацией
 * @fn			  bool	stop()											Остановка UART
 * @fn            bool	isWorking() const								Проверка включен ли UART
 *
 *
 *
 * @section     Методы чтения/записи
 *
 * @fn         uint8_t  bytesAvailable() const							Возвращает количество байт, доступных для чтения в буфере приема UART
 * @fn             int	read()											Чтение одного байта
 * @fn          size_t	read(uint8_t* buffer, size_t size)				Чтение массива данных
 * @fn            bool	write(uint8_t data)								Запись одного байта
 * @fn          size_t	write(const uint8_t* data, size_t size)			Запись массива данных
 * @fn          size_t	print(const char* str)							Запись строки 
 * @fn          size_t	println(const char* str)						Запись строки с переводом строки
 * @fn            void	flush()											Очистка буфера приема
 *
 *
 *
 * @section     Служебные методы 
 *
 * @fn            bool	setBaudRate(uint32_t baudRate)					Установка скорости передачи
 * @fn          Config	getConfig() const								Получение текущей конфигурации
 * @fn			  void	setRxCallback(UART_RxCallback callback)			Установка callback-функции для асинхронного приема
 * 
 *
 *
 * @section     Приватные поля
 * 
 * @var         UART_Resource::config_				Текущая конфигурация UART
 * @var         UART_Resource::isWorking_			Флаг работоспособности UART
 * 
 *
 *
 * @example     Пример использования:
 * @code
 * // Пример отправки сообщения в UART
 * UART_Resource uart;
 * uart.start(115200);
 * uart.println("Hello World!");
 * 
 * // Пример асинхронного приема
 * void myCallback(char c) { ## обработка ## }
 * uart.setRxCallback(myCallback);
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

#include <stdint.h>
#include <stddef.h>
#include "../drivers/UART/UART.h"

/**
 * @class UART_Resource
 * @brief Класс-ресурс для работы с UART интерфейсом
 * 
 * Инкапсулирует функциональность UART, предоставляя высокоуровневый API
 * для управления последовательным портом микроконтроллера.
 */
class UART_Resource
{
public:
    /**
     * @struct Config
     * @brief Структура конфигурации параметров UART
     */
    struct Config
    {
        UART_State state;           ///< Состояние UART (включен/выключен)
        UART_DataBits dataBits;     ///< Количество бит данных (5-9)
        UART_StopBits stopBits;     ///< Количество стоп-битов (1-2)
        UART_Parity parity;         ///< Контроль четности
        UART_Interrupt interrupt;   ///< Режим прерываний
        uint32_t baudRate;          ///< Скорость передачи (бод)
        
        /**
         * @brief Конструктор по умолчанию
         * @details Инициализирует параметры значениями по умолчанию:
         * - 8 бит данных, 1 стоп-бит, без контроля четности
         * - Прерывания приема включены, скорость 9600 бод
         */
        Config() :
            state(UART_ENABLED),
            dataBits(UART_8_BITS),
            stopBits(UART_1_STOP_BIT),
            parity(UART_PARITY_NONE),
            interrupt(UART_INTERRUPT_RX_ENABLED),
            baudRate(9600)
        {}
    };

public:    
    /**
     * @brief Конструктор UART ресурса
     * @param[in] config Конфигурация UART (по умолчанию используются стандартные настройки)
     */
    explicit UART_Resource(const Config& config = Config());
    
    /**
     * @brief Деструктор UART ресурса
     * @note Автоматически отключает UART при уничтожении объекта
     */
    ~UART_Resource();
    
    // Запрет копирования и перемещения
    UART_Resource(const UART_Resource&) = delete;
    UART_Resource& operator=(const UART_Resource&) = delete;
    UART_Resource(UART_Resource&&) = delete;
    UART_Resource& operator=(UART_Resource&&) = delete;
    
public:
    /**
     * @brief Запуск UART с текущими настройками
     * @return true если UART успешно запущен, false в случае ошибки
     */
    bool start();
    
    /**
     * @brief Запуск UART с указанной скоростью
     * @param[in] baudRate Скорость передачи в бодах
     * @return true если UART успешно запущен, false в случае ошибки
     */
    bool start(uint32_t baudRate);
    
    /**
     * @brief Запуск UART с указанной конфигурацией
     * @param[in] config Конфигурация UART
     * @return true если UART успешно запущен, false в случае ошибки
     */
    bool start(const Config& config);
	
	/**
     * @brief Остановка UART 
     * @return true если UART успешно остановлен, false в случае ошибки
     */
    bool stop();
    
    /**
     * @brief Проверка работоспособности UART
     * @return true если UART работает и готов к обмену данными
     */
    bool isWorking() const;
    
    /**
     * @brief Получение количества доступных для чтения байт
     * @return Количество байт в буфере приема
     */
    uint8_t bytesAvailable() const;
    
    /**
     * @brief Чтение одного байта из буфера приема
     * @return Прочитанный байт или -1 если данных нет
     */
    int read();
    
    /**
     * @brief Чтение массива данных из буфера приема
     * @param[out] buffer Буфер для записи данных
     * @param[in] size Размер буфера
     * @return Количество фактически прочитанных байт
     */
    size_t read(uint8_t* buffer, size_t size);
    
    /**
     * @brief Запись одного байта в UART
     * @param[in] data Байт для передачи
     * @return true если запись успешна, false в случае ошибки
     */
    bool write(uint8_t data);
    
    /**
     * @brief Запись массива данных в UART
     * @param[in] data Указатель на данные для передачи
     * @param[in] size Размер данных в байтах
     * @return Количество фактически записанных байт
     */
    size_t write(const uint8_t* data, size_t size);
    
    /**
     * @brief Запись строки в UART
     * @param[in] str Нуль-терминированная строка
     * @return Количество записанных символов
     */
    size_t print(const char* str);
    
    /**
     * @brief Запись строки с переводом строки
     * @param[in] str Нуль-терминированная строка
     * @return Количество записанных символов (включая \r\n)
     */
    size_t println(const char* str);
    
    /**
     * @brief Очистка буфера приема
     */
    void flush();
    
    /**
     * @brief Установка скорости передачи
     * @param[in] baudRate Скорость в бодах
     * @return true если скорость успешно изменена, false в случае ошибки
     */
    bool setBaudRate(uint32_t baudRate);
    
    /**
     * @brief Получение текущей конфигурации UART
     * @return Текущая конфигурация
     */
    Config getConfig() const;
    
    /**
     * @brief Установка callback-функции для асинхронного приема
     * @param[in] callback Функция обратного вызова
     */
    void setRxCallback(UART_RxCallback callback);
    
private:
    Config config_;     ///< Текущая конфигурация UART
    bool isWorking_;    ///< Флаг работоспособности UART
};