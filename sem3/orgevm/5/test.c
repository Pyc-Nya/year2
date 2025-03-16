#include <stdio.h>
#include <conio.h>
#include <dos.h>
#include <ctype.h>

// Pointer to the old keyboard interrupt handler
void interrupt (*oldkeyboard)();

// Variable to track the Shift key status
int shiftPressed = 0;

// Table for converting scancodes to characters
char scancodeToChar(unsigned char scancode) {
    // Convert scancodes to characters using a lookup table
    switch (scancode) {
        case 0x10: return 'Q';  // Q
        case 0x11: return 'W';  // W
        case 0x12: return 'E';  // E
        case 0x13: return 'R';  // R
        case 0x14: return 'T';  // T
        case 0x15: return 'Y';  // Y
        case 0x16: return 'U';  // U
        case 0x17: return 'I';  // I
        case 0x18: return 'O';  // O
        case 0x19: return 'P';  // P
        case 0x1E: return 'A';  // A
        case 0x1F: return 'S';  // S
        case 0x20: return 'D';  // D
        case 0x21: return 'F';  // F
        case 0x22: return 'G';  // G
        case 0x23: return 'H';  // H
        case 0x24: return 'J';  // J
        case 0x25: return 'K';  // K
        case 0x26: return 'L';  // L
        case 0x2C: return 'Z';  // Z
        case 0x2D: return 'X';  // X
        case 0x2E: return 'C';  // C
        case 0x2F: return 'V';  // V
        case 0x30: return 'B';  // B
        case 0x31: return 'N';  // N
        case 0x32: return 'M';  // M
        default: return 0; // If it's not a letter, return 0
    }
}

// Custom implementation of toupper
char my_toupper(char c) {
    // If the character is a lowercase letter, convert it to uppercase
    if (c >= 'a' && c <= 'z') {
        return c - 32; // ASCII difference between lowercase and uppercase
    }
    return c; // Return the character unchanged if it's not lowercase
}

// Custom implementation of tolower
char my_tolower(char c) {
    // If the character is an uppercase letter, convert it to lowercase
    if (c >= 'A' && c <= 'Z') {
        return c + 32; // ASCII difference between uppercase and lowercase
    }
    return c; // Return the character unchanged if it's not uppercase
}

// Keyboard interrupt handler function
void interrupt newkeyboard() {
    unsigned char scancode;
    char key;

    // disable();  // Disable interrupts for data consistency?
    scancode = inport(0x60);  // Get the scancode of the pressed key

    // If the key is released (0x80 bit set)
    if (scancode & 0x80) {
        // If Shift key is released
        if (scancode == 0xAA || scancode == 0xB6) {
            shiftPressed = 0;  // Clear the Shift flag
        }
    } else {
        // If Shift key is pressed
        if (scancode == 0x2A || scancode == 0x36) {
            shiftPressed = 1;  // Set the Shift flag
        }

        // Convert the scancode to a character
        key = scancodeToChar(scancode);

        // If it's a letter, toggle the case based on Shift status
        if (key != 0) {
            if (shiftPressed) {
                key = my_tolower(key);  // Convert to lowercase
            } else {
                key = my_toupper(key);  // Convert to uppercase
            }

            // Output the character to the screen
            printf("%c", key);
        }
    }

    // Restore the old keyboard handler
    oldkeyboard();
    // enable();  // Enable interrupts again
}

int main() {
    // Initialize screen
    clrscr();
    textbackground(0);
    gotoxy(1, 1);
    printf("Press any key. Press ESC to exit.\n");

    // Save the old keyboard handler
    oldkeyboard = getvect(0x09);

    // Set the new keyboard interrupt handler
    setvect(0x09, newkeyboard);

    // Infinite loop to keep processing keys
    while (1) {
        if (kbhit()) {
            char al = getch();
            if (al == 27) {  // ESC to exit
                break;
            }
        }
    }

    // Restore the old keyboard interrupt handler
    setvect(0x09, oldkeyboard);
    return 0;
}
