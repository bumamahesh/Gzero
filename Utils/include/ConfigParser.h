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