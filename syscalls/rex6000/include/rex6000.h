/*********************************************************************
 *
 * Copyright (C) 2001-2006,  Simon Kagstrom <ska@bth.se>
 *
 * Filename:      sysdeps.h
 * Description:   This file contains the defined functions that
 *                each frontend (drawing, keyboard input etc)
 *                should implement.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA  02111-1307, USA.
 *
 * $Id: sysdeps.h $
 *
 *********************************************************************/
#ifndef __REX6000_H__
#define __REX6000_H__
#if defined(__cplusplus)
extern "C" {
#endif

#include <java/lang.h>
#include <javax/microedition/lcdui.h>
#include <javax/microedition/lcdui/game.h>
#include <javax/microedition/media.h>
#include <resource-manager.h>

#include <stdint.h>

/** @file
 *
 * Platform-independent library for REX 6000 programs. The library is
 * optimized for the REX, so most of these functions have a directly
 * corresponding REX system call. Most calls can be defined as macros
 * in order to save execution time and space.
 *
 * The library has been created in order to make development of REX
 * programs easier, mostly for my own good. Why not just write a
 * wrapper around the REX system calls for some other architecture?
 * Well, there are a few calls here that don't have any equivavilent
 * in the REX library, like fe_load_data().
 *
 * The library can be compiled for zcc, sdcc (mostly) and gcc with
 * libSDL so far.
 */

/* Some convenient definitions */
#undef TRUE
#define TRUE  1

#undef FALSE
#define FALSE 0

#ifndef NULL
#define NULL ((void*)0)
#endif /* NULL */

/* The size of a REX screen */
#define FE_PHYS_WIDTH      240 /**< The physical width of a REX screen, in pixels */
#define FE_PHYS_HEIGHT     120 /**< The physical height of a REX screen, in pixels */
#define FE_PHYS_BYTE_WIDTH (FE_PHYS_WIDTH/8)

/* The events. 16 possible (since a 16-bit number is used) */
#define FE_EVENT_NONE    0
#define FE_EVENT_EXIT    1  /* The home button was pressed */
#define FE_EVENT_BACK    2  /* The back button was pressed */
#define FE_EVENT_SELECT  4  /* The select button was pressed */
#define FE_EVENT_LEFT    8  /* The left/up button was pressed */
#define FE_EVENT_RIGHT   16 /* The right/down button was pressd */

#define FE_FONT_NORMAL   0  /* Normal font */
#define FE_FONT_INVERTED 1  /* Inverted font */
#define FE_FONT_BOLD     16 /* Bold text */
#define FE_FONT_SMALL    32 /* Small text */


/**
 * Structure for representing points (for stylus-use). This is exactly
 * as found on the REX.
 */
typedef struct
{
  uint16_t x; /**< X position of the stylus. */
  uint16_t y; /**< Y position of the stylus. */
} fe_point_t;

/*
 * Structure for representing events, like key-presses and
 * stylus-pointing. This is exactly as found on the REX.
 *
 * @todo this is not yet used and contains bogus values.
 */
typedef struct
{
  uint8_t tmp;
} fe_event_t;

/**
 * Fill an area of the screen with black color.
 *
 * @param x the x-position to start on.
 * @param y the y-position to start on.
 * @param w the width to fill.
 * @param h the height to fill.
 *
 * @see fe_clear_area()
 * @see fe_draw_bitmap()
 */
void fe_fill_area(uint8_t x, uint8_t y, uint8_t w, uint8_t h); /* Not generated */

/**
 * Clear an area of the screen (white color)
 *
 * @param x the x-position to start on.
 * @param y the y-position to start on.
 * @param w the width to clear.
 * @param h the height to clear.
 *
 * @see fe_fill_area()
 */
void fe_clear_area(uint8_t x, uint8_t y, uint8_t w, uint8_t h); /* Not generated */

/**
 * Clear the entire screen.
 *
 * @see fe_clear_area()
 */
#define fe_clear_screen() (fe_clear_area(0,0, FE_PHYS_WIDTH, FE_PHYS_HEIGHT))

/**
 * Set a pixel to black color.
 *
 * @param x the x-position of the pixel.
 * @param y the y-position of the pixel.
 */
void fe_set_pixel(uint8_t x, uint8_t y); /* Not generated */

/**
 * Clear a pixel to white/green color.
 *
 * @param x the x-position of the pixel.
 * @param y the y-position of the pixel.
 */
void fe_clear_pixel(uint8_t x, uint8_t y); /* Not generated */


/**
 * Draw a bitmap on the screen. Note that this currently only accepts
 * bitmaps with a width being a multiple of 8 (for speed
 * reasons). This function works regardless of if the bitmap is
 * located on the stack or in addin code.
 *
 * The function can draw bitmaps both inverted and non-inverted.
 *
 * @param p_bitmap the bitmap to draw.
 * @param x the x-position to draw the bitmap on.
 * @param y the y-position to draw the bitmap on.
 * @param inverted the mode to draw in (0 normal, 1 inverted).
 *
 * @todo the type of p_bitmap should be changed to fe_image_t so that
 *  we can easily provide other graphics for non-REX targets.
 * @warning the bitmap has to have a multiple-of-8 width.
 * @bug fe_draw_bitmap does not work with tiles of width 32.
 */
void fe_draw_bitmap(uint8_t* p_bitmap, uint8_t x, uint8_t y, uint8_t inverted); /* Not generated */

/**
 * Get the stylus position, if used. The position is stored in a
 * fe_point_t structure.
 *
 * @param p_point the stylus position. This will contain bogus values
 *                if the screen wasn't touched.
 * @return TRUE if the screen was touched, FALSE otherwise.
 */
uint8_t fe_get_stylus(fe_point_t* p_point); /* Not generated */

/**
 * Check input from the keys. Returns a bitmask of the touched keys. The
 * possible values are:
 *
 * - <TT>FE_EVENT_NONE</TT>:   No event (nothing pressed).
 * - <TT>FE_EVENT_EXIT</TT>:   Exit key pressed (Esc in SDL).
 * - <TT>FE_EVENT_BACK</TT>:   Back key pressed (Backspace in SDL).
 * - <TT>FE_EVENT_SELECT</TT>: Select/enter key pressed (Space/Ctrl in SDL).
 * - <TT>FE_EVENT_LEFT</TT>:   Left/down key pressed (Left in SDL).
 * - <TT>FE_EVENT_RIGHT</TT>:  Right/up key pressed (Right in SDL).
 *
 * Check events by AND:ing the result with the desired event:
 * <PRE>
 * uint16_t buttons = fe_get_buttons();
 * if (buttons & FE_EVENT_EXIT)
 *  ...
 * </PRE>
 *
 * @return a bitmask of the keys that were pressed, or 0 if no keys
 *         were pressed.
 */
uint16_t fe_get_buttons(void); /* Not generated */

/*
 * Add an event-point (like a button to press). The event is also made
 * active.
 *
 * @param x the x-position of the button
 * @param y the y-position of the button
 * @param w the width of the button
 * @param h the height of the button
 * @param id the identifier to associate with the event
 *
 * @todo This function is not yet implemented.
 */
void fe_add_event(uint8_t x, uint8_t y, uint8_t w, uint8_t h, int16_t id); /* Not generated */

/*
 * Delete an event-point (like a button to press).
 *
 * @param id the identifier of the event to be deleted
 *
 * @todo This function is not yet implemented.
 */
void fe_del_event(int16_t id); /* Not generated */

/*
 * Wait blocking for an event. When an event occurs, the passed event
 * structure is filled in. This function also returns a bitmask of the
 * currently pressed keys, see fe_get_buttons() for more information.
 *
 * @param p_event a pointer to the event structure to be filled in.
 *
 * @return a bitmask of the keypresses.
 *
 * @see fe_get_buttons()
 * @see fe_get_stylus()
 *
 * @todo This function is not yet implemented
 */
uint16_t fe_wait_event(fe_event_t* p_event); /* Not generated */

/**
 * Print a string somewhere on the screen. A bitwise combination of
 * the following constants for mode is possible:
 *
 * - <TT>FE_FONT_NORMAL</TT>:    Normal.
 * - <TT>FE_FONT_INVERTED</TT>:  Inverted.
 * - <TT>FE_FONT_BOLD</TT>:      Bold.
 * - <TT>FE_FONT_SMALL</TT>:     Small.
 *
 * Do this by OR:ing the constants together, i.e.:
 * <PRE>
 * fe_print_xy(0,0, FE_FONT_NORMAL | FE_FONT_BOLD, "I print this!");
 * </PRE>
 *
 * Note however that it is not possible to combine @c FE_FONT_BOLD and
 * @c FE_FONT_SMALL.
 *
 * @param x the x-position of the string.
 * @param y the y-position of the string.
 * @param mode the character type to print in.
 * @param p_str the string to be printed.
 *
 * @bug The SDL-port does not fully emulate the behaviour of @c
 *  fe_print_xy(). The normal font is always shown, and the font does
 *  not look quite the same as on the REX (but has approximately the
 *  same size).
 */
void fe_print_xy(uint8_t x, uint8_t y, uint8_t mode, char* p_str); /* Not generated */

/**
 * Load data from a memo. Memos can only, unfortunately, store text.
 *
 * @param p_dst a pointer to the buffer to store the data in.
 * @param offset the offset into the memo to begin reading from.
 * @param len the number of bytes to read
 * @param p_filename the memo name.
 *
 * @return 0 if everything was OK, non-zero value otherwise.
 *
 * @warning I've had problems with this before, and bugs might still
 *  be lurking around here. Seems to be working, though.
 */
uint8_t fe_load_data(char* p_dst, uint16_t offset, uint16_t len, char* p_filename); /* Not generated */


/**
 * Initialize the frontend. This does things like set the graphics
 * mode etc and should be called at program start. It is very
 * frontend-dependent.
 */
void fe_init(void); /* Not generated */

/**
 * Clean up after the frontend (clear the screen, exit the program
 * etc). This should be called when the program exits.
 */
void fe_finalize(void); /* Not generated */

/**
 * Sleep for a number of ticks (each tick being 1/64s).
 *
 * @param ticks the number of ticks to sleep.
 *
 * @warning passing 0 here will cause the REX to lock (not emulated in
 *  the SDL-port).
 */
void fe_sleep(uint16_t ticks); /* Not generated */

#if defined(__cplusplus)
}
#endif
#endif /* !__REX6000_H__ */
