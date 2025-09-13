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

SystemKernel kernel; // Ядро системы

// Прототипы задач
void fastTask(void);
void slowTask(void);
void blinkTask(void);

void stressTestTask(void);
void resourceRaceTask(void);
void priorityInheritanceTest(void);

// Глобальные переменные для состояний задач (опционально, для наглядности)
volatile uint32_t fastCounter = 0;
volatile uint32_t slowCounter = 0;
volatile bool ledState = false;
volatile uint32_t stressSuccess = 0;
volatile uint32_t stressFailures = 0;
volatile uint32_t stressTotalAttempts = 0;
char itoa_buffer[16] = {0};

int main(void)
{
    kernel.initialize(); // Инициализируем ядро (UART, таймер...)
    
    // Регистрируем задачи с РАЗНЫМИ приоритетами
    kernel.addTask(fastTask,   TaskPriority::HIGH);    // Высокий приоритет
    kernel.addTask(slowTask,   TaskPriority::NORMAL);  // Средний приоритет
    kernel.addTask(blinkTask,  TaskPriority::LOW);     // Низкий приоритет
    
	// Тестовые задачи (раскомментируйте нужные)
    // kernel.addTask(stressTestTask, TaskPriority::HIGH);
    kernel.addTask(resourceRaceTask, TaskPriority::NORMAL);
    // kernel.addTask(priorityInheritanceTest, TaskPriority::LOW);
	 
    kernel.run(); // Передаем управление планировщику (сюда программа не вернется)
    
    return 1;
}

//--------------------------------------------------
// ЗАДАЧА 1: Быстрая строка (выполняется часто)
//--------------------------------------------------
void fastTask(void)
{
	auto& uart = kernel.getUART();							// Запрашиваем UART
	auto& uart_mutex = kernel.getResourceManager().getUART_Mutex();
	
	if(uart_mutex.lock(10))
	{
		uart.print("\n\rF");								// Печатаем свой символ
		uart.print(itoa(fastCounter++, itoa_buffer, 10));	// Увеличиваем счетчик
		uart_mutex.unlock();								// Освобождаем UART
	}
	else
	{
		uart.print("\n\rfastTask() - Failure To Lock UART ");
	}
    
    // Усыпаем всего на 50 мс -> будет вызываться ОЧЕНЬ часто
    // Используем новый API через TaskManager
    kernel.getTaskManager().sleep(50);
}

//--------------------------------------------------
// ЗАДАЧА 2: Медленная строка (выполняется редко)
//--------------------------------------------------
void slowTask(void)
{
    auto& uart = kernel.getUART();							// Запрашиваем UART
    auto& uart_mutex = kernel.getResourceManager().getUART_Mutex();
    
	if(uart_mutex.lock(10))
	{
		uart.print("\n\rs");								// Печатаем свой символ
		uart.print(itoa(slowCounter++, itoa_buffer, 10));	// Увеличиваем счетчик
		uart_mutex.unlock();								// Освобождаем UART
	}
	else
	{
		uart.print("\n\rslowTask() - Failure To Lock UART ");
	}

    
    // Усыпаем на 500 мс -> будет вызываться РЕДКО
    kernel.getTaskManager().sleep(500);
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
    kernel.getTaskManager().sleep(500);
}

//--------------------------------------------------
// ЗАДАЧА 4: Нагрузочный тест мьютекса
//--------------------------------------------------
void stressTestTask(void)
{
	auto& uart = kernel.getUART();
	auto& uart_mutex = kernel.getResourceManager().getUART_Mutex();
	
	// Случайное время ожидания (10-100 мс)
	uint32_t randomWait = 10 + (stressTotalAttempts % 90);
	
	// ОЧЕНЬ короткий таймаут - 1-5 мс (экстремальные условия)
	uint32_t shortTimeout = 1 + (stressTotalAttempts % 5);
	
	if (uart_mutex.lock(shortTimeout)) {
		// Успех! Работаем с ресурсом
		stressSuccess++;
		
		// Имитация разной длительности работы с ресурсом
		uart.print("\n\rSTRESS[");
		uart.print(itoa(stressSuccess, itoa_buffer, 10));
		uart.print("]");
		
		// Задержка внутри критической секции
		kernel.getTaskManager().sleep(randomWait);
		
		uart_mutex.unlock();
		} 
		else 
		{
		// Провал - мьютекс занят
		stressFailures++;
		
		// Краткий отчет о провале
		uart.print("\n\rFAIL[");
		uart.print(itoa(stressFailures, itoa_buffer, 10));
		uart.print("]");
	}
	
	stressTotalAttempts++;
	
	// Быстрый перезапуск для создания нагрузки
	kernel.getTaskManager().sleep(5);
}

//--------------------------------------------------
// ЗАДАЧА 5: Конкурентный доступ (гонка за ресурсом)
//--------------------------------------------------
void resourceRaceTask(void)
{
	auto& uart = kernel.getUART();
	auto& uart_mutex = kernel.getResourceManager().getUART_Mutex();
	
	static uint8_t taskId = 0;
	static bool initialized = false;
	
	if (!initialized) {
		taskId = stressTotalAttempts % 5; // ID задачи 0-4
		initialized = true;
	}
	
	// Попытка захватить мьютекс с разными таймаутами
	uint32_t timeout = 5 + (taskId * 3); // Разные таймауты для каждой задачи
	
	if (uart_mutex.lock(timeout)) 
	{
		// Критическая секция - работа с разделяемым ресурсом
		uart.print("\n\rT");
		uart.print(itoa(taskId, itoa_buffer, 10));
		uart.print("[IN]");
		
		// Длительная операция в критической секции
		for (volatile int i = 0; i < 1000; i++) 
		{
			// Имитация вычислений
		}
		
		uart.print("[OUT]");
		uart_mutex.unlock();
		}
		else 
		{
		// Не удалось захватить - конкуренция!
		uart.print("\n\rT");
		uart.print(itoa(taskId, itoa_buffer, 10));
		uart.print("[BLOCKED]");
	}
	
	kernel.getTaskManager().sleep(20 + taskId * 10);
}

//--------------------------------------------------
// ЗАДАЧА 6: Тест наследования приоритетов
//--------------------------------------------------
void priorityInheritanceTest(void)
{
	auto& uart = kernel.getUART();
	auto& uart_mutex = kernel.getResourceManager().getUART_Mutex();
	
	// Низкоприоритетная задача захватывает мьютекс надолго
	if (uart_mutex.lock(0)) {
		uart.print("\n\rLOW_PRIO_LOCKED");
		
		// Долгая работа в критической секции
		kernel.getTaskManager().sleep(200); // 200мс - ОЧЕНЬ долго!
		
		uart.print("\n\rLOW_PRIO_RELEASED");
		uart_mutex.unlock();
	}
	
	kernel.getTaskManager().sleep(1000);
}