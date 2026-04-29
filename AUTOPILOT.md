# Autopilot and DFS feature

This document describes what was added to support **graph-based maze navigation** and an **autopilot mode** that combines **BFS** (nearest pellet selection) with **deterministic DFS** (path to that pellet).

## Purpose

- The maze is modeled as an **undirected graph**: each walkable tile is a **node**; edges connect **orthogonal** neighbors (Up, Down, Left, Right). Walls are excluded (no nodes, no edges).
- **Autopilot** picks the **nearest remaining pellet** by shortest path length on the graph (ties broken by **lowest graph node ID**), then computes a **deterministic DFS path** from Pac-Man’s current node to that pellet’s node.
- The player toggles autopilot with **`A`**. Arrow keys still handle manual movement when autopilot is off.

## New files

| File | Role |
|------|------|
| [src/Graph/Graph.hpp](src/Graph/Graph.hpp) | `GraphNode` (unique `id`, grid `row`/`col`, tile-center `world_x`/`world_y`, ordered `neighbors`). `Graph` holds nodes and lookup by tile/world. |
| [src/Graph/Graph.cpp](src/Graph/Graph.cpp) | Builds the graph from the parsed map grid, connects neighbors in fixed **Up → Down → Left → Right** order, implements `findNearestTargetNode` (BFS) and `findDeterministicDfsPath` (iterative DFS using `std::stack` and a visited set). |

## Modified files

| File | Changes |
|------|---------|
| [src/Map/Map.hpp](src/Map/Map.hpp), [src/Map/Map.cpp](src/Map/Map.cpp) | Stores each map line in `_grid`, builds `_graph` once in the `Map` constructor (topology is static). Adds `getGraph()`, `getGrid()`, `const` overload of `getTargets()`, and helpers to convert between world coordinates and tile indices. |
| [src/Key/Key.hpp](src/Key/Key.hpp), [src/Key/Key.cpp](src/Key/Key.cpp) | Adds `_autopilot_enabled` and `isAutopilotEnabled()`. **`KEY_A`** toggles autopilot on each press. Only arrow keys update movement `currentKey` / `queueKey` so `A` never becomes a movement direction. |
| [src/PacMan/PacMan.hpp](src/PacMan/PacMan.hpp), [src/PacMan/PacMan.cpp](src/PacMan/PacMan.cpp) | `update()` branches: autopilot calls `updateAutopilot()`, else manual `moveSecondaryKey` / `movePrimaryKey`. Autopilot keeps a **cached path** (`_autopilot_path`, `_autopilot_step`), rebuilds when the path is empty, finished, or blocked, syncs step index when position matches a node on the path, uses **rounded** coordinates for stable node lookup, and **deduplicates** pellet node IDs when collecting targets. Score, pellet clearing, win/lose, and ghost collisions stay in existing `checkScore` / `RunGame` flow. |
| [Makefile](Makefile) | Adds `src/Graph/Graph.cpp` to `SRC` so the graph module links with the rest of the project. |

## How tile coordinates map to a unique node ID

During `Graph::buildFromGrid`, the code scans the grid **row by row, column by column**. Each **walkable** cell (`character != '1'`) receives the next sequential integer ID (`0`, `1`, `2`, …). A map `_tile_to_node` stores `(row, col) → id`. Walls are skipped, so IDs are **dense over walkable tiles only**, not full rectangular indices.

Lookup is by tile `(row, col)` or by converting world pixel coordinates to tile indices first (`getNodeIdByWorld`).

## Deterministic DFS

- **Neighbor order** is fixed when edges are built: Up, Down, Left, Right (`dr`/`dc` arrays in `Graph.cpp`).
- **DFS** expands neighbors in that stored order and takes the **first unvisited** neighbor (iterative stack implementation). That makes the search order predictable for the same graph and endpoints.

## Why BFS appears before DFS

**BFS** (`findNearestTargetNode`) finds which pellet node minimizes **graph distance** from Pac-Man’s current node (with the tie-break above). **DFS** is then used only to produce a **single deterministic path** from start to that chosen goal. Rubric separation: shortest-distance goal choice vs. DFS path exploration.

## Autopilot runtime flow

1. If autopilot is **off**, behavior matches the original arrow-key movement.
2. If autopilot is **on**, Pac-Man resolves its **current graph node**, ensures a valid **path** exists (rebuild if needed), reads the **next hop** along the path, sets the corresponding direction, and moves using the same collision checks as manual mode.
3. Press **`A`** again to disable autopilot and clear the cached path state.

## Building and running

General build/run instructions for all platforms are in [README.md](README.md). On Windows with MSYS2/MinGW, the README includes copying runtime DLLs next to `pac-man.exe` if needed.
