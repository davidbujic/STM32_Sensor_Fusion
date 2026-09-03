#ifndef COMPLEMENTARY_FILTER_H
#define COMPLEMENTARY_FILTER_H
#include <stdint.h>

typedef struct {
  float pitch_degree; // current fused pitch estimate, degrees
  float roll_degree;  // current fused roll estimate, degrees
  float alpha;        // filter weight, 0..1 - fraction trusted to gyro
} cf_state_t;

/**
 * @brief Initialize filter state.
 * @param state  Filter state to initialize.
 * @param alpha  Weight given to the gyro-integrated estimate (typical:
 * 0.96-0.98). (1 - alpha) is the weight given to the accelerometer estimate.
 */
void cf_init(cf_state_t *state, float alpha);

/**
 * @brief Update the filter with one new sensor sample.
 *
 * @param state       Filter state (updated in place).
 * @param accel_x_g   Accelerometer X reading, in g.
 * @param accel_y_g   Accelerometer Y reading, in g.
 * @param accel_z_g   Accelerometer Z reading, in g.
 * @param gyro_x_dps  Gyroscope X angular rate, degrees/sec.
 * @param gyro_y_dps  Gyroscope Y angular rate, degrees/sec.
 * @param dt_s        Time elapsed since the previous update, in seconds.
 */
void cf_update(cf_state_t *state, float accel_x_g, float accel_y_g,
               float accel_z_g, float gyro_x_dps, float gyro_y_dps, float dt_s);

#endif /* COMPLEMENTARY_FILTER_H */