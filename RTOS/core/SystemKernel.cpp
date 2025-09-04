#include "SystemKernel.hpp"

// Правильная инициализация статического члена
Scheduler* SystemKernel::schedulerInstanceForISR_ = nullptr;

SystemKernel::SystemKernel() : UART_(UART_Resource::Config{})
{
	schedulerInstanceForISR_ = &scheduler_; // Сохраняем адрес планировщика
}

void SystemKernel::initialize()
{
	// Инициализируем планировщик
	scheduler_.init();
	
	// Владелец инициализирует свои ресурсы
	UART_.start(57600);
	UART_.println("System Kernel initialized");
}

UART_Resource& SystemKernel::getUART()
{
	if (isUARTBusy_)
	{
		UART_.println("ERROR: UART is already busy!");
	}
	isUARTBusy_ = true;
	return UART_;
}

void SystemKernel::releaseUART()
{
	isUARTBusy_ = false;
}