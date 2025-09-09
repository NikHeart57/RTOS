#pragma once
#include "Task.hpp"

class TaskManager {
private:
	static const uint8_t MAX_TASKS = 10;
	Task taskPool[MAX_TASKS]; // Пул задач
	Task* taskList[MAX_TASKS]; // Список указателей на задачи для быстрого обхода
	uint8_t taskCount = 0;
	uint32_t systemTicks = 0;
	Task* currentTask = nullptr; // Задача, выполняющаяся в данный момент

public:
	TaskManager();
	void tick(); // Обновление состояний задач (вызывается из прерывания)
	void dispatch(); // Выбор и запуск следующей задачи

	// API для ядра и приложения
	bool addTask(TaskFunction function, TaskPriority priority);
	void suspendTask(TaskFunction function);
	void resumeTask(TaskFunction function);
	void sleep(uint32_t ticksToSleep); // Вызывается из самой задачи

	// Геттеры
	uint32_t getSystemTicks() const { return systemTicks; }
	Task* getCurrentTask() const { return currentTask; } // Может пригодиться ResourceManager'у
};