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
#ifndef ALGO_REQUEST_H
#define ALGO_REQUEST_H

#include <memory>
#include <string>
#include <vector>
#include "AlgoMetadata.h"

// Enum to represent supported image formats
enum class ImageFormat {
  YUV420 = 0,
  YUV422,
  YUV444,
  RGB,
  GRAYSCALE,
  JPEG,
  PNG,
  UNKNOWN
};

// Struct to represent an individual image
class ImageData {

  ImageFormat format;               // Format of the image (e.g., YUV, RGB)
  std::vector<unsigned char> data;  // Raw image data
  int width;                        // Width of the image
  int height;                       // Height of the image
  int fd;                           // File descriptor, -1 if not available
 public:
  // Constructor
  ImageData(ImageFormat fmt, int w, int h, int fileDesc = -1)
      : format(fmt), width(w), height(h), fd(fileDesc) {}
  ImageFormat GetFormat() const { return format; }
  int GetWidth() const { return width; }
  int GetHeight() const { return height; }
  int GetFd() const { return fd; }
  void SetData(std::vector<unsigned char>&& data) {
    this->data = std::move(data);
  }
  std::vector<unsigned char>& GetData() { return data; }
  size_t GetDataSize() const { return data.size(); }

  // Destructor
  ~ImageData() = default;
};

class AlgoRequest {
 private:
  std::vector<std::shared_ptr<ImageData>> images;  // Collection of images

 public:
  // Add an image to the collection
  int AddImage(ImageFormat format, int width, int height,
               std::vector<unsigned char>&& rawData, int fd = -1);

  // Add an image to the collection
  int AddImage(ImageFormat format, int width, int height);

  // Get the total number of images
  size_t GetImageCount() const;

  // Get an image by index
  std::shared_ptr<ImageData> GetImage(size_t index) const;

  // Retrieve all images in YUV format
  std::vector<std::shared_ptr<ImageData>> GetYUVImages() const;

  // Clear all stored images
  void ClearImages();

  // Destructor
  ~AlgoRequest() = default;

  size_t mProcessCnt = 0;
  int mRequestId;
  /*request id assoisiated*/  // make this conts in contruction  @todo

  AlgoMetadata mMetadata;

  uint8_t FrameChecksum();

 private:
};

#endif  // ALGO_REQUEST_H