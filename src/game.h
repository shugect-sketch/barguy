#ifndef GAME_H
#define GAME_H

#include <psxapi.h>
#include "save.h"

// Pad button masks
#define PAD_CROSS   (1 << 14)
#define PAD_CIRCLE  (1 << 13)
#define PAD_SQUARE  (1 << 15)
#define PAD_TRIANGLE (1 << 12)
#define PAD_UP      (1 << 4)
#define PAD_DOWN    (1 << 6)
#define PAD_LEFT    (1 << 7)
#define PAD_RIGHT   (1 << 5)
#define PAD_START   (1 << 3)
#define PAD_SELECT  (1 << 0)

extern uint8_t pad_buf1[34];
extern uint8_t pad_buf2[34];
extern int difficulty_selected;

extern uint16_t pad_pressed;  // buttons pressed THIS frame only
void UpdatePad(void);

typedef enum {
    STATE_MENU,
    STATE_HOW_IT_WORKS,
    STATE_HIGH_SCORE,
    STATE_ACHIEVEMENTS,
    STATE_DIFFICULTY_SELECT,
    STATE_GAMEPLAY,
    STATE_NEXT_GIRL,
    STATE_VICTORY,
    STATE_GAME_OVER
} GameState;

void InitGameplay(SaveData *save);
int UpdateGameplay(SaveData *save);
int UpdateNextGirl(void);
int UpdateVictory(SaveData *save);
int UpdateGameOver(void);
int UpdateDifficultySelect(void);

#endif
