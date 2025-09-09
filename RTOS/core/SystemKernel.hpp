#pragma once
#include "TaskManager.hpp"
#include "ResourceManager.hpp"

class SystemKernel {
	private:
	// ��������: ���� ������� �����������.
	TaskManager taskManager_;
	ResourceManager resourceManager_;

	// ��������� ��� ISR (������� ��������)
	static TaskManager* taskManagerInstanceForISR_;
	static ResourceManager* resourceManagerInstanceForISR_;

	public:
	SystemKernel();
	void initialize(); // ������������� ���� �������

	// ������������� API ���������� (�������� ��� ����������)
	bool addTask(TaskFunction function, TaskPriority priority);
	UART_Resource& getUART();
	void releaseUART();

	// ������� ���� �������
	void run();

	// ������ � ���������� (�� ������ ������� ������)
	TaskManager& getTaskManager() { return taskManager_; }
	ResourceManager& getResourceManager() { return resourceManager_; }

	// ����������� ������ ��� ISR
	static TaskManager* getTaskManagerForISR();
	static ResourceManager* getResourceManagerForISR();
};