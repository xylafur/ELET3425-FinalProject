// BumpInt.c
// Runs on MSP432, interrupt version
// Provide low-level functions that interface bump switches the robot.
// Daniel Valvano and Jonathan Valvano
// July 2, 2017

/* This example accompanies the books
   "Embedded Systems: Introduction to the MSP432 Microcontroller",
       ISBN: 978-1512185676, Jonathan Valvano, copyright (c) 2017
   "Embedded Systems: Real-Time Interfacing to the MSP432 Microcontroller",
       ISBN: 978-1514676585, Jonathan Valvano, copyright (c) 2017
   "Embedded Systems: Real-Time Operating Systems for ARM Cortex-M Microcontrollers",
       ISBN: 978-1466468863, , Jonathan Valvano, copyright (c) 2017
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/

Simplified BSD License (FreeBSD License)
Copyright (c) 2017, Jonathan Valvano, All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are
those of the authors and should not be interpreted as representing official
policies, either expressed or implied, of the FreeBSD Project.
*/

// Negative logic bump sensors
// P4.7 Bump5, left side of robot
// P4.6 Bump4
// P4.5 Bump3
// P4.3 Bump2
// P4.2 Bump1
// P4.0 Bump0, right side of robot

#include <stdint.h>
#include "msp.h"

#include "PinMap.h"
#include "PinManip.h"

#define PRIORITY 0x40000000
#define INTERRUPT_NUMBER (0x8 << 3)

void (*bump_task)(void);


void BumpInt_Init(void (*task)(void)){
    MK_PORT_GPIO_PULLUP(BUMP_SENSOR_PORT);

    FALLING_EDGE_TRIGGERED(BUMP_SENSOR_PORT);

    //Clear out the interrupt flag just in case
    BUMP_SENSOR_PORT->IFG = 0;

    ENABLE_INTERRUPT(BUMP_SENSOR_PORT);

    //Set the priority of this interrupt
    //TODO: Put this in a header somewhere maybe?
    NVIC->IP[INTERRUPT_NUMBER] = (NVIC->IP[INTERRUPT_NUMBER]&0xFFFFFF) | PRIORITY;

    //Enable this particular interrupt
    NVIC->ISER[1] = INTERRUPT_NUMBER;

    bump_task = task;
}


// we do not care about critical section/race conditions
// triggered on touch, falling edge
void PORT4_IRQHandler(void){
    BUMP_SENSOR_PORT->IFG = 0;
    bump_task();
}

