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

#include "../include/ThreadWrapper.h"
#include <iostream>
/**
@brief Construct a new Thread Wrapper:: Thread Wrapper object
 * 
 * @param start_routine 
 * @param arg 
 */
ThreadWrapper::ThreadWrapper(void* (*start_routine)(void*), void* arg) {
#ifdef __linux__
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  int ret = pthread_create(&thread, &attr, start_routine, arg);
  pthread_attr_destroy(&attr);
  if (ret != 0) {
    std::cerr << "Error creating thread: " << ret << std::endl;
  }
#else
  (void)start_routine;
  (void)arg;
#endif
}

/**
@brief  join thread 
 * 
 */
void ThreadWrapper::join() {
#ifdef __linux__
  pthread_join(thread, nullptr);
#endif
}

/**
@brief Destroy the Thread Wrapper:: Thread Wrapper object
 * 
 */
ThreadWrapper::~ThreadWrapper() {
  // join();
}

void ThreadWrapper::ThreadSetname(const char* name) {
#ifdef __linux__
  if (name != nullptr) {
    pthread_setname_np(thread, name);
  } else {
    std::cerr << "Error Setting thread Name" << std::endl;
  }
#else
  (void)(name);
#endif
}