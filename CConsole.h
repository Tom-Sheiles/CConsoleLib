#include <stdlib.h>
#include <Windows.h>
#include <WinUser.h>

#include "includes.h"


// STRUCT CONTAINING ALL INFO NEEDED FOR THE CONSOLE
typedef struct Console
{
    size_t bufferSize;       // NUMBER OF TOTAL CHARACTERS
    size_t bufferWidth;      // SIZE OF ONE HORIZONTAL LINE
    size_t bufferHeight;     // SIZE OF ONE VERTICAL LINE
    CHAR_INFO *screenBuffer; // ARRAY OF CHARACTERS THAT WILL BE DRAWN TO THE SCREEN
    WORD backgroundColor;

    int running;             // IS THE CONSOLE CURRENTLY RUNNING

    int keys[0xFF];

    // PRIVATE
    HANDLE hConsole;         // POINTER TO THE CONSOLE OBJECT NEEDED BY WINDOWS
    SMALL_RECT dwBytesWritten;
    COORD dwBufferSize;
    int previousKeys[0xFF];

} Console;


typedef struct Window
{
    int x, y, w, h;
    WORD color;
    const char *title;
    Console *console;

} Window;


typedef struct Menu
{
    int x, y;
    WORD color, bgColor;

    int numberOfItems;
    const char **items;
    Console *console;
    int selected;
    int wrapMode;

} Menu;


void ConsoleCreateMenu(Menu *menu, const char **items, Console *console, int wrapMode)
{
    menu->items = (const char **)malloc(sizeof(const char *) * menu->numberOfItems);
    menu->items = items;
    menu->console = console;
    menu->wrapMode = wrapMode;
    menu->selected = 0;
}


// FORWARD DECLARATIONS
void DrawString(Console *console, const char *string, int x, int y, WORD Color);
void Clear(Console *console);


void CreateConsole(Console *console)
{
    // ***TODO MAKE AN IN PLACE CONSOLE APP BY GETTING THE CURRENT SCREEN BUFFER ALONG WITH THE SIZE AND PRITING THAT *** //

    // CREATE A NEW CONSOLE WE CAN WRITE TO
    console->hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(console->hConsole);

    // GET THE SIZE OF THE CONSOLE
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(console->hConsole, &csbi);
    console->bufferSize = csbi.dwSize.X * csbi.dwSize.Y;
    console->bufferWidth = csbi.dwSize.X;
    console->bufferHeight = csbi.dwSize.Y;

    SMALL_RECT dwBytesWritten = {0, 0, csbi.dwSize.X, csbi.dwSize.Y};
    console->dwBytesWritten = dwBytesWritten;

    // ALLOCATE MEMORY FOR THE ARRAY OF CHARACTERS ON THE CONSOLE
    console->screenBuffer = (CHAR_INFO *)malloc(sizeof(CHAR_INFO) * console->bufferSize);
    console->backgroundColor = 7;

    // MAKE SURE SCREEN IS CLEAR
    for(int i = 0; i < console->bufferSize; i++)
    {
        console->screenBuffer[i].Char.UnicodeChar = ' '; 
        console->screenBuffer[i].Attributes = 7;
    }


    COORD dwBufferSize = {csbi.dwSize.X, csbi.dwSize.Y};
    console->dwBufferSize = dwBufferSize;
    COORD dwBufferStart = {0, 0};
    WriteConsoleOutput(console->hConsole, console->screenBuffer, console->dwBufferSize, dwBufferStart, &console->dwBytesWritten);

    console->running = 1;
}


// FREES HEAP MEMORY CREATED FOR THE CONSOLE
void FreeConsoleMemory(Console *console)
{
    free(console->screenBuffer);
    FreeConsole();
}


void UpdateKeyState(Console *console)
{
    for(int i = 0; i < 0xFF; i++)
    {
        int previous = console->previousKeys[i];
        console->previousKeys[i] = (GetAsyncKeyState(i) & 0x8000);
        console->keys[i] = console->previousKeys[i] && !previous;
    }
}


// DRAWS THE CURRENT SCREEN BUFFER
void Draw(Console *console)
{
    COORD dwBufferStart = {0, 0};
    WriteConsoleOutput(console->hConsole, console->screenBuffer, console->dwBufferSize, dwBufferStart, &console->dwBytesWritten);
}


// SETS A SINGLE CHARACTER OF THE SCREEN BUFFER AT THE COORDS X, Y
void DrawChar(Console *console, wchar_t character, int x, int y, WORD Color)
{
    if(x < console->bufferWidth && y < console->bufferHeight && x >= 0 && y >= 0){
        console->screenBuffer[x + console->bufferWidth * y].Char.UnicodeChar = character;
        console->screenBuffer[x + console->bufferWidth * y].Attributes = Color;
    }
    Draw(console);
}


// SETS A CHAR IN THE SCREEN BUFFER WITHOUT REDRAWING THE CONSOLE. FASTER THAN DRAW CHAR
void PlaceChar(Console *console, wchar_t character, int x, int y, WORD Color)
{
    if(x < console->bufferWidth && y < console->bufferHeight && x >= 0 && y >= 0){
        console->screenBuffer[x + console->bufferWidth * y].Char.UnicodeChar = character;
        console->screenBuffer[x + console->bufferWidth * y].Attributes = Color;
    }
}


void ConsoleFillRect(Console *console, wchar_t character, int x, int y, int w, int h, WORD color)
{
    for(int i = x; i < x+w; i++)
    {
        for(int j = y; j < y+h; j++)
        {
            PlaceChar(console, character, i, j, color);
        }
    }
}


void DrawWindow(Window window)
{
    for(int i = window.x; i <= window.x + window.w; i++)
    {
        for(int j = window.y; j <= window.y + window.h; j++)
        {
            if(j == window.y || j == window.y + window.h) PlaceChar(window.console, Horizontal, i, j, window.color);
            if(i == window.x || i == window.x + window.w) PlaceChar(window.console, Vertical, i, j, window.color);
        }
    }
    PlaceChar(window.console, LTCorner, window.x, window.y, window.color);
    PlaceChar(window.console, RTCorner, window.x + window.w, window.y, window.color);
    PlaceChar(window.console, LBCorner, window.x, window.y + window.h, window.color);
    PlaceChar(window.console, RBCorner, window.x + window.w , window.y + window.h, window.color);

    if(window.title != NULL)
    {
        DrawString(window.console, window.title, window.x + 1, window.y, window.color);
    }
}


void DrawMenu(Menu *menu)
{   
    if(menu->wrapMode == MENU_WRAP){
        menu->selected = menu->selected % menu->numberOfItems;
        if(menu->selected < 0) menu->selected = menu->numberOfItems-1;
    }
    else{
        if(menu->selected < 0) menu->selected = 0;
        if(menu->selected >= menu->numberOfItems) menu->selected = menu->numberOfItems-1;
    }


    int i = 0;
    for(int y = menu->y; y < menu->y + menu->numberOfItems; y++)
    {
        if(i == menu->selected)
        {
            DrawString(menu->console, menu->items[i], menu->x, y, menu->color | menu->bgColor);
        }else
            DrawString(menu->console, menu->items[i], menu->x, y, menu->color);
        i++;
    }
}


void PlaceCharWindow(Window *window, wchar_t character, int x, int y, WORD color)
{
    if(x < window->w-1 && y < window->h-1 && x >= 0 && y >= 0)
        PlaceChar(window->console, character, (window->x+1)+x, (window->y+1)+y, color);
}


void WindowFillRect(Window *window, wchar_t character, int x, int y, int w, int h, WORD color)
{
    for(int i = x; i < x+w; i++)
    {
        for(int j = y; j < y+h; j++)
        {
            if(i < window->w-1 && j < window->h-1 && i >= 0 && j >= 0)
                PlaceChar(window->console, character, (window->x+1)+i, (window->y+1)+j, color);
        }
    }
}


// CLEARS THE SCREEN
void Clear(Console *console)
{
    for(int i = 0; i < console->bufferSize; i++)
    {
        console->screenBuffer[i].Char.UnicodeChar = ' ';
        console->screenBuffer[i].Attributes = console->backgroundColor;
    }
}


// ADDS A STRING TO THE SCREEN BUFFER STARTING AT THE COORDS X, Y
void DrawString(Console *console, const char *string, int x, int y, WORD Color)
{
    int i = 0;
    wchar_t c = string[i];
    while(c != '\0')
    {
        PlaceChar(console, c, x+i, y, Color);
        c = string[++i];
    }
}