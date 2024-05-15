#include "raylib.h"
#include "globals.h"
#include <stdbool.h>
#include <math.h>

Paddle CreatePaddle(float x, float y, float width, float height, float speedX, float speedY)
{
	Paddle paddle = {
		{x, y, width, height},
		{speedX, speedY}
	};
	return paddle;
}

Ball CreateBall(float x, float y, float speedX, float speedY, float radius)
{
	Ball ball = {
		{x, y}, 
		{speedX, speedY},
		{radius}
	};
	return ball;
}

void UpdateBall(Ball *ball) {
	// Increment x and y vals for ball to create movement
	ball->position.x += ball->speed.x;
	ball->position.y += ball->speed.y;

	// Handle top and bottom edges
	if ((ball->position.y - ball->radius) <= 0 || 
	    (ball->position.y + ball->radius) >= SCREEN_HEIGHT) {
	    ball->speed.y *= -1;
	}
}

void DrawBall(Ball ball) {
	DrawCircleV(ball.position, ball.radius, WHITE);
}

void DrawBoard(void) 
{
	const char *title = "Raylib Pong";
	const char *author = "by Tom Ice";
	const int dashLength = 30;
	const int gapLength = 15;
	const int barDistFromYOrigin = 50;
	const int fontSize = 30;
	int y;
	
	// Print Title
	DrawText(title, (SCREEN_WIDTH - MeasureText(title, fontSize)) / 2, 0, fontSize, WHITE);
	DrawText(author, (SCREEN_WIDTH - MeasureText(author, fontSize - 12)) / 2, 30, fontSize - 12, WHITE);

	// Center Dashed Line
	for (y = 0 + barDistFromYOrigin; 
		 y < SCREEN_HEIGHT - barDistFromYOrigin;
		 y += (dashLength + gapLength)) {
		DrawLineEx((Vector2){SCREEN_WIDTH / 2, y}, (Vector2){SCREEN_WIDTH / 2, y + dashLength}, 2, WHITE);
	}

	// Horizontal Line. Just for reference to verify stuff is centered
	//DrawLine(0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT / 2, WHITE);

	// Top Line
	DrawLine(0, barDistFromYOrigin, SCREEN_WIDTH, barDistFromYOrigin, WHITE);

	// Bottom Line
	DrawLine(0, SCREEN_HEIGHT - barDistFromYOrigin, SCREEN_WIDTH, SCREEN_HEIGHT - barDistFromYOrigin, WHITE);
}

void PlayerScore(int p1Score)
{
	const int centerOffset = 60;
	const int posX = 50;
	const int posY = 50;

	DrawText(TextFormat("%d", p1Score), (SCREEN_WIDTH / 2) - centerOffset, posX, posY, WHITE);
}

void OpponentScore(int p2Score)
{
	const int centerOffset = 30;
	const int posX = 50;
	const int posY = 50;

	DrawText(TextFormat("%d", p2Score), (SCREEN_WIDTH / 2) + centerOffset, posX, posY, WHITE);
}

bool CheckBallCollision(Ball ball, Rectangle paddle, Sound sfx)
{
	float distX;
	float distY;

	// Calculate distance between center of ball and paddle
	distX = fabs(ball.position.x - (paddle.x + paddle.width / 2));
	distY = fabs(ball.position.y - (paddle.y + paddle.height / 2));

	// Detect collision if ball is outside bounding box of paddle
	if (distX > (paddle.width  / 2 + ball.radius)) return false;
	if (distY > (paddle.height / 2 + ball.radius)) return false;

	// Handle edge collisions and play sfx
	if (distX <= (paddle.width / 2) || distY <= (paddle.height / 2)) {
		PlaySound(sfx);
		return true;
	}

	return false;
}

int main(void)
{
	// TODO: Refactor this logic into their own special functions
	int barPosX = (SCREEN_WIDTH - PADDLE_WIDTH) / 2;
	int barPosY = (SCREEN_HEIGHT - PADDLE_HEIGHT) / 2;
	int ballPosX = (SCREEN_HEIGHT / 2) - PADDLE_HEIGHT / 2;
	int p1Score = 0;
	int p2Score = 0;
	Paddle p1;
	Paddle p2;
	Ball ball;

	// Create Player 1 and Player 2 paddles in the middle of the screen
	// Struct is x, y, width, height, speedX, speedY
	p1 = CreatePaddle(PADDLE_HEIGHT / 2, 
		                     SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2, 
		                     PADDLE_WIDTH, 
		                     PADDLE_HEIGHT, 
		                     0, 0);
	p2 = CreatePaddle(SCREEN_WIDTH - (PADDLE_HEIGHT / 2) - PADDLE_WIDTH, 
		                     SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2, 
		                     PADDLE_WIDTH, 
		                     PADDLE_HEIGHT, 
		                     0, 0);
	
	// Create the ball
	// Struct is x, y, speedX, speedY, radius
	ball = CreateBall(SCREEN_WIDTH / 2, 
		              SCREEN_HEIGHT / 2, 
		              BALL_SPEED_X, 
		              BALL_SPEED_Y, 
		              BALL_SIZE);
	
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Raylib Pong");
	SetTargetFPS(144);

	// TODO: Create a splash screen. Maybe some sort of rectangle stating
	//       controls and pressing a button to start the game.
	//       Or maybe always print controls at bottom of screen?

	// TODO: Create a win condition of 10 points or something. 
	//       Game should stop, state who won, then allow the user to press any
	//       key to go back to the splash screen

	// TODO: We can create better ways to handle this in their own functions
	//       main() really needs to be cleaned up
	InitAudioDevice();
	Music music = LoadMusicStream("Sapience.ogg");
	Sound fxOgg = LoadSound("pongBlip.ogg");
	PlayMusicStream(music);

	// Game loop
	while (!WindowShouldClose()) {
		// Raylib stuff
		BeginDrawing();
		ClearBackground(BLACK);

		// Throw the ball out there
		// TODO: We can probably have this be random instead of always going after p2
		UpdateBall(&ball);

		// Handle ball collision
		if (CheckBallCollision(ball, p1.rect, fxOgg) ||
			CheckBallCollision(ball, p2.rect, fxOgg)) {
			ball.speed.x *= -1;
		}

		// Start music
		UpdateMusicStream(music);

		// User Input
		if (IsKeyDown(KEY_W)) p1.rect.y -= PADDLE_SPEED;
		if (IsKeyDown(KEY_S)) p1.rect.y += PADDLE_SPEED;
		if (IsKeyDown(KEY_UP)) p2.rect.y -= PADDLE_SPEED;
		if (IsKeyDown(KEY_DOWN)) p2.rect.y += PADDLE_SPEED;

		// Handle scoring
		// TODO: Refactor this so we don't have to worry about repeat code
		//       Probably some ResetBall() function that handles 3 of the 4 params
		if (ball.position.x > SCREEN_WIDTH) {
			ball.position.x = SCREEN_WIDTH / 2;
			ball.position.y = SCREEN_HEIGHT / 2;
			ball.speed.x = -BALL_SPEED_X;
			ball.speed.y = BALL_SPEED_Y;
			p1Score++;
		}

		if (ball.position.x < 0) {
			ball.position.x = SCREEN_WIDTH / 2;
			ball.position.y = SCREEN_HEIGHT / 2;
			ball.speed.x = BALL_SPEED_X;
			ball.speed.y = BALL_SPEED_Y;
			p2Score++;
		}

		// Stay in bounds for Player 1
		// TODO: Refactor this so we don't have repeat code
		//       Probably some KeepPlayerInBounds() function that takes a player obj
		if (p1.rect.y < 0) p1.rect.y = 0;
		else if (p1.rect.y > SCREEN_HEIGHT - PADDLE_HEIGHT) p1.rect.y = SCREEN_HEIGHT - PADDLE_HEIGHT;

		// Stay in bounds Player 2
		if (p2.rect.y < 0) p2.rect.y = 0;
		else if (p2.rect.y > SCREEN_HEIGHT - PADDLE_HEIGHT) p2.rect.y = SCREEN_HEIGHT - PADDLE_HEIGHT;

		// Draw the things
		DrawBoard();
		PlayerScore(p1Score);
		OpponentScore(p2Score);
		DrawRectangleRec(p1.rect, WHITE);
		DrawRectangleRec(p2.rect, WHITE);
		DrawBall(ball);
		EndDrawing();
	}

	// Cleanup
	UnloadSound(fxOgg);
	UnloadMusicStream(music);
	CloseAudioDevice();
	CloseWindow();

	return 0;
}
