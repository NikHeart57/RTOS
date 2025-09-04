#include "main.hpp"

SystemKernel Kernel; // Ядро, владеющее ВСЕМ (ресурсами и планировщиком)

void heartbeatTask(void)
{
	auto& uart = Kernel.getUART();
	uart.println("[HEARTBEAT] \tSystem is alive!");
	Kernel.releaseUART();

	// Усыпляем задачу на 1000 тиков (1000 мс)
	// Обращаемся к планировщику ЧЕРЕЗ ЯДРО
	Kernel.getScheduler().sleep(1000);
}

void sensorReadTask(void)
{
	auto& uart = Kernel.getUART();
	uart.println("[SENSOR READ] \tRead sensor info");
	Kernel.releaseUART();

	// Усыпляем задачу на 500 мс
	// Обращаемся к планировщику ЧЕРЕЗ ЯДРО
	Kernel.getScheduler().sleep(500);
}

int main(void)
{
	Kernel.initialize(); // Инициализируем ядро (оно само инициализирует свои ресурсы и планировщик)

	// Регистрируем задачи ЧЕРЕЗ ЯДРО
	Kernel.addTask(heartbeatTask, TaskPriority::NORMAL);
	Kernel.addTask(sensorReadTask, TaskPriority::HIGH); // Чтение датчика важнее

	// Запускаем главный цикл ЯДРА, который внутри себя вызывает диспетчер планировщика
	Kernel.run();

	return 1; // Сюда мы никогда не попадем
}