#include "SystemKernel.hpp"
#include <avr/io.h>
#include <avr/interrupt.h>

// ISR �������
ISR(TIMER2_COMP_vect) 
{
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
	cli();
	
	// ��������� Timer2 ��� ��������� ���������� �� ���������� ������ 1 ��
	TCCR2 = (1 << WGM21);	// ����� CTC (����� ��� ���������� � OCR2)
	OCR2 = 124;				// �������� ��� ���������: (16000000 / 128 / 1000) - 1 = 124
	TIMSK |= (1 << OCIE2);	// ��������� ���������� �� ���������� � OCR2
	TCCR2 |= (1 << CS22) | (1 << CS20); // ��������� ������: ����� CTC, ������������ 128

	// ������������� ��������
	resourceManager_.initializeAllResources();
	
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
