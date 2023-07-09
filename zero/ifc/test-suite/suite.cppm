/**
 * The Zero's project custom test-suite
 *
 * This is a minimalistic implementation of a test-suite
 * that mainly allows ourselves to develop our source code
 * with confidence, without depending on some third-party
 * changing standards, implementations while avoiding the
 * typical dependency pitfalls
*/

export module tsuite;

import std;

struct TestResults {
    int passed = 0;
    int failed = 0;
    std::vector<std::string> warnings;
};

export {
    // Forward declarations  // TODO - Split in modules, so we don't have to forward declare anything
    struct TestSuite;
    struct TestCase;
    void runTest(const TestCase& testCase, TestResults& testResults); // Move to the non-public part of this interface

    /// Common group of related test cases
    struct TestSuite {
        std::string uuid;
        std::vector<TestCase> cases;

        friend bool operator==(const TestSuite& lhs, const TestSuite& rhs) {
            return lhs.uuid == rhs.uuid;
        }
    };

    /// Define a struct to represent a test case
    struct TestCase {
        std::string name;
        std::function<void()> fn;
    };

    std::vector<TestSuite> testSuites;
    std::vector<TestCase> freeTestCases;

    void TEST_CASE(const std::string& tname, const std::function<void()> tfunc) {
        freeTestCases.push_back({tname, tfunc});
    }

    void TEST_CASE(TestSuite& tsuite, const std::string& tname, const std::function<void()> tfunc) {
        if (tsuite.uuid.empty()) {
            std::ostringstream oss;
            oss << "Autogenerated_tsuite_uuid_" << (testSuites.size() + 1);
            tsuite.uuid = oss.str();
        }

        auto it = std::find_if(tsuite.cases.begin(), tsuite.cases.end(), [&](const TestCase& target) {
            return target.name == tname;
        });

        if (it != tsuite.cases.end())
            tsuite.cases.push_back({tname, tfunc});
        else
            // TODO Use a results structs or whatever to hold this info an-td print it later as ... warnings?!
            // Or convert the suite to a class and store them there
            std::cout << "\033[38;5;220m[Warning]\033[0m Already exists a test case with the name: " << tname << ". Skipping test case\n";
    }

    // Assertion function to compare two values and throw an exception if they are not equal
    template<typename T>
    void assertEquals(const T& expected, const T& actual) {
        if (expected != actual)
            throw std::runtime_error("Assertion failed: expected = " + std::to_string(expected) +
                                     ", actual = " + std::to_string(actual));
    }

// Function to run all the test cases in the test suite
    void runTests() {
        TestResults freeTestsResults;
        std::cout << "Running free tests: " << std::endl;
        for (const auto& testCase : freeTestCases) {
            runTest(testCase, freeTestsResults);
            std::cout << std::endl;
        }

        std::cout << "Free tests summary:" << std::endl;
        std::cout << "    Passed: " << freeTestsResults.passed << std::endl;
        std::cout << "    Failed: " << freeTestsResults.failed << std::endl;

        std::cout << "\nRunning test suites: " << std::endl;
        for (const auto& testSuite : testSuites) {
            TestResults suite_results;
            std::cout << "    Running test suite: " << testSuite.uuid << std::endl;

            for (const auto& testCase : testSuite.cases)
                runTest(testCase, suite_results);

            std::cout << "Test suite [" << testSuite.uuid << "] summary:" << std::endl;
            std::cout << "    Passed: " << suite_results.passed << std::endl;
            std::cout << "    Failed: " << suite_results.failed << std::endl;
        }
    }
}

void runTest(const TestCase& testCase, TestResults& results) {
    std::cout << "    Running test: \033[38;5;117m" << testCase.name << "\033[0m";

    try {
        // Call the test function
        testCase.fn();
        std::cout << " ... Result => \033[32mPassed!\033[0m";
        results.passed++;
    } catch (const std::exception& ex) {
        std::cout << " ... Result => \033[31mFailed\033[0m: " << ex.what();
        results.failed++;
    }
}