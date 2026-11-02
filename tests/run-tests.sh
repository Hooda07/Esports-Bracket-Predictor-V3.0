#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"
mkdir -p /tmp/esports-v3-tests
SRC="src/main.c src/tournament.c src/bracket.c src/prediction.c src/leaderboard.c src/simulation.c src/storage.c"
for t in tests/test_tournament.c tests/test_bracket.c tests/test_prediction.c; do
  name="$(basename "$t" .c)"
  gcc -std=c11 -O2 -Wall -Wextra -Wno-unused-parameter $SRC "$t" -lm -o "/tmp/esports-v3-tests/$name"
  "/tmp/esports-v3-tests/$name"
done
