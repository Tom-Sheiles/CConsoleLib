#include "CConsole.h"


int main()
{
    Console console;
    StartConsole(&console);

    DrawString(&console, "Hello, World!", console.bufferWidth/2-7, console.bufferHeight/2-1, LGREEN);
    DrawChar(&console, 'A', console.bufferWidth/2-1, console.bufferHeight/2, LRED);

    while(1);

    FreeConsoleMemory(&console);
}