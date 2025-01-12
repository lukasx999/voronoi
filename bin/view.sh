#!/usr/bin/env bash
set -euo pipefail

file="out.ppm"

echo $file | entr pkill -HUP mupdf
