# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g

# Directories and files
SRC = dotenv.c main.c
TEST_SRC = test_dotenv.c dotenv.c
OBJ = $(SRC:.c=.o)
TEST_OBJ = $(TEST_SRC:.c=.o)
EXEC = dotenv
TEST_EXEC = test_dotenv

# Unit test target
all: $(EXEC) $(TEST_EXEC)

# Main application target
$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

# Unit test executable
$(TEST_EXEC): $(TEST_OBJ)
	$(CC) $(CFLAGS) -o $@ $(TEST_OBJ)

# Pattern rule to compile .c files to .o
%.o: %.c dotenv.h
	$(CC) $(CFLAGS) -c $< -o $@

# Run tests
test: $(TEST_EXEC)
	./$(TEST_EXEC)

# Clean object and executable files
clean:
	rm -f $(OBJ) $(TEST_OBJ) $(EXEC) $(TEST_EXEC)

.PHONY: all test clean
