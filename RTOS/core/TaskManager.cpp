#include "TaskManager.hpp"
#include <avr/io.h>

TaskManager::TaskManager()
{
	taskCount = 0;
	systemTicks = 0;
	currentTask = nullptr;
	
	// Инициализация массива указателей
	for(uint8_t i = 0; i < MAX_TASKS; i++)
	{
		taskList[i] = nullptr;
	}
}

void TaskManager::tick() {
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

void TaskManager::dispatch() {
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
		currentTask = taskList[highestPriorityTaskIndex];
		currentTask->state = TaskState::RUNNING;
		currentTask->lastRunTime = systemTicks;

		// Выполняем саму функцию задачи
		currentTask->function();

		// После выполнения переводим задачу обратно в состояние READY,
		// если она не была переведена в SLEEPING или SUSPENDED внутри самой себя.
		if(currentTask->state == TaskState::RUNNING) {
			currentTask->state = TaskState::READY;
		}
		currentTask = nullptr;
	}
}

bool TaskManager::addTask(TaskFunction function, TaskPriority priority)
{
	if(taskCount >= MAX_TASKS) return false;
	
	// Инициализируем задачу в заранее выделенной памяти
	taskList[taskCount] = &taskPool[taskCount];
	*taskList[taskCount] = Task(function, priority);
	taskCount++;
	return true;
}

void TaskManager::suspendTask(TaskFunction function)
{
	for(uint8_t i = 0; i < taskCount; i++) {
		if(taskList[i]->function == function) {
			taskList[i]->state = TaskState::SUSPENDED;
			return;
		}
	}
}

void TaskManager::resumeTask(TaskFunction function)
{
	for(uint8_t i = 0; i < taskCount; i++) {
		if(taskList[i]->function == function) {
			taskList[i]->state = TaskState::READY;
			return;
		}
	}
}

void TaskManager::sleep(uint32_t ticksToSleep) {
	if (currentTask) {
		currentTask->state = TaskState::SLEEPING;
		currentTask->tickCounter = ticksToSleep;
	}
}