/*
 * time.h
 *
 * Created: 06/11/2024 21:53:08
 *  Author: Martynas
 */ 


#ifndef TIME_H_
#define TIME_H_



// Libraries for time to work
#include <avr/io.h>
#include <util/atomic.h>
#include <avr/interrupt.h>



ISR(TIMER1_COMPA_vect);
void time_init(unsigned long f_osc);
unsigned long time_millis(void);



#endif /* TIME_H_ */