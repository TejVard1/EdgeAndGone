import "./styles.css";

const roomCodeAlphabet = "ABCDEFGHJKLMNPQRSTUVWXYZ23456789";

export function normalizeRoomCode(value: string): string {
    return value.toUpperCase().replace(/[\s-]/g, "");
}

export function isValidRoomCode(value: string): boolean {
    const normalized = normalizeRoomCode(value);
    return (
        normalized.length === 8 &&
        [...normalized].every((character) => roomCodeAlphabet.includes(character))
    );
}

const app = document.querySelector<HTMLElement>("#app");

if (!app) {
    throw new Error("Application root was not found");
}

app.innerHTML = `
  <div class="page-shell">
    <header class="topbar">
      <a class="brand" href="/" aria-label="Boundary Cricket home">
        <span class="brand-mark" aria-hidden="true">BC</span>
        <span>Boundary Cricket</span>
      </a>
      <span class="prototype-badge">Architecture prototype</span>
    </header>

    <section class="hero">
      <div class="hero-copy">
        <p class="eyebrow">MULTIPLAYER · FIVE OVERS · CROSS-PLATFORM</p>
        <h1>Read the pitch.<br /><span>Own the moment.</span></h1>
        <p class="lede">
          A browser-first cricket game powered by a shared C++ core.
          Create a private match or join a rival with their room code.
        </p>
      </div>

      <div class="score-card" aria-label="Game format summary">
        <span class="live-dot"></span>
        <div>
          <small>QUICK MATCH</small>
          <strong>5 OVERS</strong>
        </div>
        <div class="score-divider"></div>
        <div>
          <small>PLAYERS</small>
          <strong>1 v 1</strong>
        </div>
      </div>
    </section>

    <section class="actions" aria-label="Multiplayer options">
      <article class="action-card create-card">
        <div class="card-number">01</div>
        <div>
          <p class="card-label">HOST A MATCH</p>
          <h2>Create a game</h2>
          <p>Open a private lobby, select match conditions, and share the generated code.</p>
        </div>
        <button id="create-game" class="primary-button" type="button">
          Create game <span aria-hidden="true">→</span>
        </button>
      </article>

      <article class="action-card join-card">
        <div class="card-number">02</div>
        <div>
          <p class="card-label">ENTER THE PAVILION</p>
          <h2>Join a game</h2>
          <p>Enter the eight-character code shared by the room creator.</p>
        </div>
        <label for="room-code">ROOM CODE</label>
        <input
          id="room-code"
          inputmode="text"
          maxlength="10"
          autocomplete="off"
          spellcheck="false"
          placeholder="ABCD-EFGH"
          aria-describedby="room-code-help"
        />
        <small id="room-code-help">Letters and numbers · no 0, 1, I, or O</small>
        <button id="join-game" class="secondary-button" type="button" disabled>
          Join game <span aria-hidden="true">→</span>
        </button>
      </article>
    </section>

    <section class="loadout-strip">
      <div>
        <p class="card-label">YOUR KIT BAG</p>
        <h2>Match loadouts</h2>
      </div>
      <div class="loadout-items">
        <button type="button" disabled><span>🏏</span> Batting shots <em>Soon</em></button>
        <button type="button" disabled><span>⚡</span> Bowling styles <em>Soon</em></button>
        <button type="button" disabled><span>◎</span> Field presets <em>Soon</em></button>
      </div>
    </section>

    <output id="notice" class="notice" aria-live="polite"></output>

    <footer>
      <span>C++20 core · WebAssembly-ready · Authoritative multiplayer</span>
      <span>Milestone 0.1</span>
    </footer>
  </div>
`;

const roomCodeInput = document.querySelector<HTMLInputElement>("#room-code");
const joinButton = document.querySelector<HTMLButtonElement>("#join-game");
const createButton = document.querySelector<HTMLButtonElement>("#create-game");
const notice = document.querySelector<HTMLOutputElement>("#notice");

if (!roomCodeInput || !joinButton || !createButton || !notice) {
    throw new Error("Required home-screen controls were not found");
}

roomCodeInput.addEventListener("input", () => {
    const normalized = normalizeRoomCode(roomCodeInput.value).slice(0, 8);
    const firstHalf = normalized.slice(0, 4);
    const secondHalf = normalized.slice(4);
    roomCodeInput.value = secondHalf ? `${firstHalf}-${secondHalf}` : firstHalf;
    joinButton.disabled = !isValidRoomCode(normalized);
    roomCodeInput.setAttribute(
        "aria-invalid",
        normalized.length > 0 && !isValidRoomCode(normalized) ? "true" : "false",
    );
});

createButton.addEventListener("click", () => {
    notice.textContent =
        "The interface is ready. Live room creation will be connected to the C++ WebSocket server in the next milestone.";
});

joinButton.addEventListener("click", () => {
    const code = normalizeRoomCode(roomCodeInput.value);
    notice.textContent = `Room code ${code} is valid. Server lookup will be enabled in the multiplayer lobby milestone.`;
});