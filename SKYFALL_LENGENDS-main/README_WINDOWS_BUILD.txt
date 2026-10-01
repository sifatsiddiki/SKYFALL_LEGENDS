Skyfall Legends - Windows 10 x64 Build & Installer Guide
=======================================================

This project is ready to be built on Windows 10 x64 with SFML 2.x.

1. Build the game (x64)
-----------------------

Option A: Visual Studio (recommended)
-------------------------------------

1. Install:
   - Visual Studio 2022 (Desktop development with C++)
   - SFML 2.x for Visual C++ (64-bit)

2. Create a new "Empty Project" (x64) and add all .cpp files from `src\`.
3. Set the project's working directory to the SkyfallLegends_CPP folder.
4. Configure:
   - C/C++ -> Additional Include Directories:
     - <path-to-SFML>\include
   - Linker -> General -> Additional Library Directories:
     - <path-to-SFML>\lib
   - Linker -> Input -> Additional Dependencies:
     - sfml-graphics.lib
     - sfml-window.lib
     - sfml-system.lib
     - sfml-audio.lib

5. Build the project in Release x64 configuration.
6. Copy the built `SkyfallLegends.exe` into:
   - `build\SkyfallLegends.exe` (relative to this project folder)
   Create the `build` folder if it does not exist.

7. Copy the required SFML DLLs next to the exe in `build\`, for example:
   - sfml-graphics-2.dll
   - sfml-window-2.dll
   - sfml-system-2.dll
   - sfml-audio-2.dll

Option B: MinGW-w64 + CMake
---------------------------

1. Install:
   - CMake
   - MinGW-w64 (64-bit)
   - SFML 2.x (precompiled for MinGW or build from source)

2. Open "x64 Native Tools Command Prompt" or MinGW shell and run:

   mkdir build
   cd build
   cmake .. -G "MinGW Makefiles"
   cmake --build . --config Release

3. After build, ensure `SkyfallLegends.exe` is present in `build\`.

2. Create the single-file installer (Inno Setup)
-----------------------------------------------

1. Install Inno Setup (https://jrsoftware.org/isinfo.php) on Windows.

2. Open this script in Inno Setup:

   `installer\skyfall_legends_windows_installer.iss`

3. Make sure the paths in [Files] section match:
   - Main game exe:
     Source: "..\build\SkyfallLegends.exe"
   - Assets folder:
     Source: "..\assets\*"

4. Click "Build" in Inno Setup.
   This will generate `SkyfallLegends_Setup.exe` in the project folder.

This .exe is your single-file Windows 10 x64 installer for final submission.
