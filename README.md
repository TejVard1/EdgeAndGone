# Boundary Cricket

Boundary Cricket is an open, cross-platform multiplayer cricket game under active development. The long-term client runs in modern browsers on Windows, Linux, macOS, Android, and iOS. Cricket rules and simulation are implemented in a shared C++20 core that can run natively on the authoritative server and compile to WebAssembly for the browser.

> Project status: architecture and vertical-slice skeleton. The first playable milestone is a two-player, five-over match with placeholder presentation.

## Architecture at a glance

- **C++20 game core** — deterministic cricket rules, match state, seeded simulation, and domain events.
- **Authoritative C++ server** — rooms, ready checks, timers, validation, reconnection, and match resolution.
- **TypeScript web client** — menus, lobby, controls, HUD, and network adapter.
- **WebAssembly adapter** — exposes safe pieces of the shared C++ core to the browser.
- **Babylon.js later** — 3D pitch, players, ball, cameras, animation, particles, and audio.
- **Data-driven content** — shots, deliveries, pitches, fields, and stadiums live in versioned JSON.

See [`docs/architecture.md`](docs/architecture.md) and the architectural decision records in [`docs/decisions`](docs/decisions).

## Prerequisites

The repository is designed for both Windows and Linux.

### Required now

- Git
- A C++20 compiler:
  - Windows: Visual Studio 2022 Build Tools with **Desktop development with C++**
  - Linux: GCC 12+ or Clang 15+
- CMake 3.24+
- Node.js 20+ and npm

### Required for browser WebAssembly builds

- Emscripten 3.1+

### Optional later

- Docker Desktop / Docker Engine with Compose

On this development machine, Git and Node.js are available. CMake, a C++ compiler, Emscripten, and Docker were not detected and must be installed before native or WebAssembly builds can be verified.

## Build the C++ core and server shell

### Windows PowerShell

Open a **Developer PowerShell for VS 2022**:

```powershell
cmake --preset windows-debug
cmake --build --preset windows-debug
ctest --preset windows-debug
.\build\windows-debug\server\Debug\cricket_server.exe
```

For single-config generators, the executable may instead be under `build/windows-debug/server`.

### Linux

```bash
cmake --preset linux-debug
cmake --build --preset linux-debug
ctest --preset linux-debug
./build/linux-debug/server/cricket_server
```

## Run the web shell

```bash
cd web
npm install
npm run dev
```

Open the URL printed by Vite. The current shell demonstrates the planned home and room-code validation experience; live room creation and joining arrive in the lobby milestone.

## Build the WebAssembly adapter

Activate the Emscripten SDK first, then run:

```bash
emcmake cmake -S . -B build/wasm -DCRICKET_BUILD_SERVER=OFF -DCRICKET_BUILD_TESTS=OFF -DCRICKET_BUILD_WASM=ON
cmake --build build/wasm
```

Generated WebAssembly artifacts are placed in `build/wasm/wasm`.

## Repository layout

```text
core/       Shared C++ domain model and deterministic simulation
server/     Authoritative multiplayer server
wasm/       C++ to browser WebAssembly bindings
web/        TypeScript browser client
content/    Versioned game definitions
protocol/   Network-message schemas
docs/       Architecture, roadmap, and decision records
```

## Initial product rules

- Multiplayer only; exactly two active players in the first mode.
- Eight-character room codes omit ambiguous characters.
- Both players must be present and ready before a synchronized three-second countdown.
- The server owns timers, toss, random seed, rules, score, and outcomes.
- The toss winner chooses batting or bowling.
- One innings per side, five overs per innings.
- Clients send player intent, never claimed outcomes.
- Advanced graphics are replaceable presentation driven by semantic game events.

## Development principles

1. Keep gameplay independent from rendering.
2. Keep the server authoritative.
3. Make simulations reproducible from seed plus ordered inputs.
4. Validate all network and content data.
5. Prefer a working vertical slice over broad unfinished systems.
6. Profile before optimizing protocol or physics complexity.
7. Do not copy proprietary code, art, audio, branding, likenesses, or UI from commercial games.

## Near-term milestones

1. Reproducible C++/web skeleton.
2. Create/join room and two-player ready lobby.
3. Reconnection and synchronized countdown.
4. Toss and complete text/2D five-over match.
5. Placeholder 3D delivery loop.
6. Player loadouts and field presets.

## Contributing

Development conventions and pull-request guidance will be added once the first vertical slice stabilizes. Every gameplay change should include deterministic C++ tests, and every multiplayer flow should eventually include a two-browser end-to-end test.

## License

No license has been selected yet. Until one is deliberately chosen, the source is not granted for redistribution. Before publishing the repository, select an appropriate license (for example, MIT, Apache-2.0, or GPL-3.0) based on the desired contribution and reuse policy.