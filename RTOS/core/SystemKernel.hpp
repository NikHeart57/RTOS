#pragma once
#include "../resources/UART_Resource.hpp"
#include "Scheduler.hpp" // ��������� ��� ���������
#include <stdlib.h>
#include <stdint.h>

class SystemKernel
{
	private:
	// ��������: SystemKernel - ����������� �������� ���� ��������.
	Scheduler scheduler_;                // ����������� �����
	static Scheduler* schedulerInstanceForISR_; // ����������� ��������� ��� ISR
	UART_Resource UART_;                 // ������ UART
	bool isUARTBusy_ = false;            // ���� ��������� �������

	public:
	SystemKernel();
	void initialize();
	
	// ����� ��� ������������� ������� ������� ������������
	UART_Resource& getUART();
	void releaseUART();

	// ������ ��� ���������� �������� (������������ ������������)
	bool addTask(TaskFunction function, TaskPriority priority) {
		return scheduler_.addTask(function, priority);
	}

	// ������� ���� ����
	void run() {
		while(true) {
			scheduler_.dispatch();
		}
	}

	// ������ � ������������
	Scheduler& getScheduler() { return scheduler_; }
	
	// ����������� ����� ��� ������� �� ISR
	static Scheduler* getSchedulerForISR() { return schedulerInstanceForISR_; }
};