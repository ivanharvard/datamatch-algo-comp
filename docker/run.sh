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
    user_args=(--user "$(id -u):$(id -g)" -e HOME=/home/student)
fi

# Pass your git name and email in so you can commit from inside the container.
# (Pushing still happens outside Docker, where your GitHub login lives.)
git_args=()
if command -v git >/dev/null 2>&1; then
    git_name="$(git config --global user.name || true)"
    git_email="$(git config --global user.email || true)"
    if [ -n "$git_name" ]; then
        git_args+=(-e GIT_AUTHOR_NAME="$git_name" -e GIT_COMMITTER_NAME="$git_name")
    fi
    if [ -n "$git_email" ]; then
        git_args+=(-e GIT_AUTHOR_EMAIL="$git_email" -e GIT_COMMITTER_EMAIL="$git_email")
    fi
fi

docker run -it --rm \
    ${user_args[@]+"${user_args[@]}"} \
    ${git_args[@]+"${git_args[@]}"} \
    -v "$repo_root":/workspace \
    -w /workspace \
    --hostname datamatch \
    datamatch
