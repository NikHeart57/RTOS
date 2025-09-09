#pragma once
#include <stdint.h>
#include "../resources/UART_Resource.hpp"

class ResourceManager {
private:
	// ��������: �������� - ����������� �������� ���� ��������.
	UART_Resource UART_;
	// SPI_Resource SPI_;
	// ADC_Resource ADC_;

	// ����� ��������� (����� ����� �������� �� ����� ������� ���������)
	bool isUARTBusy_ = false;
	// bool isSPIBusy_ = false;

public:
	ResourceManager();
	
	// ������ ��� ������������� ��������
	UART_Resource& getUART();
	void releaseUART();

	// ������ ��� ������������� ���� �������� (���������� ����� ���� ��� ��� ������)
	void initializeAll();
};