![image](https://github.com/user-attachments/assets/63a76ab1-49dd-4f89-8c17-0582faa2ae98)

# Clock Project

This project is a clock application that displays the time on an LCD screen.

## Key Functions

- `lcd_puts("");`: Displays a string on the LCD screen.
- `lcd_init()`: Initializes the LCD screen.
- `lcd_clear()`: Clears the LCD screen.
- `lcd_gotoxy(x, y)`: Sets the cursor position on the LCD screen.
- `lcd_putch(char)`: Displays a single character on the LCD screen.
- `lcd_putsf("string")`: Displays a string stored in flash memory on the LCD screen.

## Files

- [`clock.c`](clock.c): Main C source file.
- [`Debug/List/clock.asm`](Debug/List/clock.asm): Assembly listing file.
- [`Debug/Exe/clock.a`](Debug/Exe/clock.a): Compiled assembly file.

## How to Build

Use the provided project files and your preferred IDE or build system.

## Author

Mehdi Salmanzadeh
