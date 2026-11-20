# Sonar-Based 3D Platformer

First-person 3D platformer in C++ and OpenGL 3.3 Core. The world is dark: jumps, landings and footsteps send sonar waves that briefly reveal edges of platforms, spikes, keys and doors. Collect all keys and reach the door to go to the next level.

**Course:** Computer Graphics Fundamentals, Astana IT University
**Team:** Piriev Erkhan, Botabaeva Saltanat (SE-2413)
**Instructor:** Sabina Saleshova

## Requirements

- OS: Windows 10 / 11, 64-bit
- IDE / compiler: Visual Studio 2022 (MSVC, C++17) with the "Desktop development with C++" workload
- GPU with OpenGL 3.3 support
- Libraries: GLFW, GLAD, GLM, nlohmann/json, Dear ImGui (HUD), miniaudio (sound)

## Build

1. Clone the repository: `git clone https://github.com/Saltanat4/FINAL_CGF.git`
2. Open `FINAL_CGF.sln` in Visual Studio.
3. Select configuration `Debug` or `Release` and platform `x64`.
4. Build: `Build -> Rebuild Solution`.

All third-party libraries are in the repository (`libraries/`, NuGet packages are restored automatically). A post-build step copies the `models`, `levels`, `shaders` and `sounds` folders next to the .exe.

## Run

- From Visual Studio: press `F5` or `Ctrl+F5`.
- From the output folder: run `FINAL_CGF.exe` (the asset folders must be next to it).

The game starts in fullscreen.

## Controls

| Key | Action |
|---|---|
| W / A / S / D | Move |
| Mouse | Look around |
| Space | Jump |
| T | Sonar ON / OFF (OFF = normal lit scene) |
| Arrow keys | Rotate the light (visible when sonar is OFF) |
| P | Pause (stops physics, animation and sonar time) |
| F1 | Show / hide help |
| F2 | Free camera (Space = up, Ctrl = down, Shift = faster) |
| F11 | Fullscreen / windowed |
| Esc | Exit |

## Gameplay

- Collect all keys on the level, then touch the door to load the next level.
- Spikes and falling off the map restart the current level.
- The HUD shows collected keys, FPS, sonar state and pause / free-camera state.

## Project structure

- `FINAL_CGF/` - source code (`Main.cpp`, rendering, physics, level loader, HUD, audio)
- `FINAL_CGF/shaders/` - GLSL shaders (`sonar.vert`, `sonar.frag`)
- `FINAL_CGF/levels/` - levels in JSON (`level1.json` ... `level4.json`)
- `FINAL_CGF/models/` - SMF models (spike, key, door)
- `FINAL_CGF/sounds/` - music and sound effects

## Levels

Levels are plain JSON files (`levels/levelN.json`) with a player spawn and a list of objects (`block`, `spikes`, `key`, `door`). Objects can have `Rotator` and `Mover` behaviors. The game loads `level1.json`, `level2.json`, ... in order.

## Credits

- Libraries: GLFW, GLAD, GLM, nlohmann/json, Dear ImGui, miniaudio
- Sounds and music: see the sources and licenses listed in the project report
