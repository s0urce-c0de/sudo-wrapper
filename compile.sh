#!/bin/sh
./root bash -c " \
cd $(dirname "$0"); \
gcc -v root.c -o root-test \
  -O2 \
  -flto \
  -Wformat-security \
  -Wall \
  -Wextra \
  -Werror \
  -Wpedantic \
  -fstack-protector-strong \
  -D_FORTIFY_SOURCE=2 \
  -fPIE \
  -Wunused-command-line-argument \
  -Wnewline-eof \
  -Wshadow \
  -Wconversion \
  -Wnull-dereference \
  -Wvla; \
if [ $? -eq 0 ]; then \
  chown 0:0 root-test; \
  chmod 6777 root-test; \
  echo 'mv root-test root when you are done testing.'; \
fi \
"