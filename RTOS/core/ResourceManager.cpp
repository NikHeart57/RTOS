
#include "ResourceManager.hpp"

ResourceManager::ResourceManager() : UART_(UART_Resource::Config{})
{
	// Конструктор по умолчанию
}

UART_Resource& ResourceManager::getUART()
{
	// Теперь используем мьютекс вместо флага
	return UART_;
}

Mutex& ResourceManager::getUART_Mutex()
{
	return UART_mutex_; // Исправлено имя
}

void ResourceManager::initializeAllResources()
{
	UART_.start(57600);
	UART_.println("ResourceManager initialized");
	
	// Здесь будет инициализация других ресурсов:
	// SPI_.start();
	// ADC_.start();
	// и т.д.
}