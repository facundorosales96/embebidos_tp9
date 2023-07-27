/* Copyright 2023, Laboratorio de Microprocesadores
 * Facultad de Ciencias Exactas y Tecnología
 * Universidad Nacional de Tucuman
 * http://www.microprocesadores.unt.edu.ar/
 * Copyright 2023, Rosales Facundo Ezequiel <facundoerosales@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/** \brief Implementacion de HAL y BSP
 **
 ** \addtogroup main MAIN
 ** \brief Gestion de entradas y salidas digitales
 ** @{ */

/* === Headers files inclusions =============================================================== */

#include "FreeRTOS.h"
#include "bspciaa.h"
#include "chip.h"
#include "reloj.h"
#include "task.h"
#include <digital.h>
#include <stdbool.h>
#include <stddef.h>

/* === Macros definitions ====================================================================== */

// Tamaño de pila para tareas
#define STACK_SYSTICK 256
#define STACK_DISPLAY 256
#define STACK_KEYS 512

// Prioridades de las tareas
#define PRIORIDAD_SYSTICK (tskIDLE_PRIORITY + 1)
#define PRIORIDAD_DISPLAY (tskIDLE_PRIORITY)
#define PRIORIDAD_KEYS (tskIDLE_PRIORITY + 2)

/* === Private data type declarations ========================================================== */
typedef enum {
    SIN_CONFIGURAR,
    MOSTRANDO_HORA,
    AJUSTANDO_MINUTOS_ACTUAL,
    AJUSTANDO_HORAS_ACTUAL,
    AJUSTANDO_MINUTOS_ALARMA,
    AJUSTANDO_HORAS_ALARMA,
} modo_t;

/* === Private variable declarations =========================================================== */

/* === Private function declarations =========================================================== */

void ActivarAlarma(bool reloj);

static void TaskSysTick(void * pvParameters);
static void TaskDisplayRefresh(void * pvParameters);
static void TaskKeys(void * pvParameters);

/* === Public variable definitions ============================================================= */

static board_t board;
static clock_t reloj;

static modo_t modo;
static bool sonar_alarma = false;
static uint32_t tiempo_pulsacion = 0;
static uint32_t timeout = 0;

/* === Private variable definitions ============================================================ */

static const uint8_t LIMITE_MINUTOS[] = {5, 9};
static const uint8_t LIMITE_HORAS[] = {2, 3};

/* === Private function implementation ========================================================= */
void SonarAlarma(bool reloj) {
    sonar_alarma = reloj;

    if (reloj) {

        DigitalOutputActivate(board->buzzer);

    } else {

        DigitalOutputDeactivate(board->buzzer);
    }
}

void CambiarModo(modo_t valor) {
    modo = valor;
    switch (modo) {
    case SIN_CONFIGURAR:
        DisplayFlashDigits(board->display, 0, 3, 200);
        break;
    case MOSTRANDO_HORA:
        DisplayFlashDigits(board->display, 0, 0, 0);
        break;
    case AJUSTANDO_MINUTOS_ACTUAL:
        DisplayFlashDigits(board->display, 2, 3, 200);
        break;
    case AJUSTANDO_HORAS_ACTUAL:
        DisplayFlashDigits(board->display, 0, 1, 200);
        break;
    case AJUSTANDO_MINUTOS_ALARMA:
        DisplayFlashDigits(board->display, 2, 3, 200);
        DisplayToggleDot(board->display, 0);
        DisplayToggleDot(board->display, 1);
        DisplayToggleDot(board->display, 2);
        DisplayToggleDot(board->display, 3);
        break;
    case AJUSTANDO_HORAS_ALARMA:
        DisplayFlashDigits(board->display, 0, 1, 200);
        DisplayToggleDot(board->display, 0);
        DisplayToggleDot(board->display, 1);
        DisplayToggleDot(board->display, 2);
        DisplayToggleDot(board->display, 3);
        break;
    default:
        break;
    }
}

void IncrementarBCD(uint8_t numero[2], const uint8_t limite[2]) {
    if (numero[0] == limite[0] && numero[1] == limite[1]) {
        numero[0] = 0;
        numero[1] = 0;
    } else {
        numero[1]++;
        if (numero[1] > 9) {
            numero[1] = 0;
            numero[0]++;
            if (numero[0] > limite[0]) {
                numero[0] = 0;
            }
        }
    }
}

void DecrementarBCD(uint8_t numero[2], const uint8_t limite[2]) {
    if (numero[0] == 0 && numero[1] == 0) {
        numero[0] = limite[0];
        numero[1] = limite[1];
    } else {
        if (numero[1] == 0) {
            numero[1] = 9;
            if (numero[0] == 0) {
                numero[0] = limite[0];
            } else {
                numero[0]--;
            }
        } else {
            numero[1]--;
        }
    }
}

static void TaskSysTick(void * pvParameters) {
    TickType_t last_value = xTaskGetTickCount();
    bool current_value;
    uint8_t hora[6];

    while (true) {
        current_value = ClockUpdate(reloj);

        if (modo <= MOSTRANDO_HORA) {
            ClockGetTime(reloj, hora, sizeof(hora));
            DisplayWriteBCD(board->display, hora, sizeof(hora));
            if (DigitalInputGetState(board->set_time) || DigitalInputGetState(board->set_alarm)) {
                tiempo_pulsacion++;
            } else {
                tiempo_pulsacion = 0;
            }
            if (current_value) {
                DisplayToggleDot(board->display, 1);
            }
            if (AlarmGetState(reloj)) {
                DisplayToggleDot(board->display, 3);
            }
        }
        timeout++;
        if ((timeout > 30000) && (modo > MOSTRANDO_HORA)) {
            if (ClockGetTime(reloj, hora, sizeof(hora))) {
                CambiarModo(MOSTRANDO_HORA);
            } else {
                CambiarModo(SIN_CONFIGURAR);
            }
            timeout = 0;
        }
        vTaskDelayUntil(&last_value, pdMS_TO_TICKS(1));
    }
}

static void TaskDisplayRefresh(void * pvParameters) {
    while (true) {
        DisplayRefresh(board->display);
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

static void TaskKeys(void * pvParameters) {
    uint8_t entrada[4];
    while (true) {
        if (DigitalInputHasActivated(board->accept)) {
            if (modo == MOSTRANDO_HORA) {
                ActivateAlarm(reloj, true);

                if (sonar_alarma) {
                    ExtendAlarm(reloj, 5);
                }
            }
            if (modo == AJUSTANDO_MINUTOS_ACTUAL) {
                CambiarModo(AJUSTANDO_HORAS_ACTUAL);
            } else if (modo == AJUSTANDO_HORAS_ACTUAL) {
                ClockSetTime(reloj, entrada, sizeof(entrada));
                CambiarModo(MOSTRANDO_HORA);
            }
            if (modo == AJUSTANDO_MINUTOS_ALARMA) {
                CambiarModo(AJUSTANDO_HORAS_ALARMA);
            } else if (modo == AJUSTANDO_HORAS_ALARMA) {
                AlarmSetTime(reloj, entrada, sizeof(entrada));
                CambiarModo(MOSTRANDO_HORA);
            }
            timeout = 0;
        }

        if (DigitalInputHasActivated(board->cancel)) {
            if (modo == MOSTRANDO_HORA) {

                if (sonar_alarma) {
                    DisableAlarm(reloj);
                } else {
                    ActivateAlarm(reloj, false);
                }
            }
            if (ClockGetTime(reloj, entrada, sizeof(entrada))) {
                CambiarModo(MOSTRANDO_HORA);
            } else {
                CambiarModo(SIN_CONFIGURAR);
            };
        }

        if (DigitalInputGetState(board->set_time)) {
            if ((tiempo_pulsacion > 3000) && (modo <= MOSTRANDO_HORA)) {
                CambiarModo(AJUSTANDO_MINUTOS_ACTUAL);
                ClockGetTime(reloj, entrada, sizeof(entrada));
                DisplayWriteBCD(board->display, entrada, sizeof(entrada));
                tiempo_pulsacion = 0;
            }
            timeout = 0;
        }

        if (DigitalInputGetState(board->set_alarm)) {
            if ((tiempo_pulsacion > 3000) && (modo <= MOSTRANDO_HORA)) {
                CambiarModo(AJUSTANDO_MINUTOS_ALARMA);
                AlarmGetTime(reloj, entrada, sizeof(entrada));
                DisplayWriteBCD(board->display, entrada, sizeof(entrada));
            }
            timeout = 0;
        }

        if (DigitalInputHasActivated(board->decrement)) {
            if (modo == AJUSTANDO_MINUTOS_ACTUAL || modo == AJUSTANDO_MINUTOS_ALARMA) {
                DecrementarBCD(&entrada[2], LIMITE_MINUTOS);
            } else if (modo == AJUSTANDO_HORAS_ACTUAL || modo == AJUSTANDO_HORAS_ALARMA) {
                DecrementarBCD(entrada, LIMITE_HORAS);
            }
            DisplayWriteBCD(board->display, entrada, sizeof(entrada));
            timeout = 0;
        }

        if (DigitalInputHasActivated(board->increment)) {
            if (modo == AJUSTANDO_MINUTOS_ACTUAL || modo == AJUSTANDO_MINUTOS_ALARMA) {
                IncrementarBCD(&entrada[2], LIMITE_MINUTOS);
            } else if (modo == AJUSTANDO_HORAS_ACTUAL || modo == AJUSTANDO_HORAS_ALARMA) {
                IncrementarBCD(entrada, LIMITE_HORAS);
            }
            DisplayWriteBCD(board->display, entrada, sizeof(entrada));
            timeout = 0;
        }
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

/* === Public function implementation ========================================================= */

int main(void) {

    reloj = ClockCreate(1000, SonarAlarma);

    board = BoardCreate();

    modo = SIN_CONFIGURAR;

    SisTick_Init(1000);

    CambiarModo(SIN_CONFIGURAR);

    xTaskCreate(TaskSysTick, "TareaSysTick", STACK_SYSTICK, NULL, PRIORIDAD_SYSTICK, NULL);

    xTaskCreate(TaskDisplayRefresh, "TareaRefrescoDisplay", STACK_DISPLAY, NULL, PRIORIDAD_DISPLAY, NULL);

    xTaskCreate(TaskKeys, "TareaTeclasPrincipal", STACK_KEYS, NULL, PRIORIDAD_KEYS, NULL);

    vTaskStartScheduler();

    while (true) {
    }
}

/* === End of documentation ==================================================================== */

/** @} End of module definition for doxygen */
