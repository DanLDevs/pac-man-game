#pragma once

#include "../../lib/lib.hpp"
#include "../Graph/Graph.hpp"
#include <iostream>
#include <vector>
#include <fstream>
#include <map>

class Map
{
private:
	std::multimap<int, int> _lifes;
	std::multimap<int, int> _borders;
	std::multimap<int, int> _targets;
	std::multimap<int, int> _cpu_pos;
	std::map<int, int> _pacman_pos;
	std::vector<std::string> _grid;
	Graph _graph;
	std::multimap<int, int>::iterator _cpu_pos_it;
	std::map<int, int>::iterator _pacman_pos_it;
	float _x = MAP_X_START;
	float _y = MAP_Y_START;
	int lifes = 3;

public:
	~Map();
	Map(std::string map_path);
	Timer map_init_timer;
	Timer pacman_dead_timer;
	Timer game_over_timer;
	Timer game_won_timer;
	Sound intro;
	Sound eating;
	Sound death;
	Sound game_over;
	Sound game_won;
	bool game_pause = false;

	std::multimap<int, int> getBorders() const;
	std::multimap<int, int>& getTargets();
	const std::multimap<int, int>& getTargets() const;
	std::multimap<int, int> getCpuList();
	std::map<int, int> getPacmanList();
	std::map<int, int>::iterator getPacmanPosition();
	std::multimap<int, int>::iterator getCpuPosition();
	std::multimap<int, int>& getLifes();
	const std::vector<std::string> &getGrid() const;
	const Graph &getGraph() const;
	std::multimap<int, int>::iterator getLastLifePos();
	Rectangle getRec(float x, float y) const;
	int getLifesNumber();
	int worldToTileRow(int world_y) const;
	int worldToTileCol(int world_x) const;
	
	void draw();
	void decreaseLifes();
	void startPacmanDeadTimer();
	void startGameOverTimer();
	void startGameWonTimer();
};