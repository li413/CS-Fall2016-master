/* f3d_mag.c --- 
 * Name: Chenyang Li Partner Name: Samual Eleftheri                                
   Username: li413 Partner's Username: selefthe   
 * Filename: f3d_mag.c
 * Description: 
 * Author: Bryce Himebaugh
 * Maintainer: 
 * Created: Thu Oct 31 09:27:11 2013
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

/* Copyright (c) 2004-2007 The Trustees of Indiana University and 
 * Indiana University Research and Technology Corporation.  
 * 
 * All rights reserved. 
 * 
 * Additional copyrights may follow 
 */

/* Code: */


#include <f3d_i2c.h>
#include <f3d_mag.h>
#include <f3d_delay.h>
#include <f3d_accel.h>
#include <f3d_mag.h>
#include <math.h>

void f3d_mag_init() {
  // MAG I2C Address = 0x3C 

  uint8_t value = 0; 
 
  value = 0x14;                  //Temp sensor disable,30Hz Output Rate 
  f3d_i2c1_write(0x3C, 0x00,  &value); // Mag (0x3C), CRA (0x00) 

  value = 0xE0;                      // +/- 8.1 Gauss Full Scale
  f3d_i2c1_write(0x3C, 0x01, &value); // Mag (0x3C), CRB (0x01)

  value = 0x00;                      // Continuous Conversion
  f3d_i2c1_write(0x3C, 0x02, &value); // Mag (0x3C), MR  (0x23)
}

void f3d_mag_read(float *mag_data) {
  uint8_t buffer[6];
  int i;
  
  f3d_i2c1_read(0x3C, 0x03, buffer,2);   // Read X Axis
  f3d_i2c1_read(0x3C, 0x07, buffer+2,2); // Read Y Axis
  f3d_i2c1_read(0x3C, 0x05, buffer+4,2); // Read Z Axis (notice that Z is out of order in the chip). 
  
  for (i=0; i<2; i++) {
    mag_data[i]=(float)((int16_t)(((uint16_t)buffer[2*i] << 8) + buffer[2*i+1]))/230;
  }
  mag_data[2]=(float)((int16_t)(((uint16_t)buffer[4] << 8) + buffer[5]))/205;
}



float f[3];
float x, y, z,pitch, roll, xH, yH, heading;

void f3d_mag_screen(){
    f3d_led_all_off();
    f3d_accel_read(f); 
    x = f[0];
    y = f[1];
    z = f[2];
    //  pitch = atan(   x    / (y^2 + z^2) )^(1/2)
    pitch = atan2f( x,  pow( pow( y, 2.0 ) + pow( z, 2.0 ), 0.5 ) ) ;
    
    //	roll = atan( y / (x^2 + z^2) ^(1/2)
    roll = atan2f( y,  pow( pow( x, 2.0 ) + pow( z, 2.0 ), 0.5 ) ) ;
    


 // Compass data --> xh, yh, heading
      f3d_mag_read(f);
      x = f[0];
      y = f[1];
      z = f[2];
      // xH = X * cos(pitch) + z * sin(pitch)	
      xH = x * cosf(pitch) + z *sinf(pitch);
      
      // yh = x * sin(roll) * sin(pitch) + (y*cos(roll)) - (z * sin(roll) * cos(pitch))
      yH = x * sinf(roll) * sinf(pitch) + (y * cosf(roll)) - (z * sin(roll) * cos(pitch) );
      // heading = (atan2(yh, xh) * 180 ) /M_PI
      heading = atan2f(yH, xH) * 180 /M_PI;
      
      // compass visualizaion --> use led to visualize the north direction
      //range for heading is -135~135 --34  
      //want the lcd light always point to north
      if(heading >= -157.5 && heading < -114.5)
	f3d_led_on(0);
      if(heading >= -114.5 && heading < -67.5)
	f3d_led_on(7);
      if(heading >= -67.5 && heading < -22.5)
	f3d_led_on(6);
      if(heading >= -22.5 && heading < 22.5)
	f3d_led_on(5);
      if(heading >= 22.5 && heading < 67.5)
	f3d_led_on(4);
      if(heading >= 67.5 && heading < 114.5)
	f3d_led_on(3);
      if(heading >= 114.5 && heading < 157.5)
	f3d_led_on(2);
      if(heading >= 157.5 || heading < -157.5)
	f3d_led_on(1);

      delay(50);
      f3d_led_all_off(); }





/* f3d_mag.c ends here */
