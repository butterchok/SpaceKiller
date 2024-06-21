#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//#include <time.h>

#define SCREEN_WIDTH (448)
#define SCREEN_HEIGHT (576)
#define ZEROY (500)

#define WINDOW_TITLE "SpaceKiller"

#define LIVES 4
#define SPEED 5

//typedef struct menu {
//    MENU_START;
//    MENU_OPTIONS;
//    MENU_EXIT;
//    char menu_count;
//}menu;
//
//const char* menuText[_menu.MENU_COUNT] = {
//    "Start Game",
//    "Options",
//    "Exit"
//};

typedef struct player {
    int lives;
    int posx;
    int score;
    int bullet;
    int stage;
    //bool spawn;
}player;

typedef struct bullet {
    int posx;
    int posy;
    bool active;
}bullet;

typedef struct enemy_purp_270 {
    int posx;
    int posy;
    bool active;
}enemy_purp_270;

typedef struct crush {
    int posx;
    int posy;
}crush;

static bullet _bullet[2];
static enemy_purp_270 _enemy_purp_270[2];

int id_bullet = 0;

int main(void)
{
    InitAudioDevice();
    Sound ShootSound = LoadSound(ASSETS_PATH"shootsound.mp3");
    Sound Music = LoadSound(ASSETS_PATH"Rainbow_Road.mp3");
    PlaySound(Music);
    player _player = { LIVES, SCREEN_WIDTH / 2, 0, 0, 1 /*, true*/ };

    for (int i = 0; i < 2; i++) {
        _bullet[i].posx = _player.posx;
        _bullet[i].posy = ZEROY; //лайк подписка комментарий
        _bullet[i].active = false;
    }

    for (int i = 0; i < 2; i++) {
        _enemy_purp_270[i].posx = rand()%SCREEN_WIDTH;
        _enemy_purp_270[i].posy = rand() % 50;
        _enemy_purp_270[i].active = true;
    }

    crush _crush = { -10, -10 };

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
    SetTargetFPS(60);

    Texture2D playerspr = LoadTexture(ASSETS_PATH"player.png"); // Check README.md for how this works
    Texture2D bulletspr = LoadTexture(ASSETS_PATH"bullet.png");
    Texture2D enemy_purp_270spr = LoadTexture(ASSETS_PATH"enemy_purp_270.png");
    Texture2D explosionsheet = LoadTexture(ASSETS_PATH"explosionsheet.png");
    Rectangle explosion[] = {
        {0, 0, 32, 32},
        {0, 32, 32, 32},
        {0, 64, 32, 32},
        {0, 96, 32, 32},
        {0, 128, 32, 32}
    };

    int timer = 0;
    int timer1 = 0;
    int speedanimation = 5;
    int animationframe = 0;
    int frame = 0;

    // фокусы со слипом
    //struct timespec req;
    //req.tv_sec = 0; // Секунды
    //req.tv_nsec = 500000000L;

    while (!WindowShouldClose())
    {
        switch(frame){
            case 0: 
                BeginDrawing();
                ClearBackground(BLACK);
                DrawText("Press R to start", SCREEN_WIDTH / 3 - 48, SCREEN_HEIGHT / 2, 32, GRAY);
                DrawText("Press F return to menu", SCREEN_WIDTH / 3 - 96, SCREEN_HEIGHT / 2 + 32, 32, GRAY);
                DrawText("Press E to exit", SCREEN_WIDTH / 3 - 48, SCREEN_HEIGHT / 2 + 64, 32, GRAY);
                EndDrawing();
                if (IsKeyPressed('R')) frame = 1;
                if (IsKeyPressed('E')) exit(0);
                break;
            case 1: 
                if (IsKeyPressed('F')) {
                    frame = 0;
                    _player.score = 0;
                } 

        if (!IsKeyUp('A')) {
            _player.posx -= SPEED;
        }

        if (!IsKeyUp('D')) {
            _player.posx += SPEED;
        }

        if (IsKeyPressed(KEY_SPACE) && (_player.bullet <= 2)) {
            _player.bullet += 1;
            PlaySound(ShootSound);

            _bullet[id_bullet % 2].posx = _player.posx;
            _bullet[id_bullet % 2].posy = ZEROY;
            _bullet[id_bullet % 2].active = true;
            id_bullet++;
        }

        BeginDrawing();

        ClearBackground(BLACK);

        DrawTexture(playerspr, _player.posx, ZEROY, WHITE);
        DrawText(TextFormat("%04i", _player.score), SCREEN_WIDTH/2 - 80, SCREEN_HEIGHT/2 - 40, 80, WHITE);
        //DrawTextureRec(explosionsheet, explosion[4], (Vector2) {0, 0}, GRAY);
        
        for (int i = 0; i < 2; i++) {
            if (_bullet[i].active == true) {
                DrawTexture(bulletspr, _bullet[i].posx, _bullet[i].posy, WHITE);
                _bullet[i].posy -= 8;
                if (_bullet[i].posy < 0 || _bullet[i].posx >= SCREEN_WIDTH) {
                    _bullet[i].active = false;
                    _player.bullet -= 1;
                }
            }
        }
        
        for (int i = 0; /*_player.score < 1000 &&*/ i < 2; i++) {            
            if (_enemy_purp_270[i].active == true) {
                DrawTexture(enemy_purp_270spr, _enemy_purp_270[i].posx, _enemy_purp_270[i].posy, WHITE);
                _enemy_purp_270[i].posy += 3;
                if (_enemy_purp_270[i].posy >= SCREEN_HEIGHT) {
                    _enemy_purp_270[i].posx = rand()%SCREEN_WIDTH;
                    _enemy_purp_270[i].posy = rand()%50;
                }
                if (_enemy_purp_270[i].posy > SCREEN_HEIGHT - 10) {
                    _player.score -= 50;
                }
            }
        }

        for (int i = 0; i < 2; i++){
            if (_bullet[i].active == true) {
                for (int j = 0; j < 2; j++) {
                    if (abs(_bullet[i].posx - _enemy_purp_270[j].posx) <= 30
                        && abs(_bullet[i].posy - _enemy_purp_270[j].posy) <= 5) {
                        _enemy_purp_270[j].posx = rand()%SCREEN_WIDTH;
                        _enemy_purp_270[j].posy = -rand()%50;
                        _bullet[i].active = false;
                        _player.bullet -= 1;
                        _player.score += 100;
                        _crush.posx = _bullet[i].posx;
                        _crush.posy = _bullet[i].posy;
                        animationframe = 0;
                        //_bullet[i].posy = -10; //ПРОСТИ МАЛЫШКА...Я ПОДНИМАЮ БАКСЫ...
                    }
                }
            }
        }

        /*for (int i = 0; i < 2; i++) {
            if (_enemy_purp_270[i].posy < SCREEN_HEIGHT) {
                _enemy_purp_270[i].active == false;
                _enemy_purp_270[i].posy += 3;
            }
            if (_enemy_purp_270[i].posy >= SCREEN_HEIGHT) {
                _enemy_purp_270[i].posy = 40;
                _enemy_purp_270[i].active == true;
                _enemy_purp_270[i].posy += 3;
            }
        }*/

        if (timer >= speedanimation && animationframe < 5) {
            animationframe += 1;
            //animationframe = animationframe; бесценная строка
            timer = 0;
        }

        DrawTextureRec(explosionsheet, explosion[animationframe], (Vector2) {_crush.posx, _crush.posy}, GRAY);

        timer += 1;
        //printf("%d ", animationframe);
        

        EndDrawing();
        break;
        }
    }

    CloseWindow();

    return 0;
}