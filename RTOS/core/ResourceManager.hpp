#pragma once
#include <stdint.h>
#include "../resources/UART_Resource.hpp"

class ResourceManager {
private:
	// ВЛАДЕНИЕ: Менеджер - единоличный владелец этих ресурсов.
	UART_Resource UART_;
	// SPI_Resource SPI_;
	// ADC_Resource ADC_;

	// Флаги занятости (позже можно заменить на более сложные механизмы)
	bool isUARTBusy_ = false;
	// bool isSPIBusy_ = false;

public:
	ResourceManager();
	
	// Методы для ЗАИМСТВОВАНИЯ ресурсов
	UART_Resource& getUART();
	void releaseUART();

	// Методы для инициализации всех ресурсов (вызывается ядром один раз при старте)
	void initializeAll();
};