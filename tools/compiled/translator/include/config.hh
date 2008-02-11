/*********************************************************************
 *
 * Copyright (C) 2008,  Simon Kagstrom
 *
 * Filename:      config.hh
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   Translator configuration
 *
 * $Id:$
 *
 ********************************************************************/
#ifndef __CONFIG_HH__
#define __CONFIG_HH__

#include <stdint.h>

class Config
{
public:
  Config()
  {
    this->traceRange[0] = 0;
    this->traceRange[1] = 0;

    this->traceStores = false;
  }

  uint32_t traceRange[2]; /* start, end */

  bool traceStores;
};

extern Config *config;

#endif /* !__CONFIG_HH__ */
