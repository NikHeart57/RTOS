#include "Scheduler.hpp"
#include "SystemKernel.hpp" // Теперь здесь можно включать
#include <avr/io.h>
#include <avr/interrupt.h>

ISR(TIMER1_COMPA_vect) {
	SystemKernel::getSchedulerForISR()->tick();
}

// Определение конструктора
Scheduler::Scheduler()
{
	taskCount = 0;
	systemTicks = 0;
	currentTask = nullptr;
	
	// Инициализация массива указателей
	for(uint8_t i = 0; i < MAX_TASKS; i++)
	{
		taskList[i] = nullptr;
	}
	// taskPool автоматически инициализируется конструкторами по умолчанию
}

void Scheduler::init() {
	// Настройка Timer1 для генерации прерывания по совпадению каждую 1 мс
	// (Пример для ATmega328P, F_CPU=16MHz)
	TCCR1A = 0;
	TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10); // CTC mode, prescaler 64
	OCR1A = 249; // Compare value для 1ms: (16000000 / 64 / 1000) - 1 = 249
	TIMSK = (1 << OCIE1A); // Enable compare match interrupt

	sei(); // Глобальное разрешение прерываний
}

void Scheduler::tick() {
	systemTicks++;

	// Обновляем состояния всех задач
	for(uint8_t i = 0; i < taskCount; i++) {
		Task* task = taskList[i];
		if(task->state == TaskState::SLEEPING) {
			if(task->tickCounter > 0) {
				task->tickCounter--;
			}
			if(task->tickCounter == 0) {
				task->state = TaskState::READY; // Задача проснулась
			}
		}
	}
}

void Scheduler::dispatch() {
	// Поиск задачи с наивысшим приоритетом, готовой к выполнению (READY)
	int8_t highestPriorityTaskIndex = -1;
	TaskPriority highestPriority = TaskPriority::IDLE;

	for(uint8_t i = 0; i < taskCount; i++) {
		Task* task = taskList[i];
		if((task->state == TaskState::READY) && (task->priority > highestPriority)) {
			highestPriority = task->priority;
			highestPriorityTaskIndex = i;
		}
	}

	// Если нашли готовую задачу - выполняем ее
	if(highestPriorityTaskIndex >= 0) {
		currentTask = taskList[highestPriorityTaskIndex]; // <-- ЗАПОМИНАЕМ ТЕКУЩУЮ ЗАДАЧУ
		currentTask->state = TaskState::RUNNING;
		currentTask->lastRunTime = systemTicks;

		// Выполняем саму функцию задачи
		currentTask->function();

		// После выполнения переводим задачу обратно в состояние READY,
		// если она не была переведена в SLEEPING или SUSPENDED внутри самой себя.
		if(currentTask->state == TaskState::RUNNING) {
			currentTask->state = TaskState::READY;
		}
		currentTask = nullptr; // <-- СБРАСЫВАЕМ УКАЗАТЕЛЬ ПОСЛЕ ВЫПОЛНЕНИЯ
	}

	// Если ни одна задача не готова, можно выполнить задачу холостого хода (idle)
	// или перевести МК в режим пониженного энергопотребления.
}

bool Scheduler::addTask(TaskFunction function, TaskPriority priority)
{
	if(taskCount >= MAX_TASKS) return false;
		
	// Инициализируем задачу в заранее выделенной памяти
	taskList[taskCount] = &taskPool[taskCount];
	*taskList[taskCount] = Task(function, priority); // Используем конструктор
	taskCount++;
	return true;
}

void Scheduler::suspendTask(TaskFunction function) {
	for(uint8_t i = 0; i < taskCount; i++) {
		if(taskList[i]->function == function) {
			taskList[i]->state = TaskState::SUSPENDED;
			return;
		}
	}
}

void Scheduler::resumeTask(TaskFunction function) {
	for(uint8_t i = 0; i < taskCount; i++) {
		if(taskList[i]->function == function) {
			taskList[i]->state = TaskState::READY;
			return;
		}
	}
}

void Scheduler::sleep(uint32_t ticksToSleep) {
	// Эта функция вызывается ИЗНУТРИ самой задачи.
	// Мы знаем, какая задача выполняется в данный момент (currentTask != nullptr)
	if (currentTask) {
		currentTask->state = TaskState::SLEEPING;
		currentTask->tickCounter = ticksToSleep;
	}
	// Если sleep вызван не из задачи (что невозможно), просто игнорируем.
}


