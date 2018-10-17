/*
Name: Chenyang Li Partner Name: Samual Eleftheri
Username: li413 Partner's Username: selefthe
 main.c ---
*
* Filename: main.c
* Description: create accel data graph and compass lights
* Author: Samuel Eleftheri
          Chenyang Li
* Maintainer:
* Created: Thu Jan 10 11:23:43 2013
* Updated: Wed Oct 19 15:55 2016
/* Code: */

#include <stm32f30x.h> // Pull in include files for F30x standard drivers
#include <f3d_led.h>
#include <f3d_uart.h>
#include <f3d_user_btn.h>
#include <f3d_lcd_sd.h>
#include <f3d_i2c.h>
#include <f3d_accel.h>
#include <f3d_mag.h>
#include <stdio.h>
#include <math.h> //atan sqrt and pow(value, power)

#define PI 3.14159265
#define TIMER 20000

void delay(int count) {
  while (count-- > 0) {
    int i = 10000; 
    while (i-- > 0) {
      asm("nop");/* This stops it optimizing code out */
    }
  }
}

int main(void) {

  // print
  setvbuf(stdin, NULL, _IONBF, 0);
  setvbuf(stdout, NULL, _IONBF, 0);
  setvbuf(stderr, NULL, _IONBF, 0);

  // Inits 
  f3d_led_init(); // led
  f3d_lcd_init(); // lcd
  f3d_user_btn_init(); // user button
  f3d_uart_init(); // uart
  delay(10);
  f3d_i2c1_init();
  delay(10);
  f3d_accel_init();
  delay(10);
  f3d_mag_init();
  delay(10);

  float data[3];
  int display; // which display to show

  // accel
  float pitch;
  float roll;

  // mag
  float magX;
  float magY;
  float magZ;

  // compass needle
  float Heading;

  // Graph Setup
  char str_tilt[12]; // for float -> string conversion
  f3d_lcd_fillScreen(BLACK);  
  f3d_lcd_drawChar(10, 40, 'P', WHITE, BLACK); // Pitch
  f3d_lcd_drawChar(10, 80, 'R', WHITE, BLACK); // Roll
   
  while(1) {
    while (user_btn_read()) { // button input (switch displays)
      if (display) 
	display = 0; 
      else 
	display = 1;
    }
    if (display == 1) { // compass lights
      f3d_accel_read(data);
      pitch = atan(data[0]/(sqrt(pow(data[1],2) + pow(data[2], 2)))); // x axis rotation
      roll = atan(data[1]/(sqrt(pow(data[0],2) + pow(data[2], 2)))); // y axis rotation
      
      f3d_mag_read(data);
      magX = data[0]*cos(pitch)+data[2]*sin(pitch); // mag sensor X
      magY = data[0]*sin(roll)*sin(pitch)+data[1]*cos(roll)-data[2]*sin(roll)*cos(pitch); // mag sensor Y
      magZ = -data[0]*cos(roll)*sin(pitch)+data[1]*sin(roll)+data[2]*cos(roll)*cos(pitch); // mag sensor Z
      
    // Get Heading
      if (magX > 0 && magY >= 0){
	Heading = atan(magY/magX);
      } 
      if (magX < 0){
	Heading = PI + atan(magY/magX);
      } 
      if (magX > 0 && magY <= 0){
	Heading = 2*PI + atan(magY/magX);
      } 
      if (magX == 0 && magY < 0){
	Heading = PI/2;
      }
      if (magX == 0 && magY > 0){
	Heading = PI*1.5;
      } 
      
      printf("%f\n", Heading);
      
      // Compass Lights
      if (Heading < 1.1 && Heading > 0.25) {
	f3d_led_off(4);
	f3d_led_off(2);
	f3d_led_on(3);
      }
      if (Heading < 2.15 && Heading >= 1.1) {
	f3d_led_off(3);
	f3d_led_off(1);
	f3d_led_on(2);
      }
      if (Heading < 2.95 && Heading >= 2.15) {
	f3d_led_off(2);
	f3d_led_off(0);
	f3d_led_on(1);
      }
      if (Heading < 3.6 && Heading >= 2.95) {
	f3d_led_off(7);
	f3d_led_off(1);
	f3d_led_on(0);
      }
      if (Heading < 4.4 && Heading >= 3.6) {
	f3d_led_off(0);
	f3d_led_off(6);
	f3d_led_on(7);
      }
      if (Heading < 5.2 && Heading >= 4.4) {
	f3d_led_off(7);
	f3d_led_off(5);
	f3d_led_on(6);
      }
      if (Heading < 5.8 && Heading >= 5.2){
	f3d_led_off(6);
	f3d_led_off(4);
	f3d_led_on(5);
      }
      if (Heading < 0.25 || Heading >= 5.8) {
	f3d_led_off(5);
	f3d_led_off(3);
	f3d_led_on(4);
      }   
    }
    else { // bar graph
      f3d_accel_read(data);
      pitch = atan(data[0]/(sqrt(pow(data[1],2) + pow(data[2], 2)))); // x axis rotation
      roll = atan(data[1]/(sqrt(pow(data[0],2) + pow(data[2], 2)))); // y axis rotation

      if (pitch < 0) pitch *= -1;
      if (roll < 0) roll *= -1;

      // pitch text value
      sprintf(str_tilt, "%.3f", pitch);
      f3d_lcd_drawString(25, 40, "     ", BLACK, BLACK); // clears unnecessary digits
      f3d_lcd_drawString(25, 40, str_tilt, WHITE, BLACK);

      // roll text value
      sprintf(str_tilt, "%.3f", roll);
      f3d_lcd_drawString(25, 80, "     ", BLACK, BLACK); // clears unnecessary digits
      f3d_lcd_drawString(25, 80, str_tilt, WHITE, BLACK);
      
      pitch = pitch * 100;
      roll = roll * 100;

      // pitch bar
      f3d_lcd_barFill(50, (int)pitch, RED); 

      // roll bar
      f3d_lcd_barFill(90, (int)roll, YELLOW);
    }
  }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line) {
/* Infinite loop */
/* Use GDB to find out why we're here */
  while (1);
}
#endif

/* main.c ends here */

