/*
 * SSD1306xLED - Drivers for SSD1306 controlled dot matrix OLED/PLED 128x64 displays
 *
 * @created: 2014-08-12
 * @author: Neven Boyanov
 *
 * Source code available at: https://bitbucket.org/tinusaur/ssd1306xled
 *
 */

// ----------------------------------------------------------------------------
#include <stdlib.h>
#include <avr/io.h>

#include <avr/pgmspace.h>
#include "ssd1306xled.h"
#include "font6x8.h"



// Some code based on "IIC_wtihout_ACK" by http://www.14blog.com/archives/1358

const uint8_t ssd1306_init_sequence [] PROGMEM = {	// Initialization Sequence
	0xAE,			// Display OFF (sleep mode)
	0x20, 0b00,		// Set Memory Addressing Mode
					// 00=Horizontal Addressing Mode; 01=Vertical Addressing Mode;
					// 10=Page Addressing Mode (RESET); 11=Invalid
	0xB0,			// Set Page Start Address for Page Addressing Mode, 0-7
	0xC8,			// Set COM Output Scan Direction
	0x00,			// ---set low column address
	0x10,			// ---set high column address
	0x40,			// --set start line address
	0x81, 0x3F,		// Set contrast control register
	0xA1,			// Set Segment Re-map. A0=address mapped; A1=address 127 mapped.
	0xA6,			// Set display mode. A6=Normal; A7=Inverse
	0xA8, 0x3F,		// Set multiplex ratio(1 to 64)
	0xA4,			// Output RAM to Display
					// 0xA4=Output follows RAM content; 0xA5,Output ignores RAM content
	0xD3, 0x00,		// Set display offset. 00 = no offset
	0xD5,			// --set display clock divide ratio/oscillator frequency
	0xF0,			// --set divide ratio
	0xD9, 0x22,		// Set pre-charge period
	0xDA, 0x12,		// Set com pins hardware configuration
	0xDB,			// --set vcomh
	0x20,			// 0x20,0.77xVcc
	0x8D, 0x14,		// Set DC-DC enable
	0xAF			// Display ON in normal mode

};

// Program:    5248 bytes

SSD1306Device::SSD1306Device(void){}


void SSD1306Device::ssd1306_init(void)
{
	TinyWireM.begin();

	for (uint8_t i = 0; i < sizeof (ssd1306_init_sequence); i++) {
		ssd1306_send_command(pgm_read_byte(&ssd1306_init_sequence[i]));
	}
}

void SSD1306Device::ssd1306_send_byte(uint8_t type, uint8_t byte){
	if (TinyWireM.write(byte) == 0) {
	ssd1306_send_stop();
	ssd1306_send_start(type);
		TinyWireM.write(byte);
	}
}

void SSD1306Device::ssd1306_send_start(uint8_t type) {
	TinyWireM.beginTransmission(SSD1306_SA);
	TinyWireM.write(type);
	}

void SSD1306Device::ssd1306_send_stop(void) {
	TinyWireM.endTransmission();
}

void SSD1306Device::ssd1306_send_command(uint8_t command)
{
	ssd1306_send_start(0x00);
	ssd1306_send_byte(0x00, command);
	ssd1306_send_stop();
}

void SSD1306Device::ssd1306_setpos(uint8_t x, uint8_t y)
{
	ssd1306_send_start(0x00);
	ssd1306_send_byte(0x00, 0xb0 + y);
	ssd1306_send_byte(0x00, ((x & 0xf0) >> 4) | 0x10); // | 0x10
	ssd1306_send_byte(0x00, (x & 0x0f) | 0x01); // | 0x01
	ssd1306_send_stop();
}

void SSD1306Device::ssd1306_fillscreen(uint8_t fill)
{
uint8_t m,n;
	for (m = 0; m < 8; m++)
	{
		ssd1306_send_command(0xb0 + m);	// page0 - page1
		ssd1306_send_command(0x00);		// low column start address
		ssd1306_send_command(0x10);		// high column start address
		ssd1306_send_start(0x40);
		for (n = 0; n < 128; n++)
		{
			ssd1306_send_byte(0x40, fill);
		}
		ssd1306_send_stop();
	}
}

void SSD1306Device::ssd1306_char_font6x8(char ch) {
	uint8_t i;
	uint8_t c = ch - 32;
	ssd1306_send_start(0x40);
	for (i= 0; i < 6; i++)
	{
		ssd1306_send_byte(0x40, pgm_read_byte(&ssd1306xled_font6x8[c * 6 + i]));
	}
	ssd1306_send_stop();
}

void SSD1306Device::ssd1306_string_font6x8(char *s) {
	while (*s) {
		ssd1306_char_font6x8(*s++);
	}
}

void SSD1306Device::ssd1306_draw_bmp(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, const uint8_t bitmap[])
{
	uint16_t j = 0;
	uint8_t y, x;
	if (y1 % 8 == 0) y = y1 / 8;
	else y = y1 / 8 + 1;
	for (y = y0; y < y1; y++)
	{
		ssd1306_setpos(x0,y);
		ssd1306_send_start(0x40);
		for (x = x0; x < x1; x++)
		{
			ssd1306_send_byte(0x40, pgm_read_byte(&bitmap[j++]));
		}
		ssd1306_send_stop();
	}
}


SSD1306Device SSD1306;

// ----------------------------------------------------------------------------
