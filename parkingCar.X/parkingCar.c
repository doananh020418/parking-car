/*
 * File:   LDR.c
 * Author: doank
 *
 * Created on April 9, 2021, 11:26 AM
 */

#include <xc.h>
#include <stdint.h>
#include <pic.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "uart.h"

#define LDR1 RB3
#define LDR2 RB4

#define RS RC0
#define EN RC2

#define D0 RD0
#define D1 RD1
#define D2 RD2
#define D3 RD3
#define D4 RD4
#define D5 RD5
#define D6 RD6
#define D7 RD7

#define W_LED RC5
#define servo RB1
#define _XTAL_FREQ 16000000
#include "lcd.h"

#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = ON      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = OFF         // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3/PGM pin has PGM function; low-voltage programming enabled)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)


unsigned int counter = 0;
unsigned int parkingTime = 0; // dung de ghi thoi gian xe do  
int park = 99;
unsigned int numberID = 0;
unsigned int lock = 0;

void Rotation0();
void Rotation90();
int toa(int value,char *ptr)
     {
        int count=0,temp;
        if(ptr==NULL)
            return 0;   
        if(value==0)
        {   
            *ptr='0';
            return 1;
        }
 
        if(value<0)
        {
            value*=(-1);    
            *ptr++='-';
            count++;
        }
        for(temp=value;temp>0;temp/=10,ptr++);
        *ptr='\0';
        for(temp=value;temp>0;temp/=10)
        {
            *--ptr=temp%10+'0';
            count++;
        }
        return count;
     }

unsigned int isOpened = 0;

void addNewID(unsigned char newID[12]) {
    eeprom_write(numberID, newID);
    numberID++;
}

void RFID_init() {
    addNewID("111111111111");
}

//void loadID(){
//    for (int i=0;i<numberID;i++){
//        availableID[i] = (unsigned char)eeprom_read(i);
//    }
//}

unsigned int checkID(unsigned char id[12]) {
    for (int i = 0; i < numberID; i++) {
        if (strcmp((unsigned char) eeprom_read(i), id) == 0) {
            return 1;
        }
    }
    return 0;
}

unsigned int checkID2(unsigned char id[12], unsigned char availableID[12]) {

    if (strcmp(availableID, id) == 0) {
        return 1;
    }
    return 0;
}

void timerInit() {
    TMR1IE = 1; //Timer1 interrupt enable bit
    TMR1IF = 0; // Clear the interrupt flag bit for timer1
    PEIE = 1;
    GIE = 1;

    //config timer1
    // clear the timer1 register to start counting from 0
    TMR1 = 0;
    // Clear the timer1 clock to select bit to choose local clock source
    TMR1CS = 0;
    // prescale ratio 1:1
    T1CKPS0 = 0;
    T1CKPS1 = 0;
    // switch on timer1
    TMR1ON = 1;
}

void checkGateStatus() {
    if (lock ==1){
        Lcd8_Set_Cursor(2, 0);
        Lcd8_Write_String("Lock!!!");
    }
    else{
        Lcd8_Set_Cursor(2, 0);    
        if (isOpened == 1) {
            Lcd8_Write_String("Opened!");
        } else {
            Lcd8_Write_String("Closed!");
        }
    }
    char string[20];
    toa(park,string);
    Lcd8_Set_Cursor(2, 10);
    Lcd8_Write_String(string);
}

void main() {
    timerInit();
    //RFID_init();
    //UART_Init();
    INTEDG = 1;
    INTE = 1;
    TRISB = 0b11111101;
    PORTB = 0x00;
    TRISC = 0b11000000;
    RB7 = 1;
    TRISC6=TRISC7=1;

    TRISD = 0x00; //Port D is output LED

    Lcd8_Init();
    

    int i;
    unsigned char id[13];

    Lcd8_Set_Cursor(1, 0);
    Lcd8_Write_String("Wellcome!");
    
    while (1) {
        checkGateStatus(); 
                if(LDR1==1){ //car reach the gate

                    if(1){
                        if (isOpened == 0 && lock == 0){ // checking if gate is opened
                            Rotation90(); // open it
                            parkingTime = 0;
                          
                        }
                        if(parkingTime > 15 && LDR2 == 0){ // if gate is opened and there is no opject forward, warning if car does not move after 30s
                                
                                Rotation0();
                                lock = 1;
                            }
                    }
                }
                if(LDR2==1){ //car moved through
                    if (LDR1==0){
                        if(isOpened == 1){ // check status of gate, if gate is not closed, close it
                            Rotation0(); // close
                            park--;
                        }
                    }
                }
    }
}

void Rotation0() //0 Degree
{
    unsigned int i;
    for(i=0;i<50;i++)
    {
        RB1 = 1;
        __delay_us(900); // pulse of 800us
        RB1 = 0;
        __delay_us(19100);
    }
    isOpened = 0;
}

void Rotation90() //90 Degree
{
    unsigned int i;
    for(i=0;i<50;i++)
    {
        RB1 = 1;
        __delay_us(1950); // pulse of 1500us
        RB1 = 0;
        __delay_us(18050);
    }
    isOpened = 1;
}

void __interrupt() ISR(void){
        if(INTF == 1) {
            Rotation90();
            lock = 0;
            parkingTime = 0;
            INTF = 0;
        }
        if(TMR1IF == 1){
            counter ++;
            if(counter == 61*1){
                parkingTime ++;
                counter=0;
            }
            TMR1IF = 0;
        }
}


