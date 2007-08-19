/*********************************************************************
 *
 * Copyright (C) 2007,  Simon Kagstrom
 *
 * Filename:      console.c
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   Console for the test app
 *
 * $Id:$
 *
 ********************************************************************/
#include <javax/microedition/lcdui.h>
#include <javax/microedition/lcdui/game.h>
#include <javax/microedition/io.h>
#include <command_mgr.h>
#include <cibyl-memoryfs.h>
#include <java/lang.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct
{
  int head;
  int lines;
  int line_height;
  char **buf;
  FILE *fp;
} console_t;

static console_t console;

static NOPH_Graphics_t graphics;
static NOPH_GameCanvas_t canvas;
static NOPH_Font_t font;

void console_push(char *str)
{
  if ( 0 && NOPH_Font_stringWidth(font, str) > NOPH_Canvas_getWidth(canvas) )
    {
      int line1_chars =  NOPH_Canvas_getWidth(canvas) / NOPH_Font_charWidth(font, ' ') - 1;
      int line2_chars =  (NOPH_Font_stringWidth(font, str) - NOPH_Canvas_getWidth(canvas)) / NOPH_Font_charWidth(font, ' ') + 1;
      char line1[255];
      char line2[255];

      memset(line1, 0, 255);
      memset(line2, 0, 255);

      strncpy(line1, str, line1_chars);
      strncpy(line2, str + line1_chars, line2_chars);
      console_push(line1);
      console_push(line2);
    }
  else
    {
      fputs(str, console.fp);
      strncpy(console.buf[console.head], str, 255);
      console.head = (console.head + 1) % console.lines;
    }
}

void console_redraw(void)
{
  int i;

  NOPH_Graphics_setColor(graphics, 255, 255, 255);
  NOPH_Graphics_fillRect(graphics, 0,0,
                         NOPH_Canvas_getWidth(canvas), NOPH_Canvas_getHeight(canvas));
  NOPH_Graphics_setColor(graphics, 0, 0, 0);

  for ( i = 0; i < console.lines; i++ )
    {
      int n = (i + console.head) % console.lines;

      NOPH_Graphics_drawString(graphics, console.buf[n],
                               0, i * console.line_height,
                               NOPH_Graphics_TOP | NOPH_Graphics_LEFT);
    }
  NOPH_GameCanvas_flushGraphics(canvas);
}


static NOPH_List_t fs_list;
static char roots[15][80];
char *fs_root = NULL;

static void select_fs_callback(void *unused)
{
        int nr = NOPH_List_getSelectedIndex(fs_list);

        fs_root = roots[nr];
}

static void select_fs_root(void)
{
        NOPH_Display_t display = NOPH_Display_getDisplay(NOPH_MIDlet_get());
        NOPH_Displayable_t cur = NOPH_Display_getCurrent(display);
        NOPH_CommandMgr_t cm = NOPH_CommandMgr_getInstance();
        NOPH_Enumeration_t en = NOPH_FileSystemRegistry_listRoots();
        int i = 0;

        fs_list = NOPH_List_new("Select fs root", NOPH_Choice_IMPLICIT);

        while (NOPH_Enumeration_hasMoreElements(en))
          {
            NOPH_Object_t o = NOPH_Enumeration_nextElement(en);

            NOPH_String_toCharPtr(o, roots[i], 80);
            NOPH_List_append(fs_list, roots[i], 0);
            NOPH_delete(o);
            i++;
          }
        NOPH_delete(en);
        NOPH_Display_setCurrent(display, fs_list);
        NOPH_CommandMgr_setList(cm, fs_list, select_fs_callback, NULL);

        while(fs_root == NULL)
        {
                NOPH_Thread_sleep(250);
        }

        NOPH_Display_setCurrent(display, cur);
        NOPH_delete(fs_list);
}

void console_init(void)
{
  char buf[256];
  int height;
  int i;

  select_fs_root();

  canvas = NOPH_GameCanvas_get();
  graphics = NOPH_GameCanvas_getGraphics(canvas);
  NOPH_Canvas_setFullScreenMode(canvas, 1);

  /* Setup the font */
  font = NOPH_Font_getFont(NOPH_Font_FACE_MONOSPACE, NOPH_Font_STYLE_PLAIN, NOPH_Font_SIZE_SMALL);
  NOPH_Graphics_setFont(graphics, font);

  height = NOPH_Canvas_getHeight(canvas);

  console.head = 0;
  console.line_height = NOPH_Font_getHeight(font) + 1;
  console.lines = height / console.line_height;
  console.buf = malloc( sizeof(char*) * console.lines );

  for (i = 0; i < console.lines; i++)
    {
      /* "enough" space for each line */
      console.buf[i] = malloc( sizeof(char) * 255 );
    }

  snprintf(buf, 256, "file:///%s/cibyl-tests.log", fs_root);
  console.fp = NOPH_MemoryFile_openIndirect(buf, "w");
}

void console_finalize(void)
{
  fclose(console.fp);
}
