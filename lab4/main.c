/* Name: Chenyang Li Partner's Name: Daozhen Lv
   Username: li413   Partner's Username: daozlv
   main.c --- 
 * 
 * Filename: main.c
 * Description: 
 * Author: 
 * Maintainer: 
 * Created: Thu Jan 10 11:23:43 2013
 * Last-Updated: 
 *           By: 
 *     Update #: 0
 * Keywords: 
 * Compatibility: 
 * 
 */

/* Commentary: 
 * 
 * 
 * 
 */

/* Change log:
 * 
 * 
 */
/* Code: */

#include <f3d_uart.h>
#include <stdio.h>
#include <stm32f30x.h>  // Pull in include files for F30x standard drivers 
#include <f3d_led.h>     // Pull in include file for the local drivers

// Simple looping delay function
void delay(void) {
  int i = 2000000;
  while (i-- > 0) {
    asm("nop"); /* This stops it optimising code out */
  }
}

int main(void) {
  f3d_uart_init();

  setvbuf(stdin, NULL, _IONBF, 0);
  setvbuf(stdout, NULL, _IONBF, 0);
  setvbuf(stderr, NULL, _IONBF, 0);

  int c;
  int cha=0;
  int line=1;
  int word=1;
  while(1){ 
    while ((getchar()) != 0x1b)   /* end on Esc */{
      // putchar(getchar());
 

      //putstring("hello World"); 
      if(c == 10){
	line = line + 1;
	word = word + 1;
	
      }
      if(c == 32){
	word = word + 1;
	cha = cha + 1;
      }
      
      else{
	cha = cha + 1;
      }
      
    }
    break;
  }
  
  
  printf("%d, %d, %d", line, word, cha);

}


#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line) {
/* Infinite loop */
/* Use GDB to find out why we're here */
  while (1);
}
#endif

/* main.c ends here */
