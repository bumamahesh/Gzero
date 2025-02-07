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

#ifndef RENDERER_H
#define RENDERER_H
#pragma once
#include "AlgoInterfaceManager.h"
#include <SDL2/SDL.h>
#include <memory>
#include <unistd.h>

class Renderer {
public:
  Renderer(int width, int height);
  ~Renderer();
  void RenderLoop(std::shared_ptr<AlgoInterfaceManager> pAlgoInteface);

private:
  bool Initialize();
  void Cleanup();
  SDL_Window *mWindow;
  SDL_Renderer *mRenderer;
  SDL_Texture *mTexture;
  int mWidth;
  int mHeight;
  bool isYUV = true;
};

#endif // RENDERER_H
