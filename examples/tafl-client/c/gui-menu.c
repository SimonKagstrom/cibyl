/*********************************************************************
 *
 * Copyright (C) 2004,  Blekinge Institute of Technology
 *
 * Filename:      menu.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Code for menus
 *
 * $Id: gui-menu.c 11560 2006-10-09 18:54:27Z ska $
 *
 ********************************************************************/
#include <menu.h>
#include <utils.h>

#define IS_SUBMENU(p_msg) ( (p_msg)[0] == '^' )

static submenu_t *find_submenu(menu_t *p_menu, int index)
{
  int i;

  for (i=0; i<p_menu->n_submenus; i++)
    {
      if (p_menu->p_submenus[i].index == index)
	return &p_menu->p_submenus[i];
    }

  return NULL;
}

static void menu_draw(menu_t *p_menu)
{
  int16_t x_start = p_menu->x1 + (p_menu->x2 - p_menu->x1) / 2 - p_menu->text_w / 2;
  int16_t y_start = p_menu->y1 + (p_menu->y2 - p_menu->y1) / 2 - p_menu->text_h / 2;
  int i;

  for (i=0; i < p_menu->n_entries; i++)
    {
      char *msg = p_menu->pp_msgs[i];

      if ((p_menu->available_options & (1<<i)) == 0)
	print_font(p_menu->p_font, vRGB(128,128,128), x_start, y_start + i * (p_menu->p_font->height + 2), msg);
      else if (p_menu->cur_sel == i)
	print_font(p_menu->p_font, vRGB(255,255,0), x_start, y_start + i * (p_menu->p_font->height + 2), msg);
      else
	print_font(p_menu->p_font, vRGB(255,255,255), x_start, y_start + i * (p_menu->p_font->height + 2), msg);
      if (IS_SUBMENU(msg))
	{
	  submenu_t *p_submenu = find_submenu(p_menu, i);
	  int n_pipe = 0;
	  int n;

	  DbgPrintf("Submenu!...\n");
	  for (n=0; msg[n] != '\0'; n++)
	    {
	      /* Underline the selected entry */
	      if (msg[n] == '|')
		{
		  int16_t n_chars;

		  for (n_chars = 1; msg[n+n_chars] && msg[n+n_chars] != '|'; n_chars++);

		  n_pipe++;
		  if (p_submenu->sel == n_pipe-1)
		    {
		      vSetForeColor(vRGB(0,255,0));
		      vFillRect(x_start + (n+1) * (p_menu->p_font->width)-1,
				y_start + (i+1) * (p_menu->p_font->height + 2)-1,
				x_start + (n+n_chars) * (p_menu->p_font->width)-1,
				y_start + (i+1) * (p_menu->p_font->height + 2)-1);
		      break;
		    }
		}
	    }
	}
    }
}

static void select_next(menu_t *p_menu, int dx, int dy)
{
  p_menu->cur_sel = (p_menu->cur_sel + dy) < 0 ? p_menu->n_entries - 1 :
    (p_menu->cur_sel + dy) % p_menu->n_entries;
  int next = (p_menu->cur_sel + dy + 1) < 0 ? p_menu->n_entries - 1 :
    (p_menu->cur_sel + dy + 1) % p_menu->n_entries;

  if (p_menu->pp_msgs[p_menu->cur_sel][0] == ' ' ||
      ( (p_menu->available_options & (1<<p_menu->cur_sel)) == 0) ||
      IS_SUBMENU(p_menu->pp_msgs[p_menu->cur_sel]) )
    select_next(p_menu, dx, dy);
  /* If the next is a submenu */
  if (dx != 0 &&
      IS_SUBMENU(p_menu->pp_msgs[next]))
    {
      submenu_t *p_submenu = find_submenu(p_menu, next);

      p_submenu->sel = (p_submenu->sel + dx) < 0 ? p_submenu->n_entries - 1 :
	(p_submenu->sel + dx) % p_submenu->n_entries;
    }
}

static int is_submenu_title(menu_t *p_menu, int n)
{
  if (n+1 >= p_menu->n_entries)
    return 0;
  else
    return IS_SUBMENU(p_menu->pp_msgs[n+1]);
}


void menu_init(menu_t *p_menu, VMGPFONT *p_font, char *pp_msgs[],
	       int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
  int i;
  int j;

  p_menu->pp_msgs = pp_msgs;
  p_menu->p_font = p_font;
  p_menu->x1 = x1;
  p_menu->y1 = y1;
  p_menu->x2 = x2;
  p_menu->y2 = y2;

  p_menu->text_w = 0;
  p_menu->n_submenus = 0;

  for (p_menu->n_entries = 0; p_menu->pp_msgs[p_menu->n_entries]; p_menu->n_entries++)
    {
      int len;

      /* Is this a submenu? */
      if (IS_SUBMENU(p_menu->pp_msgs[p_menu->n_entries]))
	{
	  p_menu->n_submenus++;
	  continue; /* Length of submenus is unimportant */
	}
      for (len = 0; p_menu->pp_msgs[p_menu->n_entries][len]; len++);

      if (len * p_font->width > p_menu->text_w)
	p_menu->text_w = len * p_font->width;
    }
  if ( !(p_menu->p_submenus = vNewPtr(sizeof(submenu_t) * p_menu->n_submenus)) )
    {
      DbgPrintf("vNewPtr failed!\n");
      vTerminateVMGP();
    }

  j=0;
  for (i=0; i<p_menu->n_submenus; i++)
    {

      for (; j < p_menu->n_entries; j++)
	{
	  if (IS_SUBMENU(p_menu->pp_msgs[j]))
	    {
	      int n;

	      DbgPrintf("Init submenu %d:%d\n", i, j);
	      p_menu->p_submenus[i].index = j;
	      p_menu->p_submenus[i].sel = 0;
	      p_menu->p_submenus[i].n_entries = 0;
	      for (n=0; p_menu->pp_msgs[j][n] != '\0'; n++)
		{
		  if (p_menu->pp_msgs[j][n] == '|')
		    p_menu->p_submenus[i].n_entries++;
		}
	    }
	}
    }
  p_menu->text_h = p_menu->n_entries * (p_font->height+2);
}

void menu_fini(menu_t *p_menu)
{
  vDisposePtr(p_menu->p_submenus);
}


static uint32_t wait_key_press(void)
{
  uint32_t keys;

  while ( !(keys = vGetButtonData()) );

  return keys;
}


int menu_select(menu_t *p_menu, uint32_t available_options, int *p_submenus)
{
  vSetActiveFont(p_menu->p_font);

  while (vGetButtonData())
    ;

  p_menu->available_options = available_options;

  while(1)
    {
      uint32_t keys;


      vSetForeColor(vRGB(0,0,0));
      vFillRect(p_menu->x1, p_menu->y1, p_menu->x2, p_menu->y2);
      menu_draw(p_menu);
      vFlipScreen(1);
      msSleep(300);

      keys = wait_key_press();

      if (keys & KEY_UP)
	select_next(p_menu, 0, -1);
      else if (keys & KEY_DOWN)
	select_next(p_menu, 0, 1);
      else if (keys & KEY_LEFT)
	select_next(p_menu, -1, 0);
      else if (keys & KEY_RIGHT)
	select_next(p_menu, 1, 0);
      else if (keys & KEY_SELECT)
	return -1;
      else if (keys & KEY_FIRE ||
	       keys & KEY_FIRE2)
	{
	  int ret = p_menu->cur_sel;
	  int i;

	  if (!is_submenu_title(p_menu, ret))
	    {
	      for (i=0; i<p_menu->n_submenus; i++)
		p_submenus[i] = p_menu->p_submenus[i].sel;
	      p_menu->cur_sel = 0;

	      return ret;
	    }
	}
    }

  return -1;
}
