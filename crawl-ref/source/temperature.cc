/**
 * @file
 * @brief Temperature calculation and effects.
**/

#include "math.h"
#include "monster.h"
#include "player.h"
#include "religion.h"

/**
 * Raw temperature getters and setters.
**/

// Get the raw value of the player's current temperature.
float player::get_raw_current_temperature() const
{
    return you.temperature_current;
}

// Get the raw value of the player's upcoming temperature.
float player::get_raw_upcoming_temperature() const
{
    return you.temperature_upcoming;
}

// Get the difference between an actor's current and upcoming temperatures.
float actor::get_temperature_delta() const
{
    return get_raw_upcoming_temperature() - get_raw_current_temperature();
}

// Set the raw value of the player's current temperature.
void player::set_raw_current_temperature(float temperature)
{
    you.temperature_current = temperature;
    cap_temperature();
}

// Set the raw value of the player's upcoming temperature.
void player::set_raw_upcoming_temperature(float temperature)
{
    you.temperature_upcoming = temperature;
    cap_temperature();
}

// Ensure the actor's upcoming temperature doesn't exceed the allowable range.
void actor::cap_raw_upcoming_temperature(float degree)
{
    if (get_raw_upcoming_temperature() > TEMP_MAX)
        set_raw_upcoming_temperature((float) TEMP_MAX);
    else if (get_raw_upcoming_temperature() < TEMP_MIN)
        set_raw_upcoming_temperature((float) TEMP_MIN);
}

// Return whether the player's temperature can increase naturally.
bool player::temperature_can_increase() const
{
    // No warming up while you're exhausted!
    if (you.duration[DUR_EXHAUSTED])
        return false;
    return true;
}

// Return whether the player's temperature can decrease naturally.
bool player::temperature_can_decrease() const
{
    // No cooling off while in lava!
    if (feat_is_lava(env.grid(you.pos())) && you.ground_level())
        return false;
    // No cooling off while you're angry!
    if (you.duration[DUR_BERSERK])
        return false;
    return true;
}

// Change an actor's temperature gradually, taking effect next turn.
void actor::change_temperature(float degree)
{
    if (degree >= 0 && !temperature_can_increase())
        return;
    else if (!temperature_can_decrease())
        return;

    set_raw_upcoming_temperature(get_raw_upcoming_temperature() + degree);
}

// Exponentially decay an actor's temperature based on a decay constant, λ.
void actor::temperature_decay(float factor)
{
    // Decay is based on current (rather than upcoming) temperature.
    float base_upcoming_temperature = get_raw_current_temperature() * pow(M_E, -TEMP_DECAY_CONSTANT);
    // Upcoming temperature is set, with the current delta added back in.
    set_raw_upcoming_temperature(base_upcoming_temperature + get_temperature_delta())
}

/**
 * Temperature level methods.
**/

// Get an actor's current temperature level (floored current temperature).
uint8_t actor::get_current_temperature_level() const
{
    return (uint8_t) get_raw_current_temperature();
}

// Get an actor's next temperature level (floored upcoming temperature).
uint8_t actor::get_next_temperature_level() const
{
    return (uint8_t) get_raw_upcoming_temperature();
}

// Return whether an actor's temperature rose to (or above) a level recently.
bool actor::temperature_reached_level (uint8_t level) const
{
    return (get_current_temperature_level() < level
        && get_upcoming_temperature_level() >= level)
}

// Return whether an actor's temperature fell below a level recently.
bool actor::temperature_fell_below_level (uint8_t level) const
{
    return (get_current_temperature_level() >= level
        && get_upcoming_temperature_level() < level)
}

/**
 * Temperature effect methods.
**/

// Return whether an actor has temperature effects.
bool actor::has_temperature_effects() const
{
    return false;
}

// Return whether the player has temperature effects.
bool player::has_temperature_effects() const
{
    if (you.species == SP_LAVA_ORC)
        return true;
    return actor::has_temperature_effects();
}

// Return whether an actor's temperature effect is active.
bool actor::temperature_effect_is_active (uint8_t effect) const
{
    return temperature_effect(effect, get_current_temperature_level());
}

// Return whether an actor's temperature effect is activating.
bool actor::temperature_effect_is_activating (uint8_t effect) const
{
    return (!temperature_effect_is_active(effect)
        && temperature_effect(effect, get_next_temperature_level())
}

// Return whether an actor's temperature effect is deactivating.
bool actor::temperature_effect_is_deactivating (uint8_t effect) const
{
    return (temperature_effect_is_active(effect)
        && !temperature_effect(effect, get_next_temperature_level())
}

/**
 * Temperature helper functions.
**/

// Return whether a temperature effect is active at a temperature level.
bool temperature_effect(uint8_t effect, uint8_t level) const
{
    switch (effect)
    {
        case LORC_FIRE_RES_I:
            return true; // 1-15
        case LORC_SLOW_MOVE:
            return (level < TEMP_COOL); // 1-4
        case LORC_STONESKIN:
            return (level < TEMP_WARM); // 1-8
        case LORC_LAVA_BOOST:
            return (level >= TEMP_WARM && level < TEMP_HOT); // 9-10
        case LORC_FIRE_RES_II:
        case LORC_MELT_ICE:
        case LORC_MELT_STONE:
            return (level >= TEMP_WARM); // 9-15
        case LORC_FIRE_RES_III:
        case LORC_FIRE_BOOST:
        case LORC_COLD_VULN:
            return (level >= TEMP_HOT); // 11-15
        case LORC_FAST_MOVE:
        case LORC_PASSIVE_HEAT:
        case LORC_BOIL_WATER:
            return (level >= TEMP_FIRE); // 13-15
        case LORC_HEAT_AURA:
        case LORC_NO_SCROLLS:
            return (level >= TEMP_MAX); // 15

        default:
            return false;
    }
}

// Return the colour matching a temperature level.
colour_t temperature_colour(uint8_t level) const
{
return (level > TEMP_FIRE) ? LIGHTRED  :
       (level > TEMP_HOT)  ? RED       :
       (level > TEMP_WARM) ? YELLOW    :
       (level > TEMP_ROOM) ? WHITE     :
       (level > TEMP_COOL) ? LIGHTCYAN :
       (level > TEMP_COLD) ? LIGHTBLUE : BLUE;
}

// Return the text description matching a temperature level.
std::string temperature_description(uint8_t level) const
{
    switch (level)
    {
        case TEMP_MIN:
            return "rF+; slow movement";
        case TEMP_COOL:
            return "Average movement";
        case TEMP_WARM:
            return "rF++; skin softens; enhanced lava magic";
        case TEMP_HOT:
            return "rF+++; rC-; enhanced fire magic";
        case TEMP_FIRE:
            return "Fast movement; burn attackers";
        case TEMP_MAX:
            return "Burn surroundings; cannot read books or scrolls";
        default:
            return "";
    }
}

/**
 * Temperature upkeep methods.
**/

// Check whether the actor's temperature has changed and/or caused effects.
void actor::check_temperature()
{
    // If our temperature can't decrease, ignore TEMP_MAXIMUM_INCREASE.
    bool ignore_maximum_increase = !temperature_can_decrease();

    // Set temperature to full if you're standing in lava.
    if (feat_is_lava(env.grid(pos())) && ground_level())
    {
        // Bypass the normal increment method.
        set_raw_upcoming_temperature((float) TEMP_MAX)
        ignore_maximum_increase = true;

        // Print a message if a new temperature level was reached.
        if (temperature_reached_level(TEMP_MAX))
            mpr("The lava instantly superheats you.");

    // Otherwise, if your temperature is high enough, boil any water you're standing in.
    } else if (feat_is_water(env.grid(pos())) && ground_level()
            && temperature_effect_is_active(LORC_BOIL_WATER))
    {
        for (adjacent_iterator ai(pos()); ai; ++ai)
        {
            const coord_def p(*ai);
            if (!one_chance_in(5))
                continue

            // Temperature decreases from boiling even if no cloud is placed.
            temperature_decrement(1);
            if (in_bounds(p) && env.cgrid(p) == EMPTY_CLOUD)
                place_cloud(CLOUD_STEAM, *ai, 2 + random2(5), &you);
        }
    }

    // Temperature undergoes exponential decay unless blocked.
    if (temperature_can_decrease())
        temperature_decay();

    // Calculate the raw tension value.
    int tension = get_tension(GOD_NO_GOD);

    // Increment temperature by the square root of tension.
    float degree = sqrt(tension)
    temperature_increment(degree);

    // Calculate temperature delta since last turn.
    float temperature_delta = get_temperature_delta()

    // Cap temperature delta by TEMP_MAXIMUM_(INCREASE/DECREASE).
    if (temperature_delta > TEMP_MAXIMUM_INCREASE and !ignore_maximum_increase)
        temperature_delta = min(temperature_delta, TEMP_MAXIMUM_INCREASE)
    else if (temperature_delta < TEMP_MAXIMUM_DECREASE)
        temperature_delta = max(temperature_delta, TEMP_MAXIMUM_DECREASE)

    // Set the new upcoming temperature, respecting the cap.
    set_raw_upcoming_temperature(get_raw_current_temperature() + temperature_delta)

    // Process the upcoming temperature change.
    process_temperature_change();
}

// Handle any effects or messaging that occur when temperature changes, then
// change temperature, then handle necessary cleanup.
void player::process_temperature_change() {

    bool invalidate_agrid = false;

    // The messages we might see while warming up:
    if (get_temperature_delta() > 0) {
        // Warmed up enough to lose slow movement.
        if (temperature_effect_is_deactivating(LORC_SLOW_MOVE))
            mpr("You feel quick.", MSGCH_DURATION);

        // Warmed up enough to lose stoneskin.
        if (temperature_effect_is_deactivating(LORC_STONESKIN))
        {
            mpr("Your stony skin warms and softens.", MSGCH_DURATION);
            you.set_duration(DUR_STONESKIN, 0);
            you.redraw_armour_class = true;
        }

        // Warmed up enough to lose statue form.
        if (temperature_effect_is_activating(LORC_MELT_STONE))
            if (you.form == TRAN_STATUE)
                untransform(true, false);
        }

        // Warmed up enough to be anathema to ice magic.
        if (temperature_effect_is_activating(LORC_MELT_ICE))
        {
            // Handles condensation shield, ozo's armour, icemail.
            expose_player_to_element(BEAM_FIRE, 0);

            // Handled separately because normally heat doesn't end ice form.
            if (you.form == TRAN_ICE_BEAST)
                untransform(true, false);
        }

        // Warmed up enough to gain fast movement.
        if (temperature_effect_is_activating(LORC_FAST_MOVE))
            mpr("You feel quick.", MSGCH_DURATION);

        // Warmed up enough to gain passive heat.
        if (temperature_effect_is_activating(LORC_PASSIVE_HEAT))
            mpr("You're getting fired up.", MSGCH_DURATION);

        // Warmed up enough to gain a heat aura.
        if (temperature_effect_is_activating(LORC_HEAT_AURA))
        {
            mpr("You blaze with the fury of an erupting volcano!", MSGCH_DURATION);
            invalidate_agrid = true;
        }

    // The messages we might see while cooling down:
    } else {
        // Cooled down enough to lose a heat aura.
        if (temperature_effect_is_deactivating(LORC_HEAT_AURA))
        {
            mpr("The intensity of your heat diminishes.", MSGCH_DURATION);
            invalidate_agrid = true;
        }

        // Cooled down enough to lose passive heat.
        if (temperature_effect_is_deactivating(LORC_PASSIVE_HEAT))
            mpr("You're cooling off.", MSGCH_DURATION);

        // Cooled down enough to lose fast movement.
        if (temperature_effect_is_deactivating(LORC_FAST_MOVE))
            mpr("You feel sluggish.", MSGCH_DURATION);

        // Cooled down enough to gain stoneskin.
        if (temperature_effect_is_activating(LORC_STONESKIN))
        {
            you.set_duration(DUR_STONESKIN, 500);
            mpr("Your stony skin cools and hardens.", MSGCH_DURATION);
            you.redraw_armour_class = true;
        }

        // Cooled down enough to gain slow movement.
        if (temperature_effect_is_activating(LORC_SLOW_MOVE))
            mpr("You feel sluggish.", MSGCH_DURATION);
    }

    // Set the current temperature to the upcoming temperature.
    set_raw_current_temperature(get_raw_upcoming_temperature())

    // Mark the temperature bar for redraw.
    you.redraw_temperature = true;

    // In tiles, redraw the player doll.
    #ifdef USE_TILE
        init_player_doll();
    #endif

    // Invalidate the area grid if necessary.
    if (invalidate_agrid)
        invalidate_agrid(true);
}