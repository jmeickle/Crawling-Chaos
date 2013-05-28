/**
 * @file
 * @brief Misc monster related functions.
**/

/**
 * Exponential decay of temperature is ultimately defined by a half life.
 *
 * Temp(t+1) = Temp(t) * e^(-λ)
 * λ = TEMP_DECAY_CONSTANT = t_1/2 / ln(2)
 * t_1/2 = TEMP_HALF_LIFE
**/

// The number of turns it takes for temperature to halve.
#define TEMP_HALF_LIFE = 10
#define TEMP_DECAY_CONSTANT = TEMP_HALF_LIFE / ln(2)

// Caps on the amount temperature can change by in a turn.
#define TEMP_MAXIMUM_INCREASE = 15
#define TEMP_MAXIMUM_DECREASE = -15

enum temperature_level
{
    TEMP_MIN = 1, // Minimum temperature. Not any warmer than bare rock.
    TEMP_STARTING = TEMP_MIN, // Starting temperature.
    TEMP_COLD = 3,
    TEMP_COOL = 5,
    TEMP_ROOM = 7, // As warm as most humanoid creatures.
    TEMP_WARM = 9, // Warmer than most humanoid creatures.
    TEMP_HOT = 11,
    TEMP_FIRE = 13, // Hot enough to flash boil water in contact with you.
    TEMP_MAX = 15, // Maximum temperature. As hot as lava!
};

enum temperature_effect
{
    LORC_LAVA_BOOST, // Enhancer boost to (Earth | Fire) spells
    LORC_FIRE_BOOST, // Enhancer boost to Fire
    LORC_STONESKIN, // Innate Stoneskin spell
    LORC_SLOW_MOVE, // Slow movement (-2)
    LORC_FAST_MOVE, // Fast movement (+2)
    LORC_PASSIVE_HEAT, // Singe attackers when attacked
    LORC_HEAT_AURA, // Burn nearby attackers
    LORC_NO_SCROLLS, // Cannot read scrolls
    LORC_COLD_VULN, // rC-
    LORC_FIRE_RES_I, // rF+
    LORC_FIRE_RES_II, // rF++
    LORC_FIRE_RES_III, // rF+++
    LORC_MELT_ICE, // Melt ice magic
    LORC_BOIL_WATER, // Generate steam when contacting water
    LORC_MELT_STONE, // Melt stone
};

colour_t temperature_colour(uint8_t level);
std::string temperature_description(uint8_t level);