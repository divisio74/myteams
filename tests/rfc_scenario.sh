#!/usr/bin/env bash
set -u

SERVER_BIN="${1:-./myteams_server}"
PORT="${PORT:-$((20000 + RANDOM % 20000))}"
SERVER_LOG="tests/rfc_scenario.server.log"

FAILS=0
CHECKS=0

print_step() {
  printf '\n[STEP] %s\n' "$1"
}

pass() {
  CHECKS=$((CHECKS + 1))
  printf '[OK]   %s\n' "$1"
}

fail() {
  CHECKS=$((CHECKS + 1))
  FAILS=$((FAILS + 1))
  printf '[FAIL] %s\n' "$1"
}

assert_regex() {
  local label="$1"
  local value="$2"
  local regex="$3"

  if [[ "$value" =~ $regex ]]; then
    pass "$label -> '$value'"
  else
    fail "$label -> attendu: /$regex/ ; recu: '$value'"
  fi
}

open_connection() {
  exec {OPENED_FD}<>"/dev/tcp/127.0.0.1/${PORT}" || return 1
  return 0
}

close_connection() {
  local fd="$1"

  exec {fd}>&-
  exec {fd}<&-
}

read_line() {
  local fd="$1"
  local __line_var="$2"
  local timeout="${3:-1}"
  local raw

  if IFS= read -r -t "$timeout" -u "$fd" raw; then
    raw="${raw%$'\r'}"
    printf -v "$__line_var" '%s' "$raw"
    return 0
  fi
  printf -v "$__line_var" ''
  return 1
}

send_cmd() {
  local fd="$1"
  local cmd="$2"

  printf '%s\r\n' "$cmd" >&"$fd"
}

wait_for_server() {
  local tries=50
  local i=0

  while (( i < tries )); do
    if open_connection 2>/dev/null; then
      close_connection "$OPENED_FD"
      return 0
    fi
    sleep 0.1
    i=$((i + 1))
  done
  return 1
}

cleanup() {
  if [[ -n "${SERVER_PID:-}" ]]; then
    kill "$SERVER_PID" 2>/dev/null || true
    wait "$SERVER_PID" 2>/dev/null || true
  fi
}
trap cleanup EXIT

print_step "Compilation serveur"
if ! make --no-print-directory myteams_server >/dev/null; then
  echo "Echec de compilation"
  exit 1
fi

print_step "Demarrage serveur sur le port ${PORT}"
"${SERVER_BIN}" "${PORT}" >"${SERVER_LOG}" 2>&1 &
SERVER_PID=$!

if ! wait_for_server; then
  echo "Le serveur n'a pas demarre a temps. Voir ${SERVER_LOG}."
  exit 1
fi

print_step "Session 1: LOGIN user1 puis LOGOUT"
if ! open_connection; then
  echo "Impossible d'ouvrir la session 1"
  exit 1
fi
fd1="$OPENED_FD"

read_line "$fd1" line || line=""
assert_regex "Banniere d'accueil" "$line" '^220 .+'

send_cmd "$fd1" "LOGIN user1"
read_line "$fd1" line || line=""
assert_regex "LOGIN user1" "$line" '^200 [^[:space:]]+$'

send_cmd "$fd1" "LOGOUT"
read_line "$fd1" line || line=""
assert_regex "LOGOUT user1" "$line" '^200 [^[:space:]]+$'

close_connection "$fd1"

print_step "Session 2: LOGIN user2, USERS, USER user1, LOGOUT"
if ! open_connection; then
  echo "Impossible d'ouvrir la session 2"
  exit 1
fi
fd2="$OPENED_FD"

read_line "$fd2" line || line=""
assert_regex "Banniere d'accueil (session 2)" "$line" '^220 .+'

send_cmd "$fd2" "LOGIN user2"
read_line "$fd2" line || line=""
assert_regex "LOGIN user2" "$line" '^200 [^[:space:]]+$'

send_cmd "$fd2" "USERS"
read_line "$fd2" users_line || users_line=""
assert_regex "USERS (debut de liste RFC)" "$users_line" '^200 \[$'

send_cmd "$fd2" "USER user1"
read_line "$fd2" line || line=""
assert_regex "USER user1" "$line" '^200 [^[:space:]]+ [^[:space:]]+$'

send_cmd "$fd2" "LOGOUT"
read_line "$fd2" line || line=""
assert_regex "LOGOUT user2" "$line" '^200 [^[:space:]]+$'

close_connection "$fd2"

print_step "Session 3: reconnexion user1"
if ! open_connection; then
  echo "Impossible d'ouvrir la session 3"
  exit 1
fi
fd3="$OPENED_FD"

read_line "$fd3" line || line=""
assert_regex "Banniere d'accueil (session 3)" "$line" '^220 .+'

send_cmd "$fd3" "LOGIN user1"
read_line "$fd3" line || line=""
assert_regex "LOGIN user1 (reconnexion)" "$line" '^200 [^[:space:]]+$'

close_connection "$fd3"

printf '\n--- Resume RFC scenario ---\n'
printf 'Checks: %d\n' "$CHECKS"
printf 'Fails : %d\n' "$FAILS"

if (( FAILS > 0 )); then
  printf 'Result: KO (ecarts RFC detectes)\n'
  exit 1
fi
printf 'Result: OK\n'
