#!/usr/bin/env bash
# Opens a shell in the datamatch container with this repo mounted at /workspace.
# Builds the image first if it doesn't exist yet.
set -euo pipefail

docker_dir="$(cd "$(dirname "$0")" && pwd)"
repo_root="$(dirname "$docker_dir")"

if ! docker image inspect datamatch >/dev/null 2>&1; then
    "$docker_dir/build.sh"
fi

# On Linux (including WSL), run as your own user so files the container
# creates aren't owned by root. Docker Desktop on macOS handles this itself.
user_args=()
if [ "$(uname -s)" = "Linux" ]; then
    user_args=(--user "$(id -u):$(id -g)" -e HOME=/tmp)
fi

docker run -it --rm \
    ${user_args[@]+"${user_args[@]}"} \
    -v "$repo_root":/workspace \
    -w /workspace \
    datamatch
