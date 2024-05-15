#ifndef GLOBALS_H
#define GLOBALS_H

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

#define PADDLE_WIDTH 15
#define PADDLE_HEIGHT 100
#define PADDLE_SPEED 7

#define BALL_SIZE 8
#define BALL_SPEED_X 5
#define BALL_SPEED_Y 5

typedef struct {
	Rectangle rect;
	Vector2 speed;
} Paddle;

typedef struct {
	Vector2 position;
	Vector2 speed;
	float radius;
} Ball;

#endif