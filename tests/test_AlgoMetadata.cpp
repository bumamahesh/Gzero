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
#include <gtest/gtest.h>
#include "../include/AlgoMetadata.h"

TEST(AlgoMetadataTest, GetSetMetadataInt) {
  AlgoMetadata metadata;
  int value;

  // Set metadata
  ASSERT_EQ(metadata.SetMetadata(MetaId::IMAGE_WIDTH, 1920), 0);
  ASSERT_EQ(metadata.SetMetadata(MetaId::IMAGE_HEIGHT, 1080), 0);

  // Get metadata
  ASSERT_EQ(metadata.GetMetadata(MetaId::IMAGE_WIDTH, value), 0);
  ASSERT_EQ(value, 1920);
  ASSERT_EQ(metadata.GetMetadata(MetaId::IMAGE_HEIGHT, value), 0);
  ASSERT_EQ(value, 1080);

  // Get non-existent metadata
  ASSERT_EQ(metadata.GetMetadata(MetaId::MODIFICATION_HISTORY, value), -1);
}

TEST(AlgoMetadataTest, GetSetMetadataFloat) {
  AlgoMetadata metadata;
  float value;

  // Set metadata
  ASSERT_EQ(metadata.SetMetadata(MetaId::EXPOSURE_TIME, 123.45f), 0);

  // Get metadata
  ASSERT_EQ(metadata.GetMetadata(MetaId::EXPOSURE_TIME, value), 0);
  ASSERT_EQ(value, 123.45f);

  // Get non-existent metadata
  ASSERT_EQ(metadata.GetMetadata(MetaId::MODIFICATION_HISTORY, value), -1);
}