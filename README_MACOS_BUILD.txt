README_MACOS_BUILD.txt
=========================

Building & Running Skyfall Legends on macOS
-------------------------------------------

Tested on macOS (including MacBook Air) with Homebrew and CMake.

1. Install dependencies (Homebrew recommended):

   /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
   brew update
   brew install cmake sfml

2. Configure the CMake build:

   cd SkyfallLegends_CPP
   mkdir -p build
   cd build
   cmake ..

   If CMake cannot find SFML automatically, set SFML_DIR manually, for example:

   cmake -DSFML_DIR=/opt/homebrew/Cellar/sfml/2.5.1/lib/cmake/SFML ..

3. Build the game:

   cmake --build .

4. Run the game:

   ./SkyfallLegends

Notes
-----
- The project uses cross‑platform C++17 and SFML only; no Windows‑specific APIs are used.
- All assets are included inside the `assets/` folder next to the executable.
- On Apple Silicon machines, Homebrew usually installs to `/opt/homebrew`.
