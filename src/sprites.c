#include "sprites.h"

GirlModel GetGirlModelForDifficulty(int difficulty) {
    switch(difficulty) {
        case 0: // Easy
        case 1: // Medium
            return GIRL_EASY;
        case 2: // Hard
            return GIRL_HARD;
        case 3: // Extreme
            return GIRL_EXTREME;
        case 4: // Nightmare
            return GIRL_NIGHTMARE;
        default:
            return GIRL_EASY;
    }
}
