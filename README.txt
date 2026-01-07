# Real Kendo Ippon Match

A high-stakes 1v1 C fighting game using Raylib.

## Compilation Instructions
To compile the game, navigate to this directory in your terminal and run:

    make

This will generate an executable named `kendo_game`.
To run the game:

    ./kendo_game

## Controls

### Player 1 (Blue)
- **Move**: Arrow Keys (Left/Right)
- **Dash**: Enter
- **Attack Men (Head)**: Z
- **Attack Kote (Wrist)**: X
- **Attack Do (Torso)**: C
- **Block**: Down Arrow

### Player 2 (Red)
- **Move**: A / D
- **Dash**: Left Shift
- **Attack Men (Head)**: R
- **Attack Kote (Wrist)**: T
- **Attack Do (Torso)**: Y
- **Block**: S

## Game Mechanics & Appeal Points

### 1. Finite State Machine (FSM)
The player logic is built on a robust FSM handling states like `IDLE`, `MOVING`, `DASHING`, `ATTACKING`, `BLOCKING`, and `RECOILING`. This ensures actions are mutually exclusive (e.g., you cannot move while attacking) and allows for precise frame-based logic.

### 2. Precision Hitboxes
Unlike simple distance checks, this game uses specific `Rectangle` hitboxes for weapons and bodies.
- **Weapon Hitboxes**: Only active during specific frames of the attack animation (the "active frames").
- **Directionality**: Hitboxes adjust based on which way the player is facing.

### 3. Stamina System
A stamina bar prevents button mashing.
- **Dash**: Consumes Stamina.
- **Attack**: Consumes Stamina.
- **Block**: Consumes Stamina over time.
- **Regeneration**: Only occurs when Idle or Moving (not blocking).

### 4. Collision Resolution
- **Hit Detection**: Checks if an active weapon hitbox overlaps an opponent's body hitbox.
- **Blocking**: If the opponent is in `STATE_BLOCKING`, the attacker is recoiled instead of scoring a hit.
- **Pushing**: Players cannot walk through each other; they push each other via simple AABB collision resolution.
