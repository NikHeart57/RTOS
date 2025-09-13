#include "Mutex.hpp"
#include "SystemKernel.hpp" 

bool Mutex::lock(uint32_t timeout_ticks)
{
	Task* current_task;
	
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		current_task = SystemKernel::getTaskManagerForISR()->getCurrentTask();
		if (!current_task) return false;
		
		if (owner == current_task) return true;
	}
	
	uint32_t start_ticks = SystemKernel::getTaskManagerForISR()->getSystemTicks();
	
	while (true)
	{
		bool acquired = false;
		
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			if (owner == current_task) return true;
			
			if (!locked)
			{
				locked = true;
				owner = current_task;
				original_priority = owner->priority;
				acquired = true;
			}
		}
		
		if (acquired) {
			break; // Мьютекс захвачен
		}
		
		// Priority inheritance - повышаем приоритет владельца
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			if (locked && owner && current_task->priority > owner->priority)
			{
				owner->priority = current_task->priority;
			}
		}
		
		if (timeout_ticks > 0) {
			uint32_t current_ticks = SystemKernel::getTaskManagerForISR()->getSystemTicks();
			if ((current_ticks - start_ticks) >= timeout_ticks) {
				return false;
			}
		}
		
		SystemKernel::getTaskManagerForISR()->sleep(1);
	}
	
	return true;
}

bool Mutex::unlock() {
	Task* current_task = SystemKernel::getTaskManagerForISR()->getCurrentTask();
	
	// Только владелец может освободить мьютекс
	if (owner != current_task) {
		return false; // Ошибка: не владелец
	}

	// Восстанавливаем исходный приоритет
	if (owner->priority != original_priority) {
		owner->priority = original_priority;
	}

	// Освобождаем мьютекс
	locked = false;
	owner = nullptr;
	
	return true;
}