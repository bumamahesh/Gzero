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
// ConfigParser.h
#ifndef CONFIGPARSER_H
#define CONFIGPARSER_H

#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>

const std::string CONFIGPATH = "/home/uma/workspace/Gzero/Config/";
class ConfigParser {
 private:
  std::unordered_map<std::string, std::string> keyValueStore;
  std::unordered_map<std::string, std::vector<std::string>> arrayStore;
  int errorCode;

 public:
  ConfigParser();
  bool loadFile(const std::string& filename);
  std::string getValue(const std::string& key);
  int getIntValue(const std::string& key);
  int getErrorCode() const;

  std::string trim(const std::string& str);
  std::vector<std::string> parseArray(const std::string& value);
  void parseKeyValue(const std::string& line);
  std::string join(const std::vector<std::string>& vec,
                   const std::string& delimiter);
};

#endif  // CONFIGPARSER_H