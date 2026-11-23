[readme.md](https://github.com/user-attachments/files/35248700/readme.md)
# Riichi Mahjong Hand-Value Calculator (Stage II Prototype)

A C++ console application designed to validate winning Riichi Mahjong hands, calculate *han*, *fu*, and final rounded point values, and manage saved hand records across isolated client profiles.

## Features

- **Hand Validation & Scoring (`HandCalculator`)**:
  - Parses 14-tile hand notations (e.g., `123m 456m 789p 234s 55p`).
  - Validates tile counts, standard 4-meld + 1-pair structures, *Chiitoitsu* (Seven Pairs), and *Kokushi Musou* (Thirteen Orphans).
  - Evaluates *yaku* conditions (*Riichi*, *Pinfu*, *Chiitoitsu*, *Kokushi Musou*, *Dora*) and blocks *yaku*-less wins.
  - Calculates *han*, *fu*, and final point payouts rounded up to the nearest 100.
- **Multi-Client Record Storage (`ClientStore`)**:
  - Supports multiple client profiles (e.g., `vilnius_club`, `kaunas_academy`).
  - Enforces client data isolation so users can only view and modify records belonging to their active client profile.
- **Automated Verification**:
  - Includes a built-in test suite covering normal scoring, boundary cases (*Chiitoitsu*), invalid tile counts, and client access isolation.

## Project Structure

```text
├── kursinisjong.cpp      # Entry point, CLI menu, user interaction, and automated test runner
├── HandCalculator.h      # Hand evaluation interface and data structures
├── HandCalculator.cpp    # Tile parsing, structure validation, yaku/fu/point calculation
├── ClientStore.h         # Multi-client storage interface and record models
└── ClientStore.cpp       # In-memory client data management and access control
```

## Prerequisites & Build Setup

- **Compiler**: C++17 compatible compiler (tested with MSVC / Visual Studio 2022 on Windows).
- **Encoding Configuration (Windows / MSVC)**:
  - Source files are encoded in **UTF-8 (with signature)**.
  - Add `/utf-8` to **Project Properties → C/C++ → Command Line → Additional Options** to ensure proper Lithuanian character rendering in the Windows console.

### Building & Running via Command Line (MSVC Developer Command Prompt)

```bash
cl /std:c++17 /EHsc /utf-8 /Fe:kursinisjong.exe kursinisjong.cpp HandCalculator.cpp ClientStore.cpp
kursinisjong.exe
```

*(Alternatively, open the `.sln` solution in Visual Studio, select **Debug** mode, and press **Ctrl + F5**.)*

## Sample Data & Demo Walkthrough

The application initializes with preloaded sample records for `vilnius_club` and `kaunas_academy`.

1. **Calculate a Standard Hand**:
   - Select option `1` (`Skaičiuoti rankos vertę`).
   - Enter tiles: `123m 456m 789p 234s 55p`
   - Win type: `r` (Ron), Riichi: `t` (Yes), Dealer: `n` (No), Dora: `0`.
   - **Expected Output**: `Han: 2, Fu: 30, Taškai: 2000`.
   - Save the hand to the active client (`vilnius_club`).
2. **Inspect & Update Client Records**:
   - Select option `2` to view saved hands for the active client.
   - Select option `3` to rename a record by its ID.
3. **Verify Client Isolation**:
   - Select option `4` to switch the active client to `kaunas_academy`.
   - Confirm that `vilnius_club` records are separate and cannot be edited from another client profile.

## Running Automated Tests

1. Build the project in **Debug** mode (required so `assert()` checks remain active).
2. Run the application and select menu option `5` (`Paleisti automatinius testus`).
3. The suite verifies:
   - Standard *Riichi + Pinfu* scoring (`2000` points).
   - *Chiitoitsu* boundary scoring (`2 han`, `25 fu`, `1600` points).
   - Error handling for invalid 15-tile hands.
   - Cross-client modification prevention.
4. Upon passing all checks, the console outputs: `Visi 4 automatiniai testai pavyko!`
