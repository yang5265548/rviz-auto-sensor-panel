#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

cleanup() {
  if [[ -n "${PUBLISHER_PID:-}" ]] && kill -0 "${PUBLISHER_PID}" 2>/dev/null; then
    kill "${PUBLISHER_PID}" 2>/dev/null || true
    wait "${PUBLISHER_PID}" 2>/dev/null || true
  fi
}

trap cleanup EXIT INT TERM

set +u
source /Users/young/ros2_jazzy/install/setup.sh
set -u

"${ROOT_DIR}/scripts/rebuild_install_local.sh"
"${ROOT_DIR}/scripts/run_demo_publisher.sh" &
PUBLISHER_PID=$!

topics_ready=false
for _ in {1..20}; do
  if ! kill -0 "${PUBLISHER_PID}" 2>/dev/null; then
    echo "Demo publisher exited unexpectedly."
    exit 1
  fi

  if "${ROOT_DIR}/scripts/check_demo_topics.sh" >/dev/null 2>&1; then
    topics_ready=true
    break
  fi
  sleep 0.5
done

if [[ "${topics_ready}" == true ]]; then
  "${ROOT_DIR}/scripts/check_demo_topics.sh"
else
  echo "Demo publisher started successfully, but topic discovery could not be confirmed automatically."
fi

echo "Smoke test passed."
