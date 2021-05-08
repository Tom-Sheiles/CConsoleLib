#include "CConsole.h"

#define numberOfAliens 45
#define numberOfBullets 9999

struct Entity{
    int x, y;
    char character;
    int color;
};

typedef struct Entity Entity;


int alienDirection = 1;
void AlienThink(Entity *alien, int width )
{
    if((alien->x + 2 == width+1 && alienDirection == 1) || (alien->x - 1 == -1 && alienDirection == -1) && alien->character != ' ')
    {
        alienDirection = -(alienDirection);
    }
    alien->x += alienDirection;
}


void BulletThink(Entity *bullet, Entity *aliens)
{
    bullet->y--;

    for(int i = 0; i < numberOfAliens; i++)
    {
        if(bullet->y == aliens[i].y && bullet->x == aliens[i].x)
            aliens[i].character = ' ';
    }
}



int main()
{
    Console console;
    StartConsole(&console);

    Entity aliens[numberOfAliens];
    Entity bullets[numberOfBullets];

    int currentBullet = 0;

    Entity player = {console.bufferWidth/2 ,console.bufferHeight - 2, FULL_PIXEL, LRED};

    int x = 20;
    for(int i = 0; i < numberOfAliens; i++){
        Entity alien = {x, 1, 146, LGREEN};
        aliens[i] = alien;
        x++;
    }

    int ticks = 0;

    while(1)
    {
        if(ticks % 40 == 0){
            if(GetAsyncKeyState('D')) player.x++;
            if(GetAsyncKeyState('A')) player.x--;
        }

        if(ticks % 500 == 0)
        {
            if(GetAsyncKeyState(' '))
            {
                Entity bullet = {player.x, player.y-1, 186, YELLOW};
                bullets[currentBullet] = bullet;
                currentBullet++;
            }
        }

        if(ticks % 200 == 0)
        {
           for(int i = 0; i < numberOfAliens; i++)
           {
               AlienThink(&aliens[i], console.bufferWidth);
           }
        }

        if(ticks % 50 == 0){
            
            for(int i = 0; i < currentBullet; i++)
            {
                BulletThink(&bullets[i], aliens);    
            }
        }
        
        Clear(&console);
        PlaceChar(&console, player.character, player.x, player.y, player.color);

        for(int i = 0; i < numberOfAliens; i++)
        {
            PlaceChar(&console, aliens[i].character, aliens[i].x, aliens[i].y, aliens[i].color);
        }

        for(int i = 0; i < currentBullet; i++)
        {
            PlaceChar(&console, bullets[i].character, bullets[i].x, bullets[i].y, bullets[i].color);    
        }

        Draw(&console);

        ticks++;
    }

    FreeConsoleMemory(&console);
}