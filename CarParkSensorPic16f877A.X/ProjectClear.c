#include <xc.h>               
#include <pic16f877a.h>        
           
#define _XTAL_FREQ 8000000    // define the oscillator frequency 8 MHz
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF       
#pragma config PWRTE = OFF      
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = ON         
#pragma config CPD = OFF        
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         


#define RS PORTCbits.RC4              
#define EN PORTCbits.RC5        


void lcd_cmd(unsigned char cmd) {
    PORTD = (cmd & 0xF0);       
    EN = 1;                     
    RS = 0;                     
    __delay_ms(10);             
    EN = 0;                     
    PORTD = ((cmd << 4) & 0xF0);
    EN = 1;                  
    //RW = 0;                     // Write operation
    RS = 0;                     
    __delay_ms(10);             
    EN = 0;                     // Enable low
}

// Function to send data to the LCD
void lcd_data(unsigned char data) {
    PORTD = (data & 0xF0);      // Send higher nibble
    EN = 1;                     
    RS = 1;                     
    __delay_ms(10);             // Delay
    EN = 0;                     
    PORTD = ((data << 4) & 0xF0);
    EN = 1;                     
    RS = 1;                     
    __delay_ms(10);             
    EN = 0;                     
}

// Function to initialize the LCD
void lcd_init() {
    
    lcd_cmd(0x02);             
    lcd_cmd(0x28);              
    lcd_cmd(0x0C);              // Display on, cursor off
    lcd_cmd(0x06);              // Increment cursor
    lcd_cmd(0x01);           
    __delay_ms(20);            
    
}

// Function to send a string to the LCD
void Lcd_string(const unsigned char *str, unsigned char num) {
    unsigned char i;
    for(i = 0; i < num; i++) {
        lcd_data(str[i]);      
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
    TRISB = 0b00010000;        

    T1CON = 0x10;
    
    
    lcd_cmd(0x85);  
        __delay_ms(10);
        Lcd_string("CAUTION", 7);
        __delay_ms(10); 
    while(1) {               
        TMR1H = 0;                
        TMR1L = 0;                

        PORTBbits.RB5 = 1;                 
        __delay_us(10);          
        PORTBbits.RB5 = 0;                 
                
        while(!PORTBbits.RB4);              
        TMR1ON = 1;              
        while(PORTBbits.RB4);               
        TMR1ON = 0;               

        distance = ((TMR1L | (TMR1H<<8))/58)+1;
        
        if(distance < 40 && distance > 9){
            PORTCbits.RC3 = 1;
            __delay_ms(10);
            PORTCbits.RC3 = 0;
            __delay_ms(10);
            lcd_cmd(0xC2);
            __delay_ms(30);
            Lcd_string("   WARNING  ", 12);
            __delay_ms(100); }
        
        else if(distance <= 9 && distance > 2){
            PORTCbits.RC3 = 1;
            __delay_ms(1000);
            PORTCbits.RC3 = 0;
            __delay_ms(10);
            lcd_cmd(0xC2);
            __delay_ms(100);
            Lcd_string("  !!HIT!!    ", 12); 
            __delay_ms(60); }
        else{
            PORTCbits.RC3 = 0; 
            lcd_cmd(0xC2);
            __delay_ms(30);
            Lcd_string("Out of range", 12);
            __delay_ms(100); 
        }
            
            
          
           
    }
    return;
}
