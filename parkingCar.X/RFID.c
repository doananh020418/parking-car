#include<htc.h>

#define lcd_dat PORTB
#define rs RC0
#define rw RC1
#define en RC2
#define delay for(i=0;i<1000;i++)

int i;

void lcd_init();
void cmd(unsigned char a);
void dat(unsigned char b);
void show(unsigned char *s);


__CONFIG( FOSC_HS & WDTE_OFF & PWRTE_OFF & CP_OFF & BOREN_ON & LVP_OFF & CPD_OFF & WRT_OFF & DEBUG_OFF);


void ser_int();
void tx(unsigned char);
unsigned char rx();
void txstr(unsigned char *);

void main()
{
  int i;
  unsigned char id[12];
  TRISC6=TRISC7=1;
  ser_int();
  lcd_init();
  cmd(0x80);
  show("<<SHOW UR CARD>>");
  cmd(0xc0);
  for(i=0; i<12; i++) {
    id[i]=rx();
    dat(id[i]);
  }   
  while(1);  
}  

void ser_int()
{
  TXSTA=0x20;  //BRGH=0, TXEN = 1, Asynchronous Mode, 8-bit mode
  RCSTA=0b10010000; //Serial Port enabled,8-bit reception
  SPBRG=17;      //9600 baudrate for 11.0592Mhz
  TXIF=RCIF=0;
}
  
void tx(unsigned char a)
{
  TXREG=a;
  while(!TXIF);
  TXIF = 0;
}

unsigned char rx()
{
  while(!RCIF);
  RCIF=0;
  return RCREG;
}

void txstr(unsigned char *s)
{
  while(*s)
  {
    tx(*s++);
  }
}

void lcd_init()
{
  TRISC0=TRISC1=TRISC2=TRISB=0;
  cmd(0x38);
  cmd(0x0c);
  cmd(0x06);
  cmd(0x80);
}

void cmd(unsigned char a)
{
  lcd_dat=a;
  rs=0;
  rw=0;
  en=1;
  delay;
  en=0;
}

void dat(unsigned char b)
{
  lcd_dat=b;
  rs=1;
  rw=0;
  en=1;
  delay;
  en=0;
}

void show(unsigned char *s)
{
  while(*s)
  {
    dat(*s++);
  }
}