#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"
command -v emcc >/dev/null 2>&1 || { echo "Emscripten (emcc) not found. Install and activate the Emscripten SDK first." >&2; exit 1; }
mkdir -p wasm
emcc src/main.c src/tournament.c src/bracket.c src/prediction.c src/leaderboard.c src/simulation.c src/storage.c \
  -std=c11 -O3 -flto -Wall -Wextra --no-entry \
  -s WASM=1 -s MODULARIZE=1 -s EXPORT_NAME=createEsportsModule \
  -s ENVIRONMENT=web -s ALLOW_MEMORY_GROWTH=1 \
  -s EXPORTED_FUNCTIONS='["_malloc","_free","_api_reset","_api_create","_api_register","_api_withdraw","_api_start","_api_winner","_api_rating","_api_probability","_api_simulate","_api_upset_run","_api_upset_team","_api_upset_winner","_api_run_prediction","_api_prediction_team","_api_prediction_winner","_api_pick","_api_pick_team","_api_player_pick","_api_score","_api_player_name","_api_players_count","_api_player_nick","_api_rank","_api_export","_api_import","_api_error","_api_state","_api_status","_api_size","_api_count","_api_active","_api_team_name","_api_team_tag","_api_team_color","_api_team_rating","_api_rounds","_api_match_count","_api_match_team","_api_match_winner","_api_champion","_api_completed","_api_team_wins","_api_team_losses","_api_accuracy"]' \
  -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap","UTF8ToString","stringToNewUTF8"]' \
  -o wasm/esports.js
echo "Built wasm/esports.js and wasm/esports.wasm"
