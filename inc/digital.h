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

#ifndef DIGITAL_H
#define DIGITAL_H

/** \brief Control de entradas y salidas digitales
 **
 **
 **
 ** \addtogroup HAL
 ** \brief Capa de abstraccion de hardware
 ** @{ */

/* === Headers files inclusions ================================================================ */

#include <stdbool.h>
#include <stdint.h>

/* === Cabecera C++ ============================================================================ */

#ifdef __cplusplus
extern "C" {
#endif

/* === Public macros definitions =============================================================== */

/* === Public data type declarations =========================================================== */

//! Puntero al descriptor de las salidas
typedef struct digital_output_s * digital_output_t;

//! Puntero al descriptor de las entradas
typedef struct digital_input_s * digital_input_t;

/* === Public variable declarations ============================================================ */

/* === Public function declarations ============================================================ */

/* SALIDAS */

/**
 * @brief Crea una salida
 * 
 * @param port puerto GPIO de la salida
 * @param pin  terminal del puerto correspondiente
 * @param inverted indica si trabaja en forma inversa
 * @return digital_output_t puntero al descriptor de la salida
 */
digital_output_t DigitalOutputCreate(uint8_t port, uint8_t pin, bool inverted);

/**
 * @brief Activa la salida
 * 
 * @param output puntero al descriptor de la salida
 */
void DigitalOutputActivate(digital_output_t output);

/**
 * @brief Desactiva la salida
 * 
 * @param output puntero al descriptor de la salida
 */
void DigitalOutputDeactivate(digital_output_t output);

/**
 * @brief Cambia el estado de la salida
 * 
 * @param output puntero al descriptor de la salida
 */
void DigitalOutputToggle(digital_output_t output);

/* ENTRADAS */

/**
 * @brief Crea una entrada
 * 
 * @param port puerto GPIO de la entrada
 * @param pin  terminal del puerto correspondiente
 * @param inverted indica si trabaja en forma inversa
 * @return digital_output_t puntero al descriptor de la entrada
 */
digital_input_t DigitalInputCreate(uint8_t port, uint8_t pin, bool inverted);

/**
 * @brief Comprueba el estado de la entrada
 * 
 * @param input puntero al descriptor de la entrada
 * @return true entrada activa
 * @return false entrada inactiva
 */
bool DigitalInputGetState(digital_input_t input);

/**
 * @brief Comprueba si cambio el estado de la entrada
 * 
 * @param input puntero al descriptor de la entrada
 * @return true la entrada cambio
 * @return false la entrada no cambio
 */
bool DigitalInputHasChanged(digital_input_t input);

/**
 * @brief Comprueba si se activo la entrada
 * 
 * @param input puntero al descriptor de la entrada
 * @return true se activo
 * @return false no se activo
 */
bool DigitalInputHasActivated(digital_input_t input);

/**
 * @brief Comprueba si se desactivo la entrada
 * 
 * @param input puntero al descriptor de la entrada
 * @return true se desactivo
 * @return false no se desactivo
 */
bool DigitalInputHasDeactivated(digital_input_t input);

/* === End of documentation ==================================================================== */

#ifdef __cplusplus
}
#endif

/** @} End of module definition for doxygen */

#endif /* DIGITAL_H */
