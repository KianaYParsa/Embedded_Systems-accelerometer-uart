#include <xc.h>

#ifndef IMU_H
#define IMU_H

// Defining a structure to hold 3 axes
typedef struct {
    int x;
    int y;
    int z;
} AxesRaw_t;

void imu_init(void);
void accel_write_reg(unsigned char addr, unsigned char value);
AxesRaw_t accel_read_xyz(void);

#endif

