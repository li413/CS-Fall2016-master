/* main.c --- 
 * 
 * Filename: main.c
 * Description: 
 * Author: chenyang li(li413) ke zhao (zhaok)
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


#include <stm32f30x.h>  
#include <f3d_led.h>   
#include <f3d_uart.h>
#include <f3d_gyro.h>
#include <f3d_dac.h>
#include <f3d_lcd_sd.h>
#include <f3d_i2c.h>
#include <f3d_accel.h>
#include <f3d_mag.h>
#include <f3d_nunchuk.h>
#include <f3d_rtc.h>
#include <f3d_systick.h>
#include <ff.h>
#include <diskio.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TIMER 20000
#define AUDIOBUFSIZE 128

extern uint8_t Audiobuf[AUDIOBUFSIZE];
extern int audioplayerHalf;
extern int audioplayerWhole;

FATFS Fatfs;	
FIL fid;	
BYTE Buff[512];		
int ret;

struct ckhd {
  uint32_t ckID;
  uint32_t cksize;
};

struct fmtck {
  uint16_t wFormatTag;      
  uint16_t nChannels;
  uint32_t nSamplesPerSec;
  uint32_t nAvgBytesPerSec;
  uint16_t nBlockAlign;
  uint16_t wBitsPerSample;
};

void readckhd(FIL *fid, struct ckhd *hd, uint32_t ckID) {
  f_read(fid, hd, sizeof(struct ckhd), &ret);
  if (ret != sizeof(struct ckhd)) {
    exit(-1);
  }
  if (ckID && (ckID != hd->ckID)) {
    exit(-1);
  }
}

void die (FRESULT rc) {
  printf("Failed with rc=%u.\n", rc);
  while (1);
}

int main(void) {
  setvbuf(stdin, NULL, _IONBF, 0);
  setvbuf(stdout, NULL, _IONBF, 0);
  setvbuf(stderr, NULL, _IONBF, 0);

  f3d_uart_init();
  delay(10);
  f3d_delay_init();
  delay(10);
  f3d_lcd_init();
  delay(10);
  f3d_i2c1_init();
  delay(10);
  f3d_accel_init();
  delay(10);
  f3d_nunchuk_init();
  delay(10);
  f3d_timer2_init();
  delay(10);
  f3d_dac_init();
  delay(10);
  f3d_rtc_init();
  delay(10);
  f3d_systick_init();
  delay(10);

  nunchuk_t nunc;
  nunchuk_t *nun = &nunc;
  int music = 0;

  const int fileNum = 3;
  char *musicFile[fileNum];
  musicFile[0] = "thermo.wav";
  musicFile[1] = "ammas.wav";
  musicFile[2] = "start.wav";

  printf("Reset\n");


  f3d_lcd_fillScreen(BLACK);
  f3d_lcd_drawString(37, 0, musicFile[0], RED, BLACK);
  f3d_lcd_drawString(37, 20 * 1, musicFile[1], BLUE, BLACK);
  f3d_lcd_drawString(37, 20 * 2, musicFile[2], BLUE, BLACK);
  

  f_mount(0, &Fatfs);
  f3d_nunchuk_read(nun);
  while(1) {
    f3d_nunchuk_read(nun);
    int lastSelect = music;
    //right
    if (nun->jx == 255) {
      music = (music + 1) % 3;
      f3d_lcd_drawString(37, lastSelect * 20, musicFile[lastSelect], BLUE, BLACK);
      f3d_lcd_drawString(37, music * 20, musicFile[music], RED, BLACK);
      //left
    } else if (nun->jx == 0) {
      music = (music + 2) % 3;
      f3d_lcd_drawString(37, lastSelect * 20, musicFile[lastSelect], BLUE, BLACK);
      f3d_lcd_drawString(37, music * 20, musicFile[music], RED, BLACK);
    }
  

  if (nun->z) {
    FRESULT rc;     
    DIR dir;   
    FILINFO fno;     
    UINT bw, br;
    unsigned int retval;
    int bytesread;

    printf("\nOpen %s\n", musicFile[music]);
    rc = f_open(&fid, musicFile[music], FA_READ);

    printf("opened file with rc: %d\n", rc);

    if (!rc) {
      printf("Success opening file\n");
      struct ckhd hd;
      uint32_t  waveid;
      struct fmtck fck;
    
      readckhd(&fid, &hd, 'FFIR');
    
      f_read(&fid, &waveid, sizeof(waveid), &ret);
      if ((ret != sizeof(waveid)) || (waveid != 'EVAW'))
	return -1;
    
      readckhd(&fid, &hd, ' tmf');
    
      f_read(&fid, &fck, sizeof(fck), &ret);
    
 
    
      if (hd.cksize != 16) {
	printf("extra header info %d\n", hd.cksize - 16);
	f_lseek(&fid, hd.cksize - 16);
      }
    
      printf("audio format 0x%x\n", fck.wFormatTag);
      printf("channels %d\n", fck.nChannels);
      printf("sample rate %d\n", fck.nSamplesPerSec);
      printf("data rate %d\n", fck.nAvgBytesPerSec);
      printf("block alignment %d\n", fck.nBlockAlign);
      printf("bits per sample %d\n", fck.wBitsPerSample);
    

      while(1){
	printf("skipping all non-data chunks\n");
	readckhd(&fid, &hd, 0);
	if (hd.ckID == 'atad')
	  break;
	f_lseek(&fid, hd.cksize);
      }
    
      printf("Samples %d\n", hd.cksize);
    

    
      f_read(&fid, Audiobuf, AUDIOBUFSIZE, &ret);
      hd.cksize -= ret;
      audioplayerStart();
      while (hd.cksize) {
	int next = hd.cksize > AUDIOBUFSIZE/2 ? AUDIOBUFSIZE/2 : hd.cksize;
	if (audioplayerHalf) {
	  if (next < AUDIOBUFSIZE/2)
	    bzero(Audiobuf, AUDIOBUFSIZE/2);
	  f_read(&fid, Audiobuf, next, &ret);
	  hd.cksize -= ret;
	  audioplayerHalf = 0;
	}
	if (audioplayerWhole) {
	  if (next < AUDIOBUFSIZE/2)
	    bzero(&Audiobuf[AUDIOBUFSIZE/2], AUDIOBUFSIZE/2);
	  f_read(&fid, &Audiobuf[AUDIOBUFSIZE/2], next, &ret);
	  hd.cksize -= ret;
	  audioplayerWhole = 0;
	}
      }
      audioplayerStop();
    }

    printf("\nClose the file.\n"); 
    rc = f_close(&fid);
  
    if (rc) die(rc);

    delay(1000);
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
