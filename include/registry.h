#pragma once

// Function pointer for a command (matches execute_command signature)
typedef void (*command_func_t)(int argc, char** argv);

// Structure for a registry entry
typedef struct command_t {
    const char* name;
    const char* description;
    command_func_t func;
    struct command_t* next; // For linked list
} command_t;

// Function declarations
void init_commands();
int register_command(const char* name, const char* description, command_func_t func);
void execute_command(int argc, char** argv);
void print_help(); // Helper for help command
