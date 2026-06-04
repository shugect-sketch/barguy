#include <psxgpu.h>
#include <psxetc.h>
#include <psxapi.h>
#include <psxpad.h>
#include "menu.h"
#include "game.h"

int UpdateMenu(void) {
    FntPrint(-1, "BARGUY\n\nPress X to Start\nPress O for How It Works\nPress [] for High Score\nPress /\\ for Achievements");
    FntFlush(-1);

    if (pad_pressed & PAD_CROSS)    return STATE_DIFFICULTY_SELECT;
    if (pad_pressed & PAD_CIRCLE)   return STATE_HOW_IT_WORKS;
    if (pad_pressed & PAD_SQUARE)   return STATE_HIGH_SCORE;
    if (pad_pressed & PAD_TRIANGLE) return STATE_ACHIEVEMENTS;

    return STATE_MENU;
}

int UpdateHowItWorks(void) {
    FntPrint(-1, "HOW IT WORKS\n\nDrink to build courage.\nThen seduce the girl.\nChoose your pick-up line wisely!\nMaybe she goes home with you...\nor you leave with a black eye.\n\nPress X to go back");
    FntFlush(-1);

    if (pad_pressed & PAD_CROSS) return STATE_MENU;
    return STATE_HOW_IT_WORKS;
}

int UpdateHighScore(SaveData *save) {
    FntPrint(-1, "HIGH SCORES\n\nEasy:    %d\nMedium:  %d\nHard:    %d\nExtreme: %d\nNightmare: %d\n\nPress X to go back",
        save->highScore[0],
        save->highScore[1],
        save->highScore[2],
        save->highScore[3],
        save->highScore[4]);
    FntFlush(-1);

    if (pad_pressed & PAD_CROSS) return STATE_MENU;
    return STATE_HIGH_SCORE;
}

int UpdateAchievements(SaveData *save) {
    FntPrint(-1, "ACHIEVEMENTS\n\nTotal Girls: %d\nTotal Slaps: %d\n\n(More coming soon)\n\nPress X to go back",
        save->totalGirls,
        save->totalSlaps);
    FntFlush(-1);

    if (pad_pressed & PAD_CROSS) return STATE_MENU;
    return STATE_ACHIEVEMENTS;
}

int UpdateDifficultySelect(void) {
    FntPrint(-1, "SELECT DIFFICULTY\n\nX  - Easy\nO  - Medium\n[] - Hard\n/\\ - Extreme\nSEL- Nightmare\n\nSTART - Back");
    FntFlush(-1);

    if (pad_pressed & PAD_CROSS)    { difficulty_selected = 0; InitGameplay(NULL); return STATE_GAMEPLAY; }
    if (pad_pressed & PAD_CIRCLE)   { difficulty_selected = 1; InitGameplay(NULL); return STATE_GAMEPLAY; }
    if (pad_pressed & PAD_SQUARE)   { difficulty_selected = 2; InitGameplay(NULL); return STATE_GAMEPLAY; }
    if (pad_pressed & PAD_TRIANGLE) { difficulty_selected = 3; InitGameplay(NULL); return STATE_GAMEPLAY; }
    if (pad_pressed & PAD_SELECT)   { difficulty_selected = 4; InitGameplay(NULL); return STATE_GAMEPLAY; }
    if (pad_pressed & PAD_START)    return STATE_MENU;

    return STATE_DIFFICULTY_SELECT;
}
