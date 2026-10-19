# R-Type

R-Type is an Epitech project that reimplements the classic side-scrolling shoot 'em up with a custom ECS engine, an authoritative server, and networked multiplayer clients. The game is written in C++17 with raylib.

## Quick start

```sh
cmake -B build && cmake --build build
./r-type_server &
./r-type_client
```

## Supported platforms

| Platform | Server | Client | Status |
|---|---|---|---|
| Linux | yes | yes | tested by the CI (Ubuntu) |
| Windows | yes | yes | handled by CMake, not tested yet |

## Dependencies

Third-party libraries are downloaded and built by CMake through [CPM](https://github.com/cpm-cmake/CPM.cmake), nothing is copied into the repository:

- [raylib](https://github.com/raysan5/raylib) 6.0: window, keyboard and drawing (client)
- [Asio](https://github.com/chriskohlhoff/asio) 1.30.2 (standalone): UDP sockets (client and server)

## Prerequisites

You need build tools: a C++17 compiler, **CMake 3.28 or newer**, and Git (used by CMake to fetch dependencies). On Linux, you also need the OpenGL and X11 development packages (step 4 below).

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

Run all commands from the repository root. The first command configures CMake and downloads raylib and Asio (an internet connection is required the first time). The second compiles the project.

On Linux:

```sh
cmake -B build
cmake --build build
```

Start the server in one terminal, then one client per player (up to 4) in separate terminals:

```sh
./r-type_server
./r-type_client
```

Both executables take an optional IPv4 address and port, `127.0.0.1:4242` by default. With a single argument, it is the port. The client and the server must use the same port:

```sh
./r-type_server [ip] [port]
./r-type_client [ip] [port]

./r-type_server 5000              # listens on 127.0.0.1:5000
./r-type_client 5000              # joins 127.0.0.1:5000
```

By default the server only accepts clients from the same machine. To play over a network, start it on `0.0.0.0` and give the clients the address of the host:

```sh
./r-type_server 0.0.0.0 4242
./r-type_client 192.168.1.20 4242
```

Move with the arrow keys; the ship fires on its own. See the [controls](https://r-type-project-documentation.vercel.app/user/controls).

On Windows, the CMake files handle the platform but the build has not been tested yet. The commands are:

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

The executables are generated at the repository root. Run them from there, so that the textures in `Assets/` are found. Linux and Windows builds use the same protocol, so they are meant to play together once the Windows build is tested.

To run the tests (Linux): `./tests/tests.sh`.

## Documentation

Read the full project documentation on the [R-Type documentation website](https://r-type-project-documentation.vercel.app/).
Players should start with the User documentation for [getting started](https://r-type-project-documentation.vercel.app/user/getting-started),
[controls](https://r-type-project-documentation.vercel.app/user/controls), [gameplay](https://r-type-project-documentation.vercel.app/user/gameplay),
[multiplayer](https://r-type-project-documentation.vercel.app/user/multiplayer), and
[troubleshooting](https://r-type-project-documentation.vercel.app/user/troubleshooting).
Contributors can use the Developer documentation: [architecture](https://r-type-project-documentation.vercel.app/developer/architecture),
[protocol](https://r-type-project-documentation.vercel.app/developer/network-protocol),
[technical study](https://r-type-project-documentation.vercel.app/developer/technical-study) and
[contributing guide](https://r-type-project-documentation.vercel.app/developer/contributing).

## License

This project is licensed under the MIT License, see [LICENSE](LICENSE).

## Authors

| Name | Role | Contact |
|---|---|---|
| Eliott Duchene | Engine, Game | eliott1.duchene@epitech.eu |
| Arthur Vignes | Engine, Game | arthur.vignes@epitech.eu |
| Clovis Nedelec | Network | clovis.nedelec@epitech.eu |
| Arthur Piron | Network | arthur.piron@epitech.eu |

Issues and source code: [GitHub repository](https://github.com/EpitechPGE3-2026/G-ING-500-LIL-5-1-rtype-4).
