/* 
 * File:   PINS_DEF_H
 * Author: Tony Tawfik, Gunnar Turnquist, Peter Salib
 *
 * Created on March 15, 2019, 5:22 PM
 */

#ifndef PINS_DEF_H
#define	PINS_DEF_H

//HC-SR04 Proximity Sensor
#define trig RB6
#define echo RB7

//10K ohm, NTC, B = 3380K
#define temp_sensor RB8 //AN48 ADC3

#define pot1 RB9    //AN49 ADC4
#define pot2 RB10   //AN5

#define IR_I PORTCbits.RC15
#define IR_O LATCbits.LATC14

#define sw1 PORTDbits.RD2
#define sw2 PORTDbits.RD3
#define sw3 PORTDbits.RD4
#define sw4 PORTDbits.RD5

#define rst_ESP RD0
#define rst_OLED RD11

#define led1 LATEbits.LATE0
#define led2 LATEbits.LATE1
#define led3 LATEbits.LATE2
#define led4 LATEbits.LATE3
#define led5 LATEbits.LATE4
#define led6 LATEbits.LATE5
#define led7 LATEbits.LATE6
#define led8 LATEbits.LATE7

#endif