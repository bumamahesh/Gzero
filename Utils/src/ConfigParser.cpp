#include "../include/ConfigParser.h"

ConfigParser::ConfigParser() : errorCode(0) {}

string ConfigParser::trim(const string &str) {
  size_t start = 0;
  size_t end = str.size();

  // Skip leading whitespace
  while (start < end && isspace(str[start])) {
    ++start;
  }

  // Skip trailing whitespace
  while (end > start && isspace(str[end - 1])) {
    --end;
  }

  return str.substr(
      start, end - start); // Return substring without leading/trailing spaces
}

vector<string> ConfigParser::parseArray(const string &value) {
  vector<string> elements;
  string element;
  bool inQuotes = false;

  // Remove the surrounding square brackets
  string trimmedValue = value;

  // Ensure the string starts with '[' and ends with ']'
  if (trimmedValue.front() == '[' && trimmedValue.back() == ']') {
    trimmedValue =
        trimmedValue.substr(1, trimmedValue.size() - 2); // Remove brackets
  }

  for (size_t i = 0; i < trimmedValue.size(); ++i) {
    char ch = trimmedValue[i];

    // Handle the quotes to ensure we don't split values with commas inside
    // quotes
    if (ch == '"') {
      inQuotes = !inQuotes;
    }
    // Split by comma only if we are not inside quotes
    else if (ch == ',' && !inQuotes) {
      if (!element.empty()) {
        elements.push_back(trim(element)); // Trim and add the element
        element.clear();
      }
    } else {
      element += ch; // Accumulate the character into the current element
    }
  }

  // Add the last element
  if (!element.empty()) {
    elements.push_back(trim(element)); // Trim and add the last element
  }

  return elements;
}

void ConfigParser::parseKeyValue(const string &line) {
  size_t pos = line.find('=');
  if (pos == string::npos) {
    errorCode = 1;
    return;
  }

  string key = trim(line.substr(0, pos));
  string value = trim(line.substr(pos + 1));

  // Check if the value starts with '[' indicating an array
  if (!value.empty() && value[0] == '[') {
    arrayStore[key] = parseArray(value); // Correctly parse as an array
  } else {
    keyValueStore[key] = value; // Store as a simple string
  }
}

bool ConfigParser::loadFile(const string &filename) {
  ifstream file(filename);
  if (!file.is_open()) {
    errorCode = 1; // File could not be opened
    return false;
  }

  string line;
  while (getline(file, line)) {
    line = trim(line); // Remove leading/trailing whitespaces
    if (line.empty() || line[0] == '#')
      continue; // Skip empty lines and comments

    size_t pos = line.find('=');
    if (pos == string::npos) {
      errorCode = 2; // Invalid line format
      return false;
    }

    parseKeyValue(line);
  }

  file.close();
  return true;
}

string ConfigParser::getValue(const string &key) {
  // First, check if the key exists in the key-value store
  if (keyValueStore.find(key) != keyValueStore.end()) {
    return keyValueStore[key];
  }

  // Then, check if the key exists in the array store
  if (arrayStore.find(key) != arrayStore.end()) {
    return join(arrayStore[key], ","); // Join array elements with commas
  }

  errorCode = 3; // Key not found
  return "";
}

string ConfigParser::join(const vector<string> &vec, const string &delimiter) {
  ostringstream oss;
  for (size_t i = 0; i < vec.size(); ++i) {
    oss << vec[i];
    if (i != vec.size() - 1) {
      oss << delimiter; // Add the delimiter between elements
    }
  }
  return oss.str();
}

int ConfigParser::getIntValue(const string &key) {
  if (keyValueStore.find(key) != keyValueStore.end()) {
    try {
      return stoi(keyValueStore[key]); // Convert to integer
    } catch (...) {
      errorCode = 4; // Conversion error
      return 0;
    }
  }
  errorCode = 3; // Key not found
  return 0;
}

int ConfigParser::getErrorCode() const {
  return errorCode; // Return the error code
}
