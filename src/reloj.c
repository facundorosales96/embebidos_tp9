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

/** \brief Brief description of the file
 **
 ** Full file description
 **
 ** \addtogroup name Module denomination
 ** \brief Brief description of the module
 ** @{ */

/* === Headers files inclusions =============================================================== */

#include "reloj.h"
#include <string.h>

/* === Macros definitions ====================================================================== */
#define SECONDS_UNITS 5
#define SECONDS_TENS 4
#define MINUTES_UNITS 3
#define MINUTES_TENS 2
#define HOURS_UNITS 1
#define HOURS_TENS 0

/* === Private data type declarations ========================================================== */

/* === Private variable declarations =========================================================== */

/* === Private function declarations =========================================================== */

/* === Public variable definitions ============================================================= */

/* === Private variable definitions ============================================================ */

/* === Private function implementation ========================================================= */

/* === Public function implementation ========================================================== */

struct clock_s {
    uint8_t hora_actual[6];
    bool valida;
    uint8_t tics_por_segundo;
    uint8_t tics;
    uint8_t hora_alarma_fijada[6];
    alarm_notification_t EnableAlarm;
    bool activate_alarm;
    uint16_t segundos_pospuestos;
    bool alarma_pospuesta;
};

clock_t ClockCreate(int tics_por_segundo, alarm_notification_t EnableAlarm) {
    static struct clock_s self[1];

    memset(self, 0, sizeof(self));
    self->tics_por_segundo = tics_por_segundo;
    self->tics = tics_por_segundo;
    self->EnableAlarm = EnableAlarm;
    return self;
}

bool ClockGetTime(clock_t reloj, uint8_t * hora, int size) {
    memcpy(hora, reloj->hora_actual, size);
    return reloj->valida;
}

bool ClockSetTime(clock_t reloj, const uint8_t * hora, int size) {
    memcpy(reloj->hora_actual, hora, size);
    reloj->valida = true;
    return true;
}

void Increment(clock_t reloj) {
    reloj->hora_actual[SECONDS_UNITS]++;

    if (reloj->hora_actual[SECONDS_UNITS] >= 10) {
        reloj->hora_actual[SECONDS_UNITS] = 0;
        reloj->hora_actual[SECONDS_TENS]++;

        if (reloj->hora_actual[SECONDS_TENS] >= 6) {
            reloj->hora_actual[SECONDS_TENS] = 0;
            reloj->hora_actual[MINUTES_UNITS]++;

            if (reloj->hora_actual[MINUTES_UNITS] >= 10) {
                reloj->hora_actual[MINUTES_UNITS] = 0;
                reloj->hora_actual[MINUTES_TENS]++;

                if (reloj->hora_actual[MINUTES_TENS] >= 6) {
                    reloj->hora_actual[MINUTES_TENS] = 0;
                    reloj->hora_actual[HOURS_UNITS]++;

                    if (reloj->hora_actual[HOURS_UNITS] >= 10) {
                        reloj->hora_actual[HOURS_UNITS] = 0;
                        reloj->hora_actual[HOURS_TENS]++;

                        if (reloj->hora_actual[HOURS_TENS] >= 2 && reloj->hora_actual[HOURS_UNITS] >= 4) {
                            reloj->hora_actual[HOURS_TENS] = 0;
                            reloj->hora_actual[HOURS_UNITS] = 0;
                        }
                    }
                }
            }
        }
    }
}

void AlarmaCheck(clock_t reloj) {

    if (reloj->activate_alarm) {
        if (reloj->alarma_pospuesta) {
            if (reloj->segundos_pospuestos == 0) {
                reloj->EnableAlarm(true);
                reloj->alarma_pospuesta = 0;
            }

        } else {
            if (memcmp(reloj->hora_actual, reloj->hora_alarma_fijada, sizeof(reloj->hora_actual)) == 0) {
                reloj->EnableAlarm(true);
            }
        }
    }
}

void ActivateAlarm(clock_t reloj, bool status) {
    reloj->activate_alarm = status;
}

bool ClockUpdate(clock_t reloj) {
    reloj->tics--;

    if (reloj->tics == 0) {
        Increment(reloj);
        reloj->tics = reloj->tics_por_segundo;
        reloj->segundos_pospuestos--;
        AlarmaCheck(reloj);
        if (reloj->hora_actual[0] == 2 && reloj->hora_actual[1] == 4) {
            reloj->hora_actual[0] = 0;
            reloj->hora_actual[1] = 0;
        }
    }
    if (reloj->tics < (reloj->tics_por_segundo / 2)) {
        return true;
    } else {
        return false;
    }
}

void AlarmGetTime(clock_t reloj, uint8_t * hora, int size) {
    memcpy(hora, reloj->hora_alarma_fijada, size);
}

void AlarmSetTime(clock_t reloj, const uint8_t * hora, int size) {
    memcpy(reloj->hora_alarma_fijada, hora, size);
}

void ExtendAlarm(clock_t reloj, int minutos) {
    reloj->segundos_pospuestos = minutos * 60;
    reloj->EnableAlarm(false);
    reloj->alarma_pospuesta = 1;
}

void DisableAlarm(clock_t reloj) {
    reloj->EnableAlarm(false);
}

bool AlarmGetState(clock_t reloj) {
    return reloj->activate_alarm;
}

/* === End of documentation ==================================================================== */

/** @} End of module definition for doxygen */
