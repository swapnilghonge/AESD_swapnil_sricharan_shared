// Base 20x4 character LCD drive over I2C without external libraries

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#define I2C_ADDR   0x3F // I2C device address

// Define start addresses of lines 0 to 3
#define Line0  0x80
#define Line1  0xC0
#define Line2  0x94
#define Line3  0xD4

// Define the control bits (low 4 bits of PCF8574)
#define LCD_DAT         0b00000001  // Sending data bit on bit off cmd
#define LCD_READ        0b00000010  // Set bit to assert read line
#define LCD_ENABLE      0b00000100  // Enable bit toggle to latch data
#define LCD_BACKLIGHT   0b00001000  // Set bi to turn backlight on

int fd;  // file descriptor seen by all subroutines

// Write byte to LCD in one IIC transaction
void lcd_wbyte(int bits, int mode)  
{
	uint8_t b[8];  // Temp buffer for transaction
	b[0] = b[1] = b[2] = mode | LCD_BACKLIGHT | (bits & 0xF0);
	b[1] |= LCD_ENABLE; b[2] &= ~LCD_ENABLE;
	b[3] = b[4] = b[5] = mode | LCD_BACKLIGHT | ((bits << 4) &0xF0);
	b[4] |= LCD_ENABLE; b[5] &= ~LCD_ENABLE;
	write(fd, b, 6);
}

// clr lcd go home loc 0x80
void ClrLcd(void)   {
  lcd_wbyte(0x01, 0);
  usleep(2000);
  lcd_wbyte(0x02, 0);
}

// Initialise display to 4 bit as per the datasheet
void lcd_init()   
{
	const uint8_t b[4] = {0x30, 0x30 | LCD_ENABLE, 0x30}; // Sequence to clock 3 into upper 4 bits
	write(fd, b, 3);  usleep(6000); write(fd, b, 3);  // Prepare for 4 bit width?
	lcd_wbyte(0x32, 0); // Force 4 bit width
	lcd_wbyte(0x06, 0); // Cursor move direction
	lcd_wbyte(0x0C, 0); // 0x0F On, Blink Off
	lcd_wbyte(0x28, 0); // Data length, number of lines, font size
	ClrLcd();
}

// out char to LCD at current position
void typeChar(char val)   { lcd_wbyte(val, LCD_DAT); }

// this allows use of any size string
// Could consider making this quicker by turning it into a single I2C write
void typeln(const char *st)   { while ( *st ) lcd_wbyte(*(st++), LCD_DAT); }

// go to location on LCD
void lcdLoc(int line)   { lcd_wbyte(line, 0); }

int main()   
{
  if ((fd = open("/dev/i2c-1", O_RDWR)) < 0) return -1 ; // Check interface for device
  if (ioctl(fd, I2C_SLAVE, I2C_ADDR) < 0) return -1 ;
  lcd_init(); // setup LCD
  while (1)   
  {
	lcdLoc(Line0); typeln("Raw IIC write");
    lcdLoc(Line1+1); typeln("Line 1");
	lcdLoc(Line2+2); typeln("Line 2");
	lcdLoc(Line3+3); typeln("Line 3");
    sleep(2);
    ClrLcd();
	sleep(1);
  }
  return 0;
}


