/* 
 * File:   main.cpp
 * Author: Tony Tawfik, Gunnar Turnquist, Peter Salib
 *
 * Created on March 12, 2019, 10:26 AM
 */

#include <cstdlib>
#include <xc.h>
#include <sys/attribs.h>
#include <proc/p32mz1024eff064.h>
#include "PIC_CONFIG.h"
#include "PINS_DEF.h"
#include "ADC.h"

using namespace std;

//variables for IR transmission
char start_IR_transmit = 0; //flag to start an IR transmission
char IR_pulse_count = 0;
char pulse_count_max = 10;  //pulse count max should be twice as many pulses as you want
char IR_state = 0;

//variables for IR receiving
char IR_I_pulse = 0;    //number of pulses received
char IR_received = 0;   //stating whether the transmission was successful or not

void __ISR_AT_VECTOR(_TIMER_1_VECTOR,IPL3AUTO) Timer1ISR(void){
    
    if(start_IR_transmit) {
        //here we are pulsing out a set number of pulses on the IR led at 50% duty cycle
        //when the transmission is finished, the flag is cleared
        
        if(IR_pulse_count <= pulse_count_max) {
            IR_state = ! IR_state;  //pulse led 50% duty cycle 
            IR_O = IR_state;    //set the IR led
            IR_pulse_count++;   //increase count
        }
        
        if(IR_pulse_count > pulse_count_max) {
            start_IR_transmit = 0;  //when transmission is complete, reset the flag
            IR_state = 0;   //clear pulse
            IR_O = 0;   //make sure IR is off
            IR_pulse_count = 0; //reset count
        }
        
    }

    IFS0CLR = 0x00000010;   //clear Interrupt
    TMR1 = 0; //reset TMR1
      
}

void IR_receive(){
    
    if(IR_I){
        led1 = 0;
        
    }
    else {
        led1 = 1;
    }
    
    if(CNFCbits.CNFC15) {
        //a negative edge has occurred
        led2 = 1;
        CNFCbits.CNFC15 = 0;    //clear flag
        
        //shitty debounce code
        //CNFC needs to be properly debounced later
        for(int i = 0; i <10000; i++) {
            Nop();
        }
        
        IR_I_pulse++;
    }
    else {
        led2 = 0;
    }
    
    if(IR_I_pulse >= 4) {
        IR_I_pulse = 0;
        IR_received = 1;
        led3 = 1;
    }
    else {
        IR_received = 0;
        led3 = 0;
    }
    
}

void init_TMR1(){
    
    __builtin_disable_interrupts();

    T1CON = 0;  //clear register first, no special features needed
    TMR1 = 0;   //clear TMR1
    PR1 = 0xFFFF;    //period register 

    T1CONbits.TCKPS = 0b11; //1:8 pre-scaler

    IPC1bits.T1IP = 3;  //priority 
    IPC1bits.T1IS = 1;  //sub-pirority

    IFS0CLR = 0x00000010;   //clears the interrupt flag 
    IEC0SET = 0x00000010;   //Enable timer interrupts

    T1CONbits.ON = 1;   //TMR1 ON

    __builtin_enable_interrupts();
}

int main(int argc, char** argv) {
    
    //set ports to input/output
    TRISB = 0xFFFF; 
    TRISC = 0xBFFF;       //RC14 output
    TRISD = 0xF7FE;     //RD0, RB11 output
    TRISE = 0x00;       //PORTE output
    
    //set ports to digital/analog IO
    ANSELB = 0x0700;    //RB8,9,10 analog, rest digital
    ANSELE = 0x00;      //PORTE digital IO
    
    //set weak pull-ups
    CNPUD = 0b00111100; //weak pull-ups on sw 1-4
    
    //disable comparator
    CVRCONbits.ON = 0;
    CVRCONbits.CVROE = 0;

    init_adc(); //initialize ADC module
    
    //set interrupt on change pins
    //IOC for all 4 switches
    /*
    CNCONDbits.ON = 1;
    CNNEDbits.CNNED2 = 1;
    CNNEDbits.CNNED3 = 1;
    CNNEDbits.CNNED4 = 1;
    CNNEDbits.CNNED5 = 1;
    CNCONDbits.EDGEDETECT = 1;  //read CNFD flag register
     */
    //IR receiver
    CNCONCbits.ON = 1;
    CNNECbits.CNNEC15 = 1;  //negative edge
    CNCONCbits.EDGEDETECT = 1;
    //prox sensor echo
    CNCONBbits.ON = 1;
    CNNEBbits.CNNEB7 = 1;
    CNCONBbits.EDGEDETECT = 1;
    
    OSCCONbits.SOSCEN = 0;  //disable secondary oscillator
    
    //initialize TMR1
    init_TMR1();
    
    LATE = 0x00;    //clear leds
    IR_O = 0;       //clear IR
    
    int c = 0;  //temp delay counter
    
    while(1) {
        
        if(start_IR_transmit == 0) {
            
            //delay before sending transmission again
            if(c >= 10000000) {
                start_IR_transmit = 1;
                c = 0;
            }
            c++;
            
        }
        
        IR_receive();
       
    }   //end while(1)
    
    return 0;
}       //end main