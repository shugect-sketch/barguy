#include <psxgpu.h>
#include <psxetc.h>
#include <psxapi.h>
#include "game.h"
#include "data.h"
#include "graphics.h"
#include "sprites.h"

static const int LIVES[5]              = {3, 2, 2, 1, 1};
static const int DRINKS_PER_COURAGE[5] = {1, 1, 2, 3, 4};
static const int CHANCES[5][4] = {
    {0,  60, 70, 90},   // Easy
    {0,  40, 55, 75},   // Medium
    {0,  30, 50, 65},   // Hard
    {-1, 20, 40, 55},   // Extreme  (-1 = instant slap risk)
    {-2, 10, 25, 40},   // Nightmare (-2 = guaranteed slap)
};

// Drinking penalty chances (percentage chance to lose heart)
// Index 0 = 1st drink (safe), 1 = 2nd drink (10%), etc.
static const int DRINK_PENALTY[9] = {0, 10, 20, 30, 50, 60, 70, 90, 0};

// Game state
static int lives;
static int stars;
static int courage;
static int girlsSeduced;
static int currentTry;       // 0-3
static int drinksThisRun;
static int slapsThisRun;
static int difficulty;       // 0=Easy 1=Medium 2=Hard 3=Extreme 4=Nightmare
static int selectedLine;     // which pick-up line player chose
static int lineChoices[3];   // 3 random line indices shown to player

// Sub-states within gameplay
typedef enum {
    GS_BAR,          // sitting at bar, waiting for input
    GS_PICK_LINE,    // player choosing pick-up line
    GS_GIRL_REACT,   // girl reacting
    GS_REJECT,       // girl rejected, show message
    GS_SUCCESS,      // girl accepted
    GS_DRINK_ANIM,   // brief drink pause
} GameSubState;

static GameSubState subState;
static int frame_counter;    // for timing animations

// Simple random (LCG)
static unsigned int rng_seed = 12345;
static int rng(int max) {
    rng_seed = rng_seed * 1664525 + 1013904223;
    return (int)((rng_seed >> 16) & 0x7FFF) % max;
}

// Pick 3 unique random line indices from the pool
static void PickLines(void) {
    int used[3] = {-1, -1, -1};
    int i, j, pick, unique;
    for(i = 0; i < 3; i++) {
        do {
            pick = rng(NUM_LINES);
            unique = 1;
            for(j = 0; j < i; j++)
                if(used[j] == pick) unique = 0;
        } while(!unique);
        lineChoices[i] = pick;
        used[i] = pick;
    }
}

// Seed rng from vblank counter so it's different every run
static void SeedRng(void) {
    rng_seed = GetRCnt(RCntCNT1);
}

// Check if drinking causes heart loss
static int CheckDrinkPenalty(int drinksThisRun) {
    int penalty_index;
    int penalty_chance;
    
    if (drinksThisRun <= 1) {
        return 0; // First drink is always safe
    }
    
    if (drinksThisRun > 9) {
        // 9th+ drink: random 50-99%
        penalty_chance = 50 + rng(50);
    } else {
        // Get penalty from table
        penalty_index = drinksThisRun - 1; // Convert to 0-based index
        penalty_chance = DRINK_PENALTY[penalty_index];
    }
    
    // Roll the dice
    if (rng(100) < penalty_chance) {
        return 1; // Lose heart
    }
    return 0;
}

int difficulty_selected = 0;

void InitGameplay(SaveData *save) {
    difficulty = difficulty_selected;
    lives = LIVES[difficulty];
    stars = 0;
    courage = 0;
    girlsSeduced = 0;
    currentTry = 0;
    drinksThisRun = 0;
    slapsThisRun = 0;
    subState = GS_BAR;
    frame_counter = 0;
    SeedRng();
}

int UpdateGameplay(SaveData *save) {
    // Read pad
    uint16_t buttons = ~((uint16_t)pad_buf1[3] | ((uint16_t)pad_buf1[2] << 8));
    
    // Draw current scene
    DrawBarScene(difficulty, lives, stars, courage, girlsSeduced);

    switch(subState) {
        case GS_BAR:
            // Cross = Drink, Circle = Seduce
            if(buttons & PAD_CROSS) {
                drinksThisRun++;
                stars++;
                
                // Check if this drink causes heart loss
                if(CheckDrinkPenalty(drinksThisRun)) {
                    lives--;
                    if(lives <= 0) {
                        WriteSave(save);
                        return STATE_GAME_OVER;
                    }
                }
                
                // Increase courage based on drinks
                if(drinksThisRun % DRINKS_PER_COURAGE[difficulty] == 0)
                    courage++;
                
                subState = GS_DRINK_ANIM;
                frame_counter = 0;
            }
            else if(buttons & PAD_CIRCLE) {
                if(courage > 0) {
                    courage--;  // Use one courage point
                    PickLines();
                    currentTry = 0;  // Reset try counter for new girl
                    subState = GS_PICK_LINE;
                    frame_counter = 0;
                }
            }
            break;

        case GS_DRINK_ANIM:
            // Brief pause then back to bar
            frame_counter++;
            if(frame_counter > 10) {
                subState = GS_BAR;
            }
            break;

        case GS_PICK_LINE:
            // Draw girl talking scene with pickup line options
            DrawGirlTalkingScene(difficulty, lives, stars, girlsSeduced);
            DrawPickupLineSelection(
                lines[lineChoices[0]].text,
                lines[lineChoices[1]].text,
                lines[lineChoices[2]].text
            );
            
            // X=line1, Circle=line2, Square=line3
            if(buttons & PAD_CROSS)   { selectedLine = lineChoices[0]; subState = GS_GIRL_REACT; }
            if(buttons & PAD_CIRCLE)  { selectedLine = lineChoices[1]; subState = GS_GIRL_REACT; }
            if(buttons & PAD_SQUARE)  { selectedLine = lineChoices[2]; subState = GS_GIRL_REACT; }
            break;

        case GS_GIRL_REACT: {
            int chance = CHANCES[difficulty][currentTry];
            int roll   = rng(100);
            int lineBonus = lines[selectedLine].bonus; // -10 / 0 / +10
            int totalChance = chance + lineBonus;

            // First try ALWAYS rejects
            if(currentTry == 0) {
                subState = GS_REJECT;
                frame_counter = 0;
                break;
            }

            // Extreme try 1: can have instant slap risk
            if(difficulty == 3 && currentTry == 1) {
                if(roll < 30) {
                    // SLAPPED
                    slapsThisRun++;
                    save->totalSlaps++;
                    lives--;
                    currentTry = 0;
                    courage = 0;
                    if(lives <= 0) {
                        WriteSave(save);
                        return STATE_GAME_OVER;
                    }
                    return STATE_NEXT_GIRL;
                }
                // Otherwise continue to next check
            }

            // Nightmare try 1: guaranteed slap risk (-2 means hard)
            if(difficulty == 4 && currentTry == 1) {
                if(roll < 50) {
                    // SLAPPED
                    slapsThisRun++;
                    save->totalSlaps++;
                    lives--;
                    currentTry = 0;
                    courage = 0;
                    if(lives <= 0) {
                        WriteSave(save);
                        return STATE_GAME_OVER;
                    }
                    return STATE_NEXT_GIRL;
                }
            }

            if(roll < totalChance) {
                // SUCCESS
                girlsSeduced++;
                save->totalGirls++;
                CheckAchievements(save, girlsSeduced, drinksThisRun, slapsThisRun, difficulty);
                if(girlsSeduced > save->highScore[difficulty])
                    save->highScore[difficulty] = girlsSeduced;
                currentTry = 0;
                subState = GS_SUCCESS;
                frame_counter = 0;
            } else if(currentTry >= 3) {
                // SLAPPED on last try
                slapsThisRun++;
                save->totalSlaps++;
                lives--;
                currentTry = 0;
                courage = 0;
                if(lives <= 0) {
                    WriteSave(save);
                    return STATE_GAME_OVER;
                }
                return STATE_NEXT_GIRL;
            } else {
                // Rejected, try again
                currentTry++;
                subState = GS_REJECT;
                frame_counter = 0;
            }
            break;
        }

        case GS_REJECT:
            DrawRejectionScreen(currentTry);
            if(buttons & PAD_CROSS) {
                lives--;  // Lose heart on rejection
                if(lives <= 0) {
                    WriteSave(save);
                    return STATE_GAME_OVER;
                }
                subState = GS_PICK_LINE;
                PickLines();  // New lines for next attempt
            }
            break;

        case GS_SUCCESS:
            DrawSuccessScreen();
            if(buttons & PAD_CROSS) {
                return STATE_NEXT_GIRL;
            }
            break;
    }

    return STATE_GAMEPLAY;
}

int UpdateNextGirl(void) {
    // Brief "Next Girl!" screen - for now just return to gameplay
    FntPrint(-1, "BARGUY\n\nNext Girl...\n\nPress X to continue");
    FntFlush(-1);
    
    uint16_t buttons = ~((uint16_t)pad_buf1[3] | ((uint16_t)pad_buf1[2] << 8));
    if(buttons & PAD_CROSS) {
        drinksThisRun = 0;  // Reset drinks for new girl
        currentTry = 0;
        courage = 0;
        return STATE_GAMEPLAY;
    }
    
    return STATE_NEXT_GIRL;
}

int UpdateVictory(SaveData *save) {
    WriteSave(save);
    return STATE_NEXT_GIRL;
}

int UpdateGameOver(void) {
    FntPrint(-1, "BARGUY\n\nGAME OVER!\n\nGirls Seduced: %d\nTotal Slaps: %d\n\nPress X to return to menu", 
             girlsSeduced, slapsThisRun);
    FntFlush(-1);
    
    uint16_t buttons = ~((uint16_t)pad_buf1[3] | ((uint16_t)pad_buf1[2] << 8));
    if(buttons & PAD_CROSS) {
        return STATE_MENU;
    }
    
    return STATE_GAME_OVER;
}
