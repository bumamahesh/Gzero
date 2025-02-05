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
#include "../include/AlgoRequest.h"
#include <gtest/gtest.h>

constexpr int Width  = 100;
constexpr int Height = 100;

class AlgoRequestTest : public ::testing::Test {
protected:
  std::shared_ptr<AlgoRequest> request;

  void SetUp() override {
    request = std::make_shared<AlgoRequest>();
    ASSERT_NE(request, nullptr);
  }
};

TEST_F(AlgoRequestTest, DefaultConstructor) {
  EXPECT_EQ(request->GetImageCount(), 0);
}

TEST_F(AlgoRequestTest, AddImageWithZeroSizeRawBuffer) {
  std::vector<unsigned char> rawData;
  EXPECT_EQ(request->GetImageCount(), 0);

  int rc = request->AddImage(ImageFormat::YUV420, Width, Height, std::move(rawData));
  EXPECT_EQ(rc, -1);
  EXPECT_EQ(request->GetImageCount(), 0);
}

TEST_F(AlgoRequestTest, RawBufferWithInvalidSize) {
  std::vector<unsigned char> rawData1(100);
  int rc = request->AddImage(ImageFormat::YUV420, 0, Height, std::move(rawData1));
  EXPECT_EQ(rc, -1);
  EXPECT_EQ(request->GetImageCount(), 0);

  std::vector<unsigned char> rawData2(100);
  rc = request->AddImage(ImageFormat::YUV420, Width, 0, std::move(rawData2));
  EXPECT_EQ(rc, -1);
  EXPECT_EQ(request->GetImageCount(), 0);
}

TEST_F(AlgoRequestTest, AddImageWithOverSizeRawBuffer) {
  std::vector<unsigned char> rawData(Width * Height * 3);
  EXPECT_EQ(request->GetImageCount(), 0);

  int rc = request->AddImage(ImageFormat::YUV420, Width, Height, std::move(rawData));
  EXPECT_EQ(rc, -2);
  EXPECT_EQ(request->GetImageCount(), 0);
}

TEST_F(AlgoRequestTest, VerifyAddImage) {
  int rc = request->AddImage(ImageFormat::YUV420, Width, Height);
  EXPECT_EQ(rc, 0);
  EXPECT_EQ(request->GetImageCount(), 1);
  EXPECT_EQ(request->GetImage(0)->GetWidth(), Width);
  EXPECT_EQ(request->GetImage(0)->GetHeight(), Height);
  EXPECT_EQ(request->GetImage(0)->GetFormat(), ImageFormat::YUV420);
  EXPECT_EQ(request->GetImage(0)->GetFd(), -1);
  EXPECT_EQ(request->GetImage(0)->GetDataSize(), Width * Height * 3 / 2);
}

TEST_F(AlgoRequestTest, AddImage_MoveSemantics) {
  AlgoRequest request;
  std::vector<unsigned char> data(32 * 32 * 3, 123);
  // Add the image using AddImage
  int rc = request.AddImage(ImageFormat::RGB, 32, 32, std::move(data));
  EXPECT_EQ(rc, 0);

  // Check that the image data was correctly moved into the request
  std::shared_ptr<ImageData> image = request.GetImage(0);
  EXPECT_NE(image, nullptr);
  EXPECT_EQ(image->GetData().size(), 32 * 32 * 3);
  for (unsigned char value : image->GetData()) {
    EXPECT_EQ(value, 123);
  }

  // Ensure the new image data has non-zero capacity in its new location
  EXPECT_NE(image->GetData().capacity(), 0);
}

TEST(AlgoRequestTests, AddImage_ZeroWidthHeight) {
  AlgoRequest request;
  std::vector<unsigned char> data1(1024, 123); // Sample image data
  std::vector<unsigned char> data2(1024, 123); // Sample image data
  std::vector<unsigned char> data3(1024, 123); // Sample image data

  int ret = request.AddImage(ImageFormat::RGB, 0, 32, std::move(data1));
  EXPECT_EQ(ret, -1);
  ret = request.AddImage(ImageFormat::RGB, 32, 0, std::move(data2));
  EXPECT_EQ(ret, -1);
  ret = request.AddImage(ImageFormat::RGB, 0, 0, std::move(data3));
  EXPECT_EQ(ret, -1);
}
