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

#ifndef TEST_COMMON_H
#define TEST_COMMON_H

#include <dlfcn.h>
#include <gtest/gtest.h>
#include <memory>
#include "../include/AlgoDefs.h"
#include "../include/AlgoMetadata.h"
#include "../include/AlgoRequest.h"
#include "../include/ConfigParser.h"

#define WIDTH 640
#define HEIGHT 480

// Type aliases for function pointers
typedef int (*Init)(void**);
typedef int (*DeInit)(void**);
typedef int (*Process)(void**, std::shared_ptr<AlgoRequest>,
                       std::vector<AlgoId>);
typedef int (*Callback)(void**, int (*)(std::shared_ptr<AlgoRequest>));

const std::string ALGOLIBPATH     = "/home/uma/workspace/Gzero/build/lib/";
const std::string ALGOLIBNAME     = "libAlgoLib.so";
const std::string FULLALGOLIBPATH = ALGOLIBPATH + ALGOLIBNAME;

#endif  // TEST_COMMON_H