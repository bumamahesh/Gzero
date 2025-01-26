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
#include "AlgoRequest.h"

/**
 * @brief Add an image to the collection
 *
 * @param format
 * @param width
 * @param height
 * @param rawData
 * @param fd
 */
void AlgoRequest::AddImage(ImageFormat format, int width, int height,
                           const std::vector<unsigned char> &rawData, int fd) {
  auto image = std::make_shared<ImageData>(format, width, height, fd);
  image->data = rawData;
  images.push_back(image);
}

/**
 * @brief Get the total number of images
 *
 * @return size_t
 */
size_t AlgoRequest::GetImageCount() const { return images.size(); }

/**
 * @brief Get an image by index
 *
 * @param index
 * @return std::shared_ptr<ImageData>
 */
std::shared_ptr<ImageData> AlgoRequest::GetImage(size_t index) const {
  if (index >= images.size()) {
    // throw std::out_of_range("Index out of range");
    return nullptr;
  }
  return images[index];
}

/**
 * @brief Retrieve all images in YUV format
 *
 * @return std::vector<std::shared_ptr<ImageData>>
 */
std::vector<std::shared_ptr<ImageData>> AlgoRequest::GetYUVImages() const {
  std::vector<std::shared_ptr<ImageData>> yuvImages;
  for (const auto &image : images) {
    if (image->format == ImageFormat::YUV420 ||
        image->format == ImageFormat::YUV422 ||
        image->format == ImageFormat::YUV444) {
      yuvImages.push_back(image);
    }
  }
  return yuvImages;
}

/**
 * @brief Clear all stored images
 *
 */
void AlgoRequest::ClearImages() { images.clear(); }
