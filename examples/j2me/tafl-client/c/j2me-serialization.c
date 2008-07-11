/*********************************************************************
 *
 * Copyright (C) 2006,  Blekinge Institute of Technology
 *
 * Filename:      j2me-serialization.c
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   J2ME-specific serialization stuff
 *
 * $Id: j2me-serialization.c 11606 2006-10-12 10:19:17Z ska $
 *
 ********************************************************************/
#include <javax/microedition/io.h>
#include <assert.h>
#include "gui.h"

static int id_to_client[] =
{
  0,
  0,
  0,
  0,
  0,
  0,
};

void exception_handler_fatal(NOPH_Exception_t exception, void *arg)
{
  NOPH_Throwable_printStackTrace(exception);
  NOPH_delete(exception);
}

static void handler_file_io(NOPH_Exception_t exception, void *arg)
{
  *(int*)arg = 1;
  NOPH_delete(exception);
}

static NOPH_RecordStore_t open_saved_file(int num)
{
  NOPH_RecordStore_t record_store;
  int exception = 0;
  char buf[40];

  assert(num >= 0 && num < 10);

  snprintf(buf, 40, "saved-%d", num);
  /* Try to open (create if fails) */

  NOPH_try(handler_file_io, (void*)&exception) {
    record_store = NOPH_RecordStore_openRecordStore(buf, FALSE);
    exception = 0;
  } NOPH_catch();
  if (exception)
    FATAL_ON_EXCEPTION(record_store = NOPH_RecordStore_openRecordStore(buf, TRUE));

  return record_store;
}

bool_t game_resume_from_file(game_t *p_game, int num)
{
  NOPH_RecordStore_t record_store;
  char buf[GAME_TOTAL_BUFLEN];
  int rec_id;
  int exception = 0;
  bool_t out = FALSE;

  record_store = open_saved_file(num);

  /* Get the record (return false if this fails) */
  NOPH_try(handler_file_io, (void*)&exception) {
    rec_id = NOPH_RecordStore_getRecord(record_store, 0, buf, 0);
    exception = 0;
  } NOPH_catch();

  if (exception)
    goto out;
  out = game_from_string(p_game, buf, GAME_TOTAL_BUFLEN);

 out:
  FATAL_ON_EXCEPTION(NOPH_RecordStore_closeRecordStore(record_store));
  return out;
}

bool_t game_store_to_file(game_t *p_game, int num)
{
  NOPH_RecordStore_t record_store;
  char buf[GAME_TOTAL_BUFLEN];
  int rec_id;
  int exception = 0;
  bool_t out = FALSE;

  record_store = open_saved_file(num);

  /* Get the record (return false if this fails) */
  NOPH_try(handler_file_io, (void*)&exception) {
    rec_id = NOPH_RecordStore_getRecord(record_store, 0, buf, 0);
    exception = 0;
  } NOPH_catch();

  out = game_to_string(p_game, buf, GAME_TOTAL_BUFLEN);

  /* Add a new one or update the old one */
  if (exception)
    rec_id = NOPH_RecordStore_addRecord(record_store, buf, 0, GAME_TOTAL_BUFLEN);
  else
    NOPH_RecordStore_setRecord(record_store, rec_id, buf, 0, GAME_TOTAL_BUFLEN);

  if (exception)
    out = FALSE;

  FATAL_ON_EXCEPTION(NOPH_RecordStore_closeRecordStore(record_store));
  return out;
}
