
<table align="center">
  <tr>
    <td><img src="Documentation/R-Type/src/lib/assets/Favicon.png" alt="R-Type logo" width="200" height="200"></td>
    <td><h1>X</h1></td>
    <td><img src="Documentation/R-Type/src/lib/assets/Gringo.png" alt="Gringo logo" width="200" height="200"></td>
  </tr>
</table>

## Rtype



R-Type is an Epitech project that reimplements the classic side-scrolling shoot 'em up with a custom ECS engine called Gringo, an authoritative server, and networked multiplayer clients. The game is written in C++17 with raylib.


## Supported platforms

| Platform | Server | Client |
|---|---|---|
| Linux | yes | yes |
| Windows | yes | yes |
| MacOS | no | no |

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

1. Install a C++ compiler: download the [Visual Studio Build Tools](https://visualstudio.microsoft.com/downloads/) and run the installer.

2. Install Git from [git-scm.com](https://git-scm.com/download/win), keeping the default options.

3. Install CMake from [cmake.org/download](https://cmake.org/download/) using the Windows `.msi` installer. During installation, choose **Add CMake to the system PATH**, otherwise the `cmake` command will not be found.

   If you have `winget`, you can install Git and CMake from PowerShell instead:

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

On Linux, build the standalone Gringo Engine/editor with:

```sh
cmake -S "Gringo Engine" -B "Gringo Engine/build"
cmake --build "Gringo Engine/build" --target Gringo
```

The root build scripts provide separate commands for the engine/editor and the
mandatory R-Type executables:

```bash
./Gringo\ Engine/build_engine.sh
./build_rtype.sh
```

`Gringo Engine/build_engine.sh` builds the standalone `Gringo` editor in `Gringo Engine/build/` without the mandatory
R-Type targets. `build_rtype.sh` configures the root CMake project and builds
both `r-type_client` and `r-type_server`.

The repository layout is:

```text
Gringo Engine/   standalone engine and editor, with its own CMake/CPM setup
├── src/
│   ├── ECS/
│   ├── editor/
│   ├── runtime/
│   └── plugins/
R-Type/          mandatory R-Type game
Gringo Engine/plugins/  runtime plugin libraries
```

Projects created with the Gringo editor are saved directly in the repository
root. A project named `MyGame` uses this structure:

```text
MyGame/
├── MyGame.gringo
├── main.cpp
├── assets/
└── scripts/
```

### Custom plugin components

A plugin should register its component and its `.gringo` deserializer together.
The deserializer receives the target scene, entity, and component JSON:

```cpp
scene.registerComponent<MyComponent>(
    [](Scene &scene, Entity entity, const nlohmann::json &data) {
        scene.addComponent(entity, MyComponent{/* read data.properties */});
    });
```

Only components from active plugins are available in the editor component picker.

### 3D plugins

The `3D` plugin provides `Camera` and `3DModel` components and renders models through the selected
renderer. `3DModel` can currently render a built-in cube or sphere, or a project model file.
Use its `Color` property to tint primitives or imported models, and select an asset in
`TexturePath` to apply a texture through the model material.
Add `CameraFollow3D` and `ChildOf` to a camera entity to keep it above and behind its parent; the
parent's `Transform.RotationY` controls the orbit around it. `Transform` also exposes independent
`RotationX`, `RotationY`, and `RotationZ` values and `ScaleX`, `ScaleY`, and `ScaleZ` values.
For an animated GLB/GLTF, set `3DModel.Animation` to its zero-based animation index and adjust
`AnimationSpeed`; use `-1` to keep the model fixed.
Add `Physics3D` as well for `Velocity3D`, `RigidBody3D`, and `Collider3D`; its `GravityX`,
`GravityY`, and `GravityZ` settings control the gravity resource. All entities use the shared
`Transform` component for 2D and 3D positions.

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

## Network plugins for Gringo projects

A generated Gringo game can enable the generic UDP transport and select its own
game-specific network plugin in its `.gringo` project file:

```json
{
    "network": {
        "enabled": true,
        "plugin": "MyGameNetwork"
    }
}
```

`Network` provides the transport, socket, connection, and network event systems.
The plugin named by `network.plugin` is loaded afterwards and can register the
game-specific messages and systems. For example, R-Type uses
`"plugin": "RTypeClient"`. The plugin shared library must be available in the
`plugins/` directory next to the executable.

## Documentation

Read the full project documentation on the [R-Type documentation website](https://r-type-project-documentation.vercel.app/).

### Contributing and community

- [Code of Conduct](.github/CODE_OF_CONDUCT.md)
- [Contributing Guide](.github/CONTRIBUTING.md)
- [Security Policy](.github/SECURITY.md)
- [Bug Report Template](.github/ISSUE_TEMPLATE/bug_report.md)
- [Pull Request Template](.github/PULL_REQUEST_TEMPLATE.md)

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
