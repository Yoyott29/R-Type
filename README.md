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

## Overview

An Epitech project: a free reimplementation of the classic side-scrolling
shoot 'em up. The repository holds three projects:

- **R-Type**: the game. An authoritative server runs it, and up to four clients
  (UDP) display it and send their inputs.
- **Gringo Engine**: a modular C++ ECS engine with an editor, plugins,
  renderers, networking, physics, audio, UI and C++ scripts. It knows nothing
  about R-Type.
- **TrackNTrap**: a second game on the same engine, a 3D multiplayer race
  between rats.

Dependencies (raylib, Asio, miniaudio, nlohmann/json) are fetched by
[CPM.cmake](https://github.com/cpm-cmake/CPM.cmake) at configuration time.

Supported platforms: Linux and Windows (MSVC), server and client. Both are
built and tested in CI. macOS is not supported.

## Build and run

**Requirements:** a C++20 compiler, CMake 3.28+, Git, and internet access for
the first configuration. On Ubuntu/Debian, raylib also needs:

```sh
sudo apt install build-essential cmake git libgl1-mesa-dev libx11-dev \
  libxrandr-dev libxi-dev libxcursor-dev libxinerama-dev
```

On Windows, install the toolchain from a PowerShell window (accept the
administrator prompt, and restart Windows if asked):

```powershell
winget install Microsoft.VisualStudio.2022.BuildTools --override "--passive --wait --add Microsoft.VisualStudio.Workload.VCTools --includeRecommended"
winget install Kitware.CMake
winget install Git.Git
```

Then run every command below in a **Developer PowerShell for VS 2022** (Start
menu), from the repository root. A regular PowerShell has no `cl.exe`, and the
`.sh` scripts do not run in PowerShell. Check the setup with `cl`,
`cmake --version` and `git --version`.

| | Linux | Windows (Developer PowerShell) |
|---|---|---|
| Build the game | `./build_rtype.sh` | `cmake -S . -B build -A x64`<br>`cmake --build build --config Release --target r-type_client r-type_server -j` |
| Start the server | `./r-type_server` | `.\r-type_server.exe` |
| Start a client | `./r-type_client` | `.\r-type_client.exe` |

Both programs accept an optional port, or a host and a port:

```sh
./r-type_server 5000                 # listens on 127.0.0.1:5000
./r-type_client 5000                 # joins 127.0.0.1:5000

./r-type_server 0.0.0.0 4242         # on the host
./r-type_client 192.168.1.20 4242    # on each player's machine
```

On Windows, allow the server through the firewall on private networks.

### Editor and TrackNTrap

| | Linux | Windows (Developer PowerShell) |
|---|---|---|
| Build the editor | `./Gringo\ Engine/build_engine.sh` | `cmake -S "Gringo Engine" -B "Gringo Engine\build" -A x64`<br>`cmake --build "Gringo Engine\build" --config Release --target Gringo -j` |
| Start the editor | `cd "Gringo Engine" && ./Gringo` | `cd "Gringo Engine"; .\Gringo.exe` |
| Build TrackNTrap | `./Gringo\ Engine/Gringo --build "Gringo Engine/TrackNTrap/TrackNTrap.gringo"` | `& ".\Gringo Engine\Gringo.exe" --build "Gringo Engine\TrackNTrap\TrackNTrap.gringo"` |

TrackNTrap then provides `trackntrap_server` and `trackntrap_client` in
`Gringo Engine/TrackNTrap/`.

### Tests

```sh
./tests/tests.sh                      # Linux: full test suite

cmake -S "Gringo Engine" -B build-unit -DGRINGO_BUILD_TESTS=ON   # engine unit tests
cmake --build build-unit --target engine_tests network_tests -j
ctest --test-dir build-unit --output-on-failure
```

On Windows, add `-A x64` to the first command, `--config Release` to the
second and `-C Release` to `ctest`.

### Windows pitfalls

- **Always build in Release** (`--config Release`). Mixing Debug and Release
  files in `Gringo Engine\` makes the editor close at startup: delete the
  `.dll` files in `Gringo Engine\`, `plugins\` and `renderers\`, then rebuild.
- **Close the editor before building**: Windows cannot replace a `.dll` that a
  running program uses (`LNK1104`).
- **`cl` is not recognised**: you are not in the Developer PowerShell.
- **Blank or invisible client window**: in the Task Manager (`Ctrl` + `Shift` +
  `Esc`), right-click the client's window and choose **Maximize**.

## Repository layout

```text
R-Type/                     R-Type game: .gringo scene, scripts, assets
Gringo Engine/              Standalone engine, with its own CMake project
├── src/                    ECS core, editor, plugins, renderers
├── tests/                  Engine unit tests
└── TrackNTrap/             Second game made with the engine
Documentation/R-Type/       SvelteKit documentation website
tests/                      Test suite and network simulation scripts
Norms/                      clang-format and clang-tidy configuration
Developer Setup/            Script to set up a development machine
.github/                    CI workflows, templates, contribution guides
```

## Documentation

The full user and developer documentation is on the
[documentation website](https://r-type-project-documentation.vercel.app/):
[getting started](https://r-type-project-documentation.vercel.app/user/getting-started),
[controls](https://r-type-project-documentation.vercel.app/user/controls),
[multiplayer](https://r-type-project-documentation.vercel.app/user/multiplayer),
[architecture](https://r-type-project-documentation.vercel.app/developer/architecture),
[network protocol](https://r-type-project-documentation.vercel.app/developer/protocol),
[running tests](https://r-type-project-documentation.vercel.app/developer/running-tests).

Its source is in [Documentation/R-Type](Documentation/R-Type/) (Node.js 20.19+
or 22.12+): `npm install && npm run dev`.

## Contributing

Read the [contribution guide](.github/CONTRIBUTING.md) and the
[Code of Conduct](.github/CODE_OF_CONDUCT.md), run the build and tests locally,
then open an issue or pull request. Security issues go through the
[Security Policy](.github/SECURITY.md), not a public issue.

## License and authors

Distributed under the [MIT License](LICENSE).

| Name | Role | Contact |
| --- | --- | --- |
| Eliott Duchene | Engine and game | eliott1.duchene@epitech.eu |
| Arthur Vignes | Engine and game | arthur.vignes@epitech.eu |
| Clovis Nedelec | Network | clovis.nedelec@epitech.eu |
| Arthur Piron | Network | arthur.piron@epitech.eu |
