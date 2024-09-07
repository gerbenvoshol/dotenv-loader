#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#include "dotenv.h"

// Utility function to trim leading and trailing whitespace
static char *trim_whitespace(char *str) {
    char *end;

    // Trim leading space
    while (isspace((unsigned char)*str)) str++;

    // Trim trailing space
    if (*str == 0)  // All spaces?
        return str;

    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    // Write new null terminator
    *(end + 1) = '\0';

    // Ensure cross-platform compatibility by stripping \r characters which are common in Windows line endings (\r\n).
    if (str[strlen(str) - 1] == '\r') {
        str[strlen(str) - 1] = '\0';
    }

    return str;
}

// Parse the value part of a line, supporting quotes and comments
static void parse_value(char *value, char *output) {
    value = trim_whitespace(value);

    if (*value == '"') {  // Handle quoted values
        value++;  // Skip the opening quote
        
        char *write = output;
        bool escaped = false;
        while (*value) {
            if (*value == '\\' && !escaped) {
                escaped = true;
            } else if (*value == '"' && !escaped) {
                break;
            } else {
                if (escaped) {
                    switch (*value) {
                        case 'n': *write++ = '\n'; break;
                        case 'r': *write++ = '\r'; break;
                        case 't': *write++ = '\t'; break;
                        default: *write++ = *value;
                    }
                    escaped = false;
                } else {
                    *write++ = *value;
                }
            }
            value++;
        }
        *write = '\0';
    } else {
        // For unquoted values, handle comments and trim spaces
        char *comment_pos = strchr(value, '#');
        if (comment_pos) {
            *comment_pos = '\0';  // Ignore everything after the comment
        }
        strncpy(output, trim_whitespace(value), LINE_SIZE - 1);
        output[LINE_SIZE - 1] = '\0';  // Ensure null-termination
    }
}

static bool is_valid_variable_name(const char *name) {
    if (!isalpha(*name) && *name != '_') return false;
    for (const char *p = name + 1; *p; p++) {
        if (!isalnum(*p) && *p != '_') return false;
    }

    if (strlen(name) > MAX_VAR_NAME) {
        fprintf(stderr, "Variable name exceeds maximum allowed length\n");
        return false;
    }

    return true;
}

static int parse_line(char *line, dotenv_entry *entry) {
    char *delimiter = strchr(line, '=');

    if (!delimiter)
        return -1;  // Invalid line (no '=' found)

    // Split into name and value
    *delimiter = '\0';
    char *name = trim_whitespace(line);
    if (!is_valid_variable_name(name)) {
        return -1;  // Invalid variable name
    }
    strncpy(entry->name, name, LINE_SIZE);
    parse_value(delimiter + 1, entry->value);

    return 0;
}

// Load the .env file and set environment variables
int dotenv_load(const char *path, bool override) {
    FILE *file = fopen(path, "r");
    if (!file) {
        perror("Failed to open .env file");
        return -1;
    }

    char line[LINE_SIZE];
    dotenv_entry entry;

    while (fgets(line, LINE_SIZE, file)) {
        // Ignore empty lines and comments
        char *trimmed = trim_whitespace(line);
        if (trimmed[0] == '\0' || trimmed[0] == '#') {
            continue;
        }

        // Parse the line into name and value
        if (parse_line(trimmed, &entry) == 0) {
            if (override || getenv(entry.name) == NULL) {
                setenv(entry.name, entry.value, 1);  // Set the environment variable
            }
        } else {
            fprintf(stderr, "Skipping invalid line: %s\n", trimmed);
        }
    }

    fclose(file);
    return 0;
}
