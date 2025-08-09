#include "Arduino.h"
//#include <avr/io.h>
//#include <util/delay.h>
//#include <avr/interrupt.h>
#include <avr/pgmspace.h>

// #define __save_interrupt() SREG
// #define __restore_interrupt(var) SREG = (var)
// #define __disable_interrupt() cli()
// #define __enable_interrupt() sei()
// #define __delay_cycles(var) _delay_us((unsigned int)(var)/(F_CPU/1000000))
// #define __flash PROGMEM const
// #define read_byte_flash(x) pgm_read_byte(&(x))

/*-*-*-*-*-*-*-*-*-*-*- SETUP -*-*-*-*--*-*-*-*-*-*/
#define SCLK    7   // SH_CP (SCLK)
#define RCLK    6   // ST_CP (LATCH)
#define DIO     5   // DS (SDI)

#define IND_AMOUNT_NUM 8   // Колличество цифр индикатора 

/*which register the segments and digits are connected to
if two registers are used,
the third line should be commented out*/

/* Shift register connected directly to the microcontroller
 * has the following values: when using two registers - 1, using three - 2. 
 * The shift register connected by the last stage always has the value - 0.
 */
#define IND_SHIFT_REG_SEG  1
#define IND_SHIFT_REG_DIG1 0

/* active segment level 0 or 1 */
#define IND_ACTIVE_LEVEL_SEG   1
/* active digits level 0 or 1 */
#define IND_ACTIVE_LEVEL_DIG   0

/* to which pins the segments are connected */  
#define IND_A    2
#define IND_B    0
#define IND_C    6
#define IND_D    4
#define IND_E    3
#define IND_F    1
#define IND_G    7
#define IND_COM  5

/* to which pins the digits are connected */
#define IND_NUM1 7
#define IND_NUM2 6
#define IND_NUM3 5
#define IND_NUM4 4
#define IND_NUM5 3
#define IND_NUM6 2
#define IND_NUM7 1
#define IND_NUM8 0
/*-*-*-*-*-*-*-*-*- SETUP end -*-*-*-*--*-*-*-*-*-*/

#define IND_LIGHT_OUT 0xff
#define IND_EMPTY_NUM 10  // All segments have been extinguished
#define IND_symbol_minus  11  // "-"
#define IND_symbol_P  12  // "P"
#define IND_symbol_u  13  // "u"

/* 7-segment display buffer */
uint8_t ind_buf[IND_AMOUNT_NUM];

/* conversion table */
 #if (IND_ACTIVE_LEVEL_SEG == 1)
uint8_t number[] =
{
  (1<<IND_A)|(1<<IND_B)|(1<<IND_C)|(1<<IND_D)|(1<<IND_E)|(1<<IND_F)|(0<<IND_G), //0
  (0<<IND_A)|(1<<IND_B)|(1<<IND_C)|(0<<IND_D)|(0<<IND_E)|(0<<IND_F)|(0<<IND_G), //1
  (1<<IND_A)|(1<<IND_B)|(0<<IND_C)|(1<<IND_D)|(1<<IND_E)|(0<<IND_F)|(1<<IND_G), //2
  (1<<IND_A)|(1<<IND_B)|(1<<IND_C)|(1<<IND_D)|(0<<IND_E)|(0<<IND_F)|(1<<IND_G), //3   
  (0<<IND_A)|(1<<IND_B)|(1<<IND_C)|(0<<IND_D)|(0<<IND_E)|(1<<IND_F)|(1<<IND_G), //4
  (1<<IND_A)|(0<<IND_B)|(1<<IND_C)|(1<<IND_D)|(0<<IND_E)|(1<<IND_F)|(1<<IND_G), //5 
  (1<<IND_A)|(0<<IND_B)|(1<<IND_C)|(1<<IND_D)|(1<<IND_E)|(1<<IND_F)|(1<<IND_G), //6
  (1<<IND_A)|(1<<IND_B)|(1<<IND_C)|(0<<IND_D)|(0<<IND_E)|(0<<IND_F)|(0<<IND_G), //7   
  (1<<IND_A)|(1<<IND_B)|(1<<IND_C)|(1<<IND_D)|(1<<IND_E)|(1<<IND_F)|(1<<IND_G), //8
  (1<<IND_A)|(1<<IND_B)|(1<<IND_C)|(1<<IND_D)|(0<<IND_E)|(1<<IND_F)|(1<<IND_G), //9    
  (0<<IND_A)|(0<<IND_B)|(0<<IND_C)|(0<<IND_D)|(0<<IND_E)|(0<<IND_F)|(0<<IND_G),  //IND_EMPTY_NUM 
  (0<<IND_A)|(0<<IND_B)|(0<<IND_C)|(0<<IND_D)|(0<<IND_E)|(0<<IND_F)|(1<<IND_G)  // IND_symbol_minus "-" 
};
#else 
  // fill in the blanks if necessary
#endif 



// digits
#if (IND_ACTIVE_LEVEL_DIG == 0)
uint8_t digit[] = 
{
  (1<<IND_NUM8)|(1<<IND_NUM7)|(1<<IND_NUM6)|(1<<IND_NUM5)|(1<<IND_NUM4)|(1<<IND_NUM3)|(1<<IND_NUM2)|(0<<IND_NUM1),
  (1<<IND_NUM8)|(1<<IND_NUM7)|(1<<IND_NUM6)|(1<<IND_NUM5)|(1<<IND_NUM4)|(1<<IND_NUM3)|(0<<IND_NUM2)|(1<<IND_NUM1),
  (1<<IND_NUM8)|(1<<IND_NUM7)|(1<<IND_NUM6)|(1<<IND_NUM5)|(1<<IND_NUM4)|(0<<IND_NUM3)|(1<<IND_NUM2)|(1<<IND_NUM1), 
  (1<<IND_NUM8)|(1<<IND_NUM7)|(1<<IND_NUM6)|(1<<IND_NUM5)|(0<<IND_NUM4)|(1<<IND_NUM3)|(1<<IND_NUM2)|(1<<IND_NUM1),
  (1<<IND_NUM8)|(1<<IND_NUM7)|(1<<IND_NUM6)|(0<<IND_NUM5)|(1<<IND_NUM4)|(1<<IND_NUM3)|(1<<IND_NUM2)|(1<<IND_NUM1),
  (1<<IND_NUM8)|(1<<IND_NUM7)|(0<<IND_NUM6)|(1<<IND_NUM5)|(1<<IND_NUM4)|(1<<IND_NUM3)|(1<<IND_NUM2)|(1<<IND_NUM1),
  (1<<IND_NUM8)|(0<<IND_NUM7)|(1<<IND_NUM6)|(1<<IND_NUM5)|(1<<IND_NUM4)|(1<<IND_NUM3)|(1<<IND_NUM2)|(1<<IND_NUM1),
  (0<<IND_NUM8)|(1<<IND_NUM7)|(1<<IND_NUM6)|(1<<IND_NUM5)|(1<<IND_NUM4)|(1<<IND_NUM3)|(1<<IND_NUM2)|(1<<IND_NUM1), 
};
#endif 

void Indicator_init(void)
{  
  pinMode(RCLK, OUTPUT);
  pinMode(SCLK, OUTPUT);
  pinMode(DIO, OUTPUT);

  for(uint8_t i = 0; i < IND_AMOUNT_NUM; i++) 
  {
    ind_buf[i] = 0; // Clear the indicator buffer
  }
}

/***********************************************************************************
* Function name :   IND_Output_f
* Converts a 16-bit number, writes it to the indicator buffer       
* value - conversion number 
* comma - decimal point 
* position - to which position to output the number (from right to left)
* amount - how many digits to output 
***********************************************************************************/
void IND_Output_f (int16_t value, uint8_t comma, uint8_t position, uint8_t amount)
{
  uint8_t tmp;
  uint8_t minus = 0; 
  if(value < 0)
  {
    minus = 1;
    value = value * -1;
  }
  if (comma > 0)  comma = position + comma;

  for(uint8_t i = 0; i < amount; i++)
  {
    tmp = value % 10;
    if (value > 0 || (position <= comma))
    {
      ind_buf[position] = number[tmp];
    }
    else
    {
      if (minus)
      {
        ind_buf[position] = number[IND_symbol_minus];
        break;
      }
      else
      {
        ind_buf[position] = number[IND_EMPTY_NUM];
      }
    }      
    value = value/10;
    position++;
    if (position > IND_AMOUNT_NUM)  break;
  }
    // Точка     
  if ((comma <= IND_AMOUNT_NUM) && (comma != 0)) {
    ind_buf[comma] |= 1<<IND_COM;
  }
}

/***********************************************************************************
* Function name :   IND_Update
* Outputs a number from the buffer to the indicator.
* This function must be called from a timer interrupt
***********************************************************************************/
void IND_Update(void)
{
  static uint8_t count_digit_num = 0; 

  uint8_t spi_buf[2];
	#define IND_OutSeg(port, value) port |= (value)
  #define IND_LightOutSeg(port, value) port &= ~(value)

	#define IND_OutDig(port, value) port &= ~(value)
  #define IND_LightOutDig(port, value) port |= (value)

  /* turn off the indicator */
  IND_LightOutSeg(spi_buf[1],0xff);    
  
  digitalWrite(RCLK, LOW); // release the latch
  shiftOut(DIO, SCLK, MSBFIRST, digit[count_digit_num]);		// digit on
  shiftOut(DIO, SCLK, MSBFIRST, ind_buf[count_digit_num]);	// send a byte with a “number”
  digitalWrite(RCLK, HIGH); // latch registers

  count_digit_num++;
  if (count_digit_num >= IND_AMOUNT_NUM) count_digit_num = 0;
}