#include <avr/io.h>
#define F_CPU 8000000UL
#include <util/delay.h>
#define LED_PIN PB5
#define TX_PIN PD1
#define BAUD 9600                               	// define baud (9600:0.2%error)
#define BAUDRATE ((F_CPU)/(BAUD*16UL)-1)        	// set baud rate value for UBRR

#define sbi(port, bit) (port) |= (1 << (bit))
#define cbi(port, bit) (port) &= ~(1 << (bit))
#define TSA1 1 // PC1 
#define TSA2 0 // PC0
#define TSA3 3 // PC3
#define TSA4 2 // PC2

#define TSB1 4 // PD4
#define TSB2 2 // PD2
#define TSB3 5 // PD5
#define TSB4 3 // PD3
/*
noxtal @8MHz
avrdude -c usbasp -p m8 -U lfuse:w:0xE4:m -U hfuse:w:0xC4:m
*/

// function to initialize UART
void uart_init (void)
{
    UBRRH = (BAUDRATE>>8);
    UBRRL = BAUDRATE;                       	// set baud rate
    UCSRB|= (1<<TXEN)|(1<<RXEN);            	// enable RX and TX
    UCSRC|= (1<<URSEL)|(1<<UCSZ0)|(1<<UCSZ1);   // 8bit data format
}
//~ int setup(void);
int setup(void)
{
  /* Set LED_PIN to output. */
  //~ DDRB |= _BV(LED_PIN);  
  //~ DDRD |= _BV(TX_PIN);  
  
  uart_init();
  return 0;	
}

// function to send data
void uart_transmit (unsigned char data)
{
    while (!( UCSRA & (1<<UDRE)));            	// wait while register is free
    UDR = data;                               	// load data in the register
}
// function to receive data
unsigned char uart_received (void)
{
    while(!((UCSRA) & (1<<RXC)));               	// wait while data is being received
    return UDR;                               	// return 8-bit data
}

void set_to_1 (unsigned char *port,unsigned char *ddr,unsigned char bit)
{
  sbi(*ddr , bit); // output
  sbi(*port, bit); // 1
}
void set_to_0 (unsigned char *port,unsigned char *ddr,unsigned char bit)
{
  sbi(*ddr , bit); // output
  cbi(*port, bit); // 0
}
void set_to_Z (unsigned char *port,unsigned char *ddr,unsigned char bit)
{
  cbi(*ddr , bit); // input
  cbi(*port, bit); // no pull-up
}
void display_n(unsigned char n, unsigned char AB)
{
    unsigned char DDR  = (AB)?DDRC:DDRD;   //make a copy of current I/O register value
    unsigned char PORT = (AB)?PORTC:PORTD; //make a copy of current I/O register value
    unsigned char TS1  = (AB)?TSA1:TSB1;
    unsigned char TS2  = (AB)?TSA2:TSB2;
    unsigned char TS3  = (AB)?TSA3:TSB3;
    unsigned char TS4  = (AB)?TSA4:TSB4;
    //default all Z
    set_to_Z(&PORT,&DDR,TS1);
    set_to_Z(&PORT,&DDR,TS2);
    set_to_Z(&PORT,&DDR,TS3);
    set_to_Z(&PORT,&DDR,TS4);
    switch(n) {
        case 0:
          set_to_1(&PORT,&DDR,TS3);
          set_to_0(&PORT,&DDR,TS1);
        break; 
        case 1:
          set_to_1(&PORT,&DDR,TS4);
          set_to_0(&PORT,&DDR,TS2);
        break; 
        case 2:
          set_to_1(&PORT,&DDR,TS1);
          set_to_0(&PORT,&DDR,TS2);
        break; 
        case 3:
          set_to_1(&PORT,&DDR,TS2);
          set_to_0(&PORT,&DDR,TS1);
        break; 
        case 4:
          set_to_1(&PORT,&DDR,TS3);
          set_to_0(&PORT,&DDR,TS4);
        break; 
        case 5:
          set_to_1(&PORT,&DDR,TS2);
          set_to_0(&PORT,&DDR,TS4);
        break; 
        case 6:
          set_to_1(&PORT,&DDR,TS2);
          set_to_0(&PORT,&DDR,TS3);
        break; 
        case 7:
          set_to_1(&PORT,&DDR,TS1);
          set_to_0(&PORT,&DDR,TS3);
        break; 
        case 8:
          set_to_1(&PORT,&DDR,TS4);
          set_to_0(&PORT,&DDR,TS1);
        break; 
        case 9:
          set_to_1(&PORT,&DDR,TS1);
          set_to_0(&PORT,&DDR,TS4);
        break;
	case 10:
		//This is all off
	break;  
        default:
          set_to_1(&PORT,&DDR,TS3);
          set_to_0(&PORT,&DDR,TS1);
    }
    if (AB)  //apply I/O register value
    {
        DDRC = DDR;
        PORTC = PORT;
    }
    else
    {
        DDRD = DDR;
        PORTD = PORT;
    }
}

int main(void)
{	
	// setup
	setup();

    //test display 00 11 22 33 ..
    int i,j,k;

    //~ display_n(10,0);
	//~ while(1) // Dimming
	//~ {
		//~ for (j=0;j<50;j++)
		//~ {
			//~ for (i=0;i<10;i++)
			//~ {
				//~ for (k=0;k<50;k++)
				//~ {
					//~ if (k<j)
					 //~ {display_n(5,1);}
					//~ else
					 //~ {display_n(10,1);}
				//~ }
				//~ _delay_ms(1); 
			//~ }
		//~ }
		//~ for (j=0;j<50;j++)
		//~ {
			//~ for (i=0;i<10;i++)
			//~ {
				//~ for (k=0;k<50;k++)
				//~ {
					//~ if (k<j)
					 //~ {display_n(10,1);}
					//~ else
					 //~ {display_n(5,1);}
				//~ }
				//~ _delay_ms(1); 
			//~ }
		//~ }
	//~ }

    for (i=0;i<10;i++)
    {
        display_n(i,0);
        display_n(i,1);
        _delay_ms(500); 
    }

    for (i=0;i<100;i++)
    {
        display_n(i/10,0);
        display_n(i%10,1);
        _delay_ms(100); 
    }

	//daisy chain BCD format ends with 0xff
	// example: 12h30m59s will be:
	//      0x12 0x30 0x59 0xff
	// if this board is the first, it will transmit 
    //      0x12 0x30 0xff
    // and will display 
    //      (5)(9) 
    unsigned char rx_prev=0xf0; //0xf0 is a reserved value meaning empty
    unsigned char rx=0xf0; 
    while(1) 
    {
        rx_prev = rx;
        rx=uart_received();
        if (rx == 0xff) //it is the end
        { 
            uart_transmit(0xff);
            display_n((rx_prev>>4)&0x0F,0);
            display_n((rx_prev   )&0x0F,1);
            rx=0xf0;
        }
        else if(rx_prev!=0xf0)
        {
            uart_transmit(rx_prev);
        }
    }
	return 0;
}





