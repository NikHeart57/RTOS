#include "SystemKernel.hpp"
#include <avr/io.h>
#include <avr/interrupt.h>

// ISR таймера
ISR(TIMER2_COMP_vect) 
{
	SystemKernel::getTaskManagerForISR()->tick();
}


// Инициализация статических членов
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
	
	// Настройка Timer2 для генерации прерывания по совпадению каждую 1 мс
	TCCR2 = (1 << WGM21);	// Режим CTC (сброс при совпадении с OCR2)
	OCR2 = 124;				// Значение для сравнения: (16000000 / 128 / 1000) - 1 = 124
	TIMSK |= (1 << OCIE2);	// Разрешить прерывание по совпадению с OCR2
	TCCR2 |= (1 << CS22) | (1 << CS20); // Запустить таймер: режим CTC, предделитель 128

	// Инициализация ресурсов
	resourceManager_.initializeAllResources();
	
	sei(); // Глобальное разрешение прерываний
}

// Делегирование API менеджерам
bool SystemKernel::addTask(TaskFunction function, TaskPriority priority)
{
	return taskManager_.addTask(function, priority);
}

UART_Resource& SystemKernel::getUART()
{
	return resourceManager_.getUART();
}


// Главный цикл системы
void SystemKernel::run()
{
	while (true)
	{
		taskManager_.dispatch();
	}
}

// Статические методы для ISR
TaskManager* SystemKernel::getTaskManagerForISR()
{
	return taskManagerInstanceForISR_;
}

ResourceManager* SystemKernel::getResourceManagerForISR()
{
	return resourceManagerInstanceForISR_;
}
