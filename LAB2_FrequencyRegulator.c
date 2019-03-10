/*********************************************************************************************************************
 *
 * FileName:        main.c
 * Processor:       PIC18F2550 / PIC18F2553
 * Compiler:        MPLAB® XC8 v2.00
 * Comment:         Main code
 * Dependencies:    Header (.h) files if applicable, see below
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Author                       Date                Version             Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Eva Andries	                12/10/2018          0.1                 Initial release
 * Eva Andries					 6/11/2018			1.0					XC8 v2.00 new interrupt declaration
 * Tim Stas                     12/11/2018          1.1                 volatile keyword: value can change beyond control of code section
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * TODO                         Date                Finished
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *********************************************************************************************************************/

/*
 * Includes
 */
#include <xc.h>

/*
 * Prototypes
 */
void __interrupt (high_priority) high_ISR(void);   //high priority interrupt routine
void __interrupt (low_priority) low_ISR(void);  //low priority interrupt routine, not used in this example
void initChip(void);
void initTimer(void);
/*
 * Global Variables
 */
volatile int  counter = 0;
unsigned int   updown = 1;
volatile int  adc_value=0;
volatile char  pwm_timer=0;//char

/*
 * Interrupt Service Routines
 */
/********************************************************* 
	Interrupt Handler
**********************************************************/
void __interrupt (high_priority) high_ISR(void)
{
	if(PIR1bits.TMR2IF==1)
    {
        pwm_timer++;
        PIR1bits.TMR2IF=0;
    }
    
    if(INTCONbits.TMR0IF == 1)
     {
        if(pwm_timer>(adc_value/50))
        {
            //increase or decrease the duty circle to make the wave
            if (updown==1) 
            {
              counter++;
            }
            else 
            {
              counter --;
             }
            pwm_timer=0;
        }
         TMR0L =0;    			//reload the value to the Timer0
         INTCONbits.TMR0IF=0;     //CLEAR interrupt flag when you are done!!!
         CCPR2L=counter;        //change the duty circle
         ADCON0bits.GO_DONE =1; //start to do the adc
     }
    if(PIR1bits.ADIF == 1)
    {
        adc_value = ADRESH;  //change the duty circle`s maximum extreme with ADC value
        PIR1bits.ADIF = 0;
    }
}

/*
 * Functions
 */
 /*************************************************
			Main
**************************************************/
void main(void)
{
    initChip();
    initTimer();
    while(1)    //Endless loop
    {
         if(PORTCbits.RC0 == 1){ //triangular wave if RC0 is not pressed
             //const duty circle
            if (counter >= 127) 
            {
                updown = 0;
            }
            if (counter <= 0) 
            {
                updown = 1;
            }
        }
         else //sawtooth wave
         {  
             updown = 1;
             if (counter >= 127) counter = 0;
         }   
        // LBTB=adc_value;
    }
    
}

/*************************************************
			Initialize the CHIP
**************************************************/
void initChip(void)
{
    LATA = 0x00; //Initial PORTA
    TRISA = 0xFF; //Define PORTA as input
    ADCON1 = 0x0F; //Turn off ADcon
    CMCON = 0x07; //Turn off Comparator
    LATB = 0x00; //Initial PORTB
    TRISB = 0x00; //Define PORTB as output
    LATC = 0x00; //Initial PORTC
    TRISC = 0b10000001; //Define PORTC as output, RC0 input
	INTCONbits.GIE = 0;	// Turn Off global interrupt
    
    //set pwm
    CCP2CON=0b00001111;  //bit5-4: LSB; bit3-0:set pwm mode
    PR2=0xFF; //PWM Period = [(PR2) + 1] ? 4 ? TOSC ?(TMR2 Prescale Value) 
              //pwm period=10kHz, Tosc=48Mhz TMR2=1:4 pr2=299?255
    CCPR2L=0b00000000;
    
    //initialize ADC
    ADCON0 = 0b00000011;//bits5-2: "0000" AN0 as analog input
                        // a/d conversion in progress
                        // a/d converter enabled
    ADCON1 = 0b00000000; //set Port A as analog
    ADCON2 = 0b00001110;  //bit7:'0' left justified
                         //2Tad
                         //Fosc/64
}

/*************************************************
			Initialize the TIMER
**************************************************/
void initTimer(void)
{
    T0CON =0x45;        //Timer0 Control Register
               		//bit7 "0": Disable Timer
               		//bit6 "1": 8-bit timer
               		//bit5 "0": Internal clock
               		//bit4 "0": not important in Timer mode
               		//bit3 "0": Timer0 prescale is assigned
               		//bit2-0 "111": Prescale 1:64  
    /********************************************************* 
	     Calculate Timer 
             F = Fosc/(4*Prescale*number of counting)
	**********************************************************/

    
    TMR0L = 0x00;    //Initialize the timer value
    

    /*Interrupt settings for Timer0*/
    INTCON= 0x20;   /*Interrupt Control Register
               		//bit7 "0": Global interrupt Enable
               		//bit6 "0": Peripheral Interrupt Enable
               		//bit5 "1": Enables the TMR0 overflow interrupt
               		//bit4 "0": Disables the INT0 external interrupt
               		//bit3 "0": Disables the RB port change interrupt
               		//bit2 "0": TMR0 Overflow Interrupt Flag bit
                    //bit1 "0": INT0 External Interrupt Flag bit
                    //bit0 "0": RB Port Change Interrupt Flag bit
                     */
    
    T0CONbits.TMR0ON = 1;  //Enable Timer 0
    INTCONbits.GIE = 1;    //Enable interrupt
    PIE1bits.ADIE=1;
    
         //initialize timer2
    T2CON=0b00000101;  //bit2:"1":timer2 is on
                       //bit1-0:"01":prescaler is 1:4
    TMR2=0x00;//initialize timer2
}
