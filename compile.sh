#!/bin/sh
./root bash -c "\
cd $(dirname "$0"); \
gcc root.c -o root-test; \
chown 0:0 root-test; \
chmod 6777 root-test; \
echo 'mv root-test root when you are done testing.'
"