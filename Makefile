# Define compiler and flags
CC = gcc
CFLAGS = -Wall -std=c11

# Define the executable name
TARGET = fetchmail

# Define the source files
SRC = main.c login.c retrieve.c helperfunctions.c
# Define the object files
OBJ = $(SRC:.c=.o)

# Define the header files
HEADERS = login.h retrieve.h helperfunctions.h

# Default target: build the executable
all: $(TARGET)

# Rule to build the executable
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

# Rule to build object files
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to clean the build
clean:
	rm -f $(OBJ) $(TARGET)

# Phony targets
.PHONY: all clean