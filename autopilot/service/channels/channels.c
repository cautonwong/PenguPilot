/*___________________________________________________
 |  _____                       _____ _ _       _    |
 | |  __ \                     |  __ (_) |     | |   |
 | | |__) |__ _ __   __ _ _   _| |__) || | ___ | |_  |
 | |  ___/ _ \ '_ \ / _` | | | |  ___/ | |/ _ \| __| |
 | | |  |  __/ | | | (_| | |_| | |   | | | (_) | |_  |
 | |_|   \___|_| |_|\__, |\__,_|_|   |_|_|\___/ \__| |
 |                   __/ |                           |
 |  GNU/Linux based |___/  Multi-Rotor UAV Autopilot |
 |___________________________________________________|
  
 Channels Mapping and Scaling Implementation

 Copyright (C) 2014 Tobias Simon, Ilmenau University of Technology

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details. */


#include <math.h>
#include <float.h>

#include <threadsafe_types.h>
#include <opcd_interface.h>
#include <util.h>
#include "../util/math/linfunc.h"

#include "channels.h"


static linfunc_t pitch_linfunc;
static tsint_t pitch_index;
static tsfloat_t pitch_min;
static tsfloat_t pitch_max;

static linfunc_t roll_linfunc;
static tsint_t roll_index;
static tsfloat_t roll_min;
static tsfloat_t roll_max;

static linfunc_t yaw_linfunc;
static tsint_t yaw_index;
static tsfloat_t yaw_min;
static tsfloat_t yaw_max;

static linfunc_t gas_linfunc;
static tsint_t gas_index;
static tsfloat_t gas_min;
static tsfloat_t gas_max;

static linfunc_t two_state_linfunc;
static tsint_t two_state_index;
static tsfloat_t two_state_min;
static tsfloat_t two_state_max;

static linfunc_t three_state_linfunc;
static tsint_t three_state_index;
static tsfloat_t three_state_min;
static tsfloat_t three_state_max;


int channels_init(void)
{
   ASSERT_ONCE();

   opcd_param_t params[] =
   {
      {"pitch.index", &pitch_index},
      {"pitch.max", &pitch_max},
      {"pitch.min", &pitch_min},
      {"roll.index", &roll_index},
      {"roll.max", &roll_max},
      {"roll.min", &roll_min},
      {"yaw.index", &yaw_index},
      {"yaw.max", &yaw_max},
      {"yaw.min", &yaw_min},
      {"gas.index", &gas_index},
      {"gas.max", &gas_max},
      {"gas.min", &gas_min},
      {"two_state.index", &two_state_index},
      {"two_state.max", &two_state_max},
      {"two_state.min", &two_state_min},
      {"three_state.index", &three_state_index},
      {"three_state.max", &three_state_max},
      {"three_state.min", &three_state_min},
      OPCD_PARAMS_END
   };
   opcd_params_apply("channels.", params);

   /* check if channels are configured: */
   if (tsint_get(&pitch_index) == -1)
      return -1;

   vec2_t v1, v2;
   vec2_set(&v1, tsfloat_get(&pitch_min), 1.0f);
   vec2_set(&v2, tsfloat_get(&pitch_max), -1.0f);
   linfunc_init_points(&pitch_linfunc, &v1, &v2);

   vec2_set(&v1, tsfloat_get(&roll_min), 1.0f);
   vec2_set(&v2, tsfloat_get(&roll_max), -1.0f);
   linfunc_init_points(&roll_linfunc, &v1, &v2);

   vec2_set(&v1, tsfloat_get(&yaw_min), -1.0f);
   vec2_set(&v2, tsfloat_get(&yaw_max), 1.0f);
   linfunc_init_points(&yaw_linfunc, &v1, &v2);

   vec2_set(&v1, tsfloat_get(&gas_min), -1.0f);
   vec2_set(&v2, tsfloat_get(&gas_max), 1.0f);
   linfunc_init_points(&gas_linfunc, &v1, &v2);

   vec2_set(&v1, tsfloat_get(&two_state_min), 0.0f);
   vec2_set(&v2, tsfloat_get(&two_state_max), 1.0f);
   linfunc_init_points(&two_state_linfunc, &v1, &v2);
   
   vec2_set(&v1, tsfloat_get(&three_state_min), 0.0f);
   vec2_set(&v2, tsfloat_get(&three_state_max), 1.0f);
   linfunc_init_points(&three_state_linfunc, &v1, &v2);
   return 0;
}


void channels_update(float out[PP_MAX_CHANNELS], float in[16])
{
   out[CH_PITCH] = linfunc_calc(&pitch_linfunc, in[tsint_get(&pitch_index)]);
   out[CH_ROLL] = linfunc_calc(&roll_linfunc, in[tsint_get(&roll_index)]);
   out[CH_YAW] = linfunc_calc(&yaw_linfunc, in[tsint_get(&yaw_index)]);
   out[CH_GAS] = linfunc_calc(&gas_linfunc, in[tsint_get(&gas_index)]);
   out[CH_TWO_STATE] = linfunc_calc(&two_state_linfunc, in[tsint_get(&two_state_index)]);
   out[CH_THREE_STATE] = linfunc_calc(&three_state_linfunc, in[tsint_get(&three_state_index)]);
}

