#pragma once
#include "Task.hpp"
#include "TaskManager.hpp"
#include <util/atomic.h>

class Mutex {
	private:
	volatile bool locked = false;     // Состояние мьютекса
	Task* owner = nullptr;            // Владелец (для наследования приоритетов)
	TaskPriority original_priority;   // Исходный приоритет владельца

	public:
	Mutex() = default;

	bool lock(uint32_t timeout_ticks = 0);
	bool unlock();
	bool isLocked() const { return locked; }
	Task* getOwner() const { return owner; }
};