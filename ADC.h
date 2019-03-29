/* 
 * File:   ADC_H
 * Author: Tony Tawfik, Gunnar Turnquist, Peter Salib
 *
 * Created on March 26, 2019, 5:22 PM
 */

#ifndef ADC_H
#define ADC_H

#include <xc.h>

void init_adc();
int read_adc(char channel);

#endif