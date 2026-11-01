#!/usr/bin/env bash
# Builds the datamatch image. Run from anywhere: ./docker/build.sh
set -euo pipefail

cd "$(dirname "$0")"
docker build -t datamatch .
