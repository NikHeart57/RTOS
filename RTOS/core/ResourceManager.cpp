#include "ResourceManager.hpp"

ResourceManager::ResourceManager() : UART_(UART_Resource::Config{})
{
	// ����������� �� ���������
}

UART_Resource& ResourceManager::getUART()
{
	if (isUARTBusy_)
	{
		// � ������� ����� ����� �������� ������ ��������
		UART_.println("ERROR: UART is already busy!");
	}
	isUARTBusy_ = true;
	return UART_;
}

void ResourceManager::releaseUART()
{
	isUARTBusy_ = false;
}

void ResourceManager::initializeAll()
{
	// ������������� ���� ��������
	UART_.start(57600);
	UART_.println("ResourceManager initialized");
	
	// ����� ����� ������������� ������ ��������:
	// SPI_.start();
	// ADC_.start();
	// � �.�.
}