
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

char text[] = "Score: 0        High Score: 0";
int curScore = 0;			//The current score
int highScore = 0;			//The highest score achieved
int temp = 0;
int reset = 0;				//Signal to reset the game

char str[15];
char str2[15];

enum LED_States {Init, Start} led_state;	
enum LCDState {LCDINIT, PRINT, LCDWAIT} lcd_state;
	
void LCD_Init(){
	lcd_state = LCDINIT;
}

void LED_Init(){
	led_state = Init;
}

void LCD_Tick(){
	switch(lcd_state){
		case LCDINIT:
			LCD_DisplayString(1, text);
			temp = curScore;
			sprintf(str2, "%d", highScore);
			LCD_DisplayString2(29, str2);
		break;
		case PRINT:
			if(curScore != 0){
			sprintf(str, "%d", curScore);
			sprintf(str2, "%d", highScore);
			//LCD_DisplayString(1, str);
			//LCD_DisplayString(1, text);
			//LCD_Cursor(8);
			LCD_DisplayString2(8, str);
			LCD_DisplayString2(29, str2);
			//LCD_WriteData(str + '0');
			LCD_Cursor(32);
			}
			//else{
				//LCD_DisplayString(1, text);
			//}
			temp = curScore;
		break;
		case LCDWAIT:
		break;
	}
	switch(lcd_state){
		case LCDINIT:
			//lcd_state = PRINT;
			lcd_state = LCDWAIT;
		break;
		case PRINT:
			if(curScore == 0){
				lcd_state = LCDINIT;
			}
			else{
			lcd_state = PRINT;
			}
		break;
		case LCDWAIT:
			if(reset){
				lcd_state = LCDINIT;
			}
			else if(temp != curScore){
				lcd_state = PRINT;
			}
			else{
				lcd_state = LCDWAIT;
			}
		break;
	}
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
				curScore = USART_Receive(0);
				//PORTA = output;
				if(highScore < curScore){
					highScore = curScore;
				}
				USART_Flush(0);
			break;
		default:
			break;
	}
}



void LCDSecTask()
{
	LCD_Init();
   for(;;) 
   { 	
	LCD_Tick();
	vTaskDelay(100); 
   } 
}

void LedSecTask()
{
	LED_Init();
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

void LCDSecPulse(unsigned portBASE_TYPE Priority)
{
	xTaskCreate(LCDSecTask, (signed portCHAR *)"LCDSecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
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
	
	//Start Tasks
	A2D_init();
	LCD_init();
	LCDSecPulse(1);
	StartSecPulse(1);
	//PhotoSecPulse(1);
	//LCD_DisplayString(1, str);
	
	//LCD_Cursor(10);
	//LCD_WriteData('F');
	vTaskStartScheduler(); 
	//while(1);
 
 
   return 0; 
}