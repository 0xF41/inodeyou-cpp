#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
set -eu
work=$(mktemp -d)
trap 'rm -rf -- "$work"' EXIT
mkdir -p "$work/tree/nested"
printf 'fixture\n' > "$work/tree/nested/file"
ln "$work/tree/nested/file" "$work/tree/alias"
ln -s nested/file "$work/tree/link"
truncate -s 32M "$work/volume.img"
mkfs.ext4 -q -F -d "$work/tree" "$work/volume.img"
"$1" "$work/volume.img" "$work/tree"
