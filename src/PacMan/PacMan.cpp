#include "PacMan.hpp"

PacMan::PacMan()
{
}

PacMan::PacMan(Color color)
{
	this->color = color;
}

void PacMan::setScore(int score)
{
	this->_score = score;
}

void PacMan::increaseScore(int value)
{
	this->_score += value;
}

int PacMan::getScore() const
{
	return _score;
}

void PacMan::setDefaultPosition(Map &map)
{
	this->pacman_position = map.getPacmanPosition();
}

void PacMan::draw(const Key &key)
{
	DrawCircle(x, y, radius, color);
	drawMouth(key);
}

void PacMan::drawMouth(const Key &key)
{
	switch (key.getCurrentKey())
	{
		case KEY_UP:
			DrawUpMouth(this->x, this->y);
			break;
		case KEY_DOWN:
			DrawDownMouth(this->x, this->y);
			break;
		case KEY_LEFT:
			DrawLeftMouth(this->x, this->y);
			break;
		case KEY_RIGHT:
			DrawRightMouth(this->x, this->y);
			break;
	}
}

void PacMan::updateStartPosition(Map &map)
{
	this->x = pacman_position->first;
	this->y = pacman_position->second;
}

void PacMan::checkScore(Map &map)
{
	std::multimap<int, int> &targets = map.getTargets();
	std::multimap<int, int>::iterator it = targets.begin();
	Rectangle pacman = {this->x - BLOCK_SIZE / 2, this->y - BLOCK_SIZE / 2, BLOCK_SIZE, BLOCK_SIZE};

	for (; it != targets.end();)
	{
		if (CheckCollisionRecs(pacman, Rectangle{(float)it->first, (float)it->second, TARGETS_SIZE, TARGETS_SIZE}))
		{
			it = targets.erase(it);
			increaseScore(10);
			PlaySound(map.eating);
		}
		else
			it++;
	}

	DrawText(TextFormat("SCORE %i", getScore()), 24, 24, 20, WHITE);

	if (map.getTargets().size() == 0)
	{
		map.startGameWonTimer();
		PlaySound(map.game_won);
	}
}

void PacMan::update(const Map &map, Key &key)
{
	if (key.isAutopilotEnabled())
	{
		updateAutopilot(map, key);
		_was_autopilot_enabled = true;
		return ;
	}

	if (_was_autopilot_enabled)
	{
		resetAutopilotPath();
		_was_autopilot_enabled = false;
	}

	if (!moveSecondaryKey(map, key))
		movePrimaryKey(map, key);
}

void PacMan::updateAutopilot(const Map &map, Key &key)
{
	if (!isAlignedToTileCenter())
	{
		movePrimaryKey(map, key);
		return ;
	}

	const Graph &graph = map.getGraph();
	int start_node_id = graph.getNodeIdByWorld((int)x, (int)y);
	if (start_node_id == -1)
	{
		resetAutopilotPath();
		return ;
	}

	std::vector<int> target_node_ids = getTargetNodeIds(map);
	if (target_node_ids.empty())
	{
		resetAutopilotPath();
		return ;
	}

	int goal_node_id = graph.findNearestTargetNode(start_node_id, target_node_ids);
	if (goal_node_id == -1)
	{
		resetAutopilotPath();
		return ;
	}

	// Recompute each tile-center step so route reacts to consumed pellets and remains deterministic.
	_autopilot_path = graph.findDeterministicDfsPath(start_node_id, goal_node_id);
	_autopilot_step = 0;
	if (_autopilot_path.size() < 2)
		return ;

	const GraphNode *current_node = graph.getNode(_autopilot_path[0]);
	const GraphNode *next_node = graph.getNode(_autopilot_path[1]);
	int direction = directionFromTo(current_node, next_node);
	if (direction == 0)
		return ;

	key.setCurrentKey(direction);
	key.setQueueKey(0);
	movePrimaryKey(map, key);
}

bool PacMan::isAlignedToTileCenter() const
{
	int offset_x = ((int)x - ((int)MAP_X_START + BLOCK_SIZE / 2));
	int offset_y = ((int)y - ((int)MAP_Y_START + BLOCK_SIZE / 2));
	return (offset_x % BLOCK_SIZE == 0 && offset_y % BLOCK_SIZE == 0);
}

std::vector<int> PacMan::getTargetNodeIds(const Map &map) const
{
	std::vector<int> target_node_ids;
	const Graph &graph = map.getGraph();
	std::multimap<int, int> targets = map.getTargets();

	for (std::multimap<int, int>::const_iterator it = targets.begin(); it != targets.end(); ++it)
	{
		int world_x = it->first + TARGETS_SIZE / 2;
		int world_y = it->second + TARGETS_SIZE / 2;
		int node_id = graph.getNodeIdByWorld(world_x, world_y);
		if (node_id != -1)
			target_node_ids.push_back(node_id);
	}

	return target_node_ids;
}

int PacMan::directionFromTo(const GraphNode *from_node, const GraphNode *to_node) const
{
	if (from_node == NULL || to_node == NULL)
		return 0;

	if (to_node->row == from_node->row - 1 && to_node->col == from_node->col)
		return KEY_UP;
	if (to_node->row == from_node->row + 1 && to_node->col == from_node->col)
		return KEY_DOWN;
	if (to_node->row == from_node->row && to_node->col == from_node->col - 1)
		return KEY_LEFT;
	if (to_node->row == from_node->row && to_node->col == from_node->col + 1)
		return KEY_RIGHT;
	return 0;
}

void PacMan::resetAutopilotPath()
{
	_autopilot_path.clear();
	_autopilot_step = 0;
}

bool PacMan::checkBorderCollision(const Map &map, float dx, float dy)
{
	auto borders = map.getBorders();
	std::multimap<int, int>::const_iterator it = borders.begin();
	Rectangle pacman = {x + dx - BLOCK_SIZE / 2, y + dy - BLOCK_SIZE / 2, BLOCK_SIZE, BLOCK_SIZE};

	for (; it != borders.end(); it++)
	{
		if (CheckCollisionRecs(pacman, map.getRec(it->first, it->second)))
			return true;
	}

	return false;
}

void PacMan::movePrimaryKey(const Map &map, Key &key)
{
	float dx = 0;
	float dy = 0;

	switch (key.getCurrentKey())
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
		key.setCurrentKey(0);
		key.setQueueKey(0);
	}
}

bool PacMan::moveSecondaryKey(const Map &map, Key &key)
{
	if (key.getQueueKey() == key.getCurrentKey() || key.getQueueKey() == 0)
		return false;

	float dx = 0;
	float dy = 0;

	switch (key.getQueueKey())
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
		key.setCurrentKey(key.getQueueKey());
		key.setQueueKey(0);
		return true;
	}

	return false;
}