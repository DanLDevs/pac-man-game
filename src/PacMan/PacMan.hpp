#pragma once

#include "../../lib/lib.hpp"
#include "../Map/Map.hpp"
#include "../Key/Key.hpp"
#include <iostream>
#include <map>
#include <vector>

class Map;

class PacMan
{
private:
	int _score = 0;
	bool _was_autopilot_enabled = false;
	std::vector<int> _autopilot_path;
	size_t _autopilot_step = 0;

public:
	float x;
	float y;
	float radius = OBJ_SIZE;
	int speed = PACMAN_SPEED;
	Color color;
	std::map<int, int>::iterator pacman_position;
	Sound eating;
	Sound death;

	PacMan();
	PacMan(Color color);

	void setScore(int score);
	void increaseScore(int value);
	int getScore() const;
	void setDefaultPosition(Map &map);

	void draw(const Key &key);
	void drawMouth(const Key &key);
	void updateStartPosition(Map &map);
	void checkScore(Map& map);
	void update(const Map &map, Key &key);
	void updateAutopilot(const Map &map, Key &key);
	bool isAlignedToTileCenter() const;
	std::vector<int> getTargetNodeIds(const Map &map) const;
	int directionFromTo(const GraphNode *from_node, const GraphNode *to_node) const;
	void resetAutopilotPath();
	bool checkBorderCollision(const Map &map, float dx, float dy);
	void movePrimaryKey(const Map &map, Key &key);
	bool moveSecondaryKey(const Map &map, Key &key);
};