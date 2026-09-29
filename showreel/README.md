# Ideas in Motion — Showreel 2026

A 15-second motion design showreel built entirely in code: every frame and every sound is a pure function of time, so the piece plays live in a browser, scrubs frame-accurately, and exports to video.

- `index.html` — the reel and its player (Canvas 2D, synthesized Web Audio score). Open it and press **Play reel — sound on**. Type a name into **Designer name** to change the end card.
- `showreel-1080p.mp4` — the exported film, 1920×1080, 60 fps, AAC audio.
- `render.mjs` — the exporter (Playwright + ffmpeg). The 4K master is built with it and is not committed (it is close to GitHub's 100 MB file limit).

## Structure (120 BPM, so every cut lands on the beat grid)

| Time | Section | What happens |
|---|---|---|
| 0.00–1.10 | Idea | A dot, an impact, 240 lines burst out; ~6,000 particles fly into IDEAS / IN / MOTION and snap. Camera dives through the O. |
| 1.10–3.00 | Space / 3D | Perspective tunnel, chrome and glass spheres, flat-shaded solids. Four shots joined by true match cuts (the hero sphere keeps its screen position and size across each cut). |
| 3.00–5.00 | Kinetic type | MOVE. slams, stretches, twists, explodes into tiles that rebuild a Swiss editorial grid. THINK / BUILD / BREAK / REBUILD each use a different technique. |
| 5.00–8.00 | Range | Liquid → wireframe torus → chrome → flat logo → particles → human figure, with breakdown labels. |
| 8.00–11.00 | Montage | Eight 0.375 s shots: kinetic type, UI, logo, 3D, particles, editorial, data viz, lighting. DESIGN / IS / MOVEMENT overlaid. |
| 11.00–13.50 | Statement | Everything collapses to a point, 0.2 s of silence, MAKE / IT / MOVE. slams in. |
| 13.50–15.00 | End card | Title, name, roles, one travelling line. Hard cut at 15.00. |

## Exporting

```bash
npm install                      # Playwright
./fetch-fonts.sh                 # optional: mirror Archivo + JetBrains Mono locally
node render.mjs --frames 0,300,720          # stills into ./stills
node render.mjs --scale 2 --samples 4 --name "ANA SILVA" --out showreel-4k.mp4   # 3840×2160, motion blur
```

`--samples` sets motion-blur sub-frames (tripled automatically on the fastest moves); sub-frames never straddle a cut.
