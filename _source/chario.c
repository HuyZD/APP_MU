#include "chario.h"
#include "uart.h"
#include "config.h"

/* gotachar()
 * check if there is a character available to read. returns 1 if there
 * is a character available, 0 if not, and negative error number on
 * failure */
int gotachar(void)
{
    return 0;
}



void rputchar(char c)
{
	uartWrite(UART_CLI, c);
}

/* putchar():
 *  Output a character to the stdout RS232 port.  If char is '\n'
 *  then precede it with '\r';
 */
int putchar(int c)
{
    if (c == '\n')
        rputchar('\r');
    rputchar((char)c);
    return((int)c);
}

void putstr(char *string)
{
	while(*string) {
		putchar(*string);
		string++;
	}
}

int puts(const char *s)
{
    while(*s != 0)
		putchar(*s++);
	return (1);
}


