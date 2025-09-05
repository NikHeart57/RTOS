#pragma once
#include <stdint.h>


// Состояние задачи
enum class TaskState {
	READY,      // Готова к выполнению
	RUNNING,    // Выполняется в данный момент
	SLEEPING,   // Заснула на определенное время
	SUSPENDED   // Приостановлена явно (вызовом stop())
};

// Приоритет задачи. Чем число больше, тем приоритет выше.
// Задача с высшим приоритетом будет выполняться первой.
enum class TaskPriority 
{
	IDLE = 0,   // Задача холостого хода (самый низкий)
	LOW = 1,
	NORMAL = 2,
	HIGH = 3,
	SYSTEM = 4  // Для критичных системных задач (самый высокий)
};

// Тип указателя на функцию задачи.
// Задача - это всегда функция вида void(*)(void).
typedef void (*TaskFunction)(void);

/**
	* @brief Структура, описывающая одну задачу в системе.
	* @detail Планировщик будет работать с массивом таких структур.
	*/
struct Task 
{
	TaskFunction function;    // Указатель на функцию задачи
	TaskState state;          // Текущее состояние
	TaskPriority priority;    // Приоритет
	uint32_t tickCounter;     // Счетчик тиков до пробуждения (для SLEEPING)
	uint32_t tickInterval;    // Интервал для периодических задач
	uint32_t lastRunTime;     // Время последнего запуска (на основе системных тиков)

	// Конструктор по умолчанию
	Task()		: function(nullptr), state(TaskState::READY), 
				priority(TaskPriority::IDLE), 
				tickCounter(0), tickInterval(0), lastRunTime(0) {}
    
	// Конструктор для удобного создания
	Task(TaskFunction func, TaskPriority prio)	: function(func), state(TaskState::READY), 
												priority(prio), tickCounter(0), 
												tickInterval(0), lastRunTime(0) {}
};

