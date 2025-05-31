#include <avr/io.h>
#include <stddef.h>   // Para NULL
#include <stdio.h>    // Para sprintf

// Definición de la frecuencia del sistema (16 MHz)
#define FRECUENCIA 16000000UL

// Definiciones de secuencias ANSI para coloress
#define GREEN 32
#define YELLOW 33
#define BLUE 34

// =========================
// Prototipos de funciones
// =========================

// Initialization
void UART_Ini(uint8_t com, uint32_t baudrate, uint8_t size, uint8_t parity, uint8_t stop);

// Send
void UART_puts(uint8_t com, char *str);
void UART_putchar(uint8_t com, char data);

// Received
uint8_t UART_available(uint8_t com);
char UART_getchar(uint8_t com);
void UART_gets(uint8_t com, char *str);

// Escape sequences
void UART_clrscr(uint8_t com);
void UART_setColor(uint8_t com, uint8_t color);
void UART_gotoxy(uint8_t com, uint8_t x, uint8_t y);

// Utils
void itoa(uint16_t number, char* str, uint8_t base);
uint16_t atoi(char *str);

// =========================
// Implementación de funciones UART
// =========================

// Función para enviar un carácter por UART
void UART_putchar(uint8_t com, char data)
{
    switch (com) {
        case 0:
            while (!(UCSR0A & (1 << UDRE0))); // Esperar a que el buffer esté listo
            UDR0 = data; // Escribir el dato
            break;
        case 1:
            while (!(UCSR1A & (1 << UDRE1)));
            UDR1 = data;
            break;
        case 2:
            while (!(UCSR2A & (1 << UDRE2)));
            UDR2 = data;
            break;
        case 3:
            while (!(UCSR3A & (1 << UDRE3)));
            UDR3 = data;
            break;
        default:
            return; // UART no válido
    }
}

// Función para verificar si hay datos disponibles en UART
uint8_t UART_available(uint8_t com)
{
    switch (com) {
        case 0: return UCSR0A & (1 << RXC0);
        case 1: return UCSR1A & (1 << RXC1);
        case 2: return UCSR2A & (1 << RXC2);
        case 3: return UCSR3A & (1 << RXC3);
        default: return 0;
    }
}

// Función para recibir un carácter desde UART
char UART_getchar(uint8_t com)
{
    switch (com) {
        case 0:
            while (!(UCSR0A & (1 << RXC0)));
            return UDR0;
        case 1:
            while (!(UCSR1A & (1 << RXC1)));
            return UDR1;
        case 2:
            while (!(UCSR2A & (1 << RXC2)));
            return UDR2;
        case 3:
            while (!(UCSR3A & (1 << RXC3)));
            return UDR3;
        default:
            return 0;
    }
}

// Función para recibir una cadena de caracteres desde UART
void UART_gets(uint8_t com, char *str)
{
    if (com > 3 || str == NULL) return;
    uint8_t i = 0;
    while (1)
    {
        char caracter = UART_getchar(com);
        if (caracter == 0x08 || caracter == 127) { // Backspace o Delete
            if (i > 0) {
                i--;
                UART_puts(com, "\b \b");
            }
        } else if (caracter == '\n' || caracter == '\r') { // Enter
            break;
        } else if (i < 19) {
            UART_putchar(com, caracter);
            str[i++] = caracter;
        }
    }
    str[i] = '\0';
}

// Función para enviar una cadena de caracteres por UART
void UART_puts(uint8_t com, char *str)
{
    if (com > 3 || str == NULL) return;
    while (*str != '\0') {
        UART_putchar(com, *str);
        str++;
    }
}

// Función para inicializar un UART específico
void UART_Ini(uint8_t com, uint32_t baudrate, uint8_t size, uint8_t parity, uint8_t stop)
{
    uint16_t ubrr_valor = (FRECUENCIA / (8UL * baudrate)) - 1;
    switch (com) {
        case 0: // UART0
            UCSR0A |= (1 << U2X0);
            UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
            UCSR0C = 0;
            UBRR0 = ubrr_valor;
            if (size == 6) UCSR0C |= (1 << UCSZ00);
            else if (size == 7) UCSR0C |= (1 << UCSZ01);
            else if (size == 8) UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);
            if (parity == 1) UCSR0C |= (1 << UPM01) | (1 << UPM00);
            else if (parity == 2) UCSR0C |= (1 << UPM01);
            if (stop == 2) UCSR0C |= (1 << USBS0);
            break;

        case 1: // UART1
            UCSR1A |= (1 << U2X0);
            UCSR1B = (1 << RXEN1) | (1 << TXEN1) | (1 << RXCIE1);
            UCSR1C = 0;
            UBRR1 = ubrr_valor;
            if (size == 6) UCSR1C |= (1 << UCSZ10);
            else if (size == 7) UCSR1C |= (1 << UCSZ11);
            else if (size == 8) UCSR1C |= (1 << UCSZ11) | (1 << UCSZ10);
            if (parity == 1) UCSR1C |= (1 << UPM11) | (1 << UPM10);
            else if (parity == 2) UCSR1C |= (1 << UPM11);
            if (stop == 2) UCSR1C |= (1 << USBS1);
            break;

        case 2: // UART2
            UCSR2A |= (1 << U2X2);
            UCSR2B = (1 << RXEN2) | (1 << TXEN2) | (1 << RXCIE2);
            UCSR2C = 0;
            UBRR2 = ubrr_valor;
            if (size == 6) UCSR2C |= (1 << UCSZ20);
            else if (size == 7) UCSR2C |= (1 << UCSZ21);
            else if (size == 8) UCSR2C |= (1 << UCSZ21) | (1 << UCSZ20);
            if (parity == 1) UCSR2C |= (1 << UPM21) | (1 << UPM20);
            else if (parity == 2) UCSR2C |= (1 << UPM21);
            if (stop == 2) UCSR2C |= (1 << USBS2);
            break;

        case 3: // UART3
            UCSR3A |= (1 << U2X3);
            UCSR3B = (1 << RXEN3) | (1 << TXEN3) | (1 << RXCIE3);
            UCSR3C = 0;
            UBRR3 = ubrr_valor;
            if (size == 6) UCSR3C |= (1 << UCSZ30);
            else if (size == 7) UCSR3C |= (1 << UCSZ31);
            else if (size == 8) UCSR3C |= (1 << UCSZ31) | (1 << UCSZ30);
            if (parity == 1) UCSR3C |= (1 << UPM31) | (1 << UPM30);
            else if (parity == 2) UCSR3C |= (1 << UPM31);
            if (stop == 2) UCSR3C |= (1 << USBS3);
            break;

        default:
            return; // UART no válido
    }
}


void itoa(uint16_t number, char* str, uint8_t base)
{
    if (str == NULL || (base != 2 && base != 8 && base != 10 && base != 16)) return;

    uint8_t i = 0;
    uint16_t residuo;

    if (number == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }

    while (number > 0) 
    {
        residuo = number % base;
        str[i++] = (residuo < 10) ? (residuo + '0') : (residuo - 10 + 'A');
        number /= base;
    }
    str[i] = '\0';

    for (uint8_t j = 0; j < i / 2; j++) {
        char temp = str[j];
        str[j] = str[i - j - 1];
        str[i - j - 1] = temp;
    }
}

uint16_t atoi(char *str)
{
    if (str == NULL) return 0;
    uint16_t result = 0;
    uint8_t i = 0;

    while (str[i] != '\0')
    {
        if (str[i] >= '0' && str[i] <= '9')
        {
            result = result * 10 + (str[i] - '0');
        }
        else if (str[i] == '.') 
        {
            break;
        }
        i++;
    }
    return result;
}




void UART_clrscr(uint8_t com) {
    UART_puts(com, "\033[2J");
    UART_puts(com, "\033[H");
}

void UART_gotoxy(uint8_t com, uint8_t x, uint8_t y) {
    char buffer[16];
    sprintf(buffer, "\033[%d;%dH", y, x);
    UART_puts(com, buffer);
}

void UART_setColor(uint8_t com, uint8_t color) {
    char buffer[10];
    sprintf(buffer, "\033[%dm", color);
    UART_puts(com, buffer);
}



// ============================================================
// MAIN
// ============================================================
int main( void )
{
    char cad[20];
    char cadUart3[20];
    uint16_t num;

    UART_Ini(0,12345,8,1,2);
    UART_Ini(2,115200,8,0,1);
    UART_Ini(3,115200,8,0,1);
    while(1) 
    {
        UART_getchar(0);
        UART_clrscr(0);

        UART_gotoxy(0,2,2);
        UART_setColor(0,YELLOW);
        UART_puts(0,"Introduce un número:");
        
        UART_gotoxy(0,22,2);
        UART_setColor(0,GREEN);
        UART_gets(0,cad);
// -------------------------------------------
        // Cycle through UART2->UART3
        UART_puts(2,cad);
        UART_puts(2,"\r");
        UART_gets(3,cadUart3);
        UART_gotoxy(0,5,3);
        UART_puts(0,cadUart3);
// -------------------------------------------
        num = atoi(cad);
        itoa(num,cad,16);
        
        UART_gotoxy(0,5,4);
        UART_setColor(0,BLUE);
        UART_puts(0,"Hex: ");
        UART_puts(0,cad);
        itoa(num,cad,2);
        
        UART_gotoxy(0,5,5);
        UART_puts(0,"Bin: ");
        UART_puts(0,cad);
    }
}
