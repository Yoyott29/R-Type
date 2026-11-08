<p align="center">
  <img src="Documentation/R-Type/src/lib/assets/Favicon.png" alt="R-Type logo" width="180">
  &nbsp;&nbsp;&nbsp;
  <img src="Gringo%20Engine/src/editor/Assets/gringo_logo.png" alt="Gringo Engine logo" width="180">
</p>

<h1 align="center">R-Type</h1>

<p align="center">
  A networked multiplayer shoot 'em up built with the Gringo ECS engine.
</p>

<p align="center">
  <a href="https://github.com/Yoyott29/R-Type/actions/workflows/tests.yml">
    <img src="https://github.com/Yoyott29/R-Type/actions/workflows/tests.yml/badge.svg" alt="Tests">
  </a>
  <a href="LICENSE">
    <img src="https://img.shields.io/badge/license-MIT-blue.svg" alt="MIT License">
  </a>
  <a href="https://r-type-project-documentation.vercel.app/">
    <img src="https://img.shields.io/badge/docs-online-4c8bf5.svg" alt="Documentation">
  </a>
</p>

## Presentation

### What is R-Type?

R-Type is a free and open-source reimplementation of the classic
side-scrolling shoot 'em up, developed as an Epitech project. The repository
contains three projects:

- **R-Type**: the multiplayer game. An authoritative server runs the game and
  up to four clients display it and send their inputs.
- **Gringo Engine**: a modular C++ ECS engine with an editor, runtime plugins,
  renderers, networking, physics, audio, and C++ gameplay scripts. R-Type is
  built on it, and the engine knows nothing about R-Type.
- **TrackNTrap**: a second game made with the same engine, to show that it is
  reusable: a 3D multiplayer race between rats.

The server owns the game state and communicates with up to four clients over
UDP. The client renders the game through the engine's raylib renderer plugin.

### Features

- Authoritative multiplayer gameplay for up to four players, with a lobby.
- UDP networking with configurable host and port.
- Entity Component System architecture.
- Runtime-loadable engine plugins and renderers.
- Gringo editor for creating engine projects and building their executables.
- 2D and 3D engine components, physics, audio, input, UI, and C++ gameplay
  scripts.
- Color-blind filters (protanopia, deuteranopia, tritanopia).
- Automated tests and continuous integration.

### Design goals

- **Modular**: use independent plugins for engine capabilities.
- **Data-oriented**: keep gameplay organised around ECS components and systems.
- **Extensible**: support new games, components, renderers, and network
  protocols without changing the engine core.
- **Playable**: provide a complete multiplayer R-Type experience, not only an
  engine prototype.

### Project status

This project is actively developed. Linux is the primary development
environment. Continuous integration builds and tests the project on both Linux
and Windows (MSVC), with the commands described below.

Supported platforms:

| Platform | Server | Client |
| --- | :---: | :---: |
| Linux | Yes | Yes |
| Windows | Yes | Yes |
| macOS | No | No |

## Dependencies

The project downloads its C++ dependencies with
[CPM.cmake](https://github.com/cpm-cmake/CPM.cmake) during configuration:

- [raylib](https://github.com/raysan5/raylib) 6.0 — client rendering.
- [Asio](https://github.com/chriskohlhoff/asio) 1.30.2 — standalone UDP
  networking.
- [miniaudio](https://github.com/mackron/miniaudio) 0.11.22 — audio.
- [nlohmann/json](https://github.com/nlohmann/json) 3.11.3 — JSON
  serialisation.

The engine also uses the platform thread and dynamic-loading libraries
provided by the operating system.

## Prerequisites

You need:

- A C++20 compiler.
- CMake 3.28 or newer.
- Git.
- An internet connection for the first CMake configuration, so CPM can fetch
  dependencies.

### WINDOWS

#### 1. Install the C++ toolchain

Download and start the
[Visual Studio Build Tools installer](https://visualstudio.microsoft.com/downloads/).
In the **Workloads** tab, select:

- **Desktop development with C++**

In the installation details on the right, make sure these components are
selected:

- **MSVC C++ build tools** (the latest available version, such as MSVC v143).
- **Windows 10 or Windows 11 SDK**.
- **C++ CMake tools for Windows**.
- **Git for Windows** (optional if Git is installed separately).

Click **Install** and restart Windows if the installer requests it. The full
Visual Studio IDE is not required; the Build Tools package is enough.

#### 2. Install Git and CMake

Install [Git for Windows](https://git-scm.com/download/win) and
[CMake](https://cmake.org/download/) if they were not installed with the
Build Tools. When installing CMake, enable **Add CMake to the system PATH**.

With `winget`, Git and CMake can be installed from PowerShell:

```powershell
winget install Kitware.CMake
winget install Git.Git
```

#### 3. Use a developer terminal

The MSVC compiler is not available in a regular PowerShell window. Open one of
the following from the Start menu, and run every build command of this README
in it:

- **Developer PowerShell for VS 2022**
- **x64 Native Tools Command Prompt for VS 2022**

These terminals configure `cl.exe`, the Windows SDK, and the linker
automatically. If you installed a different Visual Studio version, use the
matching terminal name.

Verify the installation:

```powershell
cl
cmake --version
git --version
```

The `cl` command should print the Microsoft C/C++ compiler version, and CMake
must report version 3.28 or newer. If `cl` is not recognised, close the
terminal and reopen the Visual Studio developer terminal rather than using a
regular PowerShell window.

The `.sh` scripts of this repository (`build_rtype.sh`,
`Gringo Engine/build_engine.sh`, `tests/*.sh`) are Bash scripts: they do not
run in PowerShell. The WINDOWS sections below give the `cmake` commands to use
instead.

### LINUX

Install the compiler, build tools, Git, CMake, and the libraries required by
raylib. On Ubuntu/Debian:

```sh
sudo apt update
sudo apt install build-essential cmake git \
  libgl1-mesa-dev libx11-dev libxrandr-dev libxi-dev \
  libxcursor-dev libxinerama-dev
```

Check the CMake version:

```sh
cmake --version
```

It must report version 3.28 or newer. On distributions with an older CMake,
install a current version from the
[official CMake downloads](https://cmake.org/download/) or with Snap:

```sh
sudo snap install cmake --classic
```

## Launch

Run the commands below from the repository root.

### Build R-Type

The first build downloads the dependencies listed above.

#### WINDOWS

From a **Developer PowerShell for VS 2022**:

```powershell
cmake -S . -B build -A x64
cmake --build build --config Release --target r-type_client r-type_server --parallel
```

This creates `r-type_server.exe` and `r-type_client.exe` in the repository
root, with `gringo_engine.dll` next to them. The engine plugins and the
renderer are `.dll` files in `Gringo Engine\plugins\` and
`Gringo Engine\renderers\`.

#### LINUX

The project-provided script configures the root CMake project and builds both
executables in parallel:

```sh
./build_rtype.sh
```

This creates `r-type_server` and `r-type_client` in the repository root. The
engine library, `libgringo_engine.so`, and the plugins (`.so` files) are
written in `Gringo Engine/`.

### Start a game

Start the server first, then one client per player, each in its own terminal.
A client tries to connect for about 5 seconds, then quits if no server answers.

The game finds its files from the location of the executable, so you can start
it from any folder.

Both programs use `127.0.0.1:4242` by default, so the server only accepts
clients from the same machine. Give a single argument to change the port, or an
IPv4 address and a port. The client and the server must use the same port. To
play over a network, start the server on `0.0.0.0` (every network interface)
and give the clients the IP address of the machine running the server
(`192.168.1.20` below is an example).

#### WINDOWS

These run from any PowerShell window:

```powershell
.\r-type_server.exe
.\r-type_client.exe
```

Change the port, or play over a network:

```powershell
.\r-type_server.exe 5000                 # listens on 127.0.0.1:5000
.\r-type_client.exe 5000                 # joins 127.0.0.1:5000

.\r-type_server.exe 0.0.0.0 4242         # on the host
.\r-type_client.exe 192.168.1.20 4242    # on each player's machine
```

When Windows Firewall asks the first time the server starts, allow it on
private networks. `ipconfig` shows the host's address (**IPv4 Address**).

#### LINUX

```sh
./r-type_server
./r-type_client
```

Change the port, or play over a network:

```sh
./r-type_server 5000                 # listens on 127.0.0.1:5000
./r-type_client 5000                 # joins 127.0.0.1:5000

./r-type_server 0.0.0.0 4242         # on the host
./r-type_client 192.168.1.20 4242    # on each player's machine
```

`hostname -I` shows the host's address.

How to play (lobby, controls, gameplay) is explained in the
[user documentation](https://r-type-project-documentation.vercel.app/user/getting-started).

### Run tests

The project test suite (`tests/tests.sh`) builds the project, then starts the
real server and client to check their arguments, logs, connection and
shutdown. The engine unit tests (scheduler, thread pool, network protocol...)
are built separately with CTest.

#### WINDOWS

`tests/tests.sh` is Linux only. Run the engine unit tests from a
**Developer PowerShell for VS 2022**:

```powershell
cmake -S "Gringo Engine" -B build-unit -A x64 -DGRINGO_BUILD_TESTS=ON
cmake --build build-unit --config Release --target engine_tests network_tests --parallel
ctest --test-dir build-unit -C Release --output-on-failure
```

#### LINUX

```sh
./tests/tests.sh
```

Engine unit tests:

```sh
cmake -S "Gringo Engine" -B build-unit -DGRINGO_BUILD_TESTS=ON
cmake --build build-unit --target engine_tests network_tests -j
ctest --test-dir build-unit --output-on-failure
```

## Gringo Engine and editor

The editor (`Gringo`) lets you create or open a project (a `.gringo` file), add
entities and prefabs, set their components, choose the project's plugins, and
build the project. It is built with the engine library and its plugins next to
it, in `Gringo Engine/`.

The editor can also build a project from the command line
(`Gringo --build <file.gringo>`). It generates the project's `client_main.cpp`
and `server_main.cpp`, then compiles them. Run it after changing
`R-Type/R-Type.gringo` or adding a script, then rebuild R-Type.

### WINDOWS

`build_engine.sh` does not run in PowerShell. From a
**Developer PowerShell for VS 2022**, build the editor with:

```powershell
cmake -S "Gringo Engine" -B "Gringo Engine\build" -A x64
cmake --build "Gringo Engine\build" --config Release --target Gringo --parallel
```

This creates `Gringo Engine\Gringo.exe`, with `gringo_engine.dll` and the
plugin `.dll` files next to it. Start it from its folder:

```powershell
cd "Gringo Engine"
.\Gringo.exe
```

Building a project, with the **Build** button or from the command line, calls
CMake and MSVC: start the editor from the developer terminal. From the
repository root:

```powershell
& ".\Gringo Engine\Gringo.exe" --build R-Type\R-Type.gringo
```

The `&` is needed in PowerShell because the path holds a space. The game is
built in Release, and `gringo_engine.dll` is copied next to its `.exe`.

### LINUX

Build the editor:

```sh
./Gringo\ Engine/build_engine.sh
```

Start it from its folder:

```sh
cd "Gringo Engine"
./Gringo
```

Build a project from the command line, from the repository root:

```sh
./Gringo\ Engine/Gringo --build R-Type/R-Type.gringo
```

## TrackNTrap, the second game

TrackNTrap is a 3D race for up to four players, made with the same engine to
show that it is not tied to R-Type. Rats race around a kitchen track full of
knives, brooms, and spatulas, and drop mouse traps on the track for the others.
It lives in `Gringo Engine/TrackNTrap/`.

It is built with the editor (build the editor first, see above), and creates
`trackntrap_server` and `trackntrap_client` in `Gringo Engine/TrackNTrap/`.
They take the same `[ip] [port]` arguments as R-Type.

### WINDOWS

From a **Developer PowerShell for VS 2022**, at the repository root:

```powershell
& ".\Gringo Engine\Gringo.exe" --build "Gringo Engine\TrackNTrap\TrackNTrap.gringo"
& ".\Gringo Engine\TrackNTrap\trackntrap_server.exe"
& ".\Gringo Engine\TrackNTrap\trackntrap_client.exe"
```

### LINUX

```sh
./Gringo\ Engine/Gringo --build "Gringo Engine/TrackNTrap/TrackNTrap.gringo"
./Gringo\ Engine/TrackNTrap/trackntrap_server
./Gringo\ Engine/TrackNTrap/trackntrap_client
```

## Repository layout

```text
R-Type/                     R-Type game: .gringo scene, scripts, assets
Gringo Engine/              Standalone engine, with its own CMake project
├── src/                    ECS core, editor, plugins, renderers
├── tests/                  Engine unit tests
├── TrackNTrap/             Second game made with the engine
└── build_engine.sh         Builds the engine and the editor
Documentation/R-Type/       SvelteKit documentation website
tests/                      Test suite and network simulation scripts
Norms/                      clang-format and clang-tidy configuration
Developer Setup/            Script to set up a development machine
.github/                    CI workflows, templates, contribution guides
build_rtype.sh              Builds the R-Type client and server
```

## Documentation

The complete user and developer documentation is available on the
[R-Type documentation website](https://r-type-project-documentation.vercel.app/).

Useful sections:

- [Getting started](https://r-type-project-documentation.vercel.app/user/getting-started)
- [Controls](https://r-type-project-documentation.vercel.app/user/controls)
- [Multiplayer](https://r-type-project-documentation.vercel.app/user/multiplayer)
- [Architecture](https://r-type-project-documentation.vercel.app/developer/architecture)
- [Developer setup](https://r-type-project-documentation.vercel.app/developer/developer-setup)
- [Network protocol](https://r-type-project-documentation.vercel.app/developer/protocol)
- [Running tests](https://r-type-project-documentation.vercel.app/developer/running-tests)

The documentation website source is in
[Documentation/R-Type](Documentation/R-Type/). To run it locally, use
Node.js 20.19 or newer (or 22.12 or newer):

```sh
cd Documentation/R-Type
npm install
npm run dev
```

## License, contributors, and community

This project is distributed under the [MIT License](LICENSE).

### Contributing

Contributions, bug reports, documentation improvements, and gameplay ideas are
welcome. Before contributing:

1. Read the [contribution guide](.github/CONTRIBUTING.md).
2. Read the [Code of Conduct](.github/CODE_OF_CONDUCT.md).
3. Run the relevant build and tests locally.
4. Open an issue or pull request using the repository templates.

Security issues must be reported according to the
[Security Policy](.github/SECURITY.md), not through a public issue.

### Community resources

- [Issues](https://github.com/Yoyott29/R-Type/issues) — bug reports and
  feature requests.
- [Pull requests](https://github.com/Yoyott29/R-Type/pulls) — proposed changes.
- [Contributing guide](.github/CONTRIBUTING.md) — development workflow.
- [Security policy](.github/SECURITY.md) — private vulnerability reporting.

## Authors

| Name | Role | Contact |
| --- | --- | --- |
| Eliott Duchene | Engine and game | eliott1.duchene@epitech.eu |
| Arthur Vignes | Engine and game | arthur.vignes@epitech.eu |
| Clovis Nedelec | Network | clovis.nedelec@epitech.eu |
| Arthur Piron | Network | arthur.piron@epitech.eu |

Source code and issue tracking are hosted on
[GitHub](https://github.com/Yoyott29/R-Type).
