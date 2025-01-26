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
#include "AlgoInterface.h"
#include <memory>
#define SHARED_LIB_EXPORT __attribute__((visibility("default")))
// API declarations
extern "C" {

/**
 * @brief  Initializes the shared library.
 *
 * @param libhandle
 * @return status
 */
SHARED_LIB_EXPORT int InitAlgoInterface(void **libhandle);

/**
 * @brief Deinitializes the shared library and releases resources.
 *
 * @param libhandle
 * @return status
 */
SHARED_LIB_EXPORT int DeInitAlgoInterface(void **libhandle);

/**
 * @brief Processes capture data.
 *
 * @param input
 * @return status
 */
SHARED_LIB_EXPORT int AlgoInterfaceProcess(void **libhandle,
                                           std::shared_ptr<AlgoRequest> input);

/**
 * @brief  Register callbacks
 *
 * @param Callback
 * @return status
 */
SHARED_LIB_EXPORT int
RegisterCallback(void **libhandle,
                 int (*Callback)(std::shared_ptr<AlgoRequest> input));
}