/*Name: Chenyang Li Username: li413
  Partner Name: Bokai Zhuang Partner Username: bzhuang
*/
//main.c for lab6
#include <f3d_uart.h>
#include <stdio.h>
#include <f3d_gyro.h>
#include <f3d_led.h>
#include <f3d_user_btn.h>


void delay(void) {
  int i = 1800000;
  while (i-- > 0){
    asm("nop");
  }
}
int main(void){

  f3d_uart_init(); //init of uart so we can have stuff print.
  delay();
  f3d_gyro_init(); //init of gyro
  f3d_led_init();
  f3d_user_btn_init();
  delay();
  //What other init(s) would we need if we want to utilize those handy dandy led_ons and check_button_pressed's or whatever?

  setvbuf(stdin, NULL, _IONBF, 0); //clearing the stdbuffers// 
  setvbuf(stdout, NULL, _IONBF, 0);
  setvbuf(stderr, NULL, _IONBF, 0);

  float a[3];
  int axis = 0;
  char caxis[] = {'X','Y','Z'};
  while(1){
    int  c2 = getcharplus();
    switch(c2){
    case 'x':
      axis = 0;
      break;
    case 'y':
      axis = 1;
      break;
    case 'z':
      axis = 2;
      break;
    }
    c2=0;
    f3d_gyro_getdata(a);
    if(user_btn_read()){ 
      axis++; 
      if(axis==3) 
	axis=0; 
    } 
    printf("%c-axis:%f\n",caxis[axis],a[axis]);
    f3d_led_all_off(); 
    if(a[axis]<0){
      if(a[axis]<-20)
	f3d_led_on(1);
      if(a[axis]<-40)
	f3d_led_on(2);
      if(a[axis]<-60)
	f3d_led_on(3);
      if(a[axis]<-80)
	f3d_led_on(4);
      if(a[axis]<-100)
	f3d_led_on(5);
    }else{
      if(a[axis]>20)
	f3d_led_on(1);
      if(a[axis]>40)
	f3d_led_on(0);
      if(a[axis]>60)
	f3d_led_on(7);
      if(a[axis]>80)
	f3d_led_on(6);
      if(a[axis]>100)
	f3d_led_on(5);
    }
    delay();
  }
}
  //Do whatever, call whatever function you want, as long as it and the device has been initialized
  
  

void assert_failed(uint8_t* file, uint32_t line) {
/* Infinite loop */
/* Use GDB to find out why we're here */

  while (1);
}
