#include "Map.hpp"

Map::~Map()
{
	UnloadSound(this->intro);
	UnloadSound(this->eating);
	UnloadSound(this->death);
	UnloadSound(this->game_over);
	UnloadSound(this->game_won);
}

Map::Map(std::string map_path)
{
	std::fstream file;
	std::string line;
	float x = _x;
	float y = _y;

	file.open(map_path);
	if (file.is_open())
	{
		while (getline(file, line))
		{
			_grid.push_back(line);
			for (int i = 0; i < line.length(); i++)
			{
				if (line[i] == '1')
					_borders.insert(std::pair<int, int>(x, y));
				else if (line[i] == '0')
					_targets.insert(std::pair<int, int>(x + BLOCK_SIZE / 2 - TARGETS_SIZE / 2, y + BLOCK_SIZE / 2 - TARGETS_SIZE / 2));
				else if (line[i] == 'C')
					_cpu_pos.insert(std::pair<int, int>(x + BLOCK_SIZE / 2, y + BLOCK_SIZE / 2));
				else if (line[i] == 'P')
					_pacman_pos[x + BLOCK_SIZE / 2] =  y + BLOCK_SIZE / 2;

				x += BLOCK_SIZE;
			}

			y += BLOCK_SIZE;
			x = MAP_X_START;
		}

		file.close();
	}
	// Maze topology is static, so we build the graph once and reuse it every frame.
	_graph.buildFromGrid(_grid);

	_lifes.insert(std::pair<int, int>(20  + BLOCK_SIZE / 2, 504 + BLOCK_SIZE / 2));
	_lifes.insert(std::pair<int, int>(50  + BLOCK_SIZE / 2, 504 + BLOCK_SIZE / 2));
	_lifes.insert(std::pair<int, int>(80  + BLOCK_SIZE / 2, 504 + BLOCK_SIZE / 2));

	_pacman_pos_it =  _pacman_pos.begin();
	_cpu_pos_it = _cpu_pos.begin();

	this->intro = LoadSound("./sounds/intro.wav");
	this->eating = LoadSound("./sounds/eating.wav");
	this->death = LoadSound("./sounds/death.wav");
	this->game_over = LoadSound("./sounds/game_over.mp3");
	this->game_won = LoadSound("./sounds/game_won.mp3");
}

std::multimap<int, int> Map::getBorders() const
{
	return _borders;
}

std::multimap<int, int>& Map::getTargets()
{
	return _targets;
}

const std::multimap<int, int>& Map::getTargets() const
{
	return _targets;
}

std::multimap<int, int> Map::getCpuList()
{
	return _cpu_pos;
}

std::map<int, int> Map::getPacmanList()
{
	return _pacman_pos;
}

std::multimap<int, int>& Map::getLifes()
{
	return _lifes;
}

const std::vector<std::string> &Map::getGrid() const
{
	return _grid;
}

const Graph &Map::getGraph() const
{
	return _graph;
}

std::multimap<int, int>::iterator Map::getLastLifePos()
{
	std::multimap<int, int>::iterator it = _lifes.begin();

	for (; it != _lifes.end(); it++)
	{
		if (std::next(it) == _lifes.end())
			return it;
	}

	return it;
}

std::map<int, int>::iterator Map::getPacmanPosition()
{
    return _pacman_pos_it;
}

std::multimap<int, int>::iterator Map::getCpuPosition()
{
	std::multimap<int, int>::iterator current_it;

	if (_cpu_pos_it != _cpu_pos.end())
    {
        current_it = _cpu_pos_it;
        _cpu_pos_it++;
    }
    else
    {
        _cpu_pos_it = _cpu_pos.begin();
        current_it = _cpu_pos_it;
        _cpu_pos_it++;
    }

    return current_it;
}

Rectangle Map::getRec(float x, float y) const
{
	return Rectangle{x, y, BLOCK_SIZE, BLOCK_SIZE};
}

int Map::getLifesNumber()
{
	return lifes;
}

int Map::worldToTileRow(int world_y) const
{
	return (world_y - (MAP_Y_START + BLOCK_SIZE / 2)) / BLOCK_SIZE;
}

int Map::worldToTileCol(int world_x) const
{
	return (world_x - (MAP_X_START + BLOCK_SIZE / 2)) / BLOCK_SIZE;
}

void Map::draw()
{
	std::multimap<int, int>::iterator borders_it;
	std::multimap<int, int>::iterator targets_it;
	std::multimap<int, int>::iterator lifes_it;

	for (borders_it = _borders.begin(); borders_it != _borders.end(); borders_it++)
		DrawRectangle(borders_it->first, borders_it->second, BLOCK_SIZE, BLOCK_SIZE, DARKBLUE);

	for (targets_it = _targets.begin(); targets_it != _targets.end(); targets_it++)
		DrawRectangle(targets_it->first, targets_it->second, TARGETS_SIZE, TARGETS_SIZE, WHITE);

	for (lifes_it = _lifes.begin(); lifes_it != _lifes.end(); lifes_it++)
	{
		DrawCircle(lifes_it->first, lifes_it->second, OBJ_SIZE, YELLOW);
		DrawLeftMouth(lifes_it->first, lifes_it->second);
	}
}

void Map::decreaseLifes()
{
	this->lifes -= 1;
}

void Map::startPacmanDeadTimer()
{
	StartTimer(this->pacman_dead_timer, 4);
}

void Map::startGameOverTimer()
{
	StartTimer(this->game_over_timer, 5);
}

void Map::startGameWonTimer()
{
	StartTimer(this->game_won_timer, 5);
}