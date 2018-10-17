/* f3d_nunchuk.c --- 
 * 
 * Filename: f3d_nunchuk.c
 * Description: 
 * Author: Bryce Himebaugh
 * Maintainer: 
 * Created: Thu Oct 31 09:40:37 2013
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
 * JUN WANG lab8
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
#include <f3d_nunchuk.h>
#include <f3d_lcd_sd.h>
#include <stdio.h>

#define NUNCHUK_ADDRESS 0xA4

void f3d_nunchuk_init(void) {
  uint8_t realb = 0x00;
  uint8_t buf = 0x55;
  uint8_t buf2 = 0x00;

  
  // Use for factory Wii Nunchuk controllers
  // f3d_i2c1_write(NUNCHUK_ADDRESS,0x40,&realb);
  // Use for 3 party nunchuks like we have in the lab
  f3d_i2c1_write(NUNCHUK_ADDRESS,0xf0,&buf); 
  f3d_i2c1_write(NUNCHUK_ADDRESS,0xfb,&buf2);
  
}

void f3d_nunchuk_read(nunchuk_t *n) {
  uint8_t data[6] = {0,0,0,0,0,0};
  int i;
  unsigned short tmp;
  
  f3d_i2c1_read_nunchuk(0xA5, data, 6);
  n->jx = data[0];
  n->jy = data[1];
  
  tmp = data[2];
  tmp = tmp << 2;
  n->ax = tmp | ((data[5] & 0x0C) >> 2);

  tmp = data[3];
  tmp = tmp << 2;
  n->ay = tmp | ((data[5] & 0x30) >> 4);
  
  tmp = data[4];
  tmp = tmp << 2;
  n->az = tmp | ((data[5] & 0xC0) >> 6);
  
  switch (data[5] & 0x03) {
  case 2:
    n->z = 1;
    n->c = 0;
    break;
  case 1:
    n->z = 0;
    n->c = 1;
    break;
  case 0:
    n->z = 1;
    n->c = 1;
    break;
  case 3:
    n->z = 0;
    n->c = 0;
    break;
  }
}

void f3d_nunchuk_screen(){
  nunchuk_t data;
  short x,y,z;
  char string_x[10];
  char string_y[10];
  char string_z[10];
  f3d_nunchuk_read(&data);
  x = data.ax;
  y = data.ay;
  z = data.az;

   
    sprintf(string_x,"%d", x);
    f3d_lcd_drawChar( 5, 0, 'X', WHITE, RED );
    f3d_lcd_drawString( 25, 0, string_x , WHITE, RED );
    lcd_draw_bar(WHITE, x / 10, 15);
 



    sprintf(string_y," %d", y);
    f3d_lcd_drawChar(5, 50, 'Y', WHITE, RED );
    f3d_lcd_drawString(25, 50, string_y, WHITE, RED );
    lcd_draw_bar(WHITE, y/10, 65 ); 

  

  
    sprintf(string_z," %d", z);
    f3d_lcd_drawChar( 5, 100, 'Z', WHITE, RED );
    f3d_lcd_drawString( 25, 100, string_z , WHITE, RED);  
    lcd_draw_bar( WHITE, z/10, 115);

    delay(20);
    lcd_draw_bar(RED, x / 5, 15);
    lcd_draw_bar(RED, y / 5, 65 );
    lcd_draw_bar(RED, z / 5, 115 );
}

/* f3d_nunchuk.c ends here */
