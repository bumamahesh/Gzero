
#include <fstream>
#include "test_common.h"
class ConfigParserTest : public ::testing::Test {
 protected:
  // Setup method, called before each test
  void SetUp() override {
    // Create a temporary configuration file for testing
    std::ofstream configFile("test_config.txt");
    configFile << "key1=value1\n";
    configFile << "key2=42\n";
    configFile << "key3=apple,banana,orange\n";
    configFile << "key4=[10,12,13]\n";
    configFile << "key5=[10.5,12.5,13.5]\n";
    configFile << "key6=[\"Alice\", \"Bob\", \"Charlie\"]\n";
    configFile.close();
  }

  // Cleanup method, called after each test
  void TearDown() override {
    // Remove the test configuration file after each test
    std::remove("test_config.txt");
  }

  // Helper method to load the config file
  ConfigParser loadConfig() {
    ConfigParser parser;
    parser.loadFile("test_config.txt");
    return parser;
  }
};

// Test loading file and retrieving values
TEST_F(ConfigParserTest, TestLoadFile) {
  ConfigParser parser = loadConfig();
  EXPECT_EQ(parser.getValue("key1"), "value1");
  EXPECT_EQ(parser.getIntValue("key2"), 42);
  EXPECT_EQ(parser.getValue("key3"), "apple,banana,orange");
  EXPECT_EQ(parser.getValue("key4"), "10,12,13");
  EXPECT_EQ(parser.getValue("key5"), "10.5,12.5,13.5");
  EXPECT_EQ(parser.getValue("key6"), "Alice,Bob,Charlie");
}

// Test when retrieving a non-existing key
TEST_F(ConfigParserTest, TestGetValueNonExistingKey) {
  ConfigParser parser = loadConfig();
  EXPECT_EQ(parser.getValue("non_existing_key"), "");
}

// Test error code retrieval (assuming errorCode is set to non-zero in case of
// an error)
TEST_F(ConfigParserTest, TestErrorCode) {
  ConfigParser parser;
  // Simulating an error by loading a non-existent file
  EXPECT_FALSE(parser.loadFile("non_existent_file.txt"));
  EXPECT_NE(parser.getErrorCode(), 0);
}

// Test parsing arrays
TEST_F(ConfigParserTest, TestParseArray) {
  ConfigParser parser               = loadConfig();
  std::vector<std::string> expected = {"apple", "banana", "orange"};
  std::vector<std::string> result   = parser.parseArray("apple,banana,orange");
  EXPECT_EQ(result.size(), expected.size());
  EXPECT_EQ(result, expected);
}

// Test if an invalid line (non-key-value pair) is handled gracefully
TEST_F(ConfigParserTest, TestInvalidLineHandling) {
  ConfigParser parser;
  std::ofstream configFile("test_invalid_line.txt");
  configFile << "invalid_line_without_key_or_value\n";
  configFile.close();

  EXPECT_FALSE(parser.loadFile("test_invalid_line.txt"));
  EXPECT_NE(parser.getErrorCode(), 0);
}

std::string ConfigList[] = {
    "BokehAlgorithm.config",         "FilterAlgorithm.config",
    "HdrAlgorithm.config",           "LdcAlgorithm.config",
    "MandelbrotSetAlgorithm.config", "NopAlgorithm.config",
    "WaterMarkAlgorithm.config"};

TEST_F(ConfigParserTest, TestAllConfigFiles) {
  for (auto config : ConfigList) {
    ConfigParser parser;
    std::string ConfigFileFullpath = CONFIGPATH + std::string(config);
    EXPECT_TRUE(parser.loadFile(ConfigFileFullpath.c_str()));
    EXPECT_EQ(parser.getErrorCode(), 0);
    EXPECT_EQ(parser.getValue("MAGIC_NUMBER"), "0XCAFEBABE");
  }
}