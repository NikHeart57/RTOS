#include "SystemKernel.hpp"
#include <avr/io.h>
#include <avr/interrupt.h>

// ISR �������
ISR(TIMER1_COMPA_vect) {
	SystemKernel::getTaskManagerForISR()->tick();
}

// ������������� ����������� ������
TaskManager* SystemKernel::taskManagerInstanceForISR_ = nullptr;
ResourceManager* SystemKernel::resourceManagerInstanceForISR_ = nullptr;

SystemKernel::SystemKernel()
{
	taskManagerInstanceForISR_ = &taskManager_;
	resourceManagerInstanceForISR_ = &resourceManager_;
}

void SystemKernel::initialize()
{
	// ������������� ������� (���������� �� ������� Scheduler::init())
	// ��������� Timer1 ��� ��������� ���������� �� ���������� ������ 1 ��
	TCCR1A = 0;
	TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10); // CTC mode, prescaler 64
	OCR1A = 249; // Compare value ��� 1ms: (16000000 / 64 / 1000) - 1 = 249
	TIMSK = (1 << OCIE1A); // Enable compare match interrupt

	// ������������� ��������
	resourceManager_.initializeAll();
	
	sei(); // ���������� ���������� ����������
}

// ������������� API ����������
bool SystemKernel::addTask(TaskFunction function, TaskPriority priority)
{
	return taskManager_.addTask(function, priority);
}

UART_Resource& SystemKernel::getUART()
{
	return resourceManager_.getUART();
}

void SystemKernel::releaseUART()
{
	resourceManager_.releaseUART();
}

// ������� ���� �������
void SystemKernel::run()
{
	while (true)
	{
		taskManager_.dispatch();
	}
}

// ����������� ������ ��� ISR
TaskManager* SystemKernel::getTaskManagerForISR()
{
	return taskManagerInstanceForISR_;
}

ResourceManager* SystemKernel::getResourceManagerForISR()
{
	return resourceManagerInstanceForISR_;
}
