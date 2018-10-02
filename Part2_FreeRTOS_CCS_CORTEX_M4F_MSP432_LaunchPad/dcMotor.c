/*
  This code accompanies the books
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

// Author:      Mohd A. Zainol
// Date:        1 Oct 2018
// Chip:        MSP432P401R LaunchPad Development Kit (MSP-EXP432P401R) for TI-RSLK
// File:        dcMotor.c
// Function:    Provide functions that initialise ports
//              and set motor to move the robot.Runs on MSP432
// Others:
// Sever VCCMD=VREG jumper on Motor Driver and Power Distribution Board,
// and connect VCCMD to 3.3V.
//   This makes P3.7 and P3.6 low power disables for motor drivers. 0 to sleep/stop.
// Sever nSLPL=nSLPR jumper.
//   This separates P3.7 and P3.6 allowing for independent control
//
// Left motor direction connected to P1.7 (J2.14)
// Left motor PWM connected to P2.7/TA0CCP4 (J4.40)
// Left motor enable connected to P3.7 (J4.31)
// Right motor direction connected to P1.6 (J2.15)
// Right motor PWM connected to P2.6/TA0CCP3 (J4.39)
// Right motor enable connected to P3.6 (J2.11)

#include <stdint.h>
#include "msp.h"
#include "SysTick.h"
#include "dcMotor.h"
#include "bumpSwitch.h"
#include "songFile.h"

/* Kernel includes. */
#include "FreeRTOS.h"

void Port2_Init2(void){
  P2->SEL0 &= ~0xC7;
  P2->SEL1 &= ~0xC7;    // 1) configure P2.2 P2.1 P2.0, and P2.6 P2.7 as GPIO
  P2->DIR |= 0x07;      // 2) make P2.2-P2.0 out
  P2->DS |= 0x07;       // 3) activate increased drive strength
  P2->OUT &= ~0x07;     //    all LEDs off
  P2->DIR |= 0xC0;      //    Motor
}

void dcMotor_Init(void){
// Initializes the 6 GPIO lines and puts driver to sleep
// Returns right away
// initialize P1.6 and P1.7 and make them outputs

}

void dcMotor_Stop(uint32_t delay){
// Stops both motors

  P1->OUT &= ~0xC0;
  P2->OUT &= ~0xC0;
  vTaskDelay( delay );

}

void dcMotor_Forward(uint16_t duty, uint32_t period){
// Drives both motors forward

    uint32_t i;

    // Motor Forward
    P1->DIR |= 0x00;
    P1->OUT |= 0x00;

    for(i=0; i<period; i++){
      P2->OUT |= 0xC0;   // on
      vTaskDelay( 1 );
      P2->OUT &= ~0xC0;  // off
      vTaskDelay( 1 );
    }
}

void dcMotor_Backward(uint16_t duty, uint32_t period){
// Drives both motors backward

    uint32_t i;

    // Motor Backward
    P1->DIR |= 0xC0;
    P1->OUT |= 0xC0;

    for(i=0; i<period; i++){
      P2->OUT |= 0xC0;   // on
      vTaskDelay( 1 );
      P2->OUT &= ~0xC0;  // off
      vTaskDelay( 1 );
    }
}

void dcMotor_Left(uint16_t duty, uint32_t period){
// Drives the left motor forward and right motor backward

    uint32_t i;

    // Motor Turn Left
    P1->DIR |= 0x80;
    P1->OUT |= 0x80;

    for(i=0; i<period; i++){
      P2->OUT |= 0xC0;   // on
      vTaskDelay( 1 );
      P2->OUT &= ~0xC0;  // off
      vTaskDelay( 1 );
    }
}

void dcMotor_Right(uint16_t duty, uint32_t period){
// Drives the right motor forward and left motor backward

    uint32_t i;

    // Motor Turn Right
    P1->DIR |= 0x40;
    P1->OUT |= 0x40;

    for(i=0; i<period; i++){
      P2->OUT |= 0xC0;   // on
      vTaskDelay( 1 );
      P2->OUT &= ~0xC0;  // off
      vTaskDelay( 1 );
    }
}

void dcMotor_response(unsigned char bumpSwitch_status){
// the DC Motors will response based on the bump switches

    switch(bumpSwitch_status){
      case 0xEC: // Bump switch 1
          dcMotor_Backward(500, 50);
          dcMotor_Stop(1);
          dcMotor_Left(500, 5);
          dcMotor_Stop(1);
        break;
      case 0xE9: // Bump switch 2
          dcMotor_Backward(500, 50);
          dcMotor_Stop(1);
          dcMotor_Left(500, 10);
          dcMotor_Stop(1);
        break;
      case 0xE5: // Bump switch 3
          dcMotor_Backward(500, 50);
          dcMotor_Stop(1);
          dcMotor_Left(500, 15);
          dcMotor_Stop(1);
        break;
      case 0xCD: // Bump switch 4
          dcMotor_Backward(500, 50);
          dcMotor_Stop(1);
          dcMotor_Right(500, 15);
          dcMotor_Stop(1);
        break;
      case 0xAD: // Bump switch 5
          dcMotor_Backward(500, 50);
          dcMotor_Stop(1);
          dcMotor_Right(500, 10);
          dcMotor_Stop(1);
        break;
      case 0x6D: // Bump switch 6
          dcMotor_Backward(500, 50);
          dcMotor_Stop(1);
          dcMotor_Right(500, 5);
          dcMotor_Stop(1);
        break;
      case 0xED: // neither switch pressed
          dcMotor_Forward(40, 5);
        break;
      default: // when any two switches pressed
          dcMotor_Backward(500, 50);
          dcMotor_Stop(4);
          dcMotor_Right(500, 15);
          dcMotor_Stop(4);
    }
}
