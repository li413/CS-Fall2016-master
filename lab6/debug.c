//main.c for lab6
/*
Name: Chenyang Li
Username: li413

Partner UserName : zhang435
Partner Name: Jiawei Zhang


 */
#include <stm32f30x.h>
#include <f3d_uart.h>
#include <stdio.h>
#include <f3d_gyro.h>
#include <f3d_led.h>
#include <f3d_user_btn.h>

void delay(void) {

  int i = 2000000;
  while (i-- > 0) {
    asm("nop"); /* This stops it optimising code out */
  }
}

int main(void){
  // init light , print grav sensor , and user button
  f3d_led_init();
  //f3d_uart_init();
  //f3d_gyro_init();
  //f3d_user_btn_init();

  // allow us to use print statement
  setvbuf(stdin, NULL, _IONBF, 0);
  setvbuf(stdout, NULL, _IONBF, 0);
  setvbuf(stderr, NULL, _IONBF, 0);
  int count  = 0;
while(1){
  f3d_led_all_on(10);
  printf("%i\n",count);\
  count++;
  printf("%i\n",count);
  count++;
  printf("%i\n",count);
  count++;
  printf("%i\n",count);
  count++;
  printf("%i\n",count);
  count++;  
  printf("%i\n",count);
  count++; 
  printf("%i\n",count);
  count++;
  printf("%i\n",count);
  count++;
  printf("%i\n",count);
  count++;
  printf("%i\n",count);
  count++;
  printf("%i\n",count);
  count++;
  printf("%i\n",count);
  count++;  
  printf("%i\n",count);
  count++; 
  printf("%i\n",count);
  count = 0;
  f3d_led_all_off();
  delay();
} 
  return 0 ;
}

void assert_failed(uint8_t* file, uint32_t line) {
  /* Infinite loop */
  /* Use GDB to find out why we're here */
  while (1);
}
