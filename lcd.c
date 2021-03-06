/*
 * lcd.c
 * Library for the Epson SED1335 LCD Controller
 *
 * Created on: 2009-11-08
 * John Hughes <jondo2010@gmail.com>
 */

#include <73x_lib.h>

#include "lcd.h"

void comm_out (u8 j) //Command Output 8-bit Bus Interface
{
	//A0  = 1;
	GPIO_BitWrite( GPIO3, LE, Bit_SET );
	GPIO_ByteWrite(GPIO3, GPIO_LSB, 0x01);
	GPIO_BitWrite( GPIO3, LE, Bit_RESET );

	GPIO_ByteWrite(GPIO3, GPIO_LSB, j);

	GPIO_BitWrite(GPIO3, WR, Bit_RESET);
	delay(1);
	GPIO_BitWrite(GPIO3, WR, Bit_SET);

	delay(1);
}

void data_out (u8 i) //Data Output 16-bit Bus Interface
{
	//A0  = 0;
	GPIO_BitWrite( GPIO3, LE, Bit_SET );
	GPIO_ByteWrite(GPIO3, GPIO_LSB, 0x00);
	GPIO_BitWrite( GPIO3, LE, Bit_RESET );

	GPIO_ByteWrite(GPIO3, GPIO_LSB, i);

	GPIO_BitWrite(GPIO3, WR, Bit_RESET);
	delay(1);
	GPIO_BitWrite(GPIO3, WR, Bit_SET);
	delay(1);
}

void
lcd_set_scroll
(
  uint16_t  textAddr,
  uint8_t   textHeight,
  uint16_t  picAddr,
  uint8_t   picHeight
)
{
	comm_out (SCROLL);
	data_out ((uint8_t) textAddr); // P1: (SAD 1L)
	data_out (textAddr >> 8); // P2: (SAD 1H)
	data_out (textHeight); // P3: (SL 1)

	data_out ((uint8_t) picAddr); // P4: (SAD 2L)
	data_out (picAddr >> 8); // P5: (SAD 2H)
	data_out (picHeight); // P6: (SL 2)

	data_out (0x00); // P7: (SAD 3L)
	data_out (0x00); // P8: (SAD 3H)
	data_out (0x00); // P9: (SAD 4L)
	data_out (0x00); // P10: (SAD 4H)
}

void
lcd_set_hscroll
(
  uint8_t position
)
{
	comm_out (HDOT_SCR);
	data_out (position);
}

void
lcd_set_overlay
(
  uint8_t overlay
)
{
	comm_out (OVLAY); // Set display overlay format
	data_out (overlay);
}

void 
lcd_set_cursor_pointer 
(
  uint16_t addr
)
{
	comm_out (CSRW);
	data_out ((u8)addr);
	data_out (addr >> 8);
}

void
lcd_draw_string
(
  uint8_t *string,
  uint16_t addr,
  uint8_t length
)
{
	lcd_set_cursor_pointer (addr);
	comm_out (MWRITE);
	for (u8 n=0; n<length; n++)
	{
		data_out (*string++);
	}
}

void
lcd_clear_screen ()
{
	uint8_t n;

	lcd_set_cursor_pointer (0x0000);
	comm_out (MWRITE);
	for (n=0;n<1200;n++)
		data_out (0x20); // Space character

	lcd_set_cursor_pointer (0x04b0);
	comm_out (MWRITE);
	for (n=0;n<9600;n++)
		data_out (0x00);
}

void
lcd_reset ()
{
	GPIO_BitWrite ( GPIO3, LE, Bit_SET );

	GPIO_BitWrite (GPIO3, RD, Bit_SET);
	GPIO_BitWrite (GPIO3, WR, Bit_SET);

	GPIO_BitWrite ( GPIO3, RESET, Bit_RESET );
	delay(1000);
	GPIO_BitWrite ( GPIO3, RESET, Bit_SET );
	delay(1000);
}

void
lcd_system_set
(
  uint8_t cr,
  uint8_t tcr
)
{
	comm_out (SYSTEM_SET);
	delay (5);
	//data_out(PIXEL_CHARS16 | INVERSE_COMPENSATION); // P1: (0 0 IV 1 W/S M2 M1 M0)
	data_out (PIXEL_CHARS8);
	data_out (TWO_FRAME_AC_DRIVE | WIDE_CHARS8); // P2: (WF 0 0 0 0 FX1 FX2 FX3)
	data_out (HIGH_CHARS8); // P3: (0 0 0 0 FY1 FY2 FY3 FY4)
	data_out (cr); // P4: (C/R 0..7) 39d characters per display line
	data_out (tcr); // P5: (TC/R 0..7) 57d total characters
	data_out (0xEF); // P6: (L/F 0..7) 239 Lines per frame
	data_out (0x28); // P7: (APL 0..7) 40 addresses per line virtual
	data_out (0x00); // P8: (APH 0..7)
}

void
lcd_set_display_state
(
  uint8_t on_off,
  uint8_t cursor_state,
  uint8_t sad1_state,
  uint8_t sad2_state,
  uint8_t sad3_state
)
{
	if (on_off)
		comm_out (DISP_ON);
	else
		comm_out (DISP_OFF);

	/* [FP5 FP4] [FP3 FP2] [FP1 FP0] FC1 FC0
	 * FC0,1: the cursor state
	 * FP0,1: sad1 state
	 * FP2,3: sad2 state
	 * FP4,5: sad3 state
	 */
	delay(5);

	data_out (
    cursor_state |
    (sad1_state << 2) |
    (sad2_state << 4) |
    (sad3_state << 6)
  );
}

void
lcd_set_cursor_form
(
  uint8_t cursor_width,
  uint8_t cursor_height,
  uint8_t cursor_type
)
{
	comm_out (CSRFORM);
	data_out ((cursor_width - 1) & 0x0f);
	data_out (cursor_type | ((cursor_height -1) & 0x0f));
}

void
lcd_set_cursor_direction
(
  uint8_t cursor_direction
)
{
	comm_out (CSRDIR | (cursor_direction & 0x03));
}

void
lcd_init ()
{
	lcd_set_scroll (0x0000, 0xef, 0x04b0, 0xef);
	lcd_set_hscroll (0x00); // Set horizontal scroll position
	lcd_set_overlay (0x00);

	// Turn off display, set blinking cursor, all display blocks steady on
	lcd_set_display_state (0, CURSOR_BLINK_2HZ, SAD_STEADY, SAD_STEADY, SAD_STEADY);
	lcd_set_cursor_form (5, 7, BLOCK_CURSOR); // Set a 4x6 block cursor
	lcd_set_cursor_direction (CURSOR_RIGHT);

	// Turn display back on, cursor blinking, display blocks 1 and 2 steady, 3 off.
	lcd_set_display_state (1, CURSOR_BLINK_2HZ, SAD_STEADY, SAD_STEADY, SAD_OFF);
}
