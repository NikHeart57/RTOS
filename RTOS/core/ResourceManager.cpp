
#include "ResourceManager.hpp"

ResourceManager::ResourceManager() : UART_(UART_Resource::Config{})
{
	// ����������� �� ���������
}

UART_Resource& ResourceManager::getUART()
{
	// ������ ���������� ������� ������ �����
	return UART_;
}

Mutex& ResourceManager::getUART_Mutex()
{
	return UART_mutex_; // ���������� ���
}

void ResourceManager::initializeAllResources()
{
	UART_.start(57600);
	UART_.println("ResourceManager initialized");
	
	// ����� ����� ������������� ������ ��������:
	// SPI_.start();
	// ADC_.start();
	// � �.�.
}