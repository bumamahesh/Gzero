#!/bin/bash

# Run tests and generate XML report
/home/uma/workspace/Gzero/cmake/lib/test_algo_base --gtest_output=xml:test_results.xml


# Convert XML to HTML
junit2html test_results.xml test_results.html

echo "HTML report generated: test_results.html"

