#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
RULES_FILE=""

if [[ "${1:-}" == "--with-rules" ]]; then
  RULES_FILE="${2:-}"
  if [[ -z "${RULES_FILE}" ]]; then
    echo "Usage: ./scripts/run_demo_stack.sh [--with-rules /absolute/path/to/topic_grouping_rules.ini]"
    exit 1
  fi
fi

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

"${ROOT_DIR}/scripts/run_demo_publisher.sh" &
PUBLISHER_PID=$!

echo "Started demo publisher (PID=${PUBLISHER_PID}). Waiting for demo topics..."

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
  echo "Demo publisher is running, but demo topics were not confirmed before RViz startup."
  echo "RViz can still be opened; use Refresh in the panel if needed."
fi

if [[ -n "${RULES_FILE}" ]]; then
  exec "${ROOT_DIR}/scripts/run_rviz_with_rules.sh" "${RULES_FILE}"
fi

exec "${ROOT_DIR}/scripts/run_rviz_local.sh"
