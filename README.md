# Galaga — Programming 4 Exam Project

This project is a recreation of the classic arcade game **Galaga** using a custom-built 2D C++ game engine expanded from the [Minigin](https://github.com/avadae/minigin) template.

## Repository Details
* **Source Control Depot:** [INSERT YOUR GITHUB REPOSITORY LINK HERE]
* **Minigin Engine:** [github/avadae/minigin](https://github.com/avadae/minigin)
* **Live Web Target Demo:** `https://<your-username>.github.io/<your-repository-name>/`

[![Build Status](https://github.com/avadae/minigin/actions/workflows/cmake.yml/badge.svg)](https://github.com/avadae/cmake/actions)
[![Build Status](https://github.com/avadae/minigin/actions/workflows/emscripten.yml/badge.svg)](https://github.com/avadae/emscripten/actions)
[![GitHub Release](https://img.shields.io/github/v/release/avadae/minigin?logo=github&sort=semver)](https://github.com/avadae/minigin/releases/latest)

---

## Game Features
### Gameplay Modes
* **Single Player Mode:** Playable via Keyboard or Gamepad (supporting D-Pad and Joystick inputs) across all navigation contexts, menus, and gameplay.
* **Co-Op Mode:** Cooperative dual-fighter play supporting Keyboard and multiple Gamepads.
* **Versus Mode:** Player 1 controls the starfighter. Player 2 possesses *Boss Galaga*.

### Systems
* **File-Based Level Loading:** Levels (`level1.txt`, `level2.txt`, `level3.txt`) load custom text formation layouts at runtime (e.g., `Z` for Bee/Zako, `G` for Butterfly/Goei, `B` for Boss, `.` for empty).
* **Name Entry & Highscores:** Permanent local highscore tracking. Players utilize an arcade-style initials rotation screen to log scores to a local file.

### Global Control Keybinds
* **[F1] Skip Level:** Instantly clears the remaining on-screen enemies and progresses to the next stage.
* **[F2] Audio Toggle:** A global mute/unmute macro keybind.

---

## Applied Patterns
* **Game Loop & Update Method (`Minigin`):** Passes a calculated `delta_time` through a continuous loop to update all logic, ensuring frame-rate-independent movement.
* **Component Pattern (`GameObject`):** Uses dynamic composition to add behavior to GameObjects (`RenderComponent`, `ColliderComponent`, `ScoreComponent`, etc.) instead of rigid inheritance.
* **State Pattern (`GameStateManager` / `EnemyStateComponent`):** Manages global game cycles (`MainMenuState`, `PlayState`, `HighscoreState`) and individual enemy behaviors (`IdleState`, `DivingState`, `TractorBeamState`).
* **Command Pattern (`InputManager`):** Encapsulates input actions into distinct objects (`MoveCommand`, `ShootCommand`), allowing gameplay and menu navigation actions to work across keyboard and gamepads.
* **Observer Pattern (UI & Scoring):** Allows subjects like player health or enemy destruction to notify registered observers. Features dynamic cast cleanup on destruction to prevent dangling pointers.
* **Object Pool (`BulletSpawner`):** Recycles a pre-allocated pool of bullet objects to avoid constant memory allocation, while cleanly enforcing the 2-missile on-screen arcade limit (for the fighters).
* **Service Locator (Audio Subsystem):** Provides an abstracted interface to play sounds, using a background worker thread to load and play audio asynchronously without freezing the game loop.
* **Singleton Pattern (Systems):** Used by classes (`ResourceManager`, `SceneManager`, `GameStateManager`) that require a single, universal point reference.

---

## Engine Design Choices

* **Global Game State Pattern:** Chose to put the abstract `GameState` and centralized `GameStateManager` inside the core engine library. This keeps the engine loop independent, allowing the main game executable to behave via states (`MainMenuState`, `PlayState`).
* **Observer Pattern Safe Unsubscription:** Chose to keep `Observer` a pure interface with no data members to respect the C++ multiple inheritance rules. The dangling pointer problem was solved by having `Subject::AddObserver` return a custom `Subscription` object; when this token goes out of scope or is destroyed, it automatically unsubscribes the observer safely.

---
