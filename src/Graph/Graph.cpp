#include "Graph.hpp"
#include "../../lib/lib.hpp"

namespace
{
	bool isWalkable(char cell)
	{
		return cell != '1';
	}

	int worldToRow(int world_y)
	{
		return (world_y - (MAP_Y_START + BLOCK_SIZE / 2)) / BLOCK_SIZE;
	}

	int worldToCol(int world_x)
	{
		return (world_x - (MAP_X_START + BLOCK_SIZE / 2)) / BLOCK_SIZE;
	}
}

void Graph::buildFromGrid(const std::vector<std::string> &grid)
{
	_nodes.clear();
	_tile_to_node.clear();

	int next_id = 0;
	for (int row = 0; row < (int)grid.size(); row++)
	{
		for (int col = 0; col < (int)grid[row].size(); col++)
		{
			if (!isWalkable(grid[row][col]))
				continue;

			GraphNode node;
			node.id = next_id++;
			node.row = row;
			node.col = col;
			node.world_x = MAP_X_START + col * BLOCK_SIZE + BLOCK_SIZE / 2;
			node.world_y = MAP_Y_START + row * BLOCK_SIZE + BLOCK_SIZE / 2;
			_nodes[node.id] = node;
			_tile_to_node[std::make_pair(row, col)] = node.id;
		}
	}

	const int dr[4] = {-1, 1, 0, 0};
	const int dc[4] = {0, 0, -1, 1};
	for (std::map<std::pair<int, int>, int>::const_iterator it = _tile_to_node.begin(); it != _tile_to_node.end(); ++it)
	{
		int row = it->first.first;
		int col = it->first.second;
		GraphNode &node = _nodes[it->second];

		for (int i = 0; i < 4; i++)
		{
			std::pair<int, int> neighbor_tile = std::make_pair(row + dr[i], col + dc[i]);
			std::map<std::pair<int, int>, int>::const_iterator neighbor_it = _tile_to_node.find(neighbor_tile);
			if (neighbor_it != _tile_to_node.end())
				node.neighbors.push_back(neighbor_it->second);
		}
	}
}

bool Graph::hasNode(int node_id) const
{
	return _nodes.find(node_id) != _nodes.end();
}

const GraphNode *Graph::getNode(int node_id) const
{
	std::map<int, GraphNode>::const_iterator it = _nodes.find(node_id);
	if (it == _nodes.end())
		return NULL;
	return &(it->second);
}

int Graph::getNodeIdByTile(int row, int col) const
{
	std::map<std::pair<int, int>, int>::const_iterator it = _tile_to_node.find(std::make_pair(row, col));
	if (it == _tile_to_node.end())
		return -1;
	return it->second;
}

int Graph::getNodeIdByWorld(int world_x, int world_y) const
{
	return getNodeIdByTile(worldToRow(world_y), worldToCol(world_x));
}

int Graph::findNearestTargetNode(int start_node_id, const std::vector<int> &target_node_ids) const
{
	if (!hasNode(start_node_id) || target_node_ids.empty())
		return -1;

	std::set<int> target_set(target_node_ids.begin(), target_node_ids.end());
	std::queue<int> q;
	std::map<int, int> distance;
	q.push(start_node_id);
	distance[start_node_id] = 0;

	int best_node = -1;
	int best_distance = -1;

	while (!q.empty())
	{
		int current = q.front();
		q.pop();

		if (target_set.find(current) != target_set.end())
		{
			int current_distance = distance[current];
			if (best_node == -1 || current_distance < best_distance || (current_distance == best_distance && current < best_node))
			{
				best_node = current;
				best_distance = current_distance;
			}
		}

		const GraphNode *node = getNode(current);
		if (node == NULL)
			continue;

		for (size_t i = 0; i < node->neighbors.size(); i++)
		{
			int neighbor = node->neighbors[i];
			if (distance.find(neighbor) == distance.end())
			{
				distance[neighbor] = distance[current] + 1;
				q.push(neighbor);
			}
		}
	}

	return best_node;
}

std::vector<int> Graph::findDeterministicDfsPath(int start_node_id, int goal_node_id) const
{
	std::vector<int> empty_path;
	if (!hasNode(start_node_id) || !hasNode(goal_node_id))
		return empty_path;
	if (start_node_id == goal_node_id)
	{
		empty_path.push_back(start_node_id);
		return empty_path;
	}

	// Iterative DFS keeps explicit frontier state and avoids recursion depth limits.
	std::set<int> visited;
	std::stack<int> st;
	std::map<int, int> parent;

	visited.insert(start_node_id);
	st.push(start_node_id);

	while (!st.empty())
	{
		int current = st.top();
		if (current == goal_node_id)
			break;

		const GraphNode *node = getNode(current);
		if (node == NULL)
		{
			st.pop();
			continue;
		}

		bool moved = false;
		for (size_t i = 0; i < node->neighbors.size(); i++)
		{
			int neighbor = node->neighbors[i];
			if (visited.find(neighbor) != visited.end())
				continue;
			visited.insert(neighbor);
			parent[neighbor] = current;
			st.push(neighbor);
			moved = true;
			break;
		}

		if (!moved)
			st.pop();
	}

	if (visited.find(goal_node_id) == visited.end())
		return empty_path;

	std::vector<int> reversed;
	int current = goal_node_id;
	reversed.push_back(current);

	while (current != start_node_id)
	{
		std::map<int, int>::const_iterator parent_it = parent.find(current);
		if (parent_it == parent.end())
			return empty_path;
		current = parent_it->second;
		reversed.push_back(current);
	}

	std::vector<int> path;
	for (std::vector<int>::reverse_iterator it = reversed.rbegin(); it != reversed.rend(); ++it)
		path.push_back(*it);
	return path;
}
