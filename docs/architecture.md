# System Architecture

## 1. Goals

Boundary Cricket is designed to become a real-time, cross-platform cricket game while remaining practical to build incrementally.

Primary goals:

- Two players can play from different modern browsers and operating systems.
- Cricket rules and outcome simulation are implemented in reusable C++20.
- The server, rather than either player, determines authoritative match state.
- Rendering quality can evolve independently from gameplay.
- A cloned repository can be built and run on Windows and Linux.
- The initial five-over game can grow without requiring an architectural rewrite.

Non-goals for the first vertical slice:

- Photorealistic graphics.
- Continuous networked rigid-body physics.
- Accounts, rankings, progression, monetization, or matchmaking.
- More than two active players.
- Native mobile applications.

## 2. System context

```text
┌──────────────── Browser A ────────────────┐
│ HTML/CSS UI + TypeScript application      │
│ Renderer (Babylon.js in 3D milestone)     │
│ C++ game core compiled to WebAssembly     │
└────────────────────┬──────────────────────┘
                     │ WebSocket over TLS
                     ▼
┌──────────── Authoritative C++ Server ─────────────┐
│ HTTP health/static-development endpoints          │
│ Room registry and player sessions                 │
│ Lobby state machine and synchronized timers       │
│ Match coordinator and validation                  │
│ Native shared C++ game core                       │
│ Ordered event stream and snapshots                │
└────────────────────┬──────────────────────────────┘
                     ▲
                     │ WebSocket over TLS
┌────────────────────┴──────────────────────┐
│ Browser B                                 │
│ Same portable client architecture         │
└───────────────────────────────────────────┘
```

For local development, all components can run on one computer and two tabs can represent the players. For LAN testing, another device can connect to the host computer. Internet play requires deploying the server to a publicly reachable host.

## 3. Components

### 3.1 Shared C++ game core

The core is a platform-independent library with no dependency on sockets, browsers, rendering, databases, wall clocks, or operating-system APIs.

Responsibilities:

- Match state and legal state transitions.
- Innings, overs, legal deliveries, runs, wickets, extras, target, and result.
- Toss resolution from an injected random source.
- Validation of batting, bowling, and fielding intentions.
- Deterministic delivery resolution.
- Player-skill, pitch, field, and stadium modifiers.
- Semantic domain events.
- Snapshot serialization boundary.
- Replay from initial seed and ordered accepted commands.

The core receives explicit time or deadlines from its caller. It must not call a global clock. Randomness must come from match-owned, seeded state.

### 3.2 Authoritative server

Responsibilities:

- Create unique room codes and locate rooms.
- Allocate two player seats.
- Issue unguessable session and reconnect tokens.
- Validate protocol version, message schema, size, rate, and state.
- Track ready state and cancel countdown if readiness changes.
- Own toss, choice deadlines, match seed, and match core instance.
- Hide simultaneous player intentions until resolution.
- Broadcast ordered events and periodic/full snapshots.
- Reserve disconnected seats for a grace period.
- Record structured logs and match event streams.

The first server process can keep rooms in memory. PostgreSQL is deferred until persistent identities or match history exist. Redis is deferred until rooms must span multiple server instances.

### 3.3 Browser client

Responsibilities:

- Home, loadout, multiplayer, lobby, toss, match, and result views.
- Syntactic room-code validation before enabling Join.
- WebSocket connection and protocol adapter.
- Local input handling and accessibility.
- Rendering snapshots and domain events.
- Presentation-only interpolation and prediction.
- Local preferences and draft presets.

The client may predict presentation but cannot commit runs, wickets, toss outcomes, timers, or legal transitions.

### 3.4 WebAssembly adapter

The adapter is intentionally narrow:

- Parses validated browser inputs into C++ value types.
- Offers deterministic preview and replay helpers.
- Exposes core version information.
- Converts returned values into JavaScript-safe objects.

The adapter must not duplicate server authority. Client-side WebAssembly improves code reuse and permits responsive previews; it is not an anti-cheat mechanism.

### 3.5 Content definitions

Versioned JSON files define:

- Shots.
- Bowling deliveries/styles.
- Field presets.
- Pitch types.
- Stadium metadata.

Each definition has a stable identifier and schema version. The server validates competitive values and decides which content version a room uses. Visual-only metadata may remain client-side.

## 4. State machines

### 4.1 Room lifecycle

```text
Creating
  -> WaitingForOpponent
  -> ReadyCheck
  -> Countdown
  -> InMatch
  -> Completed
  -> Closed
```

Exceptional transitions:

- Any lobby state can move to `Closed` after expiration.
- `Countdown` returns to `ReadyCheck` if a player unreadies or disconnects.
- `InMatch` enters a reconnect pause or configured forfeit flow on disconnect.

Countdown is based on a server deadline, not three independent client intervals. Clients display `deadline - synchronizedServerTime`.

### 4.2 Match lifecycle

```text
AwaitingToss
  -> AwaitingTossDecision
  -> FirstInnings
  -> InningsBreak
  -> SecondInnings
  -> MatchComplete
```

A delivery follows:

```text
AwaitingBowlerIntent
  -> AwaitingBatterIntent
  -> ResolvingDelivery
  -> AnimatingResult (client concern)
  -> ScoreUpdated
  -> next delivery / innings break / match complete
```

The protocol can collect intents in parallel using commit/reveal semantics where seeing the opponent's selection would create an advantage.

## 5. Cricket simulation strategy

The first playable simulation is event-driven rather than a fully continuous physics simulation.

Inputs include:

- Selected delivery and shot.
- Aim/line/length.
- Timing.
- Player attributes.
- Pitch conditions.
- Active field.
- Match-owned random state.

Output includes:

- Contact quality.
- Approximate trajectory parameters for presentation.
- Runs/extras.
- Wicket and dismissal kind.
- Assigned fielder.
- Ordered semantic events.

This delivers a reproducible and network-friendly match. Later versions can replace selected calculations with richer ball-flight, bat-contact, and fielding models while preserving commands and events.

## 6. Network model

### 6.1 Transport

- HTTPS for health, bootstrap metadata, and future content manifests.
- Secure WebSockets for bidirectional lobby and match traffic.
- JSON during early development for inspectability.
- A binary format is considered only after profiling.

### 6.2 Message envelope

Every client command contains:

- Protocol version.
- Message type.
- Unique request ID.
- Room identifier where applicable.
- Payload.

Every authoritative event contains:

- Protocol version.
- Event type.
- Match/room identifier.
- Monotonic sequence number.
- Server timestamp or deadline where applicable.
- Payload.

Commands are idempotent by request ID within a bounded session window. Sequence gaps cause the client to request/resume from a snapshot.

### 6.3 Snapshot and event stream

A reconnecting client sends its session token and last applied sequence. The server responds with either:

- Missing retained events; or
- A complete snapshot followed by newer events.

Event logs also enable deterministic debugging and future replay/spectator support.

## 7. Room codes and identity

Public room codes are eight characters selected from:

```text
ABCDEFGHJKLMNPQRSTUVWXYZ23456789
```

`I`, `O`, `0`, and `1` are omitted. Spaces and hyphens entered by users are removed before validation.

Room codes are locators, not credentials. A separate high-entropy private token authenticates a seat and enables reconnection. Production join attempts must be rate-limited.

The first milestone uses guest display names. Accounts are deferred.

## 8. Loadouts and fields

A player loadout initially permits:

- Up to five shots.
- Up to five deliveries/styles.
- Up to five field presets.

Availability and competitive parameters are validated by the server. Fields use cricket-semantic normalized coordinates/roles, not display pixels. In-match edits belong to match state. Persisting them back to a personal preset is an explicit post-edit action.

Room configuration initially contains:

- Fixed five-over format.
- Stadium ID or random.
- Pitch ID or random.
- Future-compatible environmental settings.

Random selections are resolved by the server and recorded in match state.

## 9. Security boundaries

Never trust the browser for:

- Skill ratings or unlocked content.
- Toss results.
- Random seeds.
- Timer expiration.
- Ball outcomes.
- Scores.
- Legal field placement.
- Match completion.

The public server additionally requires:

- TLS.
- Input schema and range validation.
- Message and connection limits.
- Join/create rate limits.
- Secure token generation.
- Escaped display names.
- Dependency and container scanning.
- Structured audit logs without private token values.

## 10. Scalability path

### Stage 1: single process

One server owns an in-memory room registry and all active matches. This is simplest and sufficient for local development and early deployment.

### Stage 2: sticky multi-instance

A gateway maps room codes to game-server instances. Connections for one room remain on one instance. Redis can hold expiring room routing metadata.

### Stage 3: durable services

Add PostgreSQL for accounts, presets, progression, rankings, and match metadata. Event archives can move to object storage. Match simulation remains single-owner to avoid distributed consistency in the hot path.

No distributed infrastructure should be introduced before measured demand.

## 11. Testing strategy

- **C++ unit tests:** scoring, overs, innings, toss, validation, and deterministic outcomes.
- **Property tests:** invariants such as non-negative score and exactly six legal balls per completed over.
- **Protocol tests:** valid and invalid message fixtures.
- **Server integration tests:** room collision, ready cancellation, timeouts, and reconnect.
- **Web unit tests:** room-code normalization and view state.
- **Two-browser tests:** create, join, ready, countdown, toss, and match completion.
- **Replay tests:** same version, seed, and commands produce the same events.
- **Cross-platform CI:** Windows and Linux native builds plus web build.

Determinism is guaranteed only for a specified core/content version. Replays carry both versions.

## 12. Observability

The server should expose:

- Health/readiness endpoint.
- Active rooms and connections as aggregate metrics.
- Command rejection counts by safe reason.
- Match duration and reconnect metrics.
- Structured logs correlated by room and match IDs.

Public metrics must never reveal reconnect tokens or hidden player intentions.

## 13. Future native and mobile support

The browser client is the universal initial platform. Future options:

- Progressive Web App for installation and offline assets.
- Capacitor/Tauri-style shell for app-store packaging.
- Native renderer consuming the same protocol and C++ core.
- Touch-specific controls and performance profiles.

The authoritative protocol and core boundaries avoid coupling future clients to the TypeScript implementation.