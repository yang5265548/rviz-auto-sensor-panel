#!/usr/bin/env bash

detect_ros_setup_path() {
  local candidates=()
  local distro="${ROS_DISTRO:-}"

  if [[ -n "${RVIZ_AUTO_SENSOR_PANEL_ROS_SETUP:-}" ]]; then
    candidates+=("${RVIZ_AUTO_SENSOR_PANEL_ROS_SETUP}")
  fi

  if [[ -n "${ROS_SETUP:-}" ]]; then
    candidates+=("${ROS_SETUP}")
  fi

  if [[ -n "${distro}" ]]; then
    candidates+=(
      "/opt/ros/${distro}/setup.bash"
      "/opt/ros/${distro}/setup.sh"
      "${HOME}/ros2_${distro}/install/setup.bash"
      "${HOME}/ros2_${distro}/install/setup.sh"
    )
  fi

  for known_distro in jazzy humble iron rolling; do
    candidates+=(
      "/opt/ros/${known_distro}/setup.bash"
      "/opt/ros/${known_distro}/setup.sh"
      "${HOME}/ros2_${known_distro}/install/setup.bash"
      "${HOME}/ros2_${known_distro}/install/setup.sh"
    )
  done

  local pattern
  for pattern in "${HOME}"/ros2_*/install/setup.bash "${HOME}"/ros2_*/install/setup.sh; do
    if [[ -e "${pattern}" ]]; then
      candidates+=("${pattern}")
    fi
  done

  local candidate
  for candidate in "${candidates[@]}"; do
    if [[ -f "${candidate}" ]]; then
      printf '%s\n' "${candidate}"
      return 0
    fi
  done

  return 1
}

ensure_ros_environment() {
  if command -v ros2 >/dev/null 2>&1 && [[ -n "${ROS_DISTRO:-}" || -n "${AMENT_PREFIX_PATH:-}" ]]; then
    return 0
  fi

  local ros_setup
  if ! ros_setup="$(detect_ros_setup_path)"; then
    echo "Could not locate a ROS 2 setup script."
    echo "Set RVIZ_AUTO_SENSOR_PANEL_ROS_SETUP to your ROS setup file, for example:"
    echo "  export RVIZ_AUTO_SENSOR_PANEL_ROS_SETUP=/opt/ros/jazzy/setup.bash"
    return 1
  fi

  set +u
  # shellcheck source=/dev/null
  source "${ros_setup}"
  set -u
}

setup_project_pythonpath() {
  local root_dir="$1"
  export PYTHONPATH="${root_dir}/vendor:${PYTHONPATH:-}"
}

prepend_library_path() {
  local lib_dir="$1"
  if [[ ! -d "${lib_dir}" ]]; then
    return 0
  fi

  if [[ "$(uname -s)" == "Darwin" ]]; then
    case ":${DYLD_LIBRARY_PATH:-}:" in
      *":${lib_dir}:"*) ;;
      *) export DYLD_LIBRARY_PATH="${lib_dir}${DYLD_LIBRARY_PATH:+:${DYLD_LIBRARY_PATH}}" ;;
    esac
  else
    case ":${LD_LIBRARY_PATH:-}:" in
      *":${lib_dir}:"*) ;;
      *) export LD_LIBRARY_PATH="${lib_dir}${LD_LIBRARY_PATH:+:${LD_LIBRARY_PATH}}" ;;
    esac
  fi
}

setup_runtime_library_path() {
  local prefix
  local old_ifs="${IFS}"
  IFS=":"
  for prefix in ${AMENT_PREFIX_PATH:-}; do
    if [[ -n "${prefix}" ]]; then
      prepend_library_path "${prefix}/lib"
      local opt_lib
      for opt_lib in "${prefix}"/opt/*/lib; do
        if [[ -d "${opt_lib}" ]]; then
          prepend_library_path "${opt_lib}"
        fi
      done
    fi
  done

  for prefix in ${CMAKE_PREFIX_PATH:-}; do
    if [[ -n "${prefix}" ]]; then
      prepend_library_path "${prefix}/lib"
    fi
  done
  IFS="${old_ifs}"
}

source_project_install_setup() {
  local root_dir="$1"
  local install_prefix="${root_dir}/install"
  local local_setup="${root_dir}/install/share/rviz_auto_sensor_panel/local_setup.sh"

  if [[ -f "${local_setup}" ]]; then
    set +u
    export COLCON_CURRENT_PREFIX="${install_prefix}"
    export AMENT_CURRENT_PREFIX="${install_prefix}"
    # shellcheck source=/dev/null
    source "${local_setup}"
    set -u
  fi

  setup_runtime_library_path
}
