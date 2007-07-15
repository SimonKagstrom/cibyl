/*********************************************************************
 *
 * Copyright (C) 2007,  Blekinge Institute of Technology
 *
 * Filename:      midlet.h
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   javax.microedition.midlet class
 *
 * $Id: midlet.h 14158 2007-03-11 09:45:19Z ska $
 *
 ********************************************************************/
#ifndef __JAVAX__MICROEDITION__MIDLET_H__
#define __JAVAX__MICROEDITION__MIDLET_H__
#if defined(__cplusplus)
extern "C" {
#endif

#include <cibyl.h>

typedef int NOPH_MIDlet_t;

NOPH_MIDlet_t NOPH_MIDlet_get(void); /* Not generated */
bool_t NOPH_MIDlet_platformRequest(NOPH_MIDlet_t midlet, char* URL); /* Throws */

#if defined(__cplusplus)
}
#endif
#endif /* !__JAVAX__MICROEDITION__MIDLET_H__ */
