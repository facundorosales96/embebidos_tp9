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

/** \brief Control de entradas y salidas digitales
 **
 **
 **
 ** \addtogroup HAL
 ** \brief Capa de abstraccion de hardware
 ** @{ */

/* === Headers files inclusions =============================================================== */

#include "digital.h"
#include "chip.h"
#include "stdbool.h"

/* === Macros definitions ====================================================================== */
#ifndef OUTPUT_INSTANCES
#define OUTPUT_INSTANCES 1
#endif

#ifndef INPUT_INSTANCES
#define INPUT_INSTANCES 6
#endif
/* === Private data type declarations ========================================================== */

//! Estructura para almacenar el descriptor de cada salida digital
struct digital_output_s {
    uint8_t port;   //!< Puerto GPIO de la salida digital.
    uint8_t pin;    //!< Terminal del puerto GPIO de la salida digital.
    bool allocated; //!< Bandera que indica si el descriptor esta en uso
    bool inverted;  //!< Bandera que indica si trabaja de forma inversa
};

//! Estructura para almacenar el descriptor de cada entrada digital
struct digital_input_s {
    uint8_t port;    //!< Puerto GPIO de la entrada digital.
    uint8_t pin;     //!< Terminal del puerto GPIO de la entrada digital.
    bool allocated;  //!< Bandera que indica si el descriptor esta en uso
    bool last_state; //!< Bandera con el ultimo estado de la entrada
    bool inverted;   //!< Bandera que indica si trabaja de forma inversa
};
/* === Private variable declarations =========================================================== */

/* === Private function declarations =========================================================== */

digital_output_t DigitalOutputAllocate(void);
digital_input_t DigitalInputAllocate(void);

/* === Public variable definitions ============================================================= */

/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */

digital_output_t DigitalOutputAllocate(void) {
    digital_output_t output = NULL;

    static struct digital_output_s instances[OUTPUT_INSTANCES] = {0};

    for (int i = 0; i < OUTPUT_INSTANCES; i++) {

        if (!instances[i].allocated) {

            instances[i].allocated = true;
            output = &instances[i];
            break;
        }
    }
    return output;
}

digital_input_t DigitalInputAllocate(void) {
    digital_input_t input = NULL;

    static struct digital_input_s instances[INPUT_INSTANCES] = {0};

    for (int i = 0; i < INPUT_INSTANCES; i++) {

        if (!instances[i].allocated) {

            instances[i].allocated = true;
            input = &instances[i];
            break;
        }
    }
    return input;
}
/* === Public function implementation ========================================================== */

/* SALIDAS */

digital_output_t DigitalOutputCreate(uint8_t port, uint8_t pin, bool inverted) {

    digital_output_t output = DigitalOutputAllocate();

    if (output) {
        output->port = port;
        output->pin = pin;
        output->inverted = inverted;

        Chip_GPIO_SetPinState(LPC_GPIO_PORT, output->port, output->pin, false);
        Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, output->port, output->pin, true);
    }

    return output;
}
void DigitalOutputActivate(digital_output_t output) {
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, output->port, output->pin, output->inverted ^ true);
}
void DigitalOutputDeactivate(digital_output_t output) {
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, output->port, output->pin, output->inverted ^ false);
}
void DigitalOutputToggle(digital_output_t output) {
    Chip_GPIO_SetPinToggle(LPC_GPIO_PORT, output->port, output->pin);
}

/* ENTRADAS */

digital_input_t DigitalInputCreate(uint8_t port, uint8_t pin, bool inverted) {

    digital_input_t input = DigitalInputAllocate();

    if (input) {
        input->port = port;
        input->pin = pin;
        input->inverted = inverted;

        Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, input->port, input->pin, false);
    }

    return input;
}
bool DigitalInputGetState(digital_input_t input) {

    return input->inverted ^ Chip_GPIO_ReadPortBit(LPC_GPIO_PORT, input->port, input->pin);
}
bool DigitalInputHasChanged(digital_input_t input) {

    bool state = DigitalInputGetState(input);
    bool resultado = state != input->last_state;
    input->last_state = state;

    return resultado;
}
bool DigitalInputHasActivated(digital_input_t input) {

    bool state = DigitalInputGetState(input);
    bool resultado = state && !input->last_state;
    input->last_state = state;

    return resultado;
}
bool DigitalInputHasDeactivated(digital_input_t input) {

    bool state = DigitalInputGetState(input);
    bool resultado = !state && input->last_state;
    input->last_state = state;

    return resultado;
}

/* === End of documentation ==================================================================== */

/** @} End of module definition for doxygen */
