/* Copyright 2024 GSKY <gskyGit@gsky.com.tw>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

/* External spi flash */
#define EXTERNAL_FLASH_SPI_SLAVE_SELECT_PIN B14

/* SPI Config for LED Driver */
#define SPI_DRIVER SPIDQ
#define SPI_SCK_PIN A5
#define SPI_MOSI_PIN A7
#define SPI_MISO_PIN A6

/* RGB Matrix */
#define RGB_MATRIX_FRAMEBUFFER_EFFECTS
#define RGB_MATRIX_KEYPRESSES
#define RGB_MATRIX_LED_COUNT 82

#define RGB_MATRIX_MAXIMUM_BRIGHTNESS 160
#define RGB_MATRIX_STARTUP_MODE 13
#define RGB_MATRIX_DEFAULT_SAT 255
#define RGB_MATRIX_VAL_STEP 40
#define RGB_MATRIX_DEFAULT_VAL 160
#define RGB_MATRIX_SPD_STEP 60
#define RGB_MATRIX_DEFAULT_SPD 195
#define RGB_TRIGGER_ON_KEYDOWN

/* WS2812 */
#define WS2812_SPI_DRIVER SPIDM2
#define WS2812_SPI_DIVISOR 16

