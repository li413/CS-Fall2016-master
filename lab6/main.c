/*
Name: Chenyang Li
Username: li413
Partner UserName : zhang435
Partner Name: Jiawei Zhang
*/
/* main.c ---
*
* Filename: main.c
* Description:
* Author:
* Maintainer:
* Created: Thu Jan 10 11:23:43 2013
/* Code: */

#include <stm32f30x.h> // Pull in include files for F30x standard drivers
#include <f3d_led.h> // Pull in include file for the local drivers
#include <f3d_uart.h>
#include <f3d_gyro.h>
#include <f3d_lcd_sd.h>
#include <f3d_delay.h>
#include <stdio.h>

#define TIMER 20000


void show_xyz_chart(){
    /*
   


   */
  // If you have your inits set up, this should turn your LCD screen red
  f3d_led_init();
  f3d_uart_init();
  f3d_gyro_init();
  f3d_led_all_on();
  f3d_lcd_init();


  // allow us to use print statement
  setvbuf(stdin, NULL, _IONBF, 0);
  setvbuf(stdout, NULL, _IONBF, 0);
  setvbuf(stderr, NULL, _IONBF, 0);


  float grav[3];


  char x[100];
  char y[100];
  char z[100];
  
  f3d_lcd_fillScreen2(RED);
  //f3d_lcd_fillScreen2(RED);
    while(1){
      f3d_gyro_getdata(grav);
      sprintf(x, "%0.4f", grav[0]);
      sprintf(y, "%0.4f", grav[1]);
      sprintf(z, "%0.4f", grav[2]);
   
      // this will paint x dimension
      f3d_lcd_drawChar(5,10,'X',GREEN,RED);
      f3d_lcd_drawString(15,10,x,GREEN,RED);
      
      // this will paint y dimension
      f3d_lcd_drawChar(5,20,'Y',GREEN,RED);
      f3d_lcd_drawString(15,20,y,GREEN,RED);
     
      // this can display z of the dimension
      f3d_lcd_drawChar(5,30,'Z',GREEN,RED);
      f3d_lcd_drawString(15,30,z,GREEN,RED);
      //f3d_lcd_drawChar(15,20,'Y',GREEN,RED);
      //f3d_lcd_drawString(20,20,x,GREEN,RED);
      f3d_lcd_drawChar(40,40,'X',BLACK,RED);
      


      // draw the x coord to figure out the differnt bwtween positive and negative number
      int temp = 0;
      for(temp = 0; temp<126;++temp){
	// f3d_lcd_drawChar(5*temp,100,'Z',BLUE,BLUE);
	f3d_lcd_drawPixel(temp , 101  , BLUE);
      }
      
      int charx = grav[0]/30 + 1;
      int chary = grav[1]/30 + 1;
      int charz = grav[2]/30 + 1;
      int count = 0;
      int signx = 1;
      int signy = 1;
      int signz = 1;
      // this linea re used to decide the side that char go to increment 
      // if the 
      if (charx<0)
	signx = -1;
      
      while(count != abs(charx) && count <=5){
	if(signx == 1 ){
	  f3d_lcd_drawChar(5,100 -signx * 8 * count,'Z',GREEN,GREEN);
	}else{
	  f3d_lcd_drawChar(5,100 -signx * 8 * count,'Z',YELLOW,YELLOW);
	}
	++count;
      }
       if (chary<0)
	 signy = -1;

       count = 0;
      while(count != abs(chary) &&  count <=5){
	if(signy == 1 ){
	  f3d_lcd_drawChar(60,100 -signy * 8 * count,'Z',GREEN,GREEN);
	}else{
	  f3d_lcd_drawChar(60,100 -signy * 8 * count,'Z',YELLOW,YELLOW);
	}
	++count;
      }

        if (charz<0)
	 signz = -1;

       count = 0;
      while(count != abs(charz) &&  count <=5){
	if(signz == 1 ){
	  f3d_lcd_drawChar(100,100 -signz * 8 * count,'Z',GREEN,GREEN);
	}else{
	  f3d_lcd_drawChar(100,100 -signz * 8 * count,'Z',YELLOW,YELLOW);
	}
	++count;
      }
     
      delay(50);
      
      count = 0;
      while(count != 10){
	f3d_lcd_drawChar(5,100 - signx   * 8 * count,'Z',RED,RED);
	f3d_lcd_drawChar(60,100 - signy  * 8 * count,'Z',RED,RED);
	f3d_lcd_drawChar(100,100 - signz * 8 * count,'Z',RED,RED);
	++count;
      }
      delay(100);  
      
    }
}

int main(void) {
  show_xyz_chart();
 //  char y*;
  //sprintf(y, "%f", grav[1]);
  
  //  char z* = "";
  //sprintf(z, "%f", grav[2]);
  //f3d_lcd_fillScreen2(RED);
  //char *x = "apple";
  //f3d_lcd_drawString(30,10,x,GREEN,YELLOW);
  

  
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line) {
/* Infinite loop */
/* Use GDB to find out why we're here */
  while (1);
}
#endif

/* main.c ends here */
