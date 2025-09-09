#include "ResourceManager.hpp"

ResourceManager::ResourceManager() : UART_(UART_Resource::Config{})
{
	//  онструктор по умолчанию
}

UART_Resource& ResourceManager::getUART()
{
	if (isUARTBusy_)
	{
		// ¬ будущем здесь можно добавить логику ожидани€
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
	// »нициализаци€ всех ресурсов
	UART_.start(57600);
	UART_.println("ResourceManager initialized");
	
	// «десь будет инициализаци€ других ресурсов:
	// SPI_.start();
	// ADC_.start();
	// и т.д.
}