/**
 * ������ ��������� � ��� ��� ��� ������ ������:
 *
 * "�� ���� � ����� �������. ����������� ��������������� � ��� �������� � ������ ��������� ���.
 * �� ��� �����: �� �������� ����� �� �������������.
 * ������� � ������ ���� ������������� �������. ��� ��� �������� working RTOS!
 * ����� ��� �� ����������� �� ��������. ������ ��������� �����, ������� ����� ������������� �� ������� � �����.
 * ����� �������� ������������� (��������, ����������� ������) ������� � ������������� �����."
 *
 */
#include <stdlib.h>
#include "main.hpp"

SystemKernel kernel; // ���� �������

// ��������� �����
void fastTask(void);
void slowTask(void);
void blinkTask(void);

void stressTestTask(void);
void resourceRaceTask(void);
void priorityInheritanceTest(void);

// ���������� ���������� ��� ��������� ����� (�����������, ��� �����������)
volatile uint32_t fastCounter = 0;
volatile uint32_t slowCounter = 0;
volatile bool ledState = false;
volatile uint32_t stressSuccess = 0;
volatile uint32_t stressFailures = 0;
volatile uint32_t stressTotalAttempts = 0;
char itoa_buffer[16] = {0};

int main(void)
{
    kernel.initialize(); // �������������� ���� (UART, ������...)
    
    // ������������ ������ � ������� ������������
    kernel.addTask(fastTask,   TaskPriority::HIGH);    // ������� ���������
    kernel.addTask(slowTask,   TaskPriority::NORMAL);  // ������� ���������
    kernel.addTask(blinkTask,  TaskPriority::LOW);     // ������ ���������
    
	// �������� ������ (���������������� ������)
    // kernel.addTask(stressTestTask, TaskPriority::HIGH);
    kernel.addTask(resourceRaceTask, TaskPriority::NORMAL);
    // kernel.addTask(priorityInheritanceTest, TaskPriority::LOW);
	 
    kernel.run(); // �������� ���������� ������������ (���� ��������� �� ��������)
    
    return 1;
}

//--------------------------------------------------
// ������ 1: ������� ������ (����������� �����)
//--------------------------------------------------
void fastTask(void)
{
	auto& uart = kernel.getUART();							// ����������� UART
	auto& uart_mutex = kernel.getResourceManager().getUART_Mutex();
	
	if(uart_mutex.lock(10))
	{
		uart.print("\n\rF");								// �������� ���� ������
		uart.print(itoa(fastCounter++, itoa_buffer, 10));	// ����������� �������
		uart_mutex.unlock();								// ����������� UART
	}
	else
	{
		uart.print("\n\rfastTask() - Failure To Lock UART ");
	}
    
    // ������� ����� �� 50 �� -> ����� ���������� ����� �����
    // ���������� ����� API ����� TaskManager
    kernel.getTaskManager().sleep(50);
}

//--------------------------------------------------
// ������ 2: ��������� ������ (����������� �����)
//--------------------------------------------------
void slowTask(void)
{
    auto& uart = kernel.getUART();							// ����������� UART
    auto& uart_mutex = kernel.getResourceManager().getUART_Mutex();
    
	if(uart_mutex.lock(10))
	{
		uart.print("\n\rs");								// �������� ���� ������
		uart.print(itoa(slowCounter++, itoa_buffer, 10));	// ����������� �������
		uart_mutex.unlock();								// ����������� UART
	}
	else
	{
		uart.print("\n\rslowTask() - Failure To Lock UART ");
	}

    
    // ������� �� 500 �� -> ����� ���������� �����
    kernel.getTaskManager().sleep(500);
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
    kernel.getTaskManager().sleep(500);
}

//--------------------------------------------------
// ������ 4: ����������� ���� ��������
//--------------------------------------------------
void stressTestTask(void)
{
	auto& uart = kernel.getUART();
	auto& uart_mutex = kernel.getResourceManager().getUART_Mutex();
	
	// ��������� ����� �������� (10-100 ��)
	uint32_t randomWait = 10 + (stressTotalAttempts % 90);
	
	// ����� �������� ������� - 1-5 �� (������������� �������)
	uint32_t shortTimeout = 1 + (stressTotalAttempts % 5);
	
	if (uart_mutex.lock(shortTimeout)) {
		// �����! �������� � ��������
		stressSuccess++;
		
		// �������� ������ ������������ ������ � ��������
		uart.print("\n\rSTRESS[");
		uart.print(itoa(stressSuccess, itoa_buffer, 10));
		uart.print("]");
		
		// �������� ������ ����������� ������
		kernel.getTaskManager().sleep(randomWait);
		
		uart_mutex.unlock();
		} 
		else 
		{
		// ������ - ������� �����
		stressFailures++;
		
		// ������� ����� � �������
		uart.print("\n\rFAIL[");
		uart.print(itoa(stressFailures, itoa_buffer, 10));
		uart.print("]");
	}
	
	stressTotalAttempts++;
	
	// ������� ���������� ��� �������� ��������
	kernel.getTaskManager().sleep(5);
}

//--------------------------------------------------
// ������ 5: ������������ ������ (����� �� ��������)
//--------------------------------------------------
void resourceRaceTask(void)
{
	auto& uart = kernel.getUART();
	auto& uart_mutex = kernel.getResourceManager().getUART_Mutex();
	
	static uint8_t taskId = 0;
	static bool initialized = false;
	
	if (!initialized) {
		taskId = stressTotalAttempts % 5; // ID ������ 0-4
		initialized = true;
	}
	
	// ������� ��������� ������� � ������� ����������
	uint32_t timeout = 5 + (taskId * 3); // ������ �������� ��� ������ ������
	
	if (uart_mutex.lock(timeout)) 
	{
		// ����������� ������ - ������ � ����������� ��������
		uart.print("\n\rT");
		uart.print(itoa(taskId, itoa_buffer, 10));
		uart.print("[IN]");
		
		// ���������� �������� � ����������� ������
		for (volatile int i = 0; i < 1000; i++) 
		{
			// �������� ����������
		}
		
		uart.print("[OUT]");
		uart_mutex.unlock();
		}
		else 
		{
		// �� ������� ��������� - �����������!
		uart.print("\n\rT");
		uart.print(itoa(taskId, itoa_buffer, 10));
		uart.print("[BLOCKED]");
	}
	
	kernel.getTaskManager().sleep(20 + taskId * 10);
}

//--------------------------------------------------
// ������ 6: ���� ������������ �����������
//--------------------------------------------------
void priorityInheritanceTest(void)
{
	auto& uart = kernel.getUART();
	auto& uart_mutex = kernel.getResourceManager().getUART_Mutex();
	
	// ����������������� ������ ����������� ������� �������
	if (uart_mutex.lock(0)) {
		uart.print("\n\rLOW_PRIO_LOCKED");
		
		// ������ ������ � ����������� ������
		kernel.getTaskManager().sleep(200); // 200�� - ����� �����!
		
		uart.print("\n\rLOW_PRIO_RELEASED");
		uart_mutex.unlock();
	}
	
	kernel.getTaskManager().sleep(1000);
}