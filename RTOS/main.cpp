/**
 * Мнение нейросети о том что мне делать дальше:
 *
 * "Ты прав в своем желании. Вытесняющая многозадачность — это логичный и мощный следующий шаг.
 * Но мой совет: Не бросайся сразу ее реализовывать.
 * Закончи и отлади свою кооперативную систему. Она уже является working RTOS!
 * Пойми все ее ограничения на практике. Напиши несколько задач, которые будут конфликтовать за ресурсы и время.
 * Освой принципы синхронизации (мьютексы, критические секции) сначала в кооперативной среде."
 *
 */
#include <stdlib.h>
#include "main.hpp"

SystemKernel Kernel; // Ядро системы

// Прототипы задач
void fastTask(void);
void slowTask(void);
void blinkTask(void);

// Глобальные переменные для состояний задач (опционально, для наглядности)
volatile uint32_t fastCounter = 0;
volatile uint32_t slowCounter = 0;
volatile bool ledState = false;
char itoa_buffer[16] = {0};

int main(void)
{
    Kernel.initialize(); // Инициализируем ядро (UART, таймер...)
    
    // Регистрируем задачи с РАЗНЫМИ приоритетами
    Kernel.addTask(fastTask,   TaskPriority::HIGH);    // Высокий приоритет
    Kernel.addTask(slowTask,   TaskPriority::NORMAL);  // Средний приоритет
    Kernel.addTask(blinkTask,  TaskPriority::LOW);     // Низкий приоритет
    
    Kernel.run(); // Передаем управление планировщику (сюда программа не вернется)
    
    return 1;
}

//--------------------------------------------------
// ЗАДАЧА 1: Быстрая строка (выполняется часто)
//--------------------------------------------------
void fastTask(void)
{
    auto& uart = Kernel.getUART();                     // Запрашиваем UART
    
    uart.print("\n\rFAST#");                           // Печатаем свой символ
    uart.print(itoa(fastCounter++, itoa_buffer, 10));  // Увеличиваем счетчик
        
    Kernel.releaseUART();                              // Освобождаем UART
    
    // Усыпаем всего на 50 мс -> будет вызываться ОЧЕНЬ часто
    // Используем новый API через TaskManager
    Kernel.getTaskManager().sleep(50);
}

//--------------------------------------------------
// ЗАДАЧА 2: Медленная строка (выполняется редко)
//--------------------------------------------------
void slowTask(void)
{
    auto& uart = Kernel.getUART();                     // Запрашиваем UART
    
    uart.print("\n\rslow.");                           // Печатаем свой символ
    uart.print(itoa(slowCounter++, itoa_buffer, 10));  // Увеличиваем счетчик
    
    Kernel.releaseUART();                              // Освобождаем UART
    
    // Усыпаем на 500 мс -> будет вызываться РЕДКО
    Kernel.getTaskManager().sleep(500);
}

//--------------------------------------------------
// ЗАДАЧА 3: Мигающий светодиод (на порту C, пин 0)
//--------------------------------------------------
void blinkTask(void)
{
    // Работаем с железом НАПРЯМУЮ через драйвер (для разнообразия)
    // Предположим, что светодиод на PORTC, PIN0
    GPIO_WritePin(GPIO_PORT_C, GPIO_PIN_0, ledState ? GPIO_HIGH : GPIO_LOW);
    
    // Меняем состояние для следующего раза
    ledState = !ledState;
    
    // Мигаем с средней частотой
    Kernel.getTaskManager().sleep(2000);
}