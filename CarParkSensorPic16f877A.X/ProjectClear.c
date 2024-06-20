#include <xc.h>                // include device-specific header
#include <pic16f877a.h>        // PIC16F877A specific definitions
#include <string.h>            // string library
#define _XTAL_FREQ 8000000    // define the oscillator frequency (20 MHz)

// CONFIG
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = ON         // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

#define RS PORTCbits.RC4        // Define RS pin for LCD
//#define RW PORTDbits.RD0        // Define RW pin for LCD
#define EN PORTCbits.RC5        // Define EN pin for LCD

// Function to send commands to the LCD
void lcd_cmd(unsigned char cmd) {
    PORTD = (cmd & 0xF0);       // Send higher nibble
    EN = 1;                     // Enable high
    //RW = 0;                     // Write operation
    RS = 0;                     // Command mode
    __delay_ms(10);             // Delay
    EN = 0;                     // Enable low
    PORTD = ((cmd << 4) & 0xF0);// Send lower nibble
    EN = 1;                     // Enable high
    //RW = 0;                     // Write operation
    RS = 0;                     // Command mode
    __delay_ms(10);             // Delay
    EN = 0;                     // Enable low
}

// Function to send data to the LCD
void lcd_data(unsigned char data) {
    PORTD = (data & 0xF0);      // Send higher nibble
    EN = 1;                     // Enable high
    //RW = 0;                     // Write operation
    RS = 1;                     // Data mode
    __delay_ms(10);             // Delay
    EN = 0;                     // Enable low
    PORTD = ((data << 4) & 0xF0);// Send lower nibble
    EN = 1;                     // Enable high
    //RW = 0;                     // Write operation
    RS = 1;                     // Data mode
    __delay_ms(10);             // Delay
    EN = 0;                     // Enable low
}

// Function to initialize the LCD
void lcd_init() {
    lcd_cmd(0x02);              // Initialize LCD in 4-bit mode
    lcd_cmd(0x28);              // 4-bit mode, 2 lines, 5x7 matrix
    lcd_cmd(0x0C);              // Display on, cursor off
    lcd_cmd(0x06);              // Increment cursor
    lcd_cmd(0x01);              // Clear display
    __delay_ms(20);             // Delay
}

// Function to send a string to the LCD
void lcd_string(const unsigned char *str, unsigned char num) {
    unsigned char i;
    for(i = 0; i < num; i++) {
        lcd_data(str[i]);       // Send each character to the LCD
    }
}

// Main function
void main() {
    int distance = 0;
    
    TRISC = 0x00;
    TRISD = 0x00;
    lcd_init();
    __delay_ms(10);
    
    int a = 0;
    TRISB = 0b00010000;         //RB4 & 5 as Input PIN (ECHO)

    T1CON = 0x10;
    
    
    lcd_cmd(0x85);  // Set cursor position
        __delay_ms(10);
        lcd_string("CAUTION", 7); // Display "L" for left sensor
        __delay_ms(10); 
    while(1) {                  // Infinite loop
        TMR1H = 0;                //Sets the Initial Value of Timer
        TMR1L = 0;                //Sets the Initial Value of Timer

        PORTBbits.RB5 = 1;                  //TRIGGER HIGH
        __delay_us(10);           //10uS Delay 
        PORTBbits.RB5 = 0;                  //TRIGGER LOW
                
        while(!PORTBbits.RB4);              //Waiting for Echo
        TMR1ON = 1;               //Timer Starts
        while(PORTBbits.RB4);               //Waiting for Echo goes LOW
        TMR1ON = 0;               //Timer Stops

        distance = ((TMR1L | (TMR1H<<8))/58)+1;
        
        if(distance < 40 && distance > 9){
            PORTCbits.RC3 = 1;
            __delay_ms(10);
            PORTCbits.RC3 = 0;
            __delay_ms(10);
            lcd_cmd(0xC2);
            __delay_ms(30);
            lcd_string("   WARNING  ", 12); // Display "L" for left sensor
            __delay_ms(100); }
        
        else if(distance <= 9 && distance > 2){
            PORTCbits.RC3 = 1;
            __delay_ms(1000);
            PORTCbits.RC3 = 0;
            __delay_ms(10);
            lcd_cmd(0xC2);
            __delay_ms(100);
            lcd_string("  !!HIT!!    ", 12); // Display "L" for left sensor
            __delay_ms(60); }
        else{
            PORTCbits.RC3 = 0; 
            lcd_cmd(0xC2);
            __delay_ms(30);
            lcd_string("Out of range", 12); // Display "L" for left sensor
            __delay_ms(100); 
        }
            
            
          
           
    }
    return;
}