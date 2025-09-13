#pragma once
#include <stdint.h>
#include "../resources/UART_Resource.hpp"
#include "Mutex.hpp"

class ResourceManager {
private:
	// ВЛАДЕНИЕ: Менеджер - единоличный владелец этих ресурсов.
	UART_Resource UART_;
	// SPI_Resource SPI_;
	// ADC_Resource ADC_;

	// Флаги занятости 
	Mutex UART_mutex_;
	// Mutex SPI_mutex_;

public:
	ResourceManager();
	
	// Методы для ЗАИМСТВОВАНИЯ ресурсов
	UART_Resource& getUART();
	Mutex& getUART_Mutex();
		
	// Методы для инициализации всех ресурсов (вызывается ядром один раз при старте)
	void initializeAllResources();
};