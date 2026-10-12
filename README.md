# R-Type

R-Type is an Epitech project that reimplements the classic side-scrolling shoot 'em up with a custom ECS engine, an authoritative server, and networked multiplayer clients. The game is written in C++17 with raylib.

## Prerequisites

You only need build tools: a C++17 compiler, **CMake 3.28 or newer**, and Git (used by CMake to fetch dependencies).

### Linux (Ubuntu / Debian)

1. Install the compiler and Git:

```sh
sudo apt update
sudo apt install build-essential git
```

2. Install CMake:

```sh
sudo apt install cmake
```

3. Check the version:

```sh
cmake --version
```

The output must say 3.28 or higher. Ubuntu 24.04 ships CMake 3.28 in its default repositories. On older releases (such as Ubuntu 22.04) the version is too old, so install a recent one with snap instead:

```sh
sudo snap install cmake --classic
```

4. Install the low-level platform libraries (OpenGL and X11 headers), which are required to open a window and are not handled by the package manager:

```sh
sudo apt install libgl1-mesa-dev libx11-dev libxrandr-dev libxi-dev libxcursor-dev libxinerama-dev
```

### Windows

Windows needs no extra libraries, because OpenGL is already part of the system.

1. Install a C++ compiler: download the [Visual Studio Build Tools](https://visualstudio.microsoft.com/downloads/), run the installer and tick the **Desktop development with C++** workload.

2. Install Git from [git-scm.com](https://git-scm.com/download/win), keeping the default options.

3. Install CMake from [cmake.org/download](https://cmake.org/download/) using the Windows `.msi` installer. During installation, choose **Add CMake to the system PATH**, otherwise the `cmake` command will not be found.

   If you have `winget`, you can install both tools from PowerShell instead:

```powershell
winget install Kitware.CMake
winget install Git.Git
```

4. Close and reopen PowerShell, then check the versions:

```powershell
cmake --version
git --version
```

CMake must report 3.28 or higher.

## Launch

Run all commands from the repository root. The first command configures CMake and downloads raylib (an internet connection is required the first time). The second compiles the project.

On Linux:

```sh
cmake -B build
cmake --build build
```

Start the server in one terminal:

```sh
./r-type_server
```

Then start one client per player in separate terminals:

```sh
./r-type_client
```

On Windows, run:

```powershell
cmake -B build
cmake --build build
```

Start the server in one PowerShell window:

```powershell
.\r-type_server.exe
```

Then start one client per player in separate PowerShell windows:

```powershell
.\r-type_client.exe
```

The executables are generated at the repository root because of the current CMake configuration. Clients on Linux and Windows can play together on the same server.

## Documentation

Read the full project documentation on the [R-Type documentation website](https://r-type-project-documentation.vercel.app/), especially if you are a collaborator.

## Authors

- Eliott Duchene
- Arthur Piron
- Arthur Vignes
- Clovis Nedelec