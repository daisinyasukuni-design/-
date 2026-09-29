#!/usr/bin/env bash
# Mirror the Google Fonts used by index.html into ./fonts so offline/headless renders use the real faces.
set -euo pipefail
cd "$(dirname "$0")" && mkdir -p fonts && cd fonts
UA="Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/140.0 Safari/537.36"
curl -sS -A "$UA" "https://fonts.googleapis.com/css2?family=Archivo:wdth,wght@62..125,100..900&family=JetBrains+Mono:wght@400;500&display=swap" -o local.css
for u in $(grep -o "https://fonts.gstatic.com[^)]*" local.css | sort -u); do f=$(basename "$u"); curl -sS "$u" -o "$f"; sed -i "s|$u|$f|g" local.css; done
echo "fonts mirrored: $(ls *.woff2 | wc -l) files"
