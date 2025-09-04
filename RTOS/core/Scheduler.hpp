#pragma once
#include "Task.hpp"

// Forward declaration вместо включения SystemKernel.hpp
class SystemKernel;

class Scheduler {
	private:
	static const uint8_t MAX_TASKS = 10;
	Task taskPool[MAX_TASKS];
	Task* taskList[MAX_TASKS];
	uint8_t taskCount = 0;
	uint32_t systemTicks = 0;
	Task* currentTask = nullptr;
	
	public:
	Scheduler();
	~Scheduler() = default;

	Scheduler(const Scheduler&) = delete;
	void operator=(const Scheduler&) = delete;

	void init();
	void dispatch();
	bool addTask(TaskFunction function, TaskPriority priority);
	void suspendTask(TaskFunction function);
	void resumeTask(TaskFunction function);
	void tick();
	void sleep(uint32_t ticksToSleep);
	uint32_t getSystemTicks() const { return systemTicks; }
};