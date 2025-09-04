#pragma once
#include "../resources/UART_Resource.hpp"
#include "Scheduler.hpp" // Оставляем это включение
#include <stdlib.h>
#include <stdint.h>

class SystemKernel
{
	private:
	// ВЛАДЕНИЕ: SystemKernel - единоличный владелец этих ресурсов.
	Scheduler scheduler_;                // Планировщик задач
	static Scheduler* schedulerInstanceForISR_; // Статический указатель для ISR
	UART_Resource UART_;                 // Ресурс UART
	bool isUARTBusy_ = false;            // Флаг занятости ресурса

	public:
	SystemKernel();
	void initialize();
	
	// Метод для ЗАИМСТВОВАНИЯ ресурса другими компонентами
	UART_Resource& getUART();
	void releaseUART();

	// Методы для управления задачами (делегируются планировщику)
	bool addTask(TaskFunction function, TaskPriority priority) {
		return scheduler_.addTask(function, priority);
	}

	// Главный цикл ядра
	void run() {
		while(true) {
			scheduler_.dispatch();
		}
	}

	// Доступ к планировщику
	Scheduler& getScheduler() { return scheduler_; }
	
	// Статический метод для доступа из ISR
	static Scheduler* getSchedulerForISR() { return schedulerInstanceForISR_; }
};