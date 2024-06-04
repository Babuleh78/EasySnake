#include "raylib.h"
#include <iostream>
#include "rlgl.h"
#include <deque>;
#include <raymath.h>
using namespace std;
Color green = { 173, 204, 96, 255 };
Color DarkGreen = { 43, 51, 24, 255 };
int cellSize = 30;
int cellCount = 25;
int offset = 75;
double lastUpdTime = 0;
bool is_body(Vector2 pos, deque<Vector2> snakeBody) {
	for (unsigned int i = 0; i < snakeBody.size(); i++) {
		if (Vector2Equals(snakeBody[i], pos)) {
			return true;
		}
	}
	return false;
}
bool updTriggered(double inter) {
	double currentTime = GetTime();
	if (currentTime - lastUpdTime >= inter) {
		lastUpdTime = currentTime;
		return true;
	}
	return false;
}
class Food {

public:
	Vector2 pos{ 7, 8};
	Texture2D texture;
	Food(deque<Vector2> snakeBody) {
		Image image = LoadImage("image/apple.png"); ImageResize(&image, cellSize, cellSize);
		texture = LoadTextureFromImage(image);
		UnloadImage(image);
		pos = GenerateRandomPos(snakeBody);
	}
	~Food() {
		UnloadTexture(texture);
	}
	void Draw() {
		DrawTexture(texture,offset+ pos.x * cellSize, offset+ pos.y * cellSize, WHITE);
	}
	Vector2 GenerateRandomCell()
	{
		float x = GetRandomValue(0, cellCount - 1);
		float y = GetRandomValue(0, cellCount - 1);
		return Vector2{ x, y };
	}

	Vector2 GenerateRandomPos(deque<Vector2> snakeBody)
	{
		Vector2 position = GenerateRandomCell();
		while (is_body(position, snakeBody))
		{
			position = GenerateRandomCell();
		}
		return position;
	}
};
class Snake {
public:
	bool addSegment;
	deque<Vector2> body = { Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9} };
	Vector2 direction = { 1, 0 };
	void Draw() {
		for (int i = 0; i < body.size(); i++) {
			float x = body[i].x;
			float y = body[i].y;
			Rectangle segment = Rectangle{ offset+ x * cellSize,offset+ y * cellSize, (float)cellSize, (float)cellSize };
			DrawRectangleRounded(segment, 0.6, 6, DarkGreen);
		}
	}
	void Upd() {
		body.push_front(Vector2Add(body[0], direction));
		if(addSegment){
			addSegment = false;
		}
		else {

			body.pop_back();
		}
	}
	void Reset() {
		
		body = { Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9} };
		direction = { 0,1 };
	}
};
class Game {
public:
	Snake snake = Snake();
	Food food = Food(snake.body);
	bool running = true;
	int score = 0;
	void Draw() {
		food.Draw();
		snake.Draw();
	}
	void Update() {
		if (running) {
			snake.Upd();
			eat_check();
			coll_edge();
			coll_tail();
		}
	}
	void eat_check() {
		if (Vector2Equals(snake.body[0], food.pos)) {
			food.pos = food.GenerateRandomPos(snake.body);
			snake.addSegment = true;
			score += 1;
		}
	}
	void GameOver() {
		snake.Reset();
		food.pos = food.GenerateRandomPos(snake.body);
		running = false;
		score = 0;
	}
	void coll_edge() {
		if (snake.body[0].x == cellCount || snake.body[0].x == -1 || snake.body[0].y == -1 || snake.body[0].y == cellCount) {
			GameOver();
		}		
	}
	void coll_tail() {
		deque<Vector2> headlessBody = snake.body;
		headlessBody.pop_front();
		if (is_body(snake.body[0], headlessBody)) {
			GameOver();
		}
	}
};
int main() {
	SetTraceLogLevel(LOG_NONE);
	InitWindow(2*offset + cellSize * cellCount, 2*offset + cellSize * cellCount, "Snake");
	SetTargetFPS(60);
	


	Game game = Game();
	while (!WindowShouldClose()) {
		BeginDrawing();

		if (updTriggered(0.2)) {
			game.Update();
		}
		ClearBackground(green);
		game.Draw();
		DrawRectangleLinesEx(Rectangle{(float)offset- 5,(float) offset -5,(float) cellSize*cellCount+10, (float)cellSize*cellCount+10}, 5, DarkGreen);
		DrawText(TextFormat("%i", game.score), offset - 5, offset + cellSize * cellCount + 10, 40, DarkGreen);
		if (IsKeyPressed(KEY_UP) && game.snake.direction.y != 1) {
			game.snake.direction = { 0, -1 };
			game.running = true;
		}
		else if (IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1) {
			game.snake.direction = { 0, 1 };
			game.running = true;
		}
		else if (IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1) {
			game.snake.direction = { -1, 0};
			game.running = true;
		}
		else if (IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1) {
			game.snake.direction = { 1, 0 };
			game.running = true;
		}

		EndDrawing();
	}





	CloseWindow();
	return 0;
}
