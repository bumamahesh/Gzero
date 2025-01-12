
#ifndef IMAGEUTILS_H
#define IMAGEUTILS_H

#pragma once
#include <fstream>
#include <string>
#include <vector>

/**
 * @brief  Utlity to save data to File
 *
 * @param filename
 * @param rawData
 */
void SaveRawDataToFile(const std::string &filename,
                       const std::vector<unsigned char> &rawData) {
  // Open the file in binary mode
  std::ofstream file(filename, std::ios::out | std::ios::binary);
  if (!file) {
    throw std::runtime_error("Failed to open file for writing: " + filename);
  }

  // Write the raw data to the file
  file.write(reinterpret_cast<const char *>(rawData.data()), rawData.size());
  if (!file) {
    throw std::runtime_error("Failed to write data to file: " + filename);
  }

  // Close the file
  file.close();
}

#endif