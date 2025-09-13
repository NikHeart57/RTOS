#pragma once
#include "Task.hpp"
#include "TaskManager.hpp"
#include <util/atomic.h>

class Mutex {
	private:
	volatile bool locked = false;     // ��������� ��������
	Task* owner = nullptr;            // �������� (��� ������������ �����������)
	TaskPriority original_priority;   // �������� ��������� ���������

	public:
	Mutex() = default;

	bool lock(uint32_t timeout_ticks = 0);
	bool unlock();
	bool isLocked() const { return locked; }
	Task* getOwner() const { return owner; }
};