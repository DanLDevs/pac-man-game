#pragma once

#include <map>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <utility>
#include <vector>

struct GraphNode
{
	int id;
	int row;
	int col;
	int world_x;
	int world_y;
	std::vector<int> neighbors;
};

class Graph
{
private:
	std::map<int, GraphNode> _nodes;
	std::map<std::pair<int, int>, int> _tile_to_node;

public:
	void buildFromGrid(const std::vector<std::string> &grid);
	bool hasNode(int node_id) const;
	const GraphNode *getNode(int node_id) const;
	int getNodeIdByTile(int row, int col) const;
	int getNodeIdByWorld(int world_x, int world_y) const;
	int findNearestTargetNode(int start_node_id, const std::vector<int> &target_node_ids) const;
	std::vector<int> findDeterministicDfsPath(int start_node_id, int goal_node_id) const;
};
