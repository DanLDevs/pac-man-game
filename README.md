# Pac-Man Game

Forked from [maricard18/pac-man-game](https://github.com/maricard18/pac-man-game).

This project recreates classic Pac-Man gameplay in C++ using raylib for graphics and audio.

## Build And Run

### Prerequisites

- A C++ toolchain and `make`
- raylib runtime support

### Mac / Linux

From the project root:

```shell
make run
```

This builds `pac-man` and starts the game.

### Windows (MSYS2 + MinGW-w64)

From the project root in PowerShell:

```powershell
Remove-Item .\pac-man.exe -ErrorAction SilentlyContinue
mingw32-make
Copy-Item .\lib\raylib.dll .
Copy-Item C:\msys64\mingw64\bin\libstdc++-6.dll .
Copy-Item C:\msys64\mingw64\bin\libgcc_s_seh-1.dll .
Copy-Item C:\msys64\mingw64\bin\libwinpthread-1.dll .
.\pac-man.exe
```

If launch fails with missing DLL errors, verify those DLL files are in the same folder as `pac-man.exe`.

## Controls

- Arrow keys: move Pac-Man
- `A`: toggle autoplay input mode (if enabled in your current branch)

## Clean Class Structure

- `Map`: owns static world data (walls, targets, spawn points, life icons) and sound assets.
- `PacMan`: owns player movement, score updates, and input-driven movement decisions.
- `Cpu`: owns ghost movement behavior and Pac-Man collision checks.
- `game.cpp`: orchestrates the frame loop and coordinates class interactions.
- `utils.cpp`: shared helper routines for player initialization, drawing, and timers.

## Design Trade-Offs

- `Map` centralizes level state and assets to simplify runtime flow, but becomes a larger owner class.
- `PacMan` uses current-direction plus queued-direction input for smoother turns, at the cost of extra input state.
- `Cpu` uses a randomized movement policy to keep per-frame logic lightweight, accepting less predictable ghost routing than full pathfinding.
- `RunGame` keeps strict per-frame ordering (draw, input, status, update, collision) for stability and debuggability over aggressive micro-optimization.

## DFS Design Summary

Current gameplay uses randomized ghost movement, but the project design supports a DFS-based extension for deterministic navigation:

1. Convert walkable tiles into graph nodes.
2. Connect each node to walkable neighbors (up, down, left, right).
3. Run DFS from the active node toward a selected goal node.
4. Reconstruct and follow the discovered route segment by segment.

Why DFS: simple to implement and deterministic with fixed neighbor order; trade-off is that DFS does not guarantee the shortest path.

## Library Choices

- [raylib](https://www.raylib.com/): chosen for straightforward 2D rendering, input, and audio APIs.
- C++ STL (`std::map`, `std::multimap`, iterators): used for coordinate-indexed game data and lightweight state containers.

## Demo

<p align="center">
  <img src="./assets/demo.gif" width="100%">
</p>

## Attributions

- Upstream fork and base implementation: [maricard18/pac-man-game](https://github.com/maricard18/pac-man-game)
- Original Pac-Man concept: Namco (1980)
- Rendering/audio framework: [raylib](https://www.raylib.com/)

## Contributing

Contributions are welcome through bug reports, feature requests, and pull requests.
