# Architectural Decision Records

Architectural Decision Records (ADRs) document decisions that are expensive to reverse. Status values are `proposed`, `accepted`, `superseded`, or `rejected`.

## ADR-0001: Browser-first client with a shared C++ core

- **Status:** Accepted
- **Date:** 2026-09-06

### Context

The game must run on Windows and Linux, support other desktop systems, and leave a credible path to Android and iOS. C++ is desired for the gameplay core, while requiring users to install a native executable would make distribution and mobile support harder.

### Decision

Use a browser-first client. Implement rules and deterministic simulation in C++20, compile that library natively for the server, and compile selected client functionality to WebAssembly with Emscripten. Use TypeScript and web technologies for menus, accessibility, networking, and presentation. Introduce Babylon.js when the 3D vertical slice begins.

### Consequences

- One URL/client technology reaches all target operating systems.
- C++ rules can be shared without implementing UI in C++.
- A narrow JavaScript/WebAssembly boundary must be maintained.
- Web platform constraints apply to graphics, threading, storage, and networking.
- Native clients remain possible because the protocol and core are independent.

---

## ADR-0002: Authoritative dedicated server

- **Status:** Accepted
- **Date:** 2026-09-06

### Context

Room codes must connect players across networks, and competitive outcomes cannot safely be trusted to either browser. Peer hosting is difficult behind NAT and exposes host advantage, availability, and cheating concerns.

### Decision

Run each room and match on an authoritative C++ server. Clients send intentions. The server validates transitions, owns timers and randomness, invokes the shared game core, and broadcasts ordered events.

### Consequences

- Internet play needs a publicly hosted server.
- Local play can use a server on one developer machine.
- Cheating and desynchronization are reduced.
- Hosting has operational cost.
- Horizontal scaling eventually needs room-to-instance routing, but not distributed simulation.

---

## ADR-0003: Event-driven delivery simulation before continuous physics

- **Status:** Accepted
- **Date:** 2026-09-06

### Context

Realistic seam, spin, bat collision, skeletal animation, and fielding AI form a large coupled problem. Building all of them first would postpone validation of multiplayer and cricket rules.

### Decision

Resolve each delivery deterministically from player intentions, attributes, conditions, active field, and match-owned randomness. Return semantic outcomes and trajectory parameters that presentation code animates. Add richer physics only where later prototypes prove it improves gameplay.

### Consequences

- A complete match can be delivered early.
- Replays and networking are straightforward.
- Initial motion is an authored representation rather than emergent full physics.
- Commands and domain events must avoid assumptions that block future richer models.

---

## ADR-0004: JSON protocol and content during prototyping

- **Status:** Accepted
- **Date:** 2026-09-06

### Context

Early protocol and content definitions will change frequently. Binary formats are smaller but harder to inspect and evolve during experimentation.

### Decision

Use versioned JSON envelopes and JSON content definitions initially. Enforce schemas and bounds at trust boundaries. Measure bandwidth and CPU before considering FlatBuffers, Protobuf, or another binary representation.

### Consequences

- Browser tools and logs are easy to inspect.
- Message size and parse costs are higher than optimized binary formats.
- Version fields and explicit schemas are mandatory.
- Migration remains possible behind protocol adapters.

---

## ADR-0005: In-memory rooms before persistent infrastructure

- **Status:** Accepted
- **Date:** 2026-09-06

### Context

Accounts, rankings, and match history are not required for the first playable milestone. PostgreSQL and Redis would add setup and operational complexity without validating gameplay.

### Decision

Keep rooms, guest sessions, and active match state in one server process initially. Add Redis only for multi-instance room routing. Add PostgreSQL only when durable product features require it.

### Consequences

- Local setup remains small.
- Restarting the server terminates active rooms.
- The room registry must be abstracted enough to add routing later.
- Match event logs can first be files/test artifacts rather than durable production records.

---

## ADR-0006: Versioned deterministic replays

- **Status:** Accepted
- **Date:** 2026-09-06

### Context

Determinism helps diagnose multiplayer disagreements and enables future replay and spectator features. Algorithm and content changes can alter results.

### Decision

A replay identifies the core version, content version, initial match seed, configuration, and ordered accepted commands. Determinism is promised only for the same recorded versions. Server events carry monotonic sequence numbers.

### Consequences

- Regressions can be reproduced.
- Randomness must be centralized and seeded.
- Core code must not read wall-clock or platform-specific random state.
- Old replay support requires retaining compatible versions or migration tooling.