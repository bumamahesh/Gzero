/*
 * Copyright (c) [2025] [Uma Mahesh B]
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef ALGO_DEFS_H
#define ALGO_DEFS_H
#pragma once

#include <string>
#define ALGO_OFFSET(ALGO_IDX) (static_cast<int>(ALGO_IDX - ALGO_BASE_ID))
#define ALGO_MASK(ALGO_IDX) (1 << ALGO_OFFSET(ALGO_IDX))
#define ALGO_BASE_ID 0xCAFEBABE  // Base ID for algorithms

typedef enum ALGOID {
  ALGO_HDR           = ALGO_BASE_ID + 0,
  ALGO_BOKEH         = ALGO_BASE_ID + 1,
  ALGO_NOP           = ALGO_BASE_ID + 2,
  ALGO_FILTER        = ALGO_BASE_ID + 3,
  ALGO_MANDELBROTSET = ALGO_BASE_ID + 4,
  ALGO_LDC           = ALGO_BASE_ID + 5,
  ALGO_WATERMARK     = ALGO_BASE_ID + 6,
  ALGO_SWJPEG        = ALGO_BASE_ID + 7,
  ALGO_MAX           = ALGO_BASE_ID + 8,
} AlgoId;

#define ALGO_START (ALGO_OFFSET(ALGO_BASE_ID))
#define ALGO_END (ALGO_OFFSET(ALGO_MAX))

static std::string algoName[ALGO_OFFSET(ALGO_MAX) + 1] = {
    "HDR", "BOKEH",     "NOP",  "FILTER", "MANDELBROTSET",
    "LDC", "WATERMARK", "JPEG", "MAX"};

#endif  // ALGO_DEFS_H
