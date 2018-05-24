#ifndef __MOTOR_H__
#define __MOTOR_H__

#ifdef __cplusplus
extern "C" {
#endif
typedef unsigned char uint8_t;

/* assign gpios to drive the shield pins */

/*      Shield      Pi */

#define MOTORLATCH      29
#define MOTORCLK        28
//#define MOTORENABLE   22
#define MOTORDATA       27

#define MOTOR_1_PWM     5
#define MOTOR_2_PWM     6
#define MOTOR_3_PWM     13
#define MOTOR_4_PWM     19

#define PWM_MAX 100 // Max PWM value.
#define PWM_HAF 50
#define PWM_MIN 30 // MIN of the max PWM value.

/* Bit values for writing to the shift register. These correspond to bit numbers in latch_state. */

#define MOTOR1_A 3//2
#define MOTOR1_B 2//3
#define MOTOR2_A 4//1
#define MOTOR2_B 1//4
#define MOTOR4_A 0
#define MOTOR4_B 6
#define MOTOR3_A 5
#define MOTOR3_B 7

#define FORWARD  1
#define BACKWARD 2
#define BRAKE    3
#define RELEASE  4

void latch_tx(void);

int ControllerInit(void);

void ControllerShutdown(void);

void DCMotorInit(uint8_t num);

void DCMotorRun(uint8_t motornum, uint8_t cmd);

void TestRun(void);

void go_forward(void);

void go_left(void);

void go_right(void);

void go_forward_left(void);

void go_forward_right(void);

void go_back(void);

void go_back_left(void);

void go_back_right(void);

void stop(void);

#ifdef __cplusplus
}
#endif

#endif /* __WS2811_H__ */
