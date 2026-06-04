#include <psxgpu.h>
#include <psxetc.h>
#include <psxapi.h>
#include "graphics.h"
#include "sprites.h"

// Placeholder TIM loading (will be expanded based on your asset setup)
static void LoadTIM(TextureID id, const char *filename) {
    // TODO: Implement TIM loading from data/textures/
    // For now this is a stub
    (void)id;
    (void)filename;
}

void GraphicsInit(void) {
    // Load all textures
    LoadTIM(TEX_BAR_BG,       "data/textures/bar_bg.tim");
    LoadTIM(TEX_PLAYER,       "data/textures/player.tim");
    LoadTIM(TEX_GIRL_EASY,    "data/textures/girl_easy.tim");
    LoadTIM(TEX_GIRL_HARD,    "data/textures/girl_hard.tim");
    LoadTIM(TEX_GIRL_EXTREME, "data/textures/girl_extreme.tim");
    LoadTIM(TEX_GIRL_NIGHTMARE, "data/textures/girl_nightmare.tim");
}

void DrawBarScene(int difficulty, int lives, int stars, int courage, int girls_count) {
    // Draw background
    // Draw player sprite
    // Draw girl sprite based on difficulty
    // Draw UI: hearts, stars, courage counter, girls counter
    // Draw buttons: "Drink" and "Seduce"
    
    FntPrint(-1, "BARGUY\n");
    FntPrint(-1, "Lives: %d  Stars: %d  Girls: %d  Courage: %d\n", 
             lives, stars, girls_count, courage);
    FntPrint(-1, "\n[X] Drink    [O] Seduce\n");
    FntFlush(-1);
}

void DrawGirlTalkingScene(int difficulty, int lives, int stars, int girls_count) {
    // Draw background
    // Draw player sprite
    // Draw girl sprite with mouth OPEN
    // Draw UI counters
    // Display that girl is talking (no buttons yet)
    
    FntPrint(-1, "BARGUY\n");
    FntPrint(-1, "Lives: %d  Stars: %d  Girls: %d\n", 
             lives, stars, girls_count);
    FntPrint(-1, "\n[Girl is talking...]\n");
    FntFlush(-1);
}

void DrawPickupLineSelection(const char *line1, const char *line2, const char *line3) {
    // Draw the girl scene still
    // Overlay pickup line choices on screen
    // Show which button selects which line
    
    FntPrint(-1, "BARGUY - Pick Your Line\n\n");
    FntPrint(-1, "[X] %s\n", line1);
    FntPrint(-1, "[O] %s\n", line2);
    FntPrint(-1, "[SQ] %s\n", line3);
    FntFlush(-1);
}

void DrawRejectionScreen(int attempt_num) {
    // Draw girl scene with sad/angry expression
    // Show rejection message
    // Show attempt counter
    
    FntPrint(-1, "\n\n\n\n\n");  // Push text down to avoid overlap
    FntPrint(-1, "She rejected you!\n");
    FntPrint(-1, "(Attempt %d/3)\n", attempt_num);
    FntPrint(-1, "\n[X] Try Again\n");
    FntFlush(-1);
}

void DrawSuccessScreen(void) {
    // Draw girl scene with happy expression
    // Show success message
    // Show "Going home..." or similar
    
    FntPrint(-1, "BARGUY\n\n");
    FntPrint(-1, "Success! She's going home with you!\n");
    FntPrint(-1, "[X] Next Girl\n");
    FntFlush(-1);
}

void UpdateGirlMouthAnimation(GirlSprite *girl) {
    if (!girl) return;
    
    // Simple mouth animation: toggle every 10 frames
    girl->mouth_timer++;
    if (girl->mouth_timer > 10) {
        girl->mouth_open = !girl->mouth_open;
        girl->mouth_timer = 0;
    }
}

GirlSprite* GetGirlSprite(int difficulty) {
    static GirlSprite girl;
    
    // Position girl on right side of screen
    girl.x = 200;
    girl.y = 80;
    girl.width = 80;
    girl.height = 120;
    girl.mouth_open = 0;
    girl.mouth_timer = 0;
    
    // Select texture based on difficulty
    // TEX_GIRL_EASY for 0,1 (Easy, Medium)
    // TEX_GIRL_HARD for 2 (Hard)
    // TEX_GIRL_EXTREME for 3 (Extreme)
    // TEX_GIRL_NIGHTMARE for 4 (Nightmare)
    
    return &girl;
}
