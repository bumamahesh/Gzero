// ConfigParser.h
#ifndef CONFIGPARSER_H
#define CONFIGPARSER_H

#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>
using namespace std;

class ConfigParser {
private:
  unordered_map<string, string> keyValueStore;
  unordered_map<string, vector<string>> arrayStore;
  int errorCode;

public:
  ConfigParser();
  bool loadFile(const string &filename);
  string getValue(const string &key);
  int getIntValue(const string &key);
  int getErrorCode() const;

  string trim(const string &str);
  vector<string> parseArray(const string &value);
  void parseKeyValue(const string &line);
  string join(const vector<string> &vec, const string &delimiter);
};

#endif // CONFIGPARSER_H