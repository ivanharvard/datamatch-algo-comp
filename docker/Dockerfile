# check=skip=FromPlatformFlagConstDisallowed
# Dev environment for the Datamatch algo comp.
#
# Use ./docker/build.sh and ./docker/run.sh from the repo root.
#
# The answers.o files are compiled for x86-64, so this image is always
# linux/amd64 (Apple Silicon Macs run it through emulation).
FROM --platform=linux/amd64 python:3.12-slim

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        build-essential \
        valgrind \
        git \
    && rm -rf /var/lib/apt/lists/*

RUN pip install --no-cache-dir "git+https://github.com/cs50/check50@4.0.0-dev"

# The repo is mounted from the host, so its owner won't match the container user.
RUN git config --system --add safe.directory '*'

WORKDIR /workspace
CMD ["bash"]
