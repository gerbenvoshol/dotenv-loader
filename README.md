# dotenv-loader

This repository contains a C implementation for loading and parsing environment variables from .env files into system environment variables. It supports common .env features such as quoted values, escaping, comments, and more. The repository includes unit tests for various edge cases and features.

Features:
* **Whitespace handling:** Leading and trailing spaces are automatically trimmed.
* **Quoted values:** Supports values enclosed in double quotes, including escape sequences like \n, \t, and \".
* **Inline comments:** Lines can contain inline comments, starting with #.
* **Override behavior:** You can choose to overwrite existing environment variables.
* **UTF-8 support:** Fully supports UTF-8 encoded values.
* **Error handling:** Skips invalid lines or variables with malformed names.
* **Environment variable constraints:** Limits the length of variable names and values, ensuring compatibility with common system limits.

## Build and Test
### Build

The 'Makefile' provided can be used to build the main executable and the test suite.

To build the main program:

```bash
make
```

### Running Tests

The library includes extensive unit tests. To run them, simply use:

```bash
make test
```

### Clean

To remove the compiled binaries and object files:

```bash
make clean
```

### Usage

Loading `.env` Files

To load variables from a `.env` file, use the dotenv_load function from `dotenv.h`:

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#include "dotenv.h"

int main() {
    // Load the .env file and override existing variables
    if (dotenv_load("example/.env", true) == 0) {
        // Successfully loaded variables
        printf("API_KEY=%s\n", getenv("API_KEY"));
    } else {
        // Error handling
        printf("Failed to load .env file.\n");
    }
    
    return 0;
}
```

Include the dotenv.h and dotenv.c files in your project.

**Compile**

```bash
gcc -o main main.c dotenv.c -I.
./main
```

## Function Documentation

```C
int dotenv_load(const char *path, bool override);
```
* Description: Loads environment variables from a .env file and adds them to the system environment.
* Parameters:
    * path: Path to the .env file.
    * override: If true, existing environment variables will be overridden. If false, only new variables will be added.
* Returns: 0 on success, -1 on failure (e.g., file not found).

## Unit Tests

The repository includes comprehensive unit tests that cover a wide range of scenarios. The tests cover cases such as:

* Basic variable parsing.
* Quoted and escaped values.
* Handling of UTF-8 characters.
* Ignoring invalid variable names and lines.
* Large files and long lines.

Example test case from test_dotenv.c:

```c
void test_basic_variable_parsing() {
    const char *env_content = 
        "API_KEY=123456\n"
        "DEBUG=true\n";

    test_env_loading(env_content, true);

    assert(strcmp(getenv("API_KEY"), "123456") == 0);
    assert(strcmp(getenv("DEBUG"), "true") == 0);
}
```

## Contributing

If you would like to contribute to this project, feel free to submit a pull request or file an issue.

## License

This project is licensed under the Unlicense License. See the LICENSE file for more information.
