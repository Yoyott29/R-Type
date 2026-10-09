# R-Type

R-Type is an Epitech project that reimplements the classic side-scrolling shoot 'em up with a custom ECS engine, an authoritative server, and networked multiplayer clients. The game is written in C++17 with raylib.

## Launch

Requirements: CMake 3.28 or newer and a C++17 compiler. Run all commands from the repository root. The first command configures CMake and automatically downloads raylib; the second compiles the project.

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

The executable is generated at the repository root because of the current CMake configuration. On Linux, you may need OpenGL and X11 development packages before configuring the project.

## Documentation

Read the full project documentation on the [R-Type documentation website](https://r-type-project-documentation.vercel.app/). (Espacially if you are a collaborator)

## Authors

- Eliott Duchene
- Arthur Piron
- Arthur Vignes
- Clovis Nedelec