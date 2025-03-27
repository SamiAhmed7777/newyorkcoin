#!/bin/sh
# Copyright (c) 2013-2024 The Bitcoin Core, Litecoin Core, and NewYorkCoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

set -e
srcdir="$(dirname $0)"
cd "$srcdir"

# Check for required tools
for cmd in autoreconf automake autoconf libtool pkg-config; do
    command -v $cmd >/dev/null 2>&1 || { echo >&2 "ERROR: $cmd not found. Please install it."; exit 1; }
done

if [ -z ${LIBTOOLIZE} ] && GLIBTOOLIZE="`which glibtoolize 2>/dev/null`"; then
    LIBTOOLIZE="${GLIBTOOLIZE}"
    export LIBTOOLIZE
fi

# Clean up previous build files
rm -rf autom4te.cache
rm -f config.guess config.sub

# Get the latest config.guess and config.sub from upstream
wget -q -O config.guess 'https://git.savannah.gnu.org/cgit/config.git/plain/config.guess'
wget -q -O config.sub 'https://git.savannah.gnu.org/cgit/config.git/plain/config.sub'
chmod +x config.guess config.sub

echo "Running autoreconf..."
autoreconf --install --force --warnings=all

echo "Configuration completed successfully."
