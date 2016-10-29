#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
//#include <avr/cpufunc.h>

volatile uint16_t switch_count = 0;
int one = 0, ten = 0, hundred = 0, thousand = 0;

/************************************************************************
Just like bit_is_clear() but made it check a variable
************************************************************************/

uint8_t var_bit_is_clr(uint8_t test_var, uint8_t bit) {
 if((test_var & (1 << bit))  == 0){
  return  1;//TRUE;
 }
 else{
  return 0;//FALSE;
 }
}//var_bit_is_clr

/************************************************************************
Checks the state of the button number passed to it. It shifts in ones till
the button is pushed. Function returns a 1 only once per debounced button
push so a debounce and toggle function can be implemented at the same time.
Adapted to check all buttons from Ganssel's "Guide to Debouncing"
Expects active low pushbuttons on a port variable.  Debounce time is
determined by external loop delay times 12.
Note: that this function is checking a variable, not a port.
************************************************************************/

uint8_t debounceSwitch(uint8_t button_var, uint8_t button) {
 static uint16_t state[8]= {0,0,0,0,0,0,0,0};  //holds shifted in bits from buttons
 state[button] = (state[button] << 1) | (! var_bit_is_clr(button_var, button)) | 0xE000;
 if (state[button] == 0xF000){
  return 1; //if held true for 12 passes through
 }
 else{
  return 0;
 }
} //debounceSwitch

void buttonSense(){
  if(switch_count >=1023){
   switch_count = 1;
   return;
  }
  if(debounceSwitch(PINA, 0)){
    switch_count++;
   }
  if(debounceSwitch(PINA, 1)){
    switch_count=switch_count+2;
   }
  if(debounceSwitch(PINA,2)){
    switch_count=switch_count+4;
   }
  if(debounceSwitch(PINA,3)){
    switch_count = switch_count+8;
   }
  if(debounceSwitch(PINA,4)){
    switch_count=switch_count+16;
   }
  if(debounceSwitch(PINA,5)){
    switch_count=switch_count+32;
   }
  if(debounceSwitch(PINA,6)){
    switch_count=switch_count+64;
   }
  if(debounceSwitch(PINA,7)){
    switch_count=switch_count+128;
   }
}

// converts decimal value to 7-seg display
void LEDSegment(int x){ 
 if(x == 0){
 PORTA = 0xC0;
 }
 if(x == 1){
  PORTA = 0xF9; 
 } 
 if(x == 2){
  PORTA = 0xA4; 
 } 
 if(x == 3){
  PORTA = 0xB0; 
 } 
 if(x == 4){
  PORTA = 0x99; 
 } 
 if(x == 5){
  PORTA = 0x92; 
 } 
 if(x == 6){
  PORTA = 0x82; 
 } 
 if(x == 7){
  PORTA = 0xF8; 
 } 
 if(x == 8){
  PORTA = 0x80; 
 }
 if(x == 9){
  PORTA = 0x98;
 }
 if(x > 9){    			//error
  PORTA = 0x00;
 }
}
// saves the ones, tens, hundreds, thousands place of switch_count
position(uint16_t x){
 int value = x;
 one = value %10;
 value -= one;
 
 ten = (value %100)/10;
 value -= ten;
 
 hundred = (value %1000)/100;
 value -= hundred;

 thousand = (value %10000)/1000;
 
}

void inputSet(){
 PORTB = 0x70;                          // tristate buffer for pushbuttons enabled
 DDRA = 0x00;                           // PORTA set as input
 PORTA = 0xFF;                          // PORTA as pullups
 _delay_ms(2);
}

void outputSet(){
 DDRA = 0xFF;                           // segments on/pushbuttons off
 PORTA = 0xFF;                          // set segments to default off
 _delay_ms(2);
}

int main(){
// initialize
 DDRA = 0xFF; 				// segments on/pushbuttons off
 DDRB = 0xF0;				// pin 4-7 on portb set to output
 PORTB = 0x00;				// digit 4. one = 0x00 ten = 0x10 
					//hundred = 0x30 thousand = 0x40
 while(1){
  inputSet();
  buttonSense();			// sets switch_count. Based on button press

  position(switch_count);               // saves one, ten, hundred, thousand of switch_count. 

  outputSet();				// switches from push buttons to display

  PORTB = 0x00;				// setting digit position 
  LEDSegment(one);			// settings segments based on digit position
  _delay_ms(1);				// without delay -> ghosting
  PORTA = 0xFF;			 	// eliminates all ghosting

//same as above step but for digit3
  PORTB = 0x10;
  LEDSegment(ten);
  _delay_ms(1);
  PORTA = 0xFF;			

  PORTB = 0x30;
  LEDSegment(hundred);
  _delay_ms(1);
  PORTA = 0xFF;			 

  PORTB = 0x40;
  LEDSegment(thousand);
  _delay_ms(1); 
  PORTA = 0xFF;			 	
  

}//while


}//main
