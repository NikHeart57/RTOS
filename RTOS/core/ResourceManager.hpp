#pragma once
#include <stdint.h>
#include "../resources/UART_Resource.hpp"
#include "Mutex.hpp"

class ResourceManager {
private:
	// ��������: �������� - ����������� �������� ���� ��������.
	UART_Resource UART_;
	// SPI_Resource SPI_;
	// ADC_Resource ADC_;

	// ����� ��������� 
	Mutex UART_mutex_;
	// Mutex SPI_mutex_;

public:
	ResourceManager();
	
	// ������ ��� ������������� ��������
	UART_Resource& getUART();
	Mutex& getUART_Mutex();
		
	// ������ ��� ������������� ���� �������� (���������� ����� ���� ��� ��� ������)
	void initializeAllResources();
};