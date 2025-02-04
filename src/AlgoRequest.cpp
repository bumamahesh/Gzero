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
 * @brief Get the Size By Format object
 *
 * @param format
 * @param width
 * @param height
 * @return size_t
 */
static size_t GetSizeByFormat(ImageFormat format, int width, int height) {
  if ((width == 0) || (height == 0)) {
    return 0;
  }
  switch (format) {
  case ImageFormat::YUV420:
    return width * height * 3 / 2;
    break;
  case ImageFormat::YUV422:
    return width * height * 2;
    break;
  case ImageFormat::YUV444:
    return width * height * 3;
    break;
  case ImageFormat::RGB:
    return width * height * 3;
    break;
  case ImageFormat::GRAYSCALE:
    return width * height;
    break;
  default:
    return 0;
  }

  return 0;
}
/**
 * @brief Add an image to the collection
 *
 * @param format
 * @param width
 * @param height
 * @param rawData
 * @param fd
 */
int AlgoRequest::AddImage(ImageFormat format, int width, int height,
                          const std::vector<unsigned char> &rawData, int fd) {
  if ((width <= 0) || (height <= 0) || (rawData.size() == 0)) {
    return -1;
  }
  if (rawData.size() != GetSizeByFormat(format, width, height)) {
    return -2;
  }
  // Create a new ImageData object and add it to the collection
  auto image = std::make_shared<ImageData>(format, width, height, fd);
  image->SetData(std::move(rawData));
  images.push_back(image);

  return 0;
}

/**
 * @brief Creat a image and add to collection
 *
 * @param format
 * @param width
 * @param height
 * @return int
 */
int AlgoRequest::AddImage(ImageFormat format, int width, int height) {
  if ((width <= 0) || (height <= 0)) {
    return -1;
  }
  size_t size = GetSizeByFormat(format, width, height);
  if (size == 0) {
    return -2;
  }

  // Create a new ImageData object and add it to the collection
  auto image = std::make_shared<ImageData>(format, width, height, -1);
  std::vector<unsigned char> rawData(size, 0);
  image->SetData(std::move(rawData));
  images.push_back(image);
  return 0;
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
    if (image->GetFormat() == ImageFormat::YUV420 ||
        image->GetFormat() == ImageFormat::YUV422 ||
        image->GetFormat() == ImageFormat::YUV444) {
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

/**
 * @brief Compute CheckSum of Frame
 *
 * @return uint8_t
 */
uint8_t AlgoRequest::FrameChecksum() {
  unsigned int checksum = 0;
  for (const auto &image : images) {
    for (int i = 0; i < image->GetDataSize(); i++) {
      checksum += image->GetData()[i];
      checksum %= 256;
    }
  }
  return (uint8_t)(checksum % 256);
}
