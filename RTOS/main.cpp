#include "main.hpp"

SystemKernel Kernel; // ����, ��������� ���� (��������� � �������������)

void heartbeatTask(void)
{
	auto& uart = Kernel.getUART();
	uart.println("[HEARTBEAT] \tSystem is alive!");
	Kernel.releaseUART();

	// �������� ������ �� 1000 ����� (1000 ��)
	// ���������� � ������������ ����� ����
	Kernel.getScheduler().sleep(1000);
}

void sensorReadTask(void)
{
	auto& uart = Kernel.getUART();
	uart.println("[SENSOR READ] \tRead sensor info");
	Kernel.releaseUART();

	// �������� ������ �� 500 ��
	// ���������� � ������������ ����� ����
	Kernel.getScheduler().sleep(500);
}

int main(void)
{
	Kernel.initialize(); // �������������� ���� (��� ���� �������������� ���� ������� � �����������)

	// ������������ ������ ����� ����
	Kernel.addTask(heartbeatTask, TaskPriority::NORMAL);
	Kernel.addTask(sensorReadTask, TaskPriority::HIGH); // ������ ������� ������

	// ��������� ������� ���� ����, ������� ������ ���� �������� ��������� ������������
	Kernel.run();

	return 1; // ���� �� ������� �� �������
}