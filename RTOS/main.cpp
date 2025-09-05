#include "main.hpp"

SystemKernel Kernel; // ���� �������

// ��������� �����
void fastTask(void);
void slowTask(void);
void blinkTask(void);

// ���������� ���������� ��� ��������� ����� (�����������, ��� �����������)
volatile uint32_t fastCounter = 0;
volatile uint32_t slowCounter = 0;
volatile bool ledState = false;
char itoa_buffer[16] = {0};

int main(void)
{
	Kernel.initialize(); // �������������� ���� (UART, ������...)
	
	// ������������ ������ � ������� ������������
	Kernel.addTask(fastTask,   TaskPriority::HIGH);    // ������� ���������
	Kernel.addTask(slowTask,   TaskPriority::NORMAL);  // ������� ���������
	Kernel.addTask(blinkTask,  TaskPriority::LOW);     // ������ ���������
	
	Kernel.run(); // �������� ���������� ������������ (���� ��������� �� ��������)
	
	return 1;
}

//--------------------------------------------------
// ������ 1: ������� ������ (����������� �����)
//--------------------------------------------------
void fastTask(void)
{
	auto& uart = Kernel.getUART();						// ����������� UART
	
	uart.print("\n\rFAST#");							// �������� ���� ������
	uart.print(itoa(fastCounter++, itoa_buffer, 10));	// ����������� �������
		
	Kernel.releaseUART();								// ����������� UART
	
	// ������� ����� �� 50 �� -> ����� ���������� ����� �����
	Kernel.getScheduler().sleep(50);
}

//--------------------------------------------------
// ������ 2: ��������� ������ (����������� �����)
//--------------------------------------------------
void slowTask(void)
{
	auto& uart = Kernel.getUART();						// ����������� UART
	
	uart.print("\n\rslow.");							// �������� ���� ������
	uart.print(itoa(slowCounter++, itoa_buffer, 10));	// ����������� �������
	
	Kernel.releaseUART();								// ����������� UART
	
	// ������� �� 500 �� -> ����� ���������� �����
	Kernel.getScheduler().sleep(500);
}

//--------------------------------------------------
// ������ 3: �������� ��������� (�� ����� C, ��� 0)
//--------------------------------------------------
void blinkTask(void)
{
	// �������� � ������� �������� ����� ������� (��� ������������)
	// �����������, ��� ��������� �� PORTC, PIN0
	GPIO_WritePin(GPIO_PORT_C, GPIO_PIN_0, ledState ? GPIO_HIGH : GPIO_LOW);
	
	// ������ ��������� ��� ���������� ����
	ledState = !ledState;
	
	// ������ � ������� ��������
	Kernel.getScheduler().sleep(1000);
}