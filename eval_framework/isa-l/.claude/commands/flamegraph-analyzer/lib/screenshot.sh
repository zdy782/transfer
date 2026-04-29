#!/usr/bin/env bash
# lib/screenshot.sh - Convert SVG to PNG via Playwright

svg_to_png() {
    local svg_file="$1"
    local png_file="$2"

    command -v npx &>/dev/null || die "npx not found. Install Node.js for Playwright support."

    local svg_abs
    svg_abs="$(readlink -f "$svg_file")"
    local png_abs
    png_abs="$(readlink -f "$png_file" 2>/dev/null || echo "$(dirname "$svg_abs")/$(basename "$svg_abs" .svg).png")"

    log_info "Converting SVG to PNG: $svg_file -> $png_abs"

    npx playwright install chromium 2>/dev/null || true

    SVG_PATH="$svg_abs" PNG_PATH="$png_abs" node -e '
const { chromium } = require("playwright");
(async () => {
    const browser = await chromium.launch();
    const page = await browser.newPage({ viewport: { width: 1600, height: 900 } });
    await page.goto("file://" + process.env.SVG_PATH);
    await page.screenshot({ path: process.env.PNG_PATH, fullPage: true });
    await browser.close();
    console.log("Screenshot saved: " + process.env.PNG_PATH);
})();
' 2>&1

    [[ -f "$png_abs" ]] && log_info "PNG saved: $png_abs"
}
