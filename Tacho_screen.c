/*
 * Tacho.c
 *
 * Created: 19.06.2014 15:33:40
 *  Author: Jonas
 */ 

#define F_CPU 1000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define _PRIM PORTC
#define _SEC  PORTD

#define _WHEEL_SIZE 207

//Define characters (negative)
#define _C1 0b10101111//0b01010000
#define _C2 0b11001000//0b00110111
#define _C3 0b10101000//0b00000000
#define _C4 0b10100101//0b00000000
#define _C5 0b10110000//0b00000000
#define _C6 0b10010001//0b00000000
#define _C7 0b10101110//0b00000000
#define _C8 0b10000000//0b00000000
#define _C9 0b10100000//0b00000000
#define _C0 0b10000010//0b00000000
#define _C_ 0b11111101
#define _CCLEAR 255
 
uint32_t cntr; 
unsigned int speed;
 
ISR(TIMER0_OVF_vect)
{
	cntr++;
}

inline void calcSpeed(void)
{
	float tmrSpeed = F_CPU / 256;
	float tmrTime  = 1 / tmrSpeed;
	float comTime  = tmrTime * cntr;
	
	float mps      = (_WHEEL_SIZE / 100) / comTime;
	
	float kmph     = mps * 3.6;
	
	speed = (int)kmph;
}//calcSpeed()

inline void render(int _screen)
{
	char cNr[16];
	
	int i1, i2;
	
	itoa(_screen, cNr, 10); //convert int to string for rendering
	
	if(_screen < 10)
	{
		i1 = 0;
		i2 = 1;
		cNr[1] = '0';
	}
	else
	{
		i1 = 1;
		i2 = 0;
	}
	
	if(_screen > 99 || _screen < 0)
	{
		_SEC = _PRIM = _C_;
	}
	else
	{
		switch (cNr[i1]) //first character
		{
			case '0':
				_PRIM = _C0;
				PORTB &= ~(1<<PB2);
			break;
			case '1':
				_PRIM = _C1;
				PORTB &= ~(1<<PB2);
			break;
			case '2':
				_PRIM = _C2;
				PORTB |= (1<<PB2);
			break;
			case '3':
				_PRIM = _C3;
				PORTB &= ~(1<<PB2);
			break;
			case '4':
				_PRIM = _C4;
				PORTB &= ~(1<<PB2);
			break;
			case '5':
				_PRIM = _C5;
				PORTB &= ~(1<<PB2);
			break;
			case '6':
				_PRIM = _C6;
				PORTB &= ~(1<<PB2);
			break;
			case '7':
				_PRIM = _C7;
				PORTB &= ~(1<<PB2);
			break;
			case '8':
				_PRIM = _C8;
				PORTB &= ~(1<<PB2);
			break;
			case '9':
				_PRIM = _C9;
				PORTB &= ~(1<<PB2);
			break;
		}
		
		switch (cNr[i2])//second character
		{
			case '0':
				_SEC = _C0;
			break;
			case '1':
				_SEC = _C1;
			break;
			case '2':
				_SEC = _C2;
			break;
			case '3':
				_SEC = _C3;
			break;
			case '4':
				_SEC = _C4;
			break;
			case '5':
				_SEC = _C5;
			break;
			case '6':
				_SEC = _C6;
			break;
			case '7':
				_SEC = _C7;
			break;
			case '8':
				_SEC = _C8;
			break;
			case '9':
				_SEC = _C9;
			break;
		}
	}
}//render()

int main(void)
{
	TCCR0 = (1 << CS00); // Prescaler 1
	TIMSK = (1<<TOIE0); //Enable Overflowinterrupt
	
	cntr = 0; //counter for Timer
	DDRC = DDRD = 255; // Output for scrren pins
	DDRB |= (1<<PB2);
	DDRB &= ~(1<<PB1);
	
	sei();
	
	render(0);
	
	//PORTC = 0b01010000;
    while(1)//main loop
	{	
		while(PINB & (1 << PB1) && cntr < 30000){} // wait for reed
		TCCR0 = ~(1 << CS00); //disable counter
		cli(); //disable interrupts
		calcSpeed();
		//reset all variables
		cntr = 0;
		TCNT0 = 0;
		sei();//enable interrupts
		TCCR0 = (1 << CS00); //enable counter
		
		render(speed);//render the calculated speed
		_delay_ms(100);
	}
}//main()