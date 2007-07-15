/*********************************************************************
 *
 * Copyright (C) 2006,  Blekinge Institute of Technology
 *
 * Filename:      __alert.h
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Cibyl J2ME API
 *
 * $Id: __alert.h 14120 2007-03-10 09:51:33Z ska $
 *
 ********************************************************************/
#ifndef __JAVAX__MICROEDITION__LCDUI_ALERT_H__
#define __JAVAX__MICROEDITION__LCDUI_ALERT_H__

/* This should not be included directly */
#define NOPH_Alert_FOREVER -2

#define NOPH_AlertType_INFO          1
#define NOPH_AlertType_WARNING       2
#define NOPH_AlertType_ERROR         3
#define NOPH_AlertType_ALARM         4
#define NOPH_AlertType_CONFIRMATION  5

typedef int NOPH_Alert_t;
typedef int NOPH_AlertType_t;

NOPH_Alert_t NOPH_Alert_new(const char* title, const char* alertText, NOPH_Image_t alertImage, NOPH_AlertType_t alertType);

void NOPH_Alert_setTimeout(NOPH_Alert_t alert, int time);

NOPH_AlertType_t NOPH_AlertType_get(int type); /* Not generated */

#endif /* !__JAVAX__MICROEDITION__LCDUI_ALERT_H__ */
