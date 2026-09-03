#define _DEFAULT_SOURCE
#include "complementary_filter.h"
#include <math.h>

static float accel_roll_degree(float ay, float az) {
  // Standard convention:
  // roll  = rotation about X axis, derived from Y/Z accel
  return atan2f(ay, az) * (180.0f / (float)M_PI);
}

static float accel_pitch_degree(float ax, float ay, float az) {
  // Standard convention:
  // pitch = rotation about Y axis, derived from X/Z accel
  return atan2f(-ax, sqrtf(ay * ay + az * az)) * (180.0f / (float)M_PI);
}

void cf_init(cf_state_t *state, float alpha) {
  state->pitch_degree = 0.0f;
  state->roll_degree = 0.0f;
  state->alpha = alpha;
}

void cf_update(cf_state_t *state, float accel_x_g, float accel_y_g,
               float accel_z_g, float gyro_x_dps, float gyro_y_dps,
               float dt_s) {

  // 1. Gyro-only estimate: integrate angular rate over dt.
  // Axis mapping (gyro_x drives roll, gyro_y drives pitch) assumes a
  // particular sensor mounting orientation
  float gyro_roll_estimate = state->roll_degree + gyro_x_dps * dt_s;
  float gyro_pitch_estimate = state->pitch_degree + gyro_y_dps * dt_s;

  // 2. Accelerometer-only estimate: use the accelerometer readings to estimate
  // the orientation.
  float accel_roll_estimate = accel_roll_degree(accel_y_g, accel_z_g);
  float accel_pitch_estimate =
      accel_pitch_degree(accel_x_g, accel_y_g, accel_z_g);

  // 3. Blend: trust gyro short-term, accel long-term.
  state->roll_degree = state->alpha * gyro_roll_estimate +
                       (1.0f - state->alpha) * accel_roll_estimate;
  state->pitch_degree = state->alpha * gyro_pitch_estimate +
                        (1.0f - state->alpha) * accel_pitch_estimate;
}