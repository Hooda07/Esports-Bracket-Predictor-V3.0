# Esports Bracket Predictor V3.0

A static, offline-capable esports bracket PWA. **The tournament engine is C11**; the browser UI calls the compiled C engine through a small WebAssembly bridge. No paid backend is required.

## What's preserved / added
- 4, 8 and 16-team tournaments; registration, updates, withdrawals and organizer controls
- Team name, tagline, color, rating and generated shield emblem
- Single-elimination bracket, recorded results, winner validation and champion
- Elo-style win probabilities and Monte Carlo championship simulations
- Personal match picks that advance through predicted rounds, up to 32 local player profiles, and round-weighted leaderboard scoring
- Tournament backup/restore JSON (including picks and odds snapshots), strict import validation, local browser persistence
- Demo tournament, odds display, stats and champion confetti
- Futuristic responsive UI, PWA manifest, icons and offline service worker
- Native C test suite

## Build the WebAssembly engine

Install the [Emscripten SDK](https://emscripten.org/docs/getting_started/downloads.html), activate it (`source emsdk_env.sh`), then run:

```bash
./scripts/build-wasm.sh
```

This generates `wasm/esports.js` and `wasm/esports.wasm`. These generated files are intentionally not represented by fake placeholders. The delivery environment used for this build did not have `emcc`, so the WASM binary has **not** been generated or browser-tested here.

## Native C tests

```bash
./tests/run-tests.sh
```

The tests compile the exact C engine with GCC and exercise tournament setup, registration, bracket progression, probabilities, simulations and backup/restore. GCC build uses C11.

## Run locally

After building WASM, serve this directory over HTTP (not `file://`), for example:

```bash
python3 -m http.server 8000
```

Open `http://localhost:8000`. Service workers and PWA install require localhost or HTTPS.

## GitHub Pages

Upload the contents of this directory to the repository root, enable **Settings → Pages → Deploy from a branch**, select `main` and `/(root)`. Asset paths are relative (`./...`) and support repository subpaths.

## Data model / limitations

All tournament and prediction data is stored locally in the current browser using localStorage. There is no internet-based multi-device synchronization in this static build. Up to 32 local player profiles can compete on the same browser/device. Export a JSON backup to move data between devices. The C engine validates tournament state, match outcomes, team data and player picks during restore.

## C API

See `src/engine.h` and `src/main.c` for the exported interface. Core state transitions, match participant resolution, winner validation, Elo probability, simulation, prediction scoring and JSON backup/import are implemented in C. `js/app.js` is limited to DOM rendering, user interaction, local persistence and C/WASM calls.
