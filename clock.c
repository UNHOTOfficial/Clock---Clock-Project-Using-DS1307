/*******************************************************
This program was created by the
CodeWizardAVR V3.14 Advanced
Automatic Program Generator
? Copyright 1998-2014 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com

Project : 
Version : 
Date    : 1/6/2025
Author  : 
Company : 
Comments: 

Chip type               : ATmega32A
Program type            : Application
AVR Core Clock frequency: 8.000000 MHz
Memory model            : Small
External RAM size       : 0
Data Stack size         : 512
*******************************************************/

#include <mega32a.h>
#include <delay.h>
#include <stdio.h>
#include <string.h>
#include <i2c.h>
#include <ds1307.h>
#include <alcd.h>

// Global variables
unsigned char h = 0, m = 0, s = 0;
unsigned char alarm_h = 0, alarm_m = 0, alarm_s = 0;
char clock_buffer[17]; // Buffer for clock display
char alarm_buffer[17]; // Buffer for alarm display
bit alarm_set = 0; // Flag to indicate if the alarm is set

// Function prototypes
void display_info(void);
void get_time(void);
void set_time(void);
void set_alarm(void);
void check_alarm(void);

void main(void)
{
    // Port A initialization: All pins as input
    DDRA = 0x00;
    PORTA = 0x00;

    // Port B initialization: PB4 as output, others as input
    DDRB = (1 << DDB4);
    PORTB = 0x00;

    // Port C initialization: All pins as input
    DDRC = 0x00;
    PORTC = 0x00;

    // Port D initialization: All pins as input
    DDRD = 0x00;
    PORTD = 0x00;

    // Timer/Counter initialization: All timers stopped
    TCCR0 = 0x00;
    TCCR1A = 0x00;
    TCCR1B = 0x00;
    TCCR2 = 0x00;
    TIMSK = 0x00;

    // External Interrupt(s) initialization: All interrupts off
    MCUCR = 0x00;
    MCUCSR = 0x00;

    // USART initialization: Disabled
    UCSRB = 0x00;

    // Analog Comparator initialization: Disabled
    ACSR = (1 << ACD);
    SFIOR = 0x00;

    // ADC initialization: Disabled
    ADCSRA = 0x00;

    // SPI initialization: Disabled
    SPCR = 0x00;

    // TWI initialization: Disabled
    TWCR = 0x00;

    // Bit-Banged I2C Bus initialization
    i2c_init();

    // DS1307 Real Time Clock initialization
    rtc_init(0, 0, 0);

    // Alphanumeric LCD initialization
    lcd_init(16);

    // Display name and student ID
    display_info();

    while (1)
    {
        // Get and display the time
        get_time();
        delay_ms(1000);

        // Check if any button is pressed to set the time
        if (PINB & (1 << PINB0) || PINB & (1 << PINB1) || PINB & (1 << PINB2) || PINB & (1 << PINB3))
        {
            set_time();
        }

        // Check if the alarm set button is pressed
        if (PINB & (1 << PINB5))
        {
            set_alarm();
        }

        // Check the alarm
        check_alarm();
    }
}

// Function to display name and student ID
void display_info(void)
{
    lcd_gotoxy(0, 0);
    
    lcd_gotoxy(0, 1);

    delay_ms(3000);
    lcd_clear();
}

// Function to get and display the time from RTC
void get_time(void)
{
    char period[3];
    rtc_get_time(&h, &m, &s);

    // Determine AM/PM period
    if (h >= 12)
    {
        strcpy(period, "PM");
        if (h > 12)
        {
            h -= 12;
        }
    }
    else
    {
        strcpy(period, "AM");
        if (h == 0)
        {
            h = 12;
        }
    }

    lcd_clear();
    lcd_gotoxy(0, 0);
    sprintf(clock_buffer, "Time: %02d:%02d:%02d%s", h, m, s, period);
    lcd_puts(clock_buffer);

    // Display the alarm time
    lcd_gotoxy(0, 1);
     if (alarm_set)
    {
        sprintf(alarm_buffer, "Alarm: %02d:%02d:%02d", alarm_h, alarm_m, alarm_s);
    }
    else
    {
        strcpy(alarm_buffer, "Alarm: Not set");
    }
    lcd_puts(alarm_buffer);
}

// Function to set the time using buttons
void set_time(void)
{
    lcd_clear();
    lcd_gotoxy(0, 0);
    lcd_puts("Set Time:");

    while (1)
    {
        // Check if button on PB0 is pressed to increment hour
        if (PINB & (1 << PINB0))
        {
            h = (h + 1) % 24;
            lcd_gotoxy(0, 1);
            sprintf(clock_buffer, "Hour: %02d", h);
            lcd_puts(clock_buffer);
            delay_ms(50); // Debounce delay
            while (PINB & (1 << PINB0)); // Wait for button release
            delay_ms(50); // Additional debounce delay
        }

        // Check if button on PB1 is pressed to increment minute
        if (PINB & (1 << PINB1))
        {
            m = (m + 1) % 60;
            lcd_gotoxy(0, 1);
            sprintf(clock_buffer, "Minute: %02d", m);
            lcd_puts(clock_buffer);
            delay_ms(50); // Debounce delay
            while (PINB & (1 << PINB1)); // Wait for button release
            delay_ms(50); // Additional debounce delay
        }

        // Check if button on PB2 is pressed to increment second
        if (PINB & (1 << PINB2))
        {
            s = (s + 1) % 60;
            lcd_gotoxy(0, 1);
            sprintf(clock_buffer, "Second: %02d", s);
            lcd_puts(clock_buffer);
            delay_ms(50); // Debounce delay
            while (PINB & (1 << PINB2)); // Wait for button release
            delay_ms(50); // Additional debounce delay
        }

        // Check if the save button on PB3 is pressed to save the time
        if (PINB & (1 << PINB3))
        {
            rtc_set_time(h, m, s);
            lcd_clear();
            lcd_gotoxy(0, 0);
            lcd_puts("Time Saved");
            delay_ms(1000); // Display "Time Saved" message for 1 second
            while (PINB & (1 << PINB3)); // Wait for button release
            delay_ms(50); // Additional debounce delay
            break;
        }
    }

    // Clear the LCD after setting the time
    lcd_clear();
}

// Function to set the alarm time using buttons
void set_alarm(void)
{
    unsigned char temp_h = 0, temp_m = 0, temp_s = 0;

    lcd_clear();
    lcd_gotoxy(0, 0);
    lcd_puts("Set Alarm:");

    while (1)
    {
        // Check if button on PB0 is pressed to increment hour
        if (PINB & (1 << PINB0))
        {
            temp_h = (temp_h + 1) % 24;
            lcd_gotoxy(0, 1);
            sprintf(alarm_buffer, "Hour: %02d", temp_h);
            lcd_puts(alarm_buffer);
            delay_ms(50); // Debounce delay
            while (PINB & (1 << PINB0)); // Wait for button release
        }

        // Check if button on PB1 is pressed to increment minute
        if (PINB & (1 << PINB1))
        {
            temp_m = (temp_m + 1) % 60;
            lcd_gotoxy(0, 1);
            sprintf(alarm_buffer, "Minute: %02d", temp_m);
            lcd_puts(alarm_buffer);
            delay_ms(50); // Debounce delay
            while (PINB & (1 << PINB1)); // Wait for button release
        }

        // Check if button on PB2 is pressed to increment second
        if (PINB & (1 << PINB2))
        {
            temp_s = (temp_s + 1) % 60;
            lcd_gotoxy(0, 1);
            sprintf(alarm_buffer, "Second: %02d", temp_s);
            lcd_puts(alarm_buffer);
            delay_ms(50); // Debounce delay
            while (PINB & (1 << PINB2)); // Wait for button release
        }

        // Check if the save button on PB3 is pressed to save the alarm time
        if (PINB & (1 << PINB3))
        {
            int carry_m, carry_h; // Declare variables at the beginning of the block
            rtc_get_time(&h, &m, &s); // Get the current time

            // Calculate the alarm time
            alarm_s = (s + temp_s) % 60;
            carry_m = (s + temp_s) / 60;
            alarm_m = (m + temp_m + carry_m) % 60;
            carry_h = (m + temp_m + carry_m) / 60;
            alarm_h = (h + temp_h + carry_h) % 24;

            alarm_set = 1;
            lcd_clear();
            lcd_gotoxy(0, 0);
            lcd_puts("Alarm Set");
            delay_ms(1000); // Display "Alarm Set" message for 1 second
            get_time(); // Update the display with the current time and alarm time
            break;
        }
    }
}

// Function to check if the alarm time matches the current time
void check_alarm(void)
{
    unsigned char current_h, current_m, current_s;
    rtc_get_time(&current_h, &current_m, &current_s);

    if (alarm_set && current_h == alarm_h && current_m == alarm_m && current_s == alarm_s)
    {
        PORTB |= (1 << PORTB4); // Turn on the buzzer
        delay_ms(1000); // Sound the buzzer for 1 second
        PORTB &= ~(1 << PORTB4); // Turn off the buzzer
        alarm_set = 0; // Reset the alarm
    }
}