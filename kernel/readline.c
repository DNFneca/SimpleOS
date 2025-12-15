//#include <string.h>
//#include <stdbool.h>
//
//#include "../include/readline.h"
//#include "../include/console.h"
//#include "../include/keyboard.h"
//
//#define HISTORY_SIZE 10
//static char history[HISTORY_SIZE][128];
//static int history_count = 0;
//static int history_pos = -1;
//
//void readline_clear_line_and_prompt(int len) {
//    // Rewind cursor by length + prompt (" > ")
//    for (int i = 0; i < len + 2; i++) {
//        console_write("\b");
//    }
//
//    // Write blanks to clear the whole line
//    for (int i = 0; i < len + 2; i++) {
//        console_write(" ");
//    }
//
//    // Rewind cursor back to the start of the line
//    for (int i = 0; i < len + 2; i++) {
//        console_write("\b");
//    }
//}
//
//void readline_clear_line(int width) {
//    for (int x = 0; x < width; x++) {
//        console_putc_at(' ', row, x);
//    }
//    col = 0;
//    console_update_cursor();
//}
//
//void add_history(const char* line) {
//    if (*line == 0) return;
//
//    for (int i = 0; i < 128; i++)
//        history[history_count % HISTORY_SIZE][i] = line[i];
//
//    history_count++;
//}
//
//void load_history(char* buffer, int* len, int direction) {
//    if (history_count == 0)
//        return;
//
//    if (direction == -1) { // UP
//        if (history_pos < history_count - 1)
//            history_pos++;
//    } else if (direction == +1) { // DOWN
//        if (history_pos > -1)
//            history_pos--;
//    }
//
//    int index = history_count - 1 - history_pos;
//    if (history_pos == -1) {
//        buffer[0] = 0;
//        *len = 0;
//        return;
//    }
//
//    for (int i = 0; i < 128; i++) {
//        buffer[i] = history[index % HISTORY_SIZE][i];
//        if (!buffer[i]) break;
//    }
//
//    *len = 0;
//    while (buffer[*len]) (*len)++;
//}
//
//void redraw(const char* buffer, int len) {
//	for (int i = len; i >= 0; i--) {
//		console_write("\b \b");
//	}
//    console_write("> ");
//    for (int i = 0; i < len; i++)
//        console_putc(buffer[i]);
//}
//
//void readline(char* buffer, int max) {
//    int len = 0;
//    history_pos = -1;
//
//	for (int i = 0; i < max; i++) buffer[i] = 0;
//
//    console_write("> ");
//
//    while (true) {
//        char c = keyboard_get_char();
//
//        // ENTER
//        if (c == '\n') {
//            console_putc('\n');
//            buffer[len] = 0;
//            add_history(buffer);
//            return;
//        }
//
//        // BACKSPACE
//        if (c == 8) {
//            if (len > 0) {
//                len--;
//                console_write("\b");
//    			console_write(" ");
//			    console_write("\b");
//            }
//            continue;
//        }
//
//        // ARROW UP
//        if (c == (char)0xF1) {
//			readline_clear_line(80);
//            load_history(buffer, &len, -1);
//            redraw(buffer, len);
//            continue;
//        }
//
//        // ARROW DOWN
//        if (c == (char)0xF2) {
//			readline_clear_line(80);
//            load_history(buffer, &len, +1);
//            redraw(buffer, len);
//            continue;
//        }
//
//        // Only printable characters
//        if (c >= 32 && c < 127 && len < max - 1) {
//            buffer[len++] = c;
//            buffer[len] = 0;
//            console_putc(c); // echo to screen
//        }
//    }
//}
