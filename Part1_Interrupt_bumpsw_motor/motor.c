/*
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

// Sever VCCMD=VREG jumper on Motor Driver and Power Distribution Board and connect VCCMD to 3.3V.
//   This makes P3.7 and P3.6 low power disables for motor drivers.  0 to sleep/stop.
// Sever nSLPL=nSLPR jumper.
//   This separates P3.7 and P3.6 allowing for independent control
// Left motor direction connected to P1.7 (J2.14)
// Left motor PWM connected to P2.7/TA0CCP4 (J4.40)
// Left motor enable connected to P3.7 (J4.31)
// Right motor direction connected to P1.6 (J2.15)
// Right motor PWM connected to P2.6/TA0CCP3 (J4.39)
// Right motor enable connected to P3.6 (J2.11)

// Modified by: Mohd A. Zainol

#include <stdint.h>
#include "msp.h"
#include "SysTick.h"

// *******Lab 12 *******

void Motor_InitSimple(void){
// Initializes the 6 GPIO lines and puts driver to sleep
// Returns right away
// initialize P1.6 and P1.7 and make them outputs
}

void Motor_StopSimple(uint32_t time_ms){
// Stops both motors, puts driver to sleep
// Returns right away
  P1->OUT &= ~0xC0;
  P2->OUT &= ~0xC0;   // off
  SysTick_Wait10ms(time_ms);
}

void Motor_ForwardSimple(uint16_t duty, uint32_t time_ms){
// Drives both motors forward at duty (100 to 9900)
// Runs for time duration (units=10ms), and then stops
// Stop the motors and return if any bumper switch is active
// Returns after time*10ms or if a bumper switch is hit

    uint32_t i;
    uint16_t L;

    L = 1000-duty; // PWM using H and L

    P1->DIR |= 0x00;      //   Motor Forward
    P1->OUT |= 0x00;

    for(i=0; i<time_ms; i++){
      P2->OUT |= 0xC0;   // on
      SysTick_Wait1us(duty);
      P2->OUT &= ~0xC0;  // off
      SysTick_Wait1us(L);
      SysTick_Wait(48000);  // wait 1ms (assumes 48 MHz clock)
    }
}

void Motor_BackwardSimple(uint16_t duty, uint32_t time_ms){
// Drives both motors backward at duty (100 to 9900)
// Runs for time duration (units=10ms), and then stops
// Runs even if any bumper switch is active
// Returns after time*10ms

    uint32_t i;
    uint16_t L;

    L = 1000-duty; // PWM using H and L

    P1->DIR |= 0xC0;      //   Motor Backward
    P1->OUT |= 0xC0;

    for(i=0; i<time_ms; i++){
      P2->OUT |= 0xC0;   // on
      SysTick_Wait1us(duty);
      P2->OUT &= ~0xC0;  // off
      SysTick_Wait1us(L);
      SysTick_Wait(48000);  // wait 1ms (assumes 48 MHz clock)
    }
}

void Motor_LeftSimple(uint16_t duty, uint32_t time_ms){
// Drives just the left motor forward at duty (100 to 9900)
// Right motor is stopped (sleeping)
// Runs for time duration (units=10ms), and then stops
// Stop the motor and return if any bumper switch is active
// Returns after time*10ms or if a bumper switch is hit

    uint32_t i;
    uint16_t L;

    L = 1000-duty; // PWM using H and L

    P1->DIR |= 0x80;      //   Motor Turn Left
    P1->OUT |= 0x80;

    for(i=0; i<time_ms; i++){
      P2->OUT |= 0xC0;   // on
      SysTick_Wait1us(duty);
      P2->OUT &= ~0xC0;  // off
      SysTick_Wait1us(L);
      SysTick_Wait(48000);  // wait 1ms (assumes 48 MHz clock)
    }
}
void Motor_RightSimple(uint16_t duty, uint32_t time_ms){
// Drives just the right motor forward at duty (100 to 9900)
// Left motor is stopped (sleeping)
// Runs for time duration (units=10ms), and then stops
// Stop the motor and return if any bumper switch is active
// Returns after time*10ms or if a bumper switch is hit

    uint32_t i;
    uint16_t L;

    L = 1000-duty; // PWM using H and L

    P1->DIR |= 0x40;      //   Motor Turn Right
    P1->OUT |= 0x40;

    for(i=0; i<time_ms; i++){
      P2->OUT |= 0xC0;   // on
      SysTick_Wait1us(duty);
      P2->OUT &= ~0xC0;  // off
      SysTick_Wait1us(L);
      SysTick_Wait(48000);  // wait 1ms (assumes 48 MHz clock)
    }
}
