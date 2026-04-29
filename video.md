# Presentation script: maze graph, DFS, and autopilot

Read this aloud or loosely follow it while demoing. **Cue** lines tell you which file to open on screen. Code blocks use **line ranges** so your IDE jump-to-line matches.

**Quick tip:** In VS Code / Cursor, `Ctrl+G` (or `Cmd+G` on Mac) and type the line number to jump.

---

## Opening (15 seconds)

**SAY:**  
“This is a Pac-Man clone in C++ using Raylib. For my assignment I added a **graph** over the maze, **depth-first search** for paths, and an **autopilot** mode toggled with A. I’ll walk through where that lives in the code.”

**SHOW:** Project tree — highlight `src/Graph/`, `src/PacMan/PacMan.cpp`, `src/Key/Key.cpp`.

---

## Part A — Maze to graph (node IDs and edges)

### A1 — Where the graph is built

**SAY:**  
“The map file is read line by line into a grid of strings. When the `Map` finishes loading, it calls `buildFromGrid` once so the maze topology is fixed for the whole game.”

**SHOW:** [src/Map/Map.cpp](src/Map/Map.cpp) — scroll to where `_grid` is filled and `_graph.buildFromGrid` runs.

```22:46:src/Map/Map.cpp
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
```

### A2 — Walkable vs walls

**SAY:**  
“A cell becomes a graph node only if it’s walkable. Walls are character `'1'`. Anything else—pellets, spawns, empty corridors—is treated as walkable for the graph.”

**SHOW:** [src/Graph/Graph.cpp](src/Graph/Graph.cpp) — `isWalkable` and the inner loop that skips walls and assigns IDs.

```6:43:src/Graph/Graph.cpp
	bool isWalkable(char cell)
	{
		return cell != '1';
	}
	// ...
			if (!isWalkable(grid[row][col]))
				continue;

			GraphNode node;
			node.id = next_id++;
			node.row = row;
			node.col = col;
			// ...
			_tile_to_node[std::make_pair(row, col)] = node.id;
```

### A3 — Unique node IDs (not a simple row-major formula)

**SAY:**  
“Node IDs are **0, 1, 2, …** in **row-major scan order**, but **only for walkable tiles**. So it’s not `row times width plus column` for the full rectangle—it’s dense IDs over walkable cells only. The map `_tile_to_node` stores `(row, col)` to `id`.”

**SHOW:** Same file, lines 27–42 — emphasize `next_id++` and `_tile_to_node`.

### A4 — Lookup from world (pixel) coordinates

**SAY:**  
“During gameplay we convert Pac-Man’s pixel position to a tile row and column, then look up the node ID. If there’s no node—invalid tile—we get minus one.”

**SHOW:** [src/Graph/Graph.cpp](src/Graph/Graph.cpp) — `worldToRow` / `worldToCol` and `getNodeIdByTile` / `getNodeIdByWorld`.

```11:88:src/Graph/Graph.cpp
	int worldToRow(int world_y)
	{
		return (world_y - (MAP_Y_START + BLOCK_SIZE / 2)) / BLOCK_SIZE;
	}

	int worldToCol(int world_x)
	{
		return (world_x - (MAP_X_START + BLOCK_SIZE / 2)) / BLOCK_SIZE;
	}
// ...
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
```

### A5 — Edges between adjacent walkable cells

**SAY:**  
“After every node exists, we connect **orthogonal** neighbors—Up, Down, Left, Right—using fixed `dr` and `dc` arrays. If the neighbor tile is also a node, we push that neighbor’s **ID** into `neighbors`. Walls never appear as nodes, so there’s no edge through a wall.”

**SHOW:** [src/Graph/Graph.cpp](src/Graph/Graph.cpp) — second loop in `buildFromGrid`.

```46:60:src/Graph/Graph.cpp
	const int dr[4] = {-1, 1, 0, 0};
	const int dc[4] = {0, 0, -1, 1};
	for (std::map<std::pair<int, int>, int>::const_iterator it = _tile_to_node.begin(); it != _tile_to_node.end(); ++it)
	{
		// ...
		for (int i = 0; i < 4; i++)
		{
			std::pair<int, int> neighbor_tile = std::make_pair(row + dr[i], col + dc[i]);
			std::map<std::pair<int, int>, int>::const_iterator neighbor_it = _tile_to_node.find(neighbor_tile);
			if (neighbor_it != _tile_to_node.end())
				node.neighbors.push_back(neighbor_it->second);
		}
	}
```

**SAY (optional table):**  
“So: `'1'` — no node; anything else in the file that isn’t a wall — node if it’s on the grid; edges only between walkable neighbors.”

---

## Part B — DFS implementation

### B1 — Nearest pellet first (BFS), then path (DFS)

**SAY:**  
“For the assignment I need a **deterministic DFS** to a goal. I first pick **which pellet** to aim at: I use **BFS** on the graph so the goal is the **nearest** pellet by graph distance, with a tie-break toward **smaller node ID**. That’s `findNearestTargetNode`.”

**SHOW:** [src/Graph/Graph.cpp](src/Graph/Graph.cpp) — `findNearestTargetNode` (queue, distances, best candidate).

```90:134:src/Graph/Graph.cpp
int Graph::findNearestTargetNode(int start_node_id, const std::vector<int> &target_node_ids) const
{
	// ...
	std::queue<int> q;
	// ...
		if (target_set.find(current) != target_set.end())
		{
			int current_distance = distance[current];
			if (best_node == -1 || current_distance < best_distance || (current_distance == best_distance && current < best_node))
			{
				best_node = current;
				best_distance = current_distance;
			}
		}
	// ... expand neighbors ...
}
```

### B2 — DFS: stack, not recursion

**SAY:**  
“The actual path uses **`findDeterministicDfsPath`**. It’s **iterative DFS**: a `std::stack`, a `std::set` for visited, and a parent map to rebuild the path. The comment says we avoid deep recursion and stack overflow on large mazes.”

**SHOW:** [src/Graph/Graph.cpp](src/Graph/Graph.cpp) — `findDeterministicDfsPath`.

```137:184:src/Graph/Graph.cpp
std::vector<int> Graph::findDeterministicDfsPath(int start_node_id, int goal_node_id) const
{
	// ...
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
		// ...
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
```

### B3 — Visited and deterministic order

**SAY:**  
“Neighbors were stored in **Up, Down, Left, Right** order when we built the graph. DFS walks `neighbors` in that order and takes the **first unvisited** neighbor, then breaks—so the expansion order is **deterministic**. Visited nodes aren’t pushed again.”

**SHOW:** Same function — the `for` loop with `break`; tie back to `dr`/`dc` in `buildFromGrid` if needed.

### B4 — When DFS stops

**SAY:**  
“If start equals goal, return a one-node path.If we pop the stack until empty and the goal was never visited, return an empty path—no route under this search. Otherwise we rebuild from `goal` using `parent` back to `start`, then reverse.”

**SHOW:** Lines 139–146 (trivial path), 186–205 (reconstruct path).

```139:146:src/Graph/Graph.cpp
	if (start_node_id == goal_node_id)
	{
		empty_path.push_back(start_node_id);
		return empty_path;
	}
```

```186:205:src/Graph/Graph.cpp
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
```

---

## Part C — Autopilot integration

### C1 — Toggle with A

**SAY:**  
“Autopilot is toggled with the **A** key. It flips a boolean and returns immediately so A never gets queued as a movement direction. Only arrow keys update movement state.”

**SHOW:** [src/Key/Key.cpp](src/Key/Key.cpp) — `Key::update`.

```38:56:src/Key/Key.cpp
void Key::update()
{
	int key = GetKeyPressed();

	setKeyPressed(key);

	if (key == KEY_A)
	{
		_autopilot_enabled = !_autopilot_enabled;
		return ;
	}

	if (key != KEY_UP && key != KEY_DOWN && key != KEY_LEFT && key != KEY_RIGHT)
		return ;

	if (!_current_key)
		setCurrentKey(key);
	else
		setQueueKey(key);
}
```

### C2 — Pac-Man switches between manual and autopilot

**SAY:**  
“In `PacMan::update`, if autopilot is on we call `updateAutopilot`. If we turn autopilot off, we clear any cached path so manual arrows work normally again.”

**SHOW:** [src/PacMan/PacMan.cpp](src/PacMan/PacMan.cpp) — `update`.

```92:109:src/PacMan/PacMan.cpp
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
```

### C3 — How the path is built and followed

**SAY:**  
“When we need a new route, `rebuildAutopilotPath` collects target node IDs from remaining pellets, runs **BFS** to pick the nearest goal, then runs **DFS** to get the path. At tile centers we read the **next node** on the path, convert that to a direction, optionally check walls, set the key direction, and call `movePrimaryKey` like normal movement.”

**SHOW:** `rebuildAutopilotPath` and the core of `updateAutopilot`.

```209:227:src/PacMan/PacMan.cpp
bool PacMan::rebuildAutopilotPath(const Map &map, const Graph &graph, int start_node_id)
{
	std::vector<int> target_node_ids = getTargetNodeIds(map);
	// ...
	int goal_node_id = graph.findNearestTargetNode(start_node_id, target_node_ids);
	// ...
	_autopilot_path = graph.findDeterministicDfsPath(start_node_id, goal_node_id);
	_autopilot_step = 0;
	return _autopilot_path.size() >= 2;
}
```

```111:199:src/PacMan/PacMan.cpp
void PacMan::updateAutopilot(const Map &map, Key &key)
{
	if (!isAlignedToTileCenter())
	{
		movePrimaryKey(map, key);
		return ;
	}
	// ... sync path step to current node ...
	if (_autopilot_path.empty() || _autopilot_step + 1 >= _autopilot_path.size())
	{
		if (!rebuildAutopilotPath(map, graph, start_node_id))
			return ;
	}

	const GraphNode *current_node = graph.getNode(start_node_id);
	const GraphNode *next_node = graph.getNode(_autopilot_path[_autopilot_step + 1]);
	int direction = directionFromTo(current_node, next_node);
	// ... collision precheck, then ...
	key.setCurrentKey(direction);
	key.setQueueKey(0);
	movePrimaryKey(map, key);
}
```

**SAY:**  
“Between tile centers we keep moving with the current direction so motion stays smooth—only at aligned centers do we replan or advance logic.”

**SHOW (optional):** `getCurrentNodeId` — rounded coordinates for stable lookup.

```202:207:src/PacMan/PacMan.cpp
int PacMan::getCurrentNodeId(const Graph &graph) const
{
	int snapped_x = (int)std::lround(x);
	int snapped_y = (int)std::lround(y);
	return graph.getNodeIdByWorld(snapped_x, snapped_y);
}
```

---

## Closing (10 seconds)

**SAY:**  
“Graph lives in `Graph.cpp`, the map wires it in `Map.cpp`, autopilot behavior is in `PacMan.cpp`, and the A key is in `Key.cpp`. More detail is in `AUTOPILOT.md` and the overall architecture in `PROJECT_GUIDE.md`.”

**SHOW:** Optionally scroll [AUTOPILOT.md](AUTOPILOT.md) or README for build instructions.

---

## On-screen cheat sheet (leave this slide up if allowed)

| Topic | File | Lines to jump |
|-------|------|----------------|
| Walkable / IDs / edges | `src/Graph/Graph.cpp` | 6–9, 22–61 |
| World → tile → node ID | `src/Graph/Graph.cpp` | 11–19, 77–87 |
| BFS nearest pellet | `src/Graph/Graph.cpp` | 90–134 |
| DFS path | `src/Graph/Graph.cpp` | 137–205 |
| Map builds graph | `src/Map/Map.cpp` | 24, 45–46 |
| A toggle | `src/Key/Key.cpp` | 38–56 |
| Autopilot update | `src/PacMan/PacMan.cpp` | 92–109, 111–199, 209–227 |
