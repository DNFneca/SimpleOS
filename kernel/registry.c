//#include "../include/registry.h"
//#include "../include/console.h"
//#include "../include/string.h"
//#include "../include/memory.h"
//#include <stdlib.h>
//
//// Head of the command linked list
//static command_t* command_head = NULL;
//
//// Example commands (to be registered in init_commands)
//void cmd_hello(int argc, char** argv) {
//    console_write("Hello from the kernel shell!\n");
//}
//
//void cmd_help(int argc, char** argv) {
//    print_help();
//}
//
//// Helper function to print all registered commands
//void print_help() {
//    command_t* current = command_head;
//    console_write("Available Commands:\n");
//    while (current != NULL) {
//        console_write("  ");
//        console_write(current->name);
//        console_write(" - ");
//        console_write(current->description);
//        console_write("\n");
//        current = current->next;
//    }
//}
//
//// Register a new command (adds to the linked list)
//int register_command(const char* name, const char* description, command_func_t func) {
//    // 1. Allocate memory for the new command node
//    command_t* new_command = (command_t*)malloc(sizeof(command_t));
//    if (!new_command) {
//        console_write("Error: Failed to allocate memory for command.\n");
//        return -1;
//    }
//
//    // 2. Populate the node (using const pointers for name/description is fine)
//    new_command->name = name;
//    new_command->description = description;
//    new_command->func = func;
//    new_command->next = NULL;
//
//    // 3. Add to the end of the list (or beginning, using end for easier traversal check)
//    if (command_head == NULL) {
//        command_head = new_command;
//    } else {
//        command_t* current = command_head;
//        while (current->next != NULL) {
//            // Also check for duplicates here
//            if (strcmp(current->name, name) == 0) {
//                // Free and exit if duplicate is found
//                // (This check is simplified, better to check before allocation)
//                free(new_command);
//                return -1;
//            }
//            current = current->next;
//        }
//        current->next = new_command;
//    }
//
//    return 0;
//}
//
//// The main command execution function
//void execute_command(int argc, char** argv) {
//    if (argc == 0 || argv[0] == NULL) return;
//
//    command_t* current = command_head;
//    while (current != NULL) {
//        if (strcmp(argv[0], current->name) == 0) {
//            // Found command, execute function pointer
//            current->func(argc, argv);
//            return;
//        }
//        current = current->next;
//    }
//
//    console_write("Unknown command: ");
//    console_write(argv[0]);
//    console_write("\n");
//}
//
//// Initializer called from kmain
//void init_commands() {
//    // Note: The memory for the command name/description strings must persist
//    register_command("hello", "Prints a simple greeting.", cmd_hello);
//    register_command("help", "Lists all available commands.", cmd_help);
//}