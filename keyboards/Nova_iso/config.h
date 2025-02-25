// Copyright 2024 Su (@isuua)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#ifdef WIRELESS_ENABLE
#    define LPWR_TIMEOUT RGB_MATRIX_TIMEOUT
#endif

#define USB_POWER_EN_PIN C13
#define LED_POWER_EN_PIN B13
#define MM_2G4_DEF_PIN C14
#define MM_BT_DEF_PIN C15
#define USB_CABLE_IN_PIN B9
#define CHRG_PIN C7

#define RGB_MATRIX_BAT_INDEX_MAP { 66, 65, 64,  63,  62,  61,  60,  59,  58,  57 }

/* User */
#define MATRIX_IO_DELAY 2

/* UART */
#define UART_TX_PIN C10
#define UART_RX_PIN C11
#define UART_DRIVER SD3  

/* Encoder */
#define ENCODER_MAP_KEY_DELAY 1

/* SPI */
#define SPI_DRIVER SPIDQ
#define SPI_SCK_PIN A5
#define SPI_MOSI_PIN A7
#define SPI_MISO_PIN A6

/* Flash */
#define EXTERNAL_FLASH_SPI_SLAVE_SELECT_PIN B14
#define WEAR_LEVELING_LOGICAL_SIZE (WEAR_LEVELING_BACKING_SIZE / 2)

/* RGB Matrix */
#define RGB_MATRIX_FRAMEBUFFER_EFFECTS
#define RGB_MATRIX_KEYPRESSES

/* WS2812 */
#define WS2812_SPI_DRIVER SPIDM2
#define WS2812_SPI_DIVISOR 32

/* It is not mandatory to configure; adjust according to the datasheet of WS2812 LEDs. */
#define WS2812_TIMING 1500
#define WS2812_T1H 1200
#define WS2812_T0H 400

#define IM_MM_RGB_BLINK_INDEX_BT1 66
#define IM_MM_RGB_BLINK_INDEX_BT2 65
#define IM_MM_RGB_BLINK_INDEX_BT3 64
#define IM_MM_RGB_BLINK_INDEX_2G4 63
#define IM_MM_RGB_BLINK_INDEX_USB 62

// #define MM_BT1_NAME "Terra Nova1 BT1"
// #define MM_BT2_NAME "Terra Nova1 BT2"
// #define MM_BT3_NAME "Terra Nova1 BT3"
// #define MM_DONGLE_PRODUCT "Terra Nova1 2.4G"