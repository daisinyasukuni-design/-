// Frame-accurate export of the showreel: node render.mjs [--scale 2] [--samples 4] [--name "YOUR NAME"] [--frames 0,60,120] [--out reel.mp4]
// Needs Playwright (Chromium) and an ffmpeg binary (FFMPEG env var, or ffmpeg on PATH).
import { chromium } from 'playwright';
import { spawn } from 'node:child_process';
import fs from 'node:fs';
import path from 'node:path';
import os from 'node:os';
import { fileURLToPath } from 'node:url';

const here = path.dirname(fileURLToPath(import.meta.url));
const arg = (k, d) => { const i = process.argv.indexOf('--' + k); return i > 0 ? process.argv[i + 1] : d; };
const scale = +arg('scale', 1), samples = +arg('samples', 1), name = arg('name', 'YOUR NAME');
const stills = arg('frames', null), outFile = path.resolve(arg('out', path.join(here, 'showreel.mp4')));
const FFMPEG = process.env.FFMPEG || 'ffmpeg';

// The page is authored without a document skeleton (the artifact host adds one), so add it here.
const tmp = fs.mkdtempSync(path.join(os.tmpdir(), 'reel-'));
// Fonts: if fonts/local.css exists (see fetch-fonts.sh), serve the faces from disk instead of Google Fonts.
const localCss = path.join(here, 'fonts', 'local.css');
let body = fs.readFileSync(path.join(here, 'index.html'), 'utf8'), head = '';
if (fs.existsSync(localCss)) {
  body = body.replace(/<link[^>]+fonts\.(googleapis|gstatic)\.com[^>]*>\n?/g, '');
  head = `<base href="file://${path.join(here, 'fonts')}/"><link rel="stylesheet" href="local.css">`;
}
const pageFile = path.join(tmp, 'index.html');
fs.writeFileSync(pageFile, `<!doctype html><html><head><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1">${head}</head><body>${body}</body></html>`);

const proxy = process.env.HTTPS_PROXY ? { server: process.env.HTTPS_PROXY } : undefined;
const browser = await chromium.launch({ proxy, args: ['--disable-gpu-vsync', '--force-color-profile=srgb'] });
const page = await browser.newPage({ viewport: { width: 1400, height: 1000 } });
page.on('pageerror', e => console.error('PAGE ERROR', e.message));
page.on('console', m => { if (m.type() === 'error') console.error('console:', m.text()); });
page.on('requestfailed', r => console.error('request failed:', r.url().slice(0, 90), r.failure()?.errorText));
await page.addInitScript(() => { window.__EXPORT = true; });
await page.goto('file://' + pageFile);
await page.evaluate(async ([s, n]) => { await window.Reel.ready; await window.Reel.setScale(s); window.Reel.setName(n); }, [scale, name]);
const faces = await page.evaluate(() => [...document.fonts].filter(f => f.status === 'loaded').map(f => f.family + ' ' + f.weight));
console.log('fonts loaded:', [...new Set(faces)].join(', ') || 'NONE (fallback fonts in use)');

const grab = async i => Buffer.from((await page.evaluate(([i, s]) => window.Reel.frame(i, s), [i, samples])).split(',')[1], 'base64');

if (stills) {
  const dir = path.join(here, 'stills'); fs.mkdirSync(dir, { recursive: true });
  for (const f of stills.split(',').map(Number)) { fs.writeFileSync(path.join(dir, `f${String(f).padStart(3, '0')}.jpg`), await grab(f)); }
  console.log('stills written to', dir);
} else {
  const wav = path.join(tmp, 'score.wav');
  fs.writeFileSync(wav, Buffer.from(await page.evaluate(() => window.Reel.wav()), 'base64'));
  fs.copyFileSync(wav, outFile.replace(/\.mp4$/, '.wav'));
  const ff = spawn(FFMPEG, ['-y', '-loglevel', 'error', '-f', 'image2pipe', '-framerate', '60', '-c:v', 'mjpeg', '-i', '-', '-i', wav,
    '-c:v', 'libx264', '-preset', 'slow', '-crf', scale > 1 ? '17' : '16', '-pix_fmt', 'yuv420p', '-profile:v', 'high', '-movflags', '+faststart',
    '-c:a', 'aac', '-b:a', '320k', '-t', '15', outFile], { stdio: ['pipe', 'inherit', 'inherit'] });
  const t0 = Date.now();
  for (let i = 0; i < 900; i++) {
    const buf = await grab(i);
    if (!ff.stdin.write(buf)) await new Promise(r => ff.stdin.once('drain', r));
    if (i % 60 === 0) console.log(`frame ${i}/900  ${((Date.now() - t0) / 1000).toFixed(0)}s`);
  }
  ff.stdin.end();
  await new Promise((res, rej) => ff.on('close', c => c ? rej(new Error('ffmpeg exit ' + c)) : res()));
  console.log('wrote', outFile);
}
await browser.close();
