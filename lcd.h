/*
 * lcd.h
 * Library for communicating with the Epson SED1335 LCD Controller
 *
 * Created on: 2009-11-08
 * John Hughes <jondo2010@gmail.com>
 */

#ifndef LCD_H_
#define LCD_H_

#include <73x_type.h>

#define WR		GPIO_PIN_15 /* pg0 */
#define RD		GPIO_PIN_14 /* pg1 */
#define LE		GPIO_PIN_13 /* PG2 */
#define RESET	GPIO_PIN_12

/// System commands
#define SYSTEM_SET		0x40
#define SCROLL			0x44
#define HDOT_SCR		0x5A
#define OVLAY			0x5B
#define DISP_OFF		0x58
#define DISP_ON			0x59
#define CSRFORM			0x5D
#define CSRW			0x46
#define MWRITE			0x42
#define CSRDIR			0x4C

/// Defines for System Set command
#define EXTERNAL_CG_ROM 0x01 /* Otherwise internal */
#define D6_CORRECTION 0x02
#define PIXEL_CHARS8 0x10 /* Character height */
#define PIXEL_CHARS16 0x14
#define DUAL_PANEL 0x08 /* Otherwise Single */
#define INVERSE_COMPENSATION 0x20

#define WIDE_CHARS8 0x07 /* 8 pixel wide chars */
#define TWO_FRAME_AC_DRIVE 0x80

#define HIGH_CHARS8 0x07 /* 8 pixel high chars */

/// Defines for overlay settings
#define COMPOSE_OR			0x00
#define COMPOSE_XOR			0x01
#define COMPOSE_AND			0x02
#define COMPOSE_PRI_OR		0x03

#define LAYER1_GRAPHICS		0x04
#define LAYER3_GRAPHICS		0x08

#define THREE_LAYER_COM		0x10

/// Defines for lcd state
#define CURSOR_OFF			0x00
#define CURSOR_STEADY		0x01
#define CURSOR_BLINK_2HZ	0x02
#define CURSOR_BLINK_1HZ	0x03

#define SAD_OFF				0x00
#define SAD_STEADY			0x01
#define SAD_BLINK_2HZ		0x02
#define SAD_BLINK_16HZ		0x03

/// Defines for cursor form
#define UNDERSCORE_CURSOR	0x00
#define BLOCK_CURSOR		0x80

/// Defines for cursor direction
#define CURSOR_RIGHT		0x00
#define CURSOR_LEFT			0x01
#define CURSOR_UP			0x02
#define CURSOR_DOWN			0x03

/**
 * Command Output 8-bit Bus Interface
 */
void comm_out (u8 j);

/**
 * Data Output 16-bit Bus Interface
 */
void data_out (u8 i);

/**
 * Set the scroll position for the first 2 layers
 */
void 
lcd_set_scroll
(
  uint16_t textAddr,
  uint8_t textHeight,
  uint16_t picAddr,
  uint8_t picHeight
);

/**
 * Set horizontal scroll position in pixels
 */
void
lcd_set_hscroll
(
  uint8_t position
);

/**
 * Set overlay parameters
 */
void
lcd_set_overlay
(
  uint8_t overlay
);

/**
 * Set the cursor address
 */
void
lcd_set_cursor_pointer
(
  uint16_t addr
);

/**
 * Draw a string starting at this address
 */
void
lcd_draw_string
(
  uint8_t *string,
  uint16_t addr,
  uint8_t length
);

/**
 * Clear the LCD by filling the graphics layer with 0s and the text layer with space chars.
 */
void
lcd_clear_screen ();

/**
 * Hard reset of LCD
 */
void
lcd_reset ();

/**
 * Call system set command
 */
void
lcd_system_set
(
  uint8_t cr,
  uint8_t tcr
);

/**
 * Set lcd on/off status, cursor and screen blocks states
 */
void
lcd_set_display_state
(
  uint8_t on_off,
  u8 cursor_state,
  u8 sad1_state,
  u8 sad2_state,
  u8 sad3_state
);

/**
 * Set the LCD cursor form.
 * Width in pixels from 1 to 16
 * Height in pixels from 2 to 16.
 * cursor_type is either UNDERSCORE_CURSOR or BLOCK_CURSOR
 */
void
lcd_set_cursor_form
(
  u8 cursor_width,
  u8 cursor_height,
  u8 cursor_type
);

/**
 * Set the LCD cursor increment direction
 */
void
lcd_set_cursor_direction
(
  u8 cursor_direction
);

/**
 * Initialize the LCD
 */
void
lcd_init ();

#endif /* LCD_H_ */
