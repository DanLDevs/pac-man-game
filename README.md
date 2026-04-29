# **Pac-Man Game**

Forked from [maricard18/pac-man-game](https://github.com/maricard18/pac-man-game).

This project is a replica of the classic Pac-Man Game, originally developed in 1980. <br>
It has been implemented in C++ using the [raylib](https://www.raylib.com/) graphical library. <br>

## 🕹️ **Usage**

### Mac / Linux
To play this game, run:

```shell
make run
```

This builds the `pac-man` executable and launches it.

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

Notes:
- If you use `mingw32-make clean` on Windows and get `-f was unexpected at this time.`, skip it and use the `Remove-Item` command shown above.
- If the game does not launch and shows a missing DLL error, ensure the listed DLL files are in the same folder as `pac-man.exe`.

## 🎥 **Demo**

<p align="center">
  <img src="./assets/demo.gif" width="100%">
</p>

## 🧬 **Cloning**

To successfully clone this repository, use this command

```shell
git clone https://github.com/maricard18/pac-man-game.git
```

## About
All the graphical components in this game were created using functions from the raylib library, no images or textures were utilized. <br> The CPU movement pattern was implemented using a self-made random algorithm system. <br> The primary movement is always upwards unless it encounters a dead end. <br> When passing through an intersection or facing a possible new direction, it has a 60% chance of continuing forward and a 40% chance of turning left or right.

## Contributing

Contributions are welcome! Feel free to submit bug reports, feature requests, or pull requests through GitHub.

---
> Feel free to ask me any questions through Slack (**maricard**). <br>
> GitHub [@maricard18](https://github.com/maricard18) &nbsp;&middot;&nbsp;
> Linkedin [Mario Henriques](https://www.linkedin.com/in/mario18) &nbsp;&middot;&nbsp;
> [42 Porto](https://www.42porto.com/en)
