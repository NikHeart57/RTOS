#include "GPIO.h"

//////////////////////////////////////////////////////////////////////////
//  ¬—œŒÃŒ√¿“≈À‹Õ€≈ ‘”Õ ÷»»
//////////////////////////////////////////////////////////////////////////

static volatile uint8_t* GPIO_GetDDRRegister(GPIO_Port port)
{
	switch(port) {
		case GPIO_PORT_A: return &DDRA;
		case GPIO_PORT_B: return &DDRB;
		case GPIO_PORT_C: return &DDRC;
		case GPIO_PORT_D: return &DDRD;
		default: return &DDRA;
	}
}

static volatile uint8_t* GPIO_GetPORTRegister(GPIO_Port port)
{
	switch(port) {
		case GPIO_PORT_A: return &PORTA;
		case GPIO_PORT_B: return &PORTB;
		case GPIO_PORT_C: return &PORTC;
		case GPIO_PORT_D: return &PORTD;
		default: return &PORTA;
	}
}

static volatile uint8_t* GPIO_GetPINRegister(GPIO_Port port)
{
	switch(port) {
		case GPIO_PORT_A: return &PINA;
		case GPIO_PORT_B: return &PINB;
		case GPIO_PORT_C: return &PINC;
		case GPIO_PORT_D: return &PIND;
		default: return &PINA;
	}
}

//////////////////////////////////////////////////////////////////////////
//  –¿¡Œ“¿ — Œ“ƒ≈À‹Õ€Ã» œ»Õ¿Ã»
//////////////////////////////////////////////////////////////////////////

void GPIO_SetDirection(GPIO_Port port, GPIO_Pin pin, GPIO_Direction direction)
{
	volatile uint8_t* ddr = GPIO_GetDDRRegister(port);
	uint8_t mask = (1 << pin);
	
	if(direction == GPIO_OUTPUT) {
		*ddr |= mask;
		} else {
		*ddr &= ~mask;
	}
}

GPIO_Direction GPIO_GetDirection(GPIO_Port port, GPIO_Pin pin)
{
	volatile uint8_t* ddr = GPIO_GetDDRRegister(port);
	uint8_t mask = (1 << pin);
	
	return (*ddr & mask) ? GPIO_OUTPUT : GPIO_INPUT;
}

void GPIO_WritePin(GPIO_Port port, GPIO_Pin pin, GPIO_State state)
{
	volatile uint8_t* port_reg = GPIO_GetPORTRegister(port);
	uint8_t mask = (1 << pin);
	
	if(state == GPIO_HIGH) {
		*port_reg |= mask;
		} else {
		*port_reg &= ~mask;
	}
}

GPIO_State GPIO_ReadPin(GPIO_Port port, GPIO_Pin pin)
{
	volatile uint8_t* pin_reg = GPIO_GetPINRegister(port);
	uint8_t mask = (1 << pin);
	
	return (*pin_reg & mask) ? GPIO_HIGH : GPIO_LOW;
}

void GPIO_TogglePin(GPIO_Port port, GPIO_Pin pin)
{
	volatile uint8_t* port_reg = GPIO_GetPORTRegister(port);
	uint8_t mask = (1 << pin);
	*port_reg ^= mask;
}

void GPIO_SetPullup(GPIO_Port port, GPIO_Pin pin, GPIO_Pullup pullup)
{
	volatile uint8_t* port_reg = GPIO_GetPORTRegister(port);
	uint8_t mask = (1 << pin);
	
	if(pullup == GPIO_PULLUP_ON) {
		*port_reg |= mask;
		} else {
		*port_reg &= ~mask;
	}
}

GPIO_Pullup GPIO_GetPullup(GPIO_Port port, GPIO_Pin pin)
{
	volatile uint8_t* port_reg = GPIO_GetPORTRegister(port);
	uint8_t mask = (1 << pin);
	
	return (*port_reg & mask) ? GPIO_PULLUP_ON : GPIO_PULLUP_OFF;
}

//////////////////////////////////////////////////////////////////////////
//  –¿¡Œ“¿ — ÷≈À€Ã» œŒ–“¿Ã»
//////////////////////////////////////////////////////////////////////////

void GPIO_SetPortDirection(GPIO_Port port, uint8_t direction_mask)
{
	volatile uint8_t* ddr = GPIO_GetDDRRegister(port);
	*ddr = direction_mask;
}

uint8_t GPIO_GetPortDirection(GPIO_Port port)
{
	volatile uint8_t* ddr = GPIO_GetDDRRegister(port);
	return *ddr;
}

void GPIO_WritePort(GPIO_Port port, uint8_t data)
{
	volatile uint8_t* port_reg = GPIO_GetPORTRegister(port);
	*port_reg = data;
}

uint8_t GPIO_ReadPort(GPIO_Port port)
{
	volatile uint8_t* pin_reg = GPIO_GetPINRegister(port);
	return *pin_reg;
}

void GPIO_SetPortPullups(GPIO_Port port, uint8_t pullup_mask)
{
	volatile uint8_t* port_reg = GPIO_GetPORTRegister(port);
	*port_reg = pullup_mask;
}

uint8_t GPIO_GetPortPullups(GPIO_Port port)
{
	volatile uint8_t* port_reg = GPIO_GetPORTRegister(port);
	return *port_reg;
}

//////////////////////////////////////////////////////////////////////////
//  ¡»“Œ¬€≈ Œœ≈–¿÷»»
//////////////////////////////////////////////////////////////////////////

void GPIO_SetBits(GPIO_Port port, uint8_t mask)
{
	volatile uint8_t* port_reg = GPIO_GetPORTRegister(port);
	*port_reg |= mask;
}

void GPIO_ClearBits(GPIO_Port port, uint8_t mask)
{
	volatile uint8_t* port_reg = GPIO_GetPORTRegister(port);
	*port_reg &= ~mask;
}

void GPIO_ToggleBits(GPIO_Port port, uint8_t mask)
{
	volatile uint8_t* port_reg = GPIO_GetPORTRegister(port);
	*port_reg ^= mask;
}

void GPIO_WriteMasked(GPIO_Port port, uint8_t mask, uint8_t data)
{
	volatile uint8_t* port_reg = GPIO_GetPORTRegister(port);
	*port_reg = (*port_reg & ~mask) | (data & mask);
}

//////////////////////////////////////////////////////////////////////////
//   ŒÕ‘»√”–¿÷»ﬂ œŒ–“¿
//////////////////////////////////////////////////////////////////////////

void GPIO_ConfigurePort(GPIO_Port port, uint8_t direction_mask,
uint8_t output_mask, uint8_t pullup_mask)
{
	volatile uint8_t* ddr = GPIO_GetDDRRegister(port);
	volatile uint8_t* port_reg = GPIO_GetPORTRegister(port);
	
	*ddr = direction_mask;
	*port_reg = (output_mask & direction_mask) | (pullup_mask & ~direction_mask);
}