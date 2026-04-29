#include "Cpu.hpp"

Cpu::Cpu(Color color, int timer)
{
	this->color = color;
	this->timer= timer;
}

void Cpu::draw()
{
	DrawCircle(x, y, radius, color);
	DrawRectangle(x - radius, y, radius * 2, radius, color);

	DrawPixel(x - radius + 1, y + radius - 1, BLACK);
	DrawPixel(x - radius + 2, y + radius - 1, BLACK);
	DrawPixel(x - radius + 3, y + radius - 1, BLACK);
	DrawPixel(x - radius + 2, y + radius - 2, BLACK);

	DrawPixel(x - radius + 6, y + radius - 1, BLACK);
	DrawPixel(x - radius + 7, y + radius - 1, BLACK);
	DrawPixel(x - radius + 8, y + radius - 1, BLACK);
	DrawPixel(x - radius + 7, y + radius - 2, BLACK);

	DrawPixel(x - radius + 11, y + radius - 1, BLACK);
	DrawPixel(x - radius + 12, y + radius - 1, BLACK);
	DrawPixel(x - radius + 13, y + radius - 1, BLACK);
	DrawPixel(x - radius + 12, y + radius - 2, BLACK);

	DrawPixel(x - radius + 16, y + radius - 1, BLACK);
	DrawPixel(x - radius + 17, y + radius - 1, BLACK);
	DrawPixel(x - radius + 18, y + radius - 1, BLACK);
	DrawPixel(x - radius + 17, y + radius - 2, BLACK);

	DrawEllipse(x - 4, y - 2, 3.5, 4, WHITE);
	DrawEllipse(x + 4, y - 2, 3.5, 4, WHITE);

	if (current_direction == KEY_UP)
	{
		DrawCircle(x - 4, y - 4, 1.75, BLACK);
		DrawCircle(x + 4, y - 4, 1.75, BLACK);
	}
	else if (current_direction == KEY_DOWN)
	{
		DrawCircle(x - 4, y - 0, 1.75, BLACK);
		DrawCircle(x + 4, y - 0, 1.75, BLACK);
	}
	else if (current_direction == KEY_LEFT)
	{
		DrawCircle(x - 6, y - 2, 1.75, BLACK);
		DrawCircle(x + 2, y - 2, 1.75, BLACK);
	}
	else if (current_direction == KEY_RIGHT)
	{
		DrawCircle(x - 2, y - 2, 1.75, BLACK);
		DrawCircle(x + 6, y - 2, 1.75, BLACK);
	}
}

void Cpu::updateStartPosition(Map &map)
{
	this->x = cpu_position->first;
	this->y = cpu_position->second;
}

void Cpu::random(const Map &map)
{
	// Trade-off: lightweight randomized direction changes are easy to tune and fast each frame,
	// but they are intentionally less predictable and less optimal than pathfinding ghosts.
	if (!TimerDone(this->cpu_timer))
		return ;

	int *options = testOtherDirections(map);

	if (options != NULL)
	{
		moveNewDirection(map, options);
		delete[] options;
	}
	else
		moveInCurrentDirection(map);
}

int *Cpu::testOtherDirections(const Map &map)
{
	int options = howManyOptions(map);

	if (!options)
		return NULL;

	int *options_array = new int[options];
	int i = 0;

	if (current_direction != KEY_DOWN && !checkBorderCollision(map, 0, -speed))
		options_array[i++] = KEY_UP;
	if (current_direction != KEY_UP && !checkBorderCollision(map, 0, speed))
		options_array[i++] = KEY_DOWN;
	if (current_direction != KEY_RIGHT && !checkBorderCollision(map, -speed, 0))
		options_array[i++] = KEY_LEFT;
	if (current_direction != KEY_LEFT && !checkBorderCollision(map, speed, 0))
		options_array[i++] = KEY_RIGHT;

	return options_array;
}

int Cpu::howManyOptions(const Map &map)
{
	int counter = 0;

	if (current_direction != KEY_DOWN && !checkBorderCollision(map, 0, -speed))
		counter++;
	if (current_direction != KEY_UP && !checkBorderCollision(map, 0, speed))
		counter++;
	if (current_direction != KEY_RIGHT && !checkBorderCollision(map, -speed, 0))
		counter++;
	if (current_direction != KEY_LEFT && !checkBorderCollision(map, speed, 0))
		counter++;

	return counter;
}

void Cpu::moveInCurrentDirection(const Map &map)
{
	int dx = 0;
	int dy = 0;

	switch (current_direction)
	{
		case KEY_UP:
			dy = -speed;
			break;
		case KEY_DOWN:
			dy = speed;
			break;
		case KEY_LEFT:
			dx = -speed;
			break;
		case KEY_RIGHT:
			dx = speed;
			break;
	}

	if (!checkBorderCollision(map, dx, dy))
	{
		x += dx;
		y += dy;
	}
	else
	{
		x -= dx;
		y -= dy;
		oppositeDirection();
	}
}

void Cpu::moveNewDirection(const Map &map, int *options)
{
	int new_direction = options[GetRandomValue(0, howManyOptions(map) - 1)];

	current_direction = new_direction;
	moveInCurrentDirection(map);
}

void Cpu::oppositeDirection()
{
	if (current_direction == KEY_UP)
		current_direction = KEY_DOWN;
	else if (current_direction == KEY_DOWN)
		current_direction = KEY_UP;
	else if (current_direction == KEY_LEFT)
		current_direction = KEY_RIGHT;
	else if (current_direction == KEY_RIGHT)
		current_direction = KEY_LEFT;
}

void Cpu::checkCollisionPacmanCpu(Map &map, PacMan &pacman)
{
	Rectangle pacman_rec = {pacman.x, pacman.y, OBJ_SIZE, OBJ_SIZE};
	Rectangle cpu_rec = {this->x, this->y, OBJ_SIZE, OBJ_SIZE};

	if (CheckCollisionRecs(pacman_rec, cpu_rec))
	{
		std::multimap<int, int> &lifes = map.getLifes();
		std::multimap<int, int>::iterator it = map.getLastLifePos();
		lifes.erase(it);
		map.decreaseLifes();

		if (map.getLifes().size() == 0)
		{
			map.startGameOverTimer();
			PlaySound(map.game_over);
		}
		else
		{
			map.startPacmanDeadTimer();
			map.game_pause = true;
			PlaySound(map.death);
		}
	}
}

void Cpu::startTimer()
{
	StartTimer(this->cpu_timer, this->timer);
}

void Cpu::setTimer(int timer)
{
	this->timer = timer;
}

void Cpu::setDefaultPosition(Map &map)
{
	this->cpu_position = map.getCpuPosition();
}