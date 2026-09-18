# Product and Engineering Roadmap

This roadmap favors end-to-end playable increments. Dates and advanced mechanics remain deliberately uncommitted until earlier milestones have been play-tested.

## Completed: Milestone 0 — Architecture skeleton

- Browser-first, authoritative-server architecture selected.
- Shared C++20 match model created.
- Five-over innings, legal-ball counting, extras, toss, chase, and result foundations created.
- In-memory two-seat room domain and ready/countdown state created.
- TypeScript home screen with strict room-code validation created.
- WebAssembly binding boundary created.
- CMake presets, web production build, GitHub Actions, and setup documentation created.
- Network listener is intentionally not implemented yet.

## Next: Milestone 1 — Multiplayer lobby vertical slice

### User experience

1. A player enters a guest display name.
2. **Create game** sends `lobby.create`.
3. Server generates an eight-character code and private reconnect token.
4. Creator enters a lobby and can copy the code.
5. A second player enters the code; Join is enabled only for valid syntax.
6. Server either returns a safe error or seats the player in the same lobby.
7. Both clients see names, connection state, and ready state.
8. Either player can ready or unready.
9. When both are connected and ready, server announces one countdown deadline.
10. Clients display 3, 2, 1 using that deadline.
11. Unready/disconnect cancels the countdown.
12. Completion moves the room to toss.
13. Refreshing within the grace period restores the player seat.

### Server work

- Add Boost.Asio/Beast HTTP and WebSocket transport.
- Add a room registry with collision-safe secure code generation.
- Add secure session/reconnect token generation.
- Define command dispatch and protocol errors.
- Add server-owned monotonic event sequences.
- Replace room methods' internal clock reads with injected timestamps.
- Add room expiration and reconnect grace timers.
- Add structured logs and `/health`.
- Add graceful shutdown.

### Client work

- Add explicit home, connecting, lobby, countdown, and error view states.
- Add configurable WebSocket endpoint.
- Store reconnect credentials in session storage.
- Add copy-code and reconnect behavior.
- Estimate server/client clock offset from protocol timestamps.
- Add accessible status announcements and mobile layout.
- Keep all authoritative state read-only on the client.

### Protocol commands

```text
lobby.create
lobby.join
lobby.resume
lobby.set_ready
session.ping
```

### Protocol events

```text
lobby.created
lobby.snapshot
lobby.player_joined
lobby.player_connection_changed
lobby.ready_changed
lobby.countdown_started
lobby.countdown_cancelled
match.starting
request.rejected
```

### Acceptance criteria

- Two independent browser contexts can create and join one room.
- An invalid or nonexistent code cannot enter a lobby.
- A third player cannot take a seat.
- Countdown starts only with two connected, ready players.
- Countdown cancellation is visible to both players.
- Refresh/reconnect restores the correct seat without exposing private tokens.
- Automated tests cover happy path, invalid code, full room, duplicate requests, unready, and disconnect.
- Works on Windows and Linux and between devices on one LAN.

## Milestone 2 — Rules-only complete match

- Server toss and timed bat/bowl decision.
- Two five-over innings.
- Runs, wickets, wides, no-balls, byes, leg-byes, strike, target, tie, and result.
- Initial delivery and shot intentions.
- Commit/reveal or server-hidden simultaneous choices.
- Scorecard and text/2D feedback.
- Versioned replay record and deterministic replay tests.

## Milestone 3 — Placeholder 3D delivery loop

- Babylon.js scene, pitch, stadium blockout, lighting, and broadcast camera.
- Placeholder bowler, batter, wicketkeeper, and fielders.
- Semantic-event animation adapter.
- Authored run-up, release, ball path, shot, catch, boundary, and wicket placeholders.
- HUD, sound hooks, performance presets, touch controls, and reduced-motion mode.

## Milestone 4 — Loadouts and tactics

- Up to five selected shots and deliveries per player.
- Up to five named field presets.
- Server validation of availability and competitive values.
- Stadium, pitch, and random room settings.
- In-match legal field adjustment.
- Explicit save-back from an in-match field to a personal local preset.
- Versioned JSON content schemas and validation tooling.

## Milestone 5 — Gameplay depth

- Timing, direction, power, and footwork inputs.
- Pace, swing, seam, spin, bounce, and pitch condition models.
- Player skill attributes and balanced risk/reward.
- Fielding assignment and AI.
- Multiple cameras and animation blending.
- Better models, environments, effects, and audio.

## Milestone 6 — Hosted service and expanded platforms

- Public TLS deployment.
- Accounts, cloud presets, progression, rankings, and match history.
- Matchmaking, spectators, replay sharing, and moderation.
- Sticky multi-instance room routing with Redis only when needed.
- PostgreSQL for durable data.
- PWA installation followed by optional Android/iOS wrappers.
- Controller support and device-specific performance profiles.

## Deferred design questions

These require prototypes and play-testing rather than early permanent decisions:

- Exact batting and bowling control schemes.
- Real-time versus deadline-based input windows per delivery.
- Skill progression and economy.
- Ranked fairness rules and allowed assists.
- Art direction and asset pipeline.
- Detailed physical simulation boundaries.
- Team size, roster depth, and licensed content.
- In-match communication and moderation.
- Spectator delay and replay retention.