/* 
 * File:   UART.cpp
 * Author: tawfika
 *
 * Created on May 16, 2019, 2:45 PM
 */

#include <cstdlib>
#include <xc.h>
#include <sys/attribs.h>
#include <proc/p32mz1024eff064.h>
#include "PIC_CONFIG.h"
#include "PINS_DEF.h"
#include "ADC.h"

using namespace std;

/*
 * 
 */

void InitUart(void);
void SendChar(char c);
char ReadChar(void);
void Unlockpps();
void lockPPS();
void SendString(char *string);

int main() {
    //set ports to input/output
    TRISB = 0xFFFF; 
    TRISC = 0xBFFF;       //RC14 output
    TRISD = 0xF7FE;     //RD0, RB11 output
    TRISE = 0x00;       //PORTE output
    
    
    
    
    //set ports to digital/analog IO
    ANSELB = 0x0700;    //RB8,9,10 analog, rest digital
    ANSELE = 0x00;      //PORTE digital IO
    
    TRISDbits.TRISD0=1;
    CNPUDbits.CNPUD0=1;
   
   
    
InitUart();

char string[90]="AT";
//char c='a';
    while(1)
    {
         SendString(string);
       // SendChar(c);
       // for(int i=0; i<1000000;i++){
        //    Nop();
        //}
        //c++;
      char c= ReadChar();
      LATE = c;
       for(int i=0; i<1000000;i++){
            Nop();
        }
    }
}
void lockPPS(){
        /* Unlock sequence to enable writing to IOLOCK nit */
    SYSKEY = 0x00000000;
    SYSKEY = 0xAA996655;
    SYSKEY = 0x556699AA;
    
    CFGCONbits.IOLOCK=1; // 
     /* Lock */
    SYSKEY = 0x33333333;
}

void Unlockpps(){
     /* Unlock sequence to enable writing to IOLOCK nit */
    SYSKEY = 0x00000000;
    SYSKEY = 0xAA996655;
    SYSKEY = 0x556699AA;
    
    CFGCONbits.IOLOCK=0; // clears the lock bit to unlock it
}



void InitUart(){
    
    PB2DIVbits.PBDIV=0b0001011; // dividing the system clock by 11 to get 18.432 MHZ.
    
    Unlockpps();
    U1RXRbits.U1RXR=0b0010; // maps the receiver to pin RF4 
    TRISFbits.TRISF4=1;
    
    RPF5Rbits.RPF5R=0b0001; // maps the transmitter to pin RF5
    TRISFbits.TRISF5 =0;
    lockPPS();
    
    U1MODEbits.BRGH = 0; // Standard Speed mode ? 16x baud clock enabled
    U1BRG = 9;
   
    U1MODEbits.SIDL = 0; // Continue operation in Idle mode
    
    U1MODEbits.IREN = 0;//  IrDA Encoder and Decoder Enable bit: disabled
    
    U1MODEbits.RTSMD = 0; // U1RTS pin is in Flow Control mode
    
    U1MODEbits.RTSMD = 0; // U1RTS pin is in Flow Control mode
    
    U1MODEbits.WAKE = 1;  // Wake-up enabled
     
    U1MODEbits.LPBACK = 0; // Loopback mode is disabled
     
    U1MODEbits.RXINV = 0; // U1RX IDLE state is '1'
     
    U1MODEbits.PDSEL = 0b00;// 8-bit data, no parity
     
    U1MODEbits.STSEL = 0;// 1 stop bit
     
    U1STAbits.UTXINV = 0; // U1TX IDLE state is '1'
     
    U1MODEbits.ON = 1; // UART1 is enabled
     
    U1STAbits.URXEN = 1; // UART1 receiver is enabled
     
    U1STAbits.UTXEN = 1;// UART1 transmitter is enabled
    
}

char ReadChar(void)
{
     U1STAbits.URXEN = 1;    
     
    while(!U1STAbits.URXDA);             // Wait for information to be received
    
    return U1RXREG;                     // Return received character
    
}



    void SendChar(char c)
{
    U1STAbits.UTXEN = 1;                // Make sure transmitter is enabled
                    
    while(U1STAbits.UTXBF);             // Wait while buffer is full
    U1TXREG = c;                     // Transmit character
}
   
 void SendString(char *string)
{
     
   int i = 0;
     
    U1STAbits.UTXEN = 1;                // Make sure transmitter is enabled
     
    while(*string)
    {
        while(U1STAbits.UTXBF);         // Wait while buffer is full
        U1TXREG = *string;              // Transmit one character
        string++;                       // Go to next character in string
    }
}
 

    
    