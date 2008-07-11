#ifndef __FORM_COMMAND_MGR_H__
#define __FORM_COMMAND_MGR_H__

#include <cibyl.h>
#include "javax/microedition/lcdui.h"

/* FormCommandMgr class (this is not in J2ME) */
typedef int NOPH_FormCommandMgr_t;

NOPH_FormCommandMgr_t NOPH_FormCommandMgr_new(NOPH_Form_t form);
void NOPH_FormCommandMgr_addCommand(NOPH_FormCommandMgr_t fc, const char* name, void* c_addr, char* c_name, void* c_context);
void NOPH_FormCommandMgr_setCallBackNotif(int* callback_addr, void* callback_name, void* callback_context);
void NOPH_FormCommandMgr_addCallback(NOPH_FormCommandMgr_t fc, NOPH_Item_t item, void* c_addr, char* c_name, void* c_context);

#endif /* !__FORM_COMMAND_MGR_H__ */
