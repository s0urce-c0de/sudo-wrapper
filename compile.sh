#!/bin/sh
# or sudo
./root bash -c " \
cd $(dirname "$0"); \
gcc -v root.c -o root-test \
  -O3 \
  -Wall \
  -Wextra \
  -Werror;
if [ $? -eq 0 ]; then \
  chown 0:0 root-test; \
  chmod 6777 root-test; \
  echo 'mv root-test root when you are done testing.'; \
fi \
"
