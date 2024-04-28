#pragma once

#define  GPIO_0 0
#define  GPIO_1 1
#define  GPIO_2 2
#define  GPIO_3 3
#define  GPIO_4 4
#define  GPIO_5 5
#define  GPIO_6 6
#define  GPIO_7 7
#define  GPIO_8 8
#define  GPIO_9 9
#define  GPIO_10 10
#define  GPIO_11 11
#define  GPIO_12 12
#define  GPIO_13 13
#define  GPIO_14 14
#define  GPIO_15 15
#define  GPIO_16 16
#define  GPIO_17 17
#define  GPIO_18 18
#define  GPIO_19 19
#define  GPIO_20 20
#define  GPIO_21 21
#define  GPIO_22 22
#define  GPIO_23 23
#define  GPIO_24 24
#define  GPIO_25 25
#define  GPIO_26 26
#define  GPIO_27 27
#define  GPIO_28 28
#define  GPIO_29 29
#define  GPIO_30 30
#define  GPIO_31 31
#define  GPIO_32 32
#define  GPIO_33 33
#define  GPIO_34 34
#define  GPIO_35 35
#define  GPIO_36 36 // ADC1_0
#define  GPIO_37 37
#define  GPIO_38 38
#define  GPIO_39 39

#define PIN_D0            0         // Pin D0  mapped to pin GPIO0/BOOT/ADC11/TOUCH1 of ESP32
#define PIN_D1            1         // Pin D1  mapped to pin GPIO1/TX0 of ESP32
#define PIN_D2            2         // Pin D2  mapped to pin GPIO2/ADC12/TOUCH2 of ESP32
#define PIN_D3            3         // Pin D3  mapped to pin GPIO3/RX0 of ESP32
#define PIN_D4            4         // Pin D4  mapped to pin GPIO4/ADC10/TOUCH0 of ESP32
#define PIN_D5            5         // Pin D5  mapped to pin GPIO5/SPISS/VSPI_SS of ESP32
#define PIN_D6            6         // Pin D6  mapped to pin GPIO6 of ESP32
#define PIN_D7            7         // Pin D7  mapped to pin GPIO7 of ESP32
#define PIN_D8            8         // Pin D8  mapped to pin GPIO8 of ESP32
#define PIN_D9            9         // Pin D9  mapped to pin GPIO9 of ESP32
#define PIN_D10           10        // Pin D10 mapped to pin GPIO10 of ESP32
#define PIN_D11           11        // Pin D11 mapped to pin GPIO11 of ESP32
#define PIN_D12           12        // Pin D12 mapped to pin GPIO12/HSPI_MISO/ADC15/TOUCH5/TDI of ESP32
#define PIN_D13           13        // Pin D13 mapped to pin GPIO13/HSPI_MOSI/ADC14/TOUCH4/TCK of ESP32
#define PIN_D14           14        // Pin D14 mapped to pin GPIO14/HSPI_SCK/ADC16/TOUCH6/TMS of ESP32
#define PIN_D15           15        // Pin D15 mapped to pin GPIO15/HSPI_SS/ADC13/TOUCH3/TDO of ESP32
#define PIN_D16           16        // Pin D16 mapped to pin GPIO16/TX2 of ESP32
#define PIN_D17           17        // Pin D17 mapped to pin GPIO17/RX2 of ESP32     
#define PIN_D18           18        // Pin D18 mapped to pin GPIO18/VSPI_SCK of ESP32
#define PIN_D19           19        // Pin D19 mapped to pin GPIO19/VSPI_MISO of ESP32

#define PIN_D21           21        // Pin D21 mapped to pin GPIO21/SDA of ESP32
#define PIN_D22           22        // Pin D22 mapped to pin GPIO22/SCL of ESP32
#define PIN_D23           23        // Pin D23 mapped to pin GPIO23/VSPI_MOSI of ESP32
#define PIN_D25           25        // Pin D25 mapped to pin GPIO25/ADC18/DAC1 of ESP32
#define PIN_D26           26        // Pin D26 mapped to pin GPIO26/ADC19/DAC2 of ESP32
#define PIN_D27           27        // Pin D27 mapped to pin GPIO27/ADC17/TOUCH7 of ESP32 






#ifdef ESP32
#include "esp32-hal.h"
#define ADC_pin    GPIO_36//ADC1_0    
#define KEY_1_pin  GPIO_1 // 8
#define KEY_2_pin  GPIO_2 // 10
#define KEY_3_pin  GPIO_3 // 12
#define BEEP_pin   GPIO_4 // A2
#define GEN0_pin   GPIO_5 // A0
#define GEN1_pin   GPIO_6 // 9
#define RX_LED_pin GPIO_7  // 2
#define TX_LED_pin GPIO_8//  3
#define LED_pin    GPIO_9 //  13_???
#define CLC_pin    GPIO_10
#endif


#ifdef ESP8266

#define ADC_pin    GPIO_0//ADC1_0    ????
#define KEY_1_pin  GPIO_1 // 8
#define KEY_2_pin  GPIO_2 // 10
#define KEY_3_pin  GPIO_3 // 12
#define BEEP_pin   GPIO_4 // A2
#define GEN0_pin   GPIO_5 // A0
#define GEN1_pin   GPIO_6 // 9
#define RX_LED_pin GPIO_7  // 2
#define TX_LED_pin GPIO_8//  3
#define LED_pin    GPIO_9 //  13_???
#define CLC_pin    GPIO_10
#endif


#ifdef __AVR__
#define  ver = 8.0
//#define  ver_10 
#if ver == 10.2
#define OSC_pin A7
#define KEY_1_pin  12 // 8
#define KEY_2_pin  A1 // 10
#define KEY_3_pin  10 // 12
#define BEEP_pin   A2 // A2
#define GEN0_pin   10 // A0
#define GEN1_pin   9 // 9
#define RX_LED_pin 0  // 2
#define TX_LED_pin 1//  3
#define LED_pin    13 //  13_???
#define CLC_pin    2
 #endif

#if ver == 8.0
#define OSC_pin A7
#define KEY_1_pin  8 // 8
#define KEY_2_pin  10 // 10
#define KEY_3_pin  12 // 12
#define BEEP_pin   A2 // A2
#define GEN0_pin   A0 // A0
#define GEN1_pin   9 // 9
#define RX_LED_pin 2  // 2
#define TX_LED_pin 3//  3
#define LED_pin    13 //  13_???
#define CLC_pin    2
 #endif
#endif






