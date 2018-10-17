//Name : Chenyang Li(li413) Partner Name: Brad Jones(Bradj)


#include <stm32f30x.h>
#include <f3d_led.h>
#include <f3d_uart.h>
#include <f3d_gyro.h>
#include <f3d_lcd_sd.h>
#include <f3d_i2c.h>
#include <f3d_accel.h>
#include <f3d_mag.h>
#include <f3d_nunchuk.h>
#include <f3d_rtc.h>
#include <ff.h>
#include <math.h>
#include <diskio.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
void init() {
           printf("Initialize starting\n");
        setvbuf(stdin, NULL, _IONBF, 0);
        setvbuf(stdout, NULL, _IONBF, 0);
        setvbuf(stderr, NULL, _IONBF, 0);
        f3d_uart_init();
        f3d_lcd_init();
        f3d_delay_init();
        f3d_rtc_init();
        f3d_led_init();
        f3d_led_all_off();
        f3d_i2c1_init();
        delay(10);
        f3d_accel_init();
        delay(10);
        f3d_mag_init();
        delay(10);
        f3d_nunchuk_init();
        delay(10);
        f3d_rtc_init();
        delay(10);
        f3d_user_btn_init();
        delay(10);
        printf("All initialized\n");
}

FATFS Fatfs;/* File system object */
FIL fid;/* File object */
//BYTE Buff[512];/* File read buffer */
BYTE Buff[128];/* File read buffer */
nunchuk_t nun;
uint8_t side[]={0x02,0x03,0x05,0x06};
char *pics[]={"cat1.bmp","cat2.bmp","cat3.bmp"};

typedef struct {
  unsigned char magic [2];
} bmpfile_magic;

typedef struct {
  uint32_t filesz ;
  uint16_t creator1 ;
  uint16_t creator2 ;
  uint32_t bmp_offset ;
} bmpfile_header;

typedef struct {
  uint32_t header_sz ;
  int32_t width ;
  int32_t height ;
  uint16_t nplanes ;
  uint16_t bitspp ;
  uint32_t compress_type ;
  uint32_t bmp_bytesz ;
  int32_t hres;
  int32_t vres;
  uint32_t ncolors ;
  uint32_t nimpcolors ;
} BITMAPINFOHEADER ;

typedef struct { // little endian byte order
  uint8_t b;
  uint8_t g;
  uint8_t r;
} bmppixel;

bmpfile_magic magic;
bmpfile_header header;
BITMAPINFOHEADER info;


uint16_t convertColor(bmppixel* color);
void truncateImageBottom();
void readHeaders();

static const uint16_t height_width_difference = ST7735_height - ST7735_width;

void die (FRESULT rc) {
        printf("Failed with rc=%u.n", rc);
        while (1) ;
}

uint16_t convertColor(bmppixel *color) {  
  uint16_t r = color->r >> 3;
  uint16_t g = color->g >> 2;
  uint16_t b = color->b >> 3;

  uint16_t pixel = r;
  pixel |= (g << 5);
  pixel |= (b << 11);
  return pixel;
}

void truncateImageBottom() {
  UINT br;
  bmppixel trashPixelBuffer[ST7735_width];
  int i;
  for (i = 0; i < height_width_difference; i++) {
    f_read(&fid, (void *) trashPixelBuffer, sizeof(bmppixel) * ST7735_width, &br);
  }
}

void readHeaders() {
  UINT br;
  // read bmp headers
  f_read(&fid, (void *) &magic, 2, &br);
  f_read(&fid, (void *) &header, sizeof(header), &br);
  f_read(&fid, (void *) &info, sizeof(info), &br);

  // @hack - we have to throw away a bunch of bytes here
  //         for the image to line up correctly
  const int TRASH_BYTES = sizeof(bmppixel) * 28;
  bmppixel bmpTrash[TRASH_BYTES];
  //f_read(&Fil, (void *) bmpTrash, TRASH_BYTES, &br);
}

int gyro(){
float a[3];
float temp,pitch,roll;

f3d_accel_read(a);
temp=(a[1]*a[1])+(a[2]*a[2]);
pitch=atan(a[0]/sqrt(temp));
pitch=pitch*(180/M_PI);
temp=(a[2]*a[2])+(a[0]*a[0]);
roll=atan(a[1]/sqrt(temp));
roll=roll*(180/M_PI);
if(roll>1){
return 1;
}
else if(roll<-1){
return 2;
}
else if(pitch>1){
return 3;
}
else{
return 0;
}
}

void drawpicture(char* x,uint8_t way){
    char footer[20];
    int count=0;
    int i;
    FRESULT rc;         /* Result code */
    DIR dir;            /* Directory object */
    FILINFO fno;            /* File information object */
    UINT bw, br;
    unsigned int retval;
    f_mount(0, &Fatfs); /* Register volume work area (never fails) */
    char fname[10];
    strcpy(fname, x);
    rc = f_open(&fid, fname, FA_READ);
        if (rc) die(rc);
    readHeaders(); 

    int row, col;
    bmppixel bmpBuffer[sizeof(bmppixel) * ST7735_width];
    uint16_t rowBuffer[ST7735_width];

    int border = 0;
        for (row = border; row <= ST7735_height - 1 - border; row++) {
            // read in one row of the image
            f_read(&fid, (void *) bmpBuffer, sizeof(bmppixel) * ST7735_width, &br);
        for (col = ST7735_width - 1; col >= 0; col--) {
        // convert bmp pixels to ST7735 pixels
        rowBuffer[col] = convertColor(&bmpBuffer[col]);
        }
        // write to screen
        f3d_lcd_setAddrWindow(0, row, ST7735_width - 1, row + 1,way);
        f3d_lcd_pushColor(rowBuffer, ST7735_width);
    }


}

int main(void) {
      int c=0;
      int e;
      int f=0;
        init();
        f3d_lcd_fillScreen(WHITE);
 while(1){
      e=gyro();
    f3d_nunchuk_read(&nun);   
  if(e!=f){
      f3d_lcd_fillScreen(WHITE);
      drawpicture(pics[c],side[e]);
      f=e;     
    delay(100);
     }

     

   else if(nun.z==1){
      c++;
    if(c==3){
    c=0;
   }
   f3d_lcd_fillScreen(WHITE);
   drawpicture(pics[c],side[e]);
delay(100);
}
else if(nun.c==1){
c--;
if(c==-1){
c=2;
}
f3d_lcd_fillScreen(WHITE);
drawpicture(pics[c],side[e]);
delay(100);
}
     
delay(100);
}
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line){
while(1);
}
#endif
