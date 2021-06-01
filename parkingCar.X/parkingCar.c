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


#define delay for(i=0;i<=1000;i++)
#define LDR1 RB6
#define LDR2 RB5
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
#define LED RC3
#define LED2 RC4
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

unsigned int adc();
unsigned int adc1();
unsigned int counter = 0;
unsigned int parkingTime = 0; // dung de ghi thoi gian xe do  
unsigned int hold = 0;
unsigned int numberID = 0;

void Rotation0();
void Rotation90();
void ser_int();
void tx(unsigned char);
unsigned char rx();
void txstr(unsigned char *);

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

unsigned int checkID2(unsigned char id[12], unsigned availableID[12]) {

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
    Lcd8_Set_Cursor(2, 1);
    Lcd8_Write_String("Status: ");
    Lcd8_Set_Cursor(2, 9);
    if (isOpened == 1) {
        Lcd8_Write_String("Opened");
    } else {
        Lcd8_Write_String("Closed");
    }

}

void main() {
    timerInit();
    //RFID_init();
    INTEDG = 1;
    RBIE = 1;
    INTE = 1;
    TRISB = 0b11110001;
    PORTB = 0x00;
    TRISC = 0b11000000;
    RB7 = 1;


    TRISD = 0; //Port D is output LED
    TRISA0 = 1;
    TRISA1 = 1; //RA1 is input (ADC)
    Lcd8_Init();
    W_LED = 0;

    int i;
    unsigned char id[12];
    ser_int();

    Lcd8_Set_Cursor(1, 1);
    Lcd8_Write_String("<<SHOW UR CARD>>");

    while (1) {

        
        //checkGateStatus();
        //        if(LDR1==1){ //car reach the gate
        //            LED = 1;
        //            
        //            // check id card ???
        ////            for(int i=0; i<12; i++) {
        ////                id[i]=rx();
        ////            }
        //            Lcd8_Set_Cursor(1,1);
        //            //Lcd8_Write_String(id);
        //            if(1){
        //                parkingTime = 0;
        //                if (isOpened == 0){ // checking if gate is opened
        //
        //                    Rotation90(); // open it
        //                    isOpened = 1; // set gate status = 1
        //                  
        //                    if(parkingTime > 5 && LDR2 == 0){ // if gate is opened and there is no opject forward, warning if car does not move after 30s
        //                        W_LED = 1; // warning
        //                    }
        //                    else{
        //                        W_LED = 0;
        //                    }
        //                }
        //            }
        //                
        //        }
        //        
        //        
        //        if(LDR2==0){ //car moved through
        //            
        //            if (LDR1==1){
        //                W_LED = 0;
        //                if(isOpened == 1){ // check status of gate, if gate is not closed, close it
        //                    Rotation0(); // close
        //                    isOpened = 0;
        //                }
        //            }
        //        }
        //        
        for(int i=0; i<12; i++) {
            id[i]=rx();
            //Lcd8_Write_String(id[i]);
        }
//        if (sizeof(id)<12){
//            Lcd8_Set_Cursor(2,1);
//            Lcd8_Write_String("hjhj");
//        }
        Lcd8_Set_Cursor(2,1);
        Lcd8_Write_String("hjhj");
        
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
}

//void __interrupt() ISR(void){
//        if(INTF == 1) {
//            Rotation90();
//            isOpened = 1;
//            INTF = 0;
//        }
//        if(TMR1IF == 1){
//            counter ++;
//            if(counter == 15*1){
//                parkingTime ++;
//                counter=0;
//            }
//            TMR1IF = 0;
//        }
//}



void ser_int() {
    TXSTA = 0x20; //BRGH=0, TXEN = 1, Asynchronous Mode, 8-bit mode
    RCSTA = 0b10010000; //Serial Port enabled,8-bit reception
    SPBRG = 17; //9600 baudrate for 11.0592Mhz
    TXIF = RCIF = 0;
}

void tx(unsigned char a) {
    TXREG = a;
    while (!TXIF);
    TXIF = 0;
}

unsigned char rx() {
    while (!RCIF);
    RCIF = 0;
    return RCREG;
}

void txstr(unsigned char *s) {
    while (*s) {
        tx(*s++);
    }
}