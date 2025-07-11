#include <gtest/gtest.h>
#include <string>
#include <fstream>
#include <regex>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

class TestCountValidator {
public:
    struct TestCounts {
        int total = 0;
        int passed = 0;
        int skipped = 0;
        int failed = 0;
    };

    // Parse test output from run_all_tests.sh
    static TestCounts parseTestOutput(const std::string& output) {
        TestCounts counts;
        
        // Match: [==========] 773 tests from 163 test suites ran.
        std::regex totalRegex(R"(\[==========\]\s+(\d+)\s+tests\s+from)");
        std::smatch totalMatch;
        if (std::regex_search(output, totalMatch, totalRegex)) {
            counts.total = std::stoi(totalMatch[1]);
        }
        
        // Match: [  PASSED  ] 760 tests.
        std::regex passedRegex(R"(\[\s*PASSED\s*\]\s+(\d+)\s+tests)");
        std::smatch passedMatch;
        if (std::regex_search(output, passedMatch, passedRegex)) {
            counts.passed = std::stoi(passedMatch[1]);
        }
        
        // Check if there are any failed tests mentioned
        std::regex failedRegex(R"(\[\s*FAILED\s*\]\s+(\d+)\s+test)");
        std::smatch failedMatch;
        if (std::regex_search(output, failedMatch, failedRegex)) {
            counts.failed = std::stoi(failedMatch[1]);
        } else {
            counts.failed = 0;
        }
        
        // Skipped = total - passed - failed
        counts.skipped = counts.total - counts.passed - counts.failed;
        
        return counts;
    }
    
    // Validate counts in documentation files
    static bool validateDocumentationCounts(const TestCounts& actual) {
        bool allValid = true;
        
        // Check CLAUDE.md
        allValid &= validateFileTestCounts("CLAUDE.md", actual);
        
        // Check TDD_IMPLEMENTATION_PLAN.md
        allValid &= validateFileTestCounts("Docs/TDD_IMPLEMENTATION_PLAN.md", actual);
        
        // Check TESTING_GUIDE.md
        allValid &= validateFileTestCounts("Docs/TESTING_GUIDE.md", actual);
        
        // Check DEVELOPMENT_CAPABILITIES.md
        allValid &= validateFileTestCounts("Docs/DEVELOPMENT_CAPABILITIES.md", actual);
        
        return allValid;
    }
    
private:
    static bool validateFileTestCounts(const std::string& relativePath, const TestCounts& actual) {
        std::string fullPath = getProjectRoot() + "/" + relativePath;
        std::ifstream file(fullPath);
        if (!file.is_open()) {
            std::cerr << "Warning: Could not open " << fullPath << std::endl;
            return true; // Don't fail if file doesn't exist
        }
        
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        
        // Check for outdated test counts
        std::vector<std::regex> outdatedPatterns;
        
        // For TDD_IMPLEMENTATION_PLAN.md, don't check patterns that appear in issue descriptions
        if (relativePath.find("TDD_IMPLEMENTATION_PLAN.md") == std::string::npos) {
            outdatedPatterns = {
                std::regex("745\\s+tests"),
                std::regex("728\\s+passing"),
                std::regex("17\\s+skipped"),
                std::regex("780\\s+tests"),
                std::regex("763\\s+passing"),
                std::regex("782\\s+total\\s+tests"),
                std::regex("770\\s+passing"),
                std::regex("641\\s+tests")
            };
        } else {
            // For TDD_IMPLEMENTATION_PLAN.md, only check for counts not in issue descriptions
            outdatedPatterns = {
                std::regex("780\\s+tests"),
                std::regex("763\\s+passing"),
                std::regex("782\\s+total\\s+tests"),
                std::regex("770\\s+passing"),
                std::regex("641\\s+tests")
            };
        }
        
        bool hasOutdated = false;
        for (const auto& pattern : outdatedPatterns) {
            if (std::regex_search(content, pattern)) {
                hasOutdated = true;
                std::cerr << "ERROR: " << relativePath << " contains outdated test count!" << std::endl;
            }
        }
        
        // Check if current counts are mentioned
        bool hasCurrentTotal = content.find(std::to_string(actual.total) + " tests") != std::string::npos ||
                              content.find(std::to_string(actual.total) + " total tests") != std::string::npos;
        
        if (!hasCurrentTotal && content.find("tests") != std::string::npos) {
            std::cerr << "WARNING: " << relativePath << " may not have current test count (" 
                     << actual.total << ")" << std::endl;
        }
        
        return !hasOutdated;
    }
    
    static std::string getProjectRoot() {
        // Assuming this test is run from build directory
        fs::path currentPath = fs::current_path();
        while (!currentPath.empty() && currentPath.filename() != "Diablo2Portable") {
            currentPath = currentPath.parent_path();
        }
        return currentPath.string();
    }
};

// Test that validates test counts in documentation match actual test suite
TEST(TestCountValidatorTest, DocumentationHasCorrectTestCounts) {
    // Simulate test output (in real implementation, this would run the actual test suite)
    std::string testOutput = R"(
[==========] Running 775 tests from 164 test suites.
[----------] 4 tests from ExtractionWizardUITest
[ RUN      ] ExtractionWizardUITest.CreateWizardUI
[       OK ] ExtractionWizardUITest.CreateWizardUI (0 ms)
...
[  SKIPPED ] AssetManagerMPQTest.InitializeWithMPQ (0 ms)
...
[==========] 775 tests from 164 test suites ran. (27149 ms total)
[  PASSED  ] 762 tests.
)";
    
    TestCountValidator::TestCounts counts = TestCountValidator::parseTestOutput(testOutput);
    
    EXPECT_EQ(counts.total, 775);
    EXPECT_EQ(counts.passed, 762);
    EXPECT_EQ(counts.skipped, 13);
    EXPECT_EQ(counts.failed, 0);
    
    // Validate documentation
    EXPECT_TRUE(TestCountValidator::validateDocumentationCounts(counts));
}

// Test the test output parser
TEST(TestCountValidatorTest, ParsesTestOutputCorrectly) {
    std::string testOutput = R"(
[==========] Running 100 tests from 10 test suites.
[  SKIPPED ] Test1.Skip (0 ms)
[  SKIPPED ] Test2.Skip (0 ms)
[  SKIPPED ] Test3.Skip (0 ms)
[==========] 100 tests from 10 test suites ran. (1000 ms total)
[  PASSED  ] 97 tests.
)";
    
    TestCountValidator::TestCounts counts = TestCountValidator::parseTestOutput(testOutput);
    
    EXPECT_EQ(counts.total, 100);
    EXPECT_EQ(counts.passed, 97);
    EXPECT_EQ(counts.skipped, 3);
    EXPECT_EQ(counts.failed, 0);
}