#!/bin/sh
# run.sh starts the container as your own user ID on Linux. The image doesn't
# know that ID, so give it a name ("student") to keep the shell prompt readable.
if ! whoami >/dev/null 2>&1; then
    echo "student:x:$(id -u):$(id -g):student:/home/student:/bin/bash" >> /etc/passwd
fi
if ! getent group "$(id -g)" >/dev/null 2>&1; then
    echo "student:x:$(id -g):" >> /etc/group
fi
exec "$@"
