#pragma once
#include "TaskManager.hpp"
#include "ResourceManager.hpp"

class SystemKernel {
	private:
	// ВЛАДЕНИЕ: Ядро владеет менеджерами.
	TaskManager taskManager_;
	ResourceManager resourceManager_;

	// Указатели для ISR (статика остается)
	static TaskManager* taskManagerInstanceForISR_;
	static ResourceManager* resourceManagerInstanceForISR_;

	public:
	SystemKernel();
	void initialize(); // Инициализация всей системы

	// Делегирование API менеджерам (удобство для приложения)
	bool addTask(TaskFunction function, TaskPriority priority);
	UART_Resource& getUART();
	void releaseUART();

	// Главный цикл системы
	void run();

	// Доступ к менеджерам (на случай сложной логики)
	TaskManager& getTaskManager() { return taskManager_; }
	ResourceManager& getResourceManager() { return resourceManager_; }

	// Статические методы для ISR
	static TaskManager* getTaskManagerForISR();
	static ResourceManager* getResourceManagerForISR();
};