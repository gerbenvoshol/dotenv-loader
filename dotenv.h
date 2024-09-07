#ifndef DOTENV_H
#define DOTENV_H

#if !defined(_POSIX_C_SOURCE) || _POSIX_C_SOURCE < 200112L
#undef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200112L
#endif

#include <stdbool.h>

// Define the maximum size for a line and a buffer for environment variable names/values
#define LINE_SIZE 1024

// To conform with conventional limits (e.g., NAME_MAX for UNIX systems), we limit the name length to 256
#define MAX_VAR_NAME 256

// Structure to hold a dotenv entry with a name and value
typedef struct {
    char name[LINE_SIZE];
    char value[LINE_SIZE];
} dotenv_entry;

/**
 * @brief Loads the environment variables from a `.env` file.
 *
 * @param path Path to the `.env` file.
 * @param override If true, existing environment variables will be overridden. If false, existing variables will be preserved.
 * @return int Returns 0 on success, and -1 on failure (e.g., file could not be opened).
 *
 * This function reads a `.env` file at the given path, parses each line for name-value pairs, and sets them as environment variables using `setenv`.
 */
int dotenv_load(const char *path, bool override);

#endif // DOTENV_H
