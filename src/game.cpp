#include "main.hpp"

int RunGame(Map &map, Key &key, PacMan &pacman, Cpu &cpu1, Cpu &cpu2, Cpu &cpu3, Cpu &cpu4)
{
	// Orchestration layer: this function controls frame order while individual classes
	// encapsulate their own game rules (Map state, PacMan logic, and Cpu behavior).
	ClearBackground(BLACK);
	map.draw();
	key.update();

	int game_status = GameStatus(map, key, pacman, cpu1, cpu2, cpu3, cpu4);
	if (game_status == 0 || game_status == -1)
		return game_status;

	pacman.update(map, key);
	cpu1.random(map);
	cpu2.random(map);
	cpu3.random(map);
	cpu4.random(map);

	pacman.checkScore(map);

	cpu1.checkCollisionPacmanCpu(map, pacman);
	cpu2.checkCollisionPacmanCpu(map, pacman);
	cpu3.checkCollisionPacmanCpu(map, pacman);
	cpu4.checkCollisionPacmanCpu(map, pacman);

	DrawPlayers(pacman, key, cpu1, cpu2, cpu3, cpu4);

	return 1;
}

int GameStatus(Map &map, const Key &key, PacMan &pacman, Cpu &cpu1, Cpu &cpu2, Cpu &cpu3, Cpu &cpu4)
{
	if (!TimerDone(map.map_init_timer))
	{
		DrawPlayers(pacman, key, cpu1, cpu2, cpu3, cpu4);
		DrawText("READY?", 235, 315, 20, YELLOW);
		return 0;
	}

	if (GetElapsed(map.map_init_timer) > 5.0f && GetElapsed(map.map_init_timer) < 5.015f )
	{
		StartCpuTimers(cpu1, cpu2, cpu3, cpu4);
		return 1;
	}

	if (map.game_pause == true)
	{
		if (!TimerDone(map.pacman_dead_timer))
		{
			UpdatePlayerPositions(map, key, pacman, cpu1, cpu2, cpu3, cpu4);
			DrawText("READY?", 235, 315, 20, YELLOW);
			return 0;
		}
		else
		{
			map.game_pause = false;
			StartCpuTimers(cpu1, cpu2, cpu3, cpu4);
			return 1;
		}
	}

	if (map.getTargets().size() == 0)
	{
		if (!TimerDone(map.game_won_timer))
		{
			DrawPlayers(pacman, key, cpu1, cpu2, cpu3, cpu4);
			DrawText("GAME WON", 222, 315, 20, YELLOW);
			return 0;
		}
		else
			return -1;
	}

	if (map.getLifes().size() == 0)
	{
		if (!TimerDone(map.game_over_timer))
		{
			DrawPlayers(pacman, key, cpu1, cpu2, cpu3, cpu4);
			DrawText("GAME OVER", 215, 315, 20, RED);
			return 0;
		}
		else
			return -1;
	}

	return 1;
}