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

    this->threadSafe = false;

    this->optimizeInlines = true;
    this->optimizeCallTable = false;
    this->optimizePartialMemoryOps = false;
    this->optimizePruneStackStores = false;
    this->optimizeFunctionReturnArguments = false;
    this->pruneUnusedFunctions = true;

    this->classSizeLimit = 16384; /* Pretty arbitrary value! */
    this->callTableHierarchy = 1;
    this->callTableClasses = 1;
  }

  /* Debugging */
  uint32_t traceRange[2]; /* start, end */
  bool traceStores;

  /* Features */
  bool threadSafe;

  /* Optimizations */
  bool optimizeInlines;
  bool optimizeCallTable;
  bool optimizePartialMemoryOps;
  bool optimizePruneStackStores;
  bool optimizeFunctionReturnArguments;
  bool pruneUnusedFunctions;

  /* Workarounds for bugs */
  size_t classSizeLimit;
  unsigned int callTableHierarchy;
  unsigned int callTableClasses;
};

extern Config *config;

#endif /* !__CONFIG_HH__ */
