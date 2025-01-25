#ifndef ALGO_DEFS_H
#define ALGO_DEFS_H
#pragma once

#include <string>

#define ALGO_BASE_ID 0xCAFEBABE // Base ID for algorithms

typedef enum ALGOID {
  ALGO_HDR = ALGO_BASE_ID + 0,
  ALGO_BOKEH = ALGO_BASE_ID + 1,
  ALGO_NOP = ALGO_BASE_ID + 2,
  ALGO_FILTER = ALGO_BASE_ID + 3,
  ALGO_MANDELBROTSET = ALGO_BASE_ID + 4,
  ALGO_LDC = ALGO_BASE_ID + 5,
  ALGO_MAX = ALGO_BASE_ID + 6,
} AlgoId;

#endif // ALGO_DEFS_H
