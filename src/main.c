#include <psxgpu.h>
#include <psxetc.h>
#include <psxapi.h>
#include <psxpad.h>

#include "game.h"
#include "menu.h"
#include "save.h"
#include "achievements.h"

uint8_t pad_buf1[34];
uint8_t pad_buf2[34];
uint16_t pad_pressed = 0;
static uint16_t pad_prev = 0;

#define SCREEN_W 320
#define SCREEN_H 240

typedef struct {
    DISPENV disp;
    DRAWENV draw;
} RenderBuffer;

static RenderBuffer buffers[2];
static int activeBuffer = 0;

void UpdatePad(void) {
    uint16_t current = ~((uint16_t)pad_buf1[3] | ((uint16_t)pad_buf1[2] << 8));
    pad_pressed = current & ~pad_prev;
    pad_prev = current;
}

void InitDisplay(void) {
    ResetGraph(0);

    SetDefDispEnv(&buffers[0].disp, 0,   0, SCREEN_W, SCREEN_H);
    SetDefDrawEnv(&buffers[0].draw, 0, 240, SCREEN_W, SCREEN_H);
    SetDefDispEnv(&buffers[1].disp, 0, 240, SCREEN_W, SCREEN_H);
    SetDefDrawEnv(&buffers[1].draw, 0,   0, SCREEN_W, SCREEN_H);

    setRGB0(&buffers[0].draw, 0, 0, 0);
    setRGB0(&buffers[1].draw, 0, 0, 0);
    buffers[0].draw.isbg = 1;
    buffers[1].draw.isbg = 1;

    PutDispEnv(&buffers[0].disp);
    PutDrawEnv(&buffers[0].draw);

    FntLoad(960, 0);
    FntOpen(0, 8, 320, 224, 0, 512);

    SetDispMask(1);
}

void FlipBuffers(void) {
    DrawSync(0);
    VSync(0);
    activeBuffer ^= 1;
    PutDispEnv(&buffers[activeBuffer].disp);
    PutDrawEnv(&buffers[activeBuffer].draw);
}

int main(void) {
    int i;

    InitDisplay();
    InitPAD(pad_buf1, 34, pad_buf2, 34);
    StartPAD();
    ChangeClearPAD(0);

    // Wait for pad to stabilize
    for(i = 0; i < 60; i++) {
        VSync(0);
    }
    pad_prev = ~((uint16_t)pad_buf1[3] | ((uint16_t)pad_buf1[2] << 8));

    SaveData save;
    LoadSave(&save);

    int currentState = STATE_MENU;

    while (1) {
        UpdatePad();

        switch (currentState) {
            case STATE_MENU:
                currentState = UpdateMenu();
                break;
            case STATE_HOW_IT_WORKS:
                currentState = UpdateHowItWorks();
                break;
            case STATE_HIGH_SCORE:
                currentState = UpdateHighScore(&save);
                break;
            case STATE_ACHIEVEMENTS:
                currentState = UpdateAchievements(&save);
                break;
            case STATE_DIFFICULTY_SELECT:
                currentState = UpdateDifficultySelect();
                break;
            case STATE_GAMEPLAY:
                currentState = UpdateGameplay(&save);
                break;
            case STATE_NEXT_GIRL:
                currentState = UpdateNextGirl();
                break;
            case STATE_VICTORY:
                currentState = UpdateVictory(&save);
                break;
            case STATE_GAME_OVER:
                currentState = UpdateGameOver();
                break;
        }

        FlipBuffers();
    }

    return 0;
}
