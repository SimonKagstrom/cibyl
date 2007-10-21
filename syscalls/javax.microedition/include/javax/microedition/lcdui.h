/*********************************************************************
 *
 * Copyright (C) 2006,  Blekinge Institute of Technology
 *
 * Filename:      lcdui.h
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Cibyl J2ME API
 *
 * $Id: lcdui.h 14095 2007-03-10 07:45:25Z ska $
 *
 ********************************************************************/
/* From GameCanvas class */
#ifndef __JAVAX__MICROEDITION__LCDUI_H__
#define __JAVAX__MICROEDITION__LCDUI_H__
#if defined(__cplusplus)
extern "C" {
#endif

#include <cibyl.h>
#include <stdlib.h>
#include "midlet.h"

/* From the Canvas class */
#define NOPH_Canvas_UP                      1
#define NOPH_Canvas_DOWN                    6
#define NOPH_Canvas_LEFT                    2
#define NOPH_Canvas_RIGHT                   5
#define NOPH_Canvas_FIRE                    8
#define NOPH_Canvas_GAME_A                  9
#define NOPH_Canvas_GAME_B                 10
#define NOPH_Canvas_GAME_C                 11
#define NOPH_Canvas_GAME_D                 12
#define NOPH_Canvas_KEY_STAR       ((int)'*')
#define NOPH_Canvas_KEY_POUND      ((int)'#')

#define NOPH_Canvas_KEY_NUM0       ((int)'0')
#define NOPH_Canvas_KEY_NUM1       ((int)'1')
#define NOPH_Canvas_KEY_NUM2       ((int)'2')
#define NOPH_Canvas_KEY_NUM3       ((int)'3')
#define NOPH_Canvas_KEY_NUM4       ((int)'4')
#define NOPH_Canvas_KEY_NUM5       ((int)'5')
#define NOPH_Canvas_KEY_NUM6       ((int)'6')
#define NOPH_Canvas_KEY_NUM7       ((int)'7')
#define NOPH_Canvas_KEY_NUM8       ((int)'8')
#define NOPH_Canvas_KEY_NUM9       ((int)'9')

/* From graphics class */
#define NOPH_Graphics_HCENTER  1
#define NOPH_Graphics_VCENTER  2
#define NOPH_Graphics_LEFT     4
#define NOPH_Graphics_RIGHT    8
#define NOPH_Graphics_TOP     16
#define NOPH_Graphics_BOTTOM  32
#define NOPH_Graphics_BASELINE 64

typedef int NOPH_Image_t;
typedef int NOPH_Graphics_t;
typedef int NOPH_Canvas_t;
typedef int NOPH_Display_t;
typedef int NOPH_Displayable_t;

/* See to it that NOPH_Image_t etc are defined */
#include "__font.h"  /* part of the lcdui package */
#include "__alert.h" /* part of the lcdui package */

#define NOPH_Item_LAYOUT_EXPAND 0x800

/* Form stuff */
#define NOPH_Choice_EXCLUSIVE 1
#define NOPH_Choice_MULTIPLE 2
#define NOPH_Choice_IMPLICIT 3
#define NOPH_Choice_POPUP 4

#define NOPH_TextField_ANY 0
#define NOPH_TextField_EMAILADDR 1
#define NOPH_TextField_NUMERIC 2
#define NOPH_TextField_PHONENUMBER 3
#define NOPH_TextField_URL 4
#define NOPH_TextField_DECIMAL 5
#define NOPH_TextField_PASSWORD 0x10000
#define NOPH_TextField_UNEDITABLE 0x20000
#define NOPH_TextField_SENSITIVE 0x40000
#define NOPH_TextField_NON_PREDICTIVE 0x80000
#define NOPH_TextField_INITIAL_CAPS_WORD 0x100000
#define NOPH_TextField_INITIAL_CAPS_SENTENCE 0x200000
#define NOPH_TextField_CONSTRAINT_MASK 0xFFFF

#define NOPH_Gauge_INDEFINITE -1
#define NOPH_Gauge_CONTINUOUS_IDLE 0
#define NOPH_Gauge_INCREMENTAL_IDLE 1
#define NOPH_Gauge_CONTINUOUS_RUNNING 2
#define NOPH_Gauge_INCREMENTAL_UPDATING 3

#define NOPH_Command_SCREEN 1
#define NOPH_Command_BACK 2
#define NOPH_Command_CANCEL 3
#define NOPH_Command_OK 4
#define NOPH_Command_HELP 5
#define NOPH_Command_STOP 6
#define NOPH_Command_EXIT 7
#define NOPH_Command_ITEM 8


typedef int NOPH_List_t;
typedef int NOPH_Form_t;
typedef int NOPH_Item_t;
typedef int NOPH_ChoiceGroup_t;
typedef int NOPH_TextField_t;
typedef int NOPH_StringItem_t;
typedef int NOPH_Gauge_t;

/* Canvas and GameCanvas classes. */
void NOPH_Canvas_setFullScreenMode(NOPH_Canvas_t canvas, bool_t mode);
int NOPH_Canvas_getKeyCode(NOPH_Canvas_t canvas, int gameAction);
int NOPH_Canvas_getGameAction(NOPH_Canvas_t canvas, int keyCode);
int NOPH_Canvas_getWidth(NOPH_Canvas_t canvas);
int NOPH_Canvas_getHeight(NOPH_Canvas_t canvas);

/**
 * Register a callback for the keyPressed event. This is the same as
 * implementing Canvas::keyPressed() in Java.
 *
 * @param fn the callback function to call when a key is pressed. Pass
 *           NULL to disable the callback
 */
static inline void NOPH_Canvas_registerKeyPressedCallback(void (*fn)(int keyCode))
{
  NOPH_registerCallback( 0, (int)fn );
}
/** @see NOPH_Canvas_registerKeyPressedCallback */
static inline void NOPH_Canvas_registerKeyReleasedCallback(void (*fn)(int keyCode))
{
  NOPH_registerCallback( 1, (int)fn );
}
/** @see NOPH_Canvas_registerKeyPressedCallback */
static inline void NOPH_Canvas_registerKeyRepeatedCallback(void (*fn)(int keyCode))
{
  NOPH_registerCallback( 2, (int)fn );
}
/** @see NOPH_Canvas_registerKeyPressedCallback */
static inline void NOPH_Canvas_registerPointerDraggedCallback(void (*fn)(int x, int y))
{
  NOPH_registerCallback( 3, (int)fn );
}
/** @see NOPH_Canvas_registerKeyPressedCallback */
static inline void NOPH_Canvas_registerPointerPressedCallback(void (*fn)(int x, int y))
{
  NOPH_registerCallback( 4, (int)fn );
}
/** @see NOPH_Canvas_registerKeyPressedCallback */
static inline void NOPH_Canvas_registerPointerReleasedCallback(void (*fn)(int x, int y))
{
  NOPH_registerCallback( 5, (int)fn );
}

/* Image class */
NOPH_Image_t NOPH_Image_createImage(NOPH_Image_t src, int x, int y, int width, int height, int transform); /* Not generated */
NOPH_Image_t NOPH_Image_createImage_string(char* name); /* Throws */
NOPH_Image_t NOPH_Image_createImage_xy(int width, int height); /* Throws */
int NOPH_Image_getWidth(NOPH_Image_t image);
int NOPH_Image_getHeight(NOPH_Image_t image);
NOPH_Graphics_t NOPH_Image_getGraphics(NOPH_Image_t image);
bool_t NOPH_Image_isMutable(NOPH_Image_t image);
NOPH_Image_t NOPH_Image_setMutable(NOPH_Image_t src); /* Not generated */

/* Graphics class */
void NOPH_Graphics_setFont(NOPH_Graphics_t graphics, NOPH_Font_t font);
NOPH_Font_t NOPH_Graphics_getFont(NOPH_Graphics_t graphics);
void NOPH_Graphics_setColor(NOPH_Graphics_t graphics, int red, int green, int blue);
void NOPH_Graphics_setColor_int(NOPH_Graphics_t graphics, int rgb);
void NOPH_Graphics_fillArc(NOPH_Graphics_t graphics, int x, int y, int width, int height, int startAngle, int arcAngle);
void NOPH_Graphics_drawArc(NOPH_Graphics_t graphics, int x, int y, int width, int height, int startAngle, int arcAngle);
void NOPH_Graphics_fillRect(NOPH_Graphics_t graphics, int x, int y, int width, int height);
void NOPH_Graphics_drawRect(NOPH_Graphics_t graphics, int x, int y, int width, int height);
void NOPH_Graphics_drawRoundRect(NOPH_Graphics_t graphics, int x, int y, int width, int height, int arcWidth, int arcHeight);
void NOPH_Graphics_fillTriangle(NOPH_Graphics_t graphics, int x1, int y1, int x2, int y2, int x3, int y3);
void NOPH_Graphics_drawLine(NOPH_Graphics_t graphics, int x1, int y1, int x2, int y2);
void NOPH_Graphics_drawImage(NOPH_Graphics_t graphics, NOPH_Image_t image, int x, int y, int anchor);
void NOPH_Graphics_drawRegion(NOPH_Graphics_t graphics, NOPH_Image_t src, int x_src, int y_src, int width, int height, int transform, int x_dest, int y_dest, int anchor);
void NOPH_Graphics_drawString(NOPH_Graphics_t graphics, const char* str, int x, int y, int anchor);
int NOPH_Graphics_getClipX(NOPH_Graphics_t graphics);
int NOPH_Graphics_getClipY(NOPH_Graphics_t graphics);
int NOPH_Graphics_getClipHeight(NOPH_Graphics_t graphics);
int NOPH_Graphics_getClipWidth(NOPH_Graphics_t graphics);

/**
 * This works slightly differently than the Java method, although the
 * principle is the same: the rgbData int-array is now always
 * CRunTime.memory(), and the rgbData parameter passed here is the
 * offset into the memory (i.e., a pointer). In the same way, the
 * offset parameter is also considered a pointer, so the calculation is
 *
 * CRuntime.memory[ (rgbData + offset) / 4 ];
 */
void NOPH_Graphics_drawRGB(NOPH_Graphics_t graphics, int rgbData, int offset, int scanlength, int x, int y, int width, int height, int processAlpha); /* Not generated */
void NOPH_Graphics_setClip(NOPH_Graphics_t graphics, int x, int y, int w, int h);

/* Display stuff */
NOPH_Display_t NOPH_Display_getDisplay(NOPH_MIDlet_t midlet);
bool_t NOPH_Display_vibrate(NOPH_Display_t display, int duration);
int NOPH_Display_numColors(NOPH_Display_t display);
int NOPH_Display_numAlphaLevels(NOPH_Display_t display);
void NOPH_Display_setCurrent(NOPH_Display_t display, NOPH_Displayable_t nextDisplayable);
void NOPH_Display_setCurrentItem(NOPH_Display_t display, NOPH_Item_t item);
NOPH_Displayable_t NOPH_Display_getCurrent(NOPH_Display_t display);

void NOPH_Item_setLayout(NOPH_Item_t item, int layout);

/* Form class */
NOPH_Form_t NOPH_Form_new(const char* title);
int NOPH_Form_append(NOPH_Form_t form, NOPH_Item_t item);
NOPH_ChoiceGroup_t NOPH_ChoiceGroup_new(const char* title, int choiceType);
void NOPH_ChoiceGroup_append(NOPH_ChoiceGroup_t choice, const char* label, NOPH_Image_t image);
int NOPH_ChoiceGroup_getSelectedIndex(NOPH_ChoiceGroup_t choice);
void NOPH_ChoiceGroup_deleteAll(NOPH_ChoiceGroup_t choice);
void NOPH_ChoiceGroup_getCString(NOPH_ChoiceGroup_t choice, int elementNum, char* buffer, int size); /* Not generated */
void NOPH_ChoiceGroup_setSelectedIndex(NOPH_ChoiceGroup_t choice, int elementNum, bool_t selected);
NOPH_TextField_t NOPH_TextField_new(const char* label, const char* text, int maxSize, int constraints);
int NOPH_TextField_size(NOPH_TextField_t tf);
void NOPH_TextField_getCString(NOPH_TextField_t tf, char* buffer, int size); /* Not generated */
void NOPH_TextField_setString(NOPH_TextField_t tf, const char* text);
NOPH_StringItem_t NOPH_StringItem_new(const char* label, const char* text);
void NOPH_StringItem_getCString(NOPH_StringItem_t si, char* buffer, int size); /* Not generated */
void NOPH_StringItem_setText(NOPH_StringItem_t si, const char* text);
NOPH_Gauge_t NOPH_Gauge_new(const char* label, bool_t interactive, int maxValue, int initialValue);
void NOPH_Gauge_setValue(NOPH_Gauge_t gauge, int value);

/* List class */
NOPH_List_t NOPH_List_new(const char* title, int listType);
int NOPH_List_append(NOPH_List_t l, const char* stringPart, NOPH_Image_t imagePart);
int NOPH_List_getSelectedIndex(NOPH_List_t l);

#if defined(__cplusplus)
}
#endif
#endif /* !__JAVAX__MICROEDITION__LCDUI_H__ */
