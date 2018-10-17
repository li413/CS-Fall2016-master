/*Name: Chenyang Li Username: li413
  Partner Name: Bokai Zhuang Partner Username: bzhuang
*/
#include <f3d_gyro.h>
#include <stm32f30x.h>
#include <f3d_lcd_sd.h>
#include <f3d_delay.h>
#include <stdio.h> 
void f3d_gyro_interface_init() {
  /**********************************************************************/
  /************** CODE HERE *********************************************/
  //You must configure and initialize the following 4 pins

  //SCK PA5 //MOSI PA6 
  
  //MISO PA7
 GPIO_InitTypeDef GPIO_InitStructure;
RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOE, ENABLE);
RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

GPIO_StructInit(&GPIO_InitStructure);
GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6| GPIO_Pin_7;
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
GPIO_Init(GPIOA,&GPIO_InitStructure);
GPIO_PinAFConfig(GPIOA,5,GPIO_AF_5);
GPIO_PinAFConfig(GPIOA,6,GPIO_AF_5); 
GPIO_PinAFConfig(GPIOA,7,GPIO_AF_5); 


  //MOSI PA6 
  
  //MISO PA7

  //CS PE3
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; //Specifications for Pin 3.
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOE,&GPIO_InitStructure);

  
  //set the CS high
  
  /**********************************************************************/
   
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
  //SPI Initialization and configuration
  SPI_InitTypeDef SPI_InitStructure;
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_Init(SPI1, &SPI_InitStructure);
  SPI_RxFIFOThresholdConfig(SPI1, SPI_RxFIFOThreshold_QF);
  SPI_Cmd(SPI1, ENABLE);
}

//the init function to be called in your main.c
void f3d_gyro_init(void) {
  //
  //SETTING THE CONTROL REGISTERS 
  f3d_gyro_interface_init();
  // CTRL1 Register 
  // Bit 7:6 Data Rate: Datarate 0
  // Bit 5:4 Bandwidth: Bandwidth 3
  // Bit 3: Power Mode: Active
  // Bit 2:0 Axes Enable: X,Y,Z enabled
  uint8_t ctrl1;
  uint8_t ctrl4;
	
  ctrl1 |= (uint8_t) (((uint8_t)0x00)|		
		      ((uint8_t)0x30)|		
		      ((uint8_t)0x08)|		
		      ((uint8_t)0x07));
  // CTRL4 Register 
  // Bit 7 Block Update: Continuous */
  // Bit 6 Endianess: LSB first  */
  // Bit 5:4 Full Scale: 500 dps */
  ctrl4 |= (uint8_t) (((uint8_t)0x00)|			
		      ((uint8_t)0x00)|				     
		      ((uint8_t)0x10));

  f3d_gyro_write(&ctrl1, 0x20, 1);
  f3d_gyro_write(&ctrl4, 0x23, 1);

}
//to read from it
void f3d_gyro_read(uint8_t* pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead) {
  //are we reading one byte or more than one byte???
  if (NumByteToRead > 1) {
    ReadAddr |= (uint8_t)(0x80 | 0x40); // sets to multibyte mode
  }
  else {
    ReadAddr |= (uint8_t) (0x80); // sets to read mode (first bit 1)
  }
  //setting chip select to low (LETS TALK!)
  GYRO_CS_LOW();
  //sending address byte
  f3d_gyro_sendbyte(ReadAddr);  
  while(NumByteToRead > 0x00) {
    //WE are now sending dummy data so we can read the valuable!
    //remember we must write to read!
    //putting the information in the buffer
    *pBuffer = f3d_gyro_sendbyte(((uint8_t)0x00));
    NumByteToRead--;
    pBuffer++;
  }
  //setting chip select to high (DONE TALKING)
  GYRO_CS_HIGH();//setting chip select to high (DONE TALKING)
}

/*writing function*/
void f3d_gyro_write(uint8_t* pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite) {
  /****************************************************************************/
  /************** CODE HERE *********************************************/

  //CHECK TO SEE HOW MANY BYTES AND HANDLE THAT CORRECTLY
  if (NumByteToWrite > 1) {
		// multiple bytes to write, set bit 6 of WriteAddr
		WriteAddr |= (uint8_t)(0x40);
	} 

	GYRO_CS_LOW();

	f3d_gyro_sendbyte(WriteAddr);
	
	while(NumByteToWrite > 0x00) {
		f3d_gyro_sendbyte(*pBuffer);
		NumByteToWrite--;
		pBuffer++;
	}

	GYRO_CS_HIGH();
  //SET THE CS

  //SEND THE FIRST BYTE

  //IF MULTIPLE, SEND THE ADDITIONAL

  /***************************************************************************/
}

/*sends the bytes*/
static uint8_t f3d_gyro_sendbyte(uint8_t byte) {
  /*********************************************************/
  /***********************CODE HERE ************************/
while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET); 
SPI_SendData8(SPI1, byte);

while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
return (uint8_t)SPI_ReceiveData8(SPI1);




  /********************** CODE HERE ************************/
  /*********************************************************/
}
/*gets the data*/
void f3d_gyro_getdata(float *pfData) {
  //
  //
  int16_t RawData[3];
  uint8_t tmpbuffer[6];
  int i;
  //We are going to write some data to the gyro
  f3d_gyro_write(tmpbuffer,0x28,6);
  //Then we are going to read it
  f3d_gyro_read(tmpbuffer,0x28,6);
  //casting the data retreiving from tmpbuffer to raw data
  for(i=0; i<3; i++) {
    RawData[i]=(int16_t)(((uint16_t)tmpbuffer[2*i+1] << 8) + tmpbuffer[2*i]);
  }
  //adjusting the data with the sensitivity
  for(i=0; i<3; i++) {
    pfData[i]=(float)RawData[i]/L3G_Sensitivity_500dps;
  }
}


void f3d_gyro_screen(){
  /*f3d_uart_init();
  f3d_lcd_init();
  f3d_delay_init();
  f3d_gyro_init();

  setvbuf(stdin, NULL, _IONBF, 0);
  setvbuf(stdout, NULL, _IONBF, 0);
  setvbuf(stderr, NULL, _IONBF, 0);*/
  
  float f[3];
  float one ,two, three;
  char string_x[10];
  char string_y[10];
  char string_z[10];
  
  f3d_gyro_getdata(f);

  one = f[0];
  if (one >= 0) {
    sprintf(string_x,"%f", one);
    f3d_lcd_drawChar( 5, 0, 'X', WHITE, RED );
    f3d_lcd_drawString( 25, 0, string_x , WHITE, RED );
    lcd_draw_bar(WHITE, one / 5, 15);
  } else {
    sprintf(string_x,"%f", one);
    f3d_lcd_drawChar( 5, 0, 'X', WHITE, RED );
    f3d_lcd_drawString( 25, 0, string_x , WHITE, RED );
    one = one * -1;
    lcd_draw_bar(BLACK, one / 5, 15);
  }
  
  two = f[1];
  if (two >= 0) {
    sprintf(string_y," %f", two);
    f3d_lcd_drawChar(5, 50, 'Y', WHITE, RED );
    f3d_lcd_drawString(25, 50, string_y, WHITE, RED );
    lcd_draw_bar(WHITE, two/5, 65 ); 
  }else{
    sprintf(string_y," %f", two);
    f3d_lcd_drawChar(5, 50, 'Y', WHITE, RED );
    f3d_lcd_drawString(25, 50, string_y, WHITE, RED );
    two = two * -1;
    lcd_draw_bar(BLACK, two/5, 65 );
  }
  
  
  three = f[2];
  if (three >= 0) {
    sprintf(string_z," %f", three);
    f3d_lcd_drawChar( 5, 100, 'Z', WHITE, RED );
    f3d_lcd_drawString( 25, 100, string_z , WHITE, RED);  
    lcd_draw_bar( WHITE, three/5, 115);
  } else {
    sprintf(string_z," %f", three);
    f3d_lcd_drawChar( 5, 100, 'Z', WHITE, RED );
    f3d_lcd_drawString( 25, 100, string_z , WHITE, RED); 
    three *= -1;  
    lcd_draw_bar( BLACK, three / 5, 115);
  }
  delay(50);
  lcd_draw_bar(RED, one / 5, 15);
  lcd_draw_bar(RED, two / 5, 65 );
  lcd_draw_bar(RED, three / 5, 115 );	
}
