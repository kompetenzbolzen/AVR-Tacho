/*
 * Tacho.c
 *  Author: noname7890
 */ 

#define F_CPU 1000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define _PRIM PORTC //Right digit
#define _SEC  PORTD //Left digit

#define _WHEEL_SIZE 207 //In cm

//Define characters (negative)
#define _C1 0b10101111
#define _C2 0b11001000
#define _C3 0b10101000
#define _C4 0b10100101
#define _C5 0b10110000
#define _C6 0b10010001
#define _C7 0b10101110
#define _C8 0b10000000
#define _C9 0b10100000
#define _C0 0b10000010
#define _C_ 0b11111101
#define _CCLEAR 255
 
uint32_t cntr; //counter for Timer
unsigned int speed; //global var to save the calculated speed in
 
ISR(TIMER0_OVF_vect) //ISR for TIMER0 overflow
{
	cntr++;
}

inline void calcSpeed(void)
{
	float tmrSpeed = F_CPU / 256; //The timer overflow speed in Hz
	float tmrTime  = 1 / tmrSpeed; //The timer overflow speed in s
	float comTime  = tmrTime * cntr; //calculate the total time
	
	float mps      = (_WHEEL_SIZE / 100) / comTime; //Convert wheelsize to m and derive by the complete time
	
	float kmph     = mps * 3.6; //km/h = m/s * 3.6
	
	speed = (int)kmph; //cast to int and write into global var speed
}//calcSpeed()

inline void render(int _screen)
{
	char cNr[16];
	
	int i1, i2;
	
	itoa(_screen, cNr, 10); //convert int to string for rendering
	/*
	* If _screen is smaller than 10, the cNr contains 1xxxxxxxxxx, but if it is bigger than 10 it looks like this: 10xxxxxxxxxx
	* So I have to change the digits
	*/
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
	
	cntr = 0; //init counter for Timer
	DDRC = DDRD = 255; // Output for screen pins
	DDRB |= (1<<PB2);
	DDRB &= ~(1<<PB1);
	
	sei(); //enable interrupts
	
	render(0); //init the display
	
    while(1)//main loop
	{	
		while(PINB & (1 << PB1) && cntr < 30000){} // wait for reedcontact on PB1
		TCCR0 = ~(1 << CS00); //disable counter
		cli(); //disable interrupts
		calcSpeed();
		//reset all variables
		cntr = 0; //Reset counter	
		TCNT0 = 0; //Reset Timer
		sei();//enable interrupts
		TCCR0 = (1 << CS00); //enable counter
		
		render(speed);//render the calculated speed
		_delay_ms(100);
	}
}//main()