/************************************************************************************************
Copyright (c) 2023, Rosales Facundo Ezequiel <facundoerosales@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial
portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

SPDX-License-Identifier: MIT
*************************************************************************************************/

#ifndef BSPCIAA_H
#define BSPCIAA_H

/** \brief Board Hardware Support (BSP)
 **
 ** Proporciona la configuracion de entradas y salidas de la placa EDUCIAA-NXP
 **
 ** \addtogroup bsp BSP
 ** \brief HAL
 ** @{ */

/* === Headers files inclusions ================================================================ */

#include "digital.h"
#include "display.h"

/* === Cabecera C++ ============================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =============================================================== */

/* === Public data type declarations =========================================================== */

/**
 * @brief Descriptores de la placa
 *  tipo de dato struct que almacena punteros a los descriptores de entradas y salidas
 */
typedef struct board_s {
    digital_output_t buzzer;   //!< Puntero a descriptor de la salida buzzer
    digital_input_t set_time;  //!< Puntero a descriptor de la entrada set_time
    digital_input_t set_alarm; //!< Puntero a descriptor de la entrada set_alarm
    digital_input_t decrement; //!< Puntero a descriptor de la entrada decrement
    digital_input_t increment; //!< Puntero a descriptor de la entrada increment
    digital_input_t accept;    //!< Puntero a descriptor de la entrada accept
    digital_input_t cancel;    //!< Puntero a descriptor de la entrada cancel
    display_t display;         //!< Puntero a descirptor de la pantalla
} const * board_t;

/* === Public variable declarations ============================================================ */

/* === Public function declarations ============================================================ */
/**
 * @brief Crea descriptor de la Placa
 *
 * @return board_t
 */
board_t BoardCreate(void);
void SisTick_Init(uint16_t ticks);

/* === End of documentation ==================================================================== */

#ifdef __cplusplus
}
#endif

/** @} End of module definition for doxygen */

#endif /* BSPCIAA_H */
