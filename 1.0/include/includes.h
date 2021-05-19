#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define MENU_WRAP   0
#define MENU_NOWRAP 1


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


enum BoxCharacters{
    LTCorner = 218,
    LBCorner = 192,
    RTCorner = 191,
    RBCorner = 217,
    Horizontal = 196,
    Vertical = 179
};
