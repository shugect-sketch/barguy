#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <psxgpu.h>
#include "save.h"

// TIM texture IDs
typedef enum {
    TEX_BAR_BG = 0,
    TEX_PLAYER,
    TEX_GIRL_EASY,      // Easy & Medium
    TEX_GIRL_HARD,      // Hard
    TEX_GIRL_EXTREME,   // Extreme
    TEX_GIRL_NIGHTMARE, // Nightmare
    TEX_COUNT
} TextureID;

// Girl sprite with position and animation state
typedef struct {
    int x, y;
    int width, height;
    int mouth_open;     // 0 = closed, 1 = open
    int mouth_timer;    // for animation
} GirlSprite;

// Initialize graphics system and load all TIM files
void GraphicsInit(void);

// Draw the bar scene
void DrawBarScene(int difficulty, int lives, int stars, int courage, int girls_count);

// Draw the girl talking scene (before pickup line selection)
void DrawGirlTalkingScene(int difficulty, int lives, int stars, int girls_count);

// Draw pickup line selection screen
void DrawPickupLineSelection(const char *line1, const char *line2, const char *line3);

// Draw rejection screen
void DrawRejectionScreen(int attempt_num);

// Draw success screen
void DrawSuccessScreen(void);

// Update girl mouth animation
void UpdateGirlMouthAnimation(GirlSprite *girl);

// Get girl sprite based on difficulty
GirlSprite* GetGirlSprite(int difficulty);

#endif
