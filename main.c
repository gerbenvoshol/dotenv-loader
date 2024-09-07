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
