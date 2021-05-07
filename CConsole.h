#include <stdlib.h>
#include <Windows.h>

// ALL THE COLORS POSSIBLE ON THE CONSOLE
enum Colors{
    BLACK,
    BLUE,
    GREEN,
    AQUA,
    RED,
    PURPLE,
    YELLOW,
    WHITE,
    GRAY,
    LBLUE,
    LGREEN,
    LAQUA,
    LRED,
    LPURPLE,
    LYELLOW,
    BWHITE,
};

// ASCII CODES FOR BOX DRAWING CHARACTERS
enum Characters{
    FULL_PIXEL = 219,
    HALF_PIXEL = 178,
    TRANS_PIXEL = 176
};


// STRUCT CONTAINING ALL INFO NEEDED FOR THE CONSOLE
typedef struct Console
{
    size_t bufferSize;       // NUMBER OF TOTAL CHARACTERS
    size_t bufferWidth;      // SIZE OF ONE HORIZONTAL LINE
    size_t bufferHeight;     // SIZE OF ONE VERTICAL LINE
    CHAR_INFO *screenBuffer; // ARRAY OF CHARACTERS THAT WILL BE DRAWN TO THE SCREEN

    // PRIVATE
    HANDLE hConsole;         // POINTER TO THE CONSOLE OBJECT NEEDED BY WINDOWS
    SMALL_RECT dwBytesWritten;
    COORD dwBufferSize;

} Console;


void StartConsole(Console *console)
{
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

}


// FREES HEAP MEMORY CREATED FOR THE CONSOLE
void FreeConsoleMemory(Console *console)
{
    free(console->screenBuffer);
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


// CLEARS THE SCREEN
void Clear(Console *console)
{
    for(int i = 0; i < console->bufferSize; i++)
    {
        console->screenBuffer[i].Char.UnicodeChar = ' ';
        console->screenBuffer[i].Attributes = 7;
    }
}


// ADDS A STRING TO THE SCREEN BUFFER STARTING AT THE COORDS X, Y
void DrawString(Console *console, const char *string, int x, int y, WORD Color)
{
    int i = 0;
    wchar_t c = string[i];
    while(c != '\0')
    {
        DrawChar(console, c, x+i, y, Color);
        c = string[++i];
    }
}