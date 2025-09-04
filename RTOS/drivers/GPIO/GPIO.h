/**
 * @note        Комментарий сгенерирован нейросетью deepseek 19.08.25
 *
 * @file        GPIO.h
 * @brief       Драйвер управления портами ввода-вывода для ATmega32
 * @details     Предоставляет полный набор функций для работы с GPIO
 * 
 * @note        Особенности:
 *              - Поддержка всех портов (A, B, C, D)
 *              - Работа с отдельными пинами и целыми портами
 *              - Битовые операции (установка/сброс/переключение)
 *              - Маскированные операции
 * 
 * @author      Николай Куркин, deepseek
 * @date        2025-08-19
 * @version     1.0
 * 
 * @copyright   MIT License
 */

#pragma once
#include <avr/io.h>
#include <stdint.h>


//////////////////////////////////////////////////////////////////////////
//  ПРОВЕРКА ПОДДЕРЖКИ C++11
//////////////////////////////////////////////////////////////////////////

#if __cplusplus < 201103L
#error "C++11 support required! Add -std=c++11 to compiler flags"
#endif


//////////////////////////////////////////////////////////////////////////
//  ПРОВЕРКА ПОДДЕРЖКИ МИКРОКОНТРОЛЛЕРА (ATmega32 ONLY)
//////////////////////////////////////////////////////////////////////////

#ifndef __AVR_ATmega32__
#error "GPIO driver requires ATmega32 MCU! Check your target device."
#endif


//////////////////////////////////////////////////////////////////////////
//  ОПРЕДЕЛЕНИЯ ПОРТОВ И ПИНОВ
//////////////////////////////////////////////////////////////////////////

typedef enum {
    GPIO_PORT_A = 0,
    GPIO_PORT_B,
    GPIO_PORT_C,
    GPIO_PORT_D
} GPIO_Port;

typedef enum {
    GPIO_PIN_0 = 0,
    GPIO_PIN_1,
    GPIO_PIN_2,
    GPIO_PIN_3,
    GPIO_PIN_4,
    GPIO_PIN_5,
    GPIO_PIN_6,
    GPIO_PIN_7
} GPIO_Pin;

typedef enum {
    GPIO_INPUT = 0,
    GPIO_OUTPUT
} GPIO_Direction;

typedef enum {
    GPIO_LOW = 0,
    GPIO_HIGH
} GPIO_State;

typedef enum {
    GPIO_PULLUP_OFF = 0,
    GPIO_PULLUP_ON
} GPIO_Pullup;

//////////////////////////////////////////////////////////////////////////
//  ФУНКЦИИ РАБОТЫ С ОТДЕЛЬНЫМИ ПИНАМИ
//////////////////////////////////////////////////////////////////////////

void GPIO_SetDirection(GPIO_Port port, GPIO_Pin pin, GPIO_Direction direction);
GPIO_Direction GPIO_GetDirection(GPIO_Port port, GPIO_Pin pin);

void GPIO_WritePin(GPIO_Port port, GPIO_Pin pin, GPIO_State state);
GPIO_State GPIO_ReadPin(GPIO_Port port, GPIO_Pin pin);
void GPIO_TogglePin(GPIO_Port port, GPIO_Pin pin);

void GPIO_SetPullup(GPIO_Port port, GPIO_Pin pin, GPIO_Pullup pullup);
GPIO_Pullup GPIO_GetPullup(GPIO_Port port, GPIO_Pin pin);

//////////////////////////////////////////////////////////////////////////
//  ФУНКЦИИ РАБОТЫ С ЦЕЛЫМИ ПОРТАМИ
//////////////////////////////////////////////////////////////////////////

void GPIO_SetPortDirection(GPIO_Port port, uint8_t direction_mask);
uint8_t GPIO_GetPortDirection(GPIO_Port port);

void GPIO_WritePort(GPIO_Port port, uint8_t data);
uint8_t GPIO_ReadPort(GPIO_Port port);

void GPIO_SetPortPullups(GPIO_Port port, uint8_t pullup_mask);
uint8_t GPIO_GetPortPullups(GPIO_Port port);

//////////////////////////////////////////////////////////////////////////
//  БИТОВЫЕ ОПЕРАЦИИ С ПОРТАМИ
//////////////////////////////////////////////////////////////////////////

void GPIO_SetBits(GPIO_Port port, uint8_t mask);
void GPIO_ClearBits(GPIO_Port port, uint8_t mask);
void GPIO_ToggleBits(GPIO_Port port, uint8_t mask);

void GPIO_WriteMasked(GPIO_Port port, uint8_t mask, uint8_t data);

//////////////////////////////////////////////////////////////////////////
//  КОНФИГУРАЦИЯ ПОРТА
//////////////////////////////////////////////////////////////////////////

void GPIO_ConfigurePort(GPIO_Port port, uint8_t direction_mask, uint8_t output_mask, uint8_t pullup_mask);