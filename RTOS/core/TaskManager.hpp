#pragma once
#include "Task.hpp"

class TaskManager {
private:
	static const uint8_t MAX_TASKS = 10;
	Task taskPool[MAX_TASKS]; // ��� �����
	Task* taskList[MAX_TASKS]; // ������ ���������� �� ������ ��� �������� ������
	uint8_t taskCount = 0;
	uint32_t systemTicks = 0;
	Task* currentTask = nullptr; // ������, ������������� � ������ ������

public:
	TaskManager();
	void tick(); // ���������� ��������� ����� (���������� �� ����������)
	void dispatch(); // ����� � ������ ��������� ������

	// API ��� ���� � ����������
	bool addTask(TaskFunction function, TaskPriority priority);
	void suspendTask(TaskFunction function);
	void resumeTask(TaskFunction function);
	void sleep(uint32_t ticksToSleep); // ���������� �� ����� ������

	// �������
	uint32_t getSystemTicks() const { return systemTicks; }
	Task* getCurrentTask() const { return currentTask; } // ����� ����������� ResourceManager'�
};