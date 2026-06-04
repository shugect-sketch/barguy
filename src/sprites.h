#ifndef SPRITES_H
#define SPRITES_H

// Girl model selection based on difficulty
typedef enum {
    GIRL_EASY = 0,      // Used for Easy (0) and Medium (1)
    GIRL_HARD = 2,      // Used for Hard
    GIRL_EXTREME = 3,   // Used for Extreme
    GIRL_NIGHTMARE = 4  // Used for Nightmare
} GirlModel;

// Get which girl model to use based on difficulty
GirlModel GetGirlModelForDifficulty(int difficulty);

#endif
