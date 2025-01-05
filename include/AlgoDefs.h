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
  ALGO_MAX = ALGO_BASE_ID + 4
} AlgoId;

const std::string BOKEH_NAME = "BokehAlgorithm";
const std::string HDR_NAME = "HDRAlgorithm";
const std::string NOP_NAME = "NopAlgorithm";
const std::string FILTER_NAME = "FilterAlgorithm";

#endif // ALGO_DEFS_H
