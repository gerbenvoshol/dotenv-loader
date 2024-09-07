#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include "dotenv.h"

// Utility function to simulate setting environment variables without writing a file
static void test_env_loading(const char *env_content, bool override) {
    FILE *file = fopen("test.env", "w");
    assert(file != NULL);
    fprintf(file, "%s", env_content);
    fclose(file);

    // Load the .env file
    int result = dotenv_load("test.env", override);
    assert(result == 0);
    remove("test.env");
}

// Utility function to run tests and print their execution time
void run_test(void (*test_func)(), const char *test_name) {
    printf("Running test: %s\n", test_name);
    clock_t start_time = clock();

    test_func();

    clock_t end_time = clock();
    double time_taken = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("Test '%s' passed. Time taken: %.6f seconds\n\n", test_name, time_taken);
}

// Test cases
void test_basic_variable_parsing() {
    const char *env_content = 
        "API_KEY=123456\n"
        "DEBUG=true\n";

    test_env_loading(env_content, true);

    assert(strcmp(getenv("API_KEY"), "123456") == 0);
    assert(strcmp(getenv("DEBUG"), "true") == 0);
}

void test_quoted_values() {
    const char *env_content = 
        "DATABASE_URL=\"postgres://user:pass@localhost/db\"\n";

    test_env_loading(env_content, true);

    assert(strcmp(getenv("DATABASE_URL"), "postgres://user:pass@localhost/db") == 0);
}

void test_escaped_quotes() {
    const char *env_content = 
        "ESCAPED1=\"This is a \\\"quoted\\\" value\"\n"
        "ESCAPED2=\"Another \\\"quote\\\" example\"\n"
        "ESCAPED3=\"Backslash \\\\ and quote \\\"\"\n"
        "ESCAPED4=\"Tricky \\\\\\\" case\"\n";

    test_env_loading(env_content, true);

    const char *test_cases[][2] = {
        {"ESCAPED1", "This is a \"quoted\" value"},
        {"ESCAPED2", "Another \"quote\" example"},
        {"ESCAPED3", "Backslash \\ and quote \""},
        {"ESCAPED4", "Tricky \\\" case"},
    };

    for (long unsigned int i = 0; i < sizeof(test_cases) / sizeof(test_cases[0]); i++) {
        const char *var_name = test_cases[i][0];
        const char *expected = test_cases[i][1];
        const char *actual = getenv(var_name);
        
        assert(strcmp(actual, expected) == 0);
    }
}

void test_utf8_support() {
    const char *env_content = 
        "UTF8_VAR=\"Café\"\n";

    test_env_loading(env_content, true);

    assert(strcmp(getenv("UTF8_VAR"), "Café") == 0);
}

void test_missing_equals_sign() {
    const char *env_content = 
        "INVALID_LINE\n"
        "DEBUG=true\n";

    test_env_loading(env_content, true);

    // DEBUG should be loaded, but INVALID_LINE should be ignored
    assert(getenv("DEBUG") != NULL);
    assert(getenv("INVALID_LINE") == NULL);
}

void test_comments() {
    const char *env_content = 
        "# This is a comment\n"
        "API_KEY=123456 # Inline comment\n";

    test_env_loading(env_content, true);

    assert(strcmp(getenv("API_KEY"), "123456") == 0);
}

void test_override_behavior() {
    // Set initial environment variable
    setenv("DEBUG", "false", 1);

    const char *env_content = 
        "DEBUG=true\n";

    test_env_loading(env_content, false); // Override = false, should not change

    assert(strcmp(getenv("DEBUG"), "false") == 0); // Should not be overridden

    test_env_loading(env_content, true); // Override = true, should change

    assert(strcmp(getenv("DEBUG"), "true") == 0); // Should be overridden
}

void test_empty_value() {
    const char *env_content = 
        "EMPTY_VAR=\n"
        "ANOTHER_VAR=value\n";

    test_env_loading(env_content, true);

    assert(strcmp(getenv("EMPTY_VAR"), "") == 0);
    assert(strcmp(getenv("ANOTHER_VAR"), "value") == 0);
}

void test_multiline_value() {
    const char *env_content = 
        "MULTILINE=\"This is a\\n"
        "multiline\\n"
        "value\"\n";

    test_env_loading(env_content, true);

    const char *expected = "This is a\nmultiline\nvalue";
    const char *actual = getenv("MULTILINE");
    
    assert(actual != NULL);
    assert(strcmp(actual, expected) == 0);
    if (strcmp(actual, expected) != 0) {
        printf("Expected: %s\n", expected);
        printf("Actual: %s\n", actual);
    }
}

void test_leading_trailing_whitespace() {
    const char *env_content = 
        "  LEADING_SPACE=value\n"
        "TRAILING_SPACE=value  \n"
        "  BOTH_SPACE  =  value  \n";

    test_env_loading(env_content, true);

    assert(strcmp(getenv("LEADING_SPACE"), "value") == 0);
    assert(strcmp(getenv("TRAILING_SPACE"), "value") == 0);
    assert(strcmp(getenv("BOTH_SPACE"), "value") == 0);
}

void test_invalid_variable_names() {
    const char *env_content = 
        "1INVALID=value\n"
        "VALID2=value\n"
        "INVALID-NAME=value\n"
        "VALID_NAME=value\n";

    test_env_loading(env_content, true);

    assert(getenv("1INVALID") == NULL);
    assert(strcmp(getenv("VALID2"), "value") == 0);
    assert(getenv("INVALID-NAME") == NULL);
    assert(strcmp(getenv("VALID_NAME"), "value") == 0);
}

void test_duplicate_variables() {
    const char *env_content = 
        "DUPLICATE=first\n"
        "DUPLICATE=second\n";

    test_env_loading(env_content, true);

    assert(strcmp(getenv("DUPLICATE"), "second") == 0);
}

void test_large_file() {
    FILE *file = fopen("large_test.env", "w");
    assert(file != NULL);
    
    for (int i = 0; i < 1000; i++) {
        fprintf(file, "VAR%d=value%d\n", i, i);
    }
    fclose(file);

    int result = dotenv_load("large_test.env", true);
    assert(result == 0);

    for (int i = 0; i < 1000; i++) {
        char var_name[20];
        char expected_value[20];
        snprintf(var_name, sizeof(var_name), "VAR%d", i);
        snprintf(expected_value, sizeof(expected_value), "value%d", i);
        assert(strcmp(getenv(var_name), expected_value) == 0);
    }

    remove("large_test.env");
}

void test_long_line() {
    char long_line[LINE_SIZE * 2];
    memset(long_line, 'A', LINE_SIZE * 2 - 1);
    long_line[LINE_SIZE * 2 - 1] = '\0';
    test_env_loading(long_line, true);
    assert(getenv("A") == NULL);  // Should fail due to long line.
}

void test_extremely_large_file() {
    FILE *file = fopen("huge_test.env", "w");
    assert(file != NULL);
    
    for (int i = 0; i < 100000; i++) {
        fprintf(file, "VAR%d=value%d\n", i, i);
    }
    fclose(file);

    int result = dotenv_load("huge_test.env", true);
    assert(result == 0);

    for (int i = 0; i < 100000; i++) {
        char var_name[20];
        char expected_value[20];
        snprintf(var_name, sizeof(var_name), "VAR%d", i);
        snprintf(expected_value, sizeof(expected_value), "value%d", i);
        assert(strcmp(getenv(var_name), expected_value) == 0);
    }

    remove("huge_test.env");
}

void test_invalid_variable_characters() {
    const char *env_content = 
        "VAR$NAME=value\n"
        "NAME_WITH_SPACE = value\n"
        "NAME-WITH-DASH=value\n";

    test_env_loading(env_content, true);

    assert(getenv("VAR$NAME") == NULL);
    assert(strcmp(getenv("NAME_WITH_SPACE"), "value") == 0);
    assert(getenv("NAME-WITH-DASH") == NULL);
}

void test_duplicate_variable_values() {
    const char *env_content = 
        "DUP_VAR=first\n"
        "DUP_VAR=second\n"
        "DUP_VAR=third\n";

    test_env_loading(env_content, true);

    assert(strcmp(getenv("DUP_VAR"), "third") == 0);  // Should retain the last value
}

void test_only_whitespace_lines() {
    const char *env_content = 
        "   \n"  // Only spaces
        "\t\t\n" // Only tabs
        "# Comment\n"
        "KEY=value\n";

    test_env_loading(env_content, true);

    assert(strcmp(getenv("KEY"), "value") == 0);
}

void test_escaped_newlines() {
    const char *env_content = 
        "MULTILINE_VAR=\"Line1\\nLine2\\nLine3\"\n";

    test_env_loading(env_content, true);

    const char *expected = "Line1\nLine2\nLine3";
    assert(strcmp(getenv("MULTILINE_VAR"), expected) == 0);
}

// Main function to run all tests
int main() {
    printf("Running extended dotenv tests...\n");

    // Run tests with timing and printing
    run_test(test_basic_variable_parsing, "Basic Variable Parsing");
    run_test(test_quoted_values, "Quoted Values");
    run_test(test_escaped_quotes, "Escaped Quotes");
    run_test(test_utf8_support, "UTF-8 Support");
    run_test(test_missing_equals_sign, "Missing Equals Sign");
    run_test(test_comments, "Comments");
    run_test(test_override_behavior, "Override Behavior");
    run_test(test_empty_value, "Empty Value");
    run_test(test_multiline_value, "Multiline Value");
    run_test(test_leading_trailing_whitespace, "Leading and Trailing Whitespace");
    run_test(test_invalid_variable_names, "Invalid Variable Names");
    run_test(test_duplicate_variables, "Duplicate Variables");
    run_test(test_long_line, "Long Line");
    run_test(test_invalid_variable_characters, "Invalid Variable Characters");
    run_test(test_duplicate_variable_values, "Duplicate Variable Values");
    run_test(test_only_whitespace_lines, "Only Whitespace Lines");
    run_test(test_escaped_newlines, "Escaped Newlines");
    run_test(test_large_file, "Large File");
    run_test(test_extremely_large_file, "Extremely Large File");

    printf("All tests passed successfully!\n");

    return 0;
}
