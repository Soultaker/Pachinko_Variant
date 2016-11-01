
/* Name: Derrick Lam, dlam012@ucr.edu
 * Partner(s) Name & E­mail: Jonathan Candelaria, jcand003@ucr.edu
 * Lab Section: 21
 * Assignment: Custom Lab
 * Exercise Description: [optional ­ include for your own benefit]
 *
 * I acknowledge all content contained herein, excluding template or example
 * code, is my own original work.
 */
#include <stdint.h> 
#include <stdlib.h> 
#include <stdio.h> 
#include <stdbool.h> 
#include <string.h> 
#include <math.h> 
#include <avr/io.h> 
#include <avr/interrupt.h> 
#include <avr/eeprom.h> 
#include <avr/portpins.h> 
#include <avr/pgmspace.h> 

 
//FreeRTOS include files 
#include "FreeRTOS.h" 
#include "task.h" 
#include "croutine.h" 
#include "bit.h"
#include "lcd.h"

//usart files
#include "usart_ATmega1284.h"

void A2D_init() {
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
	// ADEN: Enables analog-to-digital conversion
	// ADSC: Starts analog-to-digital conversion
	// ADATE: Enables auto-triggering, allowing for constant
	//        analog to digital conversions.
}




//char text[] = "Score: 0        High Score: 0";
int curScore = 0;			//The current score
int temp = 0;
int reset = 0;				//Signal to reset the game
int psLock0 = 0;			//Way to make it so points are gained no more than once per sensor per game
int psLock1 = 0;
int psLock2 = 0;
int psLock3 = 0;
int psLock4 = 0;
int psLock5 = 0;
int psLock6 = 0;
int psLock7 = 0;

//char str[15];

enum LED_States {Init, Start} led_state;
//enum LEDState {INIT,L0,L1,L2,L3,L4,L5,L6,L7} led_state;
enum PHOTOState {PHOTOINIT, CHECK} photo_state;
	
void PHOTOS_Init(){
	photo_state = PHOTOINIT;
}

void LEDS_Init(){
	led_state = Init;
}

void LED_Tick() {
	// Transitions
	switch (led_state) {
		case Init:
			led_state = Start;
			break;
		case Start:
			led_state = Start;
			break;
		default:
		led_state = Init;
		break;
	}
	// Actions
	switch (led_state) {
		case Init:
			break;
		case Start:
			if( USART_IsSendReady(0) && (temp != curScore) ) {
				USART_Send(curScore,0);
				temp = curScore;
			}
			
			if(USART_HasTransmitted(0)){
				USART_Flush(0);
			}
			//PORTA = output;
			break;
		default:
			break;
	}
}


void PHOTOS_Tick(){
	switch(photo_state){
		case PHOTOINIT:
			psLock0 = 0;
			psLock1 = 0;
			psLock2 = 0;
			psLock3 = 0;
			psLock4 = 0;
			psLock5 = 0;
			psLock6 = 0;
			psLock7 = 0;
		break;
		case CHECK:
			if((GetBit(PINA,0)) && !psLock0)		//Sensor 0 activated
			{
				psLock0 = 1;
				curScore += 1;
			}
			else if((GetBit(PINA,1)) && !psLock1)	//Sensor 0 activated
			{
				psLock1 = 1;
				curScore += 1;
			}
			else if((GetBit(PINA,2)) && !psLock2)	//Sensor 0 activated
			{
				psLock2 = 1;
				curScore += 2;
			}
			else if((GetBit(PINA,3)) && !psLock3)	//Sensor 0 activated
			{
				psLock3 = 1;
				curScore += 4;
			}
			else if((GetBit(PINA,4)) && !psLock4)	//Sensor 0 activated
			{
				psLock4 = 1;
				curScore += 8;
			}
			else if((GetBit(PINA,5)) && !psLock5)	//Sensor 0 activated
			{
				psLock5 = 1;
				curScore = 0;
			}
			else if((GetBit(PINA,6)) && !psLock6)	//Sensor 0 activated
			{
				psLock6 = 1;
				curScore += 16;
			}
			else if((GetBit(PINA,7)) && !psLock7)	//Sensor 0 activated
			{
				psLock7 = 1;
				curScore = curScore*2;
			}
			
		break;
	}
	switch(photo_state){
		case PHOTOINIT:
			photo_state = CHECK;
		break;
		case CHECK:
			if(reset){
				photo_state = PHOTOINIT;
			}
			else{
				photo_state = CHECK;
			}
		break;
	}
}

void PhotoSecTask()
{
	PHOTOS_Init();
   for(;;) 
   { 	
	PHOTOS_Tick();
	vTaskDelay(100); 
   } 
}

void LedSecTask()
{
	LEDS_Init();
   for(;;) 
   { 	
	LED_Tick();
	vTaskDelay(100); 
   } 
}

void StartSecPulse(unsigned portBASE_TYPE Priority)
{
	xTaskCreate(LedSecTask, (signed portCHAR *)"LedSecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}	


void PhotoSecPulse(unsigned portBASE_TYPE Priority)
{
	xTaskCreate(PhotoSecTask, (signed portCHAR *)"PhotoSecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}	
 
int main(void) 
{ 
   DDRA = 0x00; PORTA=0xFF;
   DDRD = 0xFF; PORTD = 0x00;
   DDRC = 0xFF; PORTC = 0x00;
   
   //char hello = "Hello World";
   
   //Start Tasks  
   //StartSecPulse(1);
   
    //RunSchedular 
	
	//int num = 10;
	//char snum[5];
	//tostring(snum, num);
	
	//int aInt = 368;
//char str[15];
//sprintf(str, "%d", curScore);
	
		initUSART(0);
	//USART
	
	//Start Tasks
	StartSecPulse(1);
	
	//LCD_init();
	//LCDSecPulse(1);
	PhotoSecPulse(1);
	//LCD_DisplayString(1, str);
	
	//LCD_Cursor(10);
	//LCD_WriteData('F');
	vTaskStartScheduler(); 
	//while(1);
 
 
   return 0; 
}