#!/usr/bin/env node

const fs = require('fs');
const path = require('path');

const files = {
    'mazer': path.join(__dirname, 'public/mazer.js'),
    'mazer-fallback': path.join(__dirname, 'public/mazer-fallback.js'),
};

const replacement = `_emscripten_exit_fullscreen() {
    if (!JSEvents.fullscreenEnabled()) return -1;
    // Make sure no queued up calls will fire after this.
    JSEvents.removeDeferredCalls(_JSEvents_requestFullscreen);
`;

const file = files[process.argv[2]];

// Eh, the files are not that huge..
const contents = fs.readFileSync(file).toString();
const startIdx = contents.indexOf('_emscripten_exit_fullscreen');
if (startIdx === -1) {
    throw new Error('Cannot find injection point');
}

const foundSubstring = contents.substring(startIdx);
const endIdx = foundSubstring.indexOf('return 0;');
if (endIdx === -1) {
    throw new Error('Cannot find injection point');
}
const newContents = contents.substring(0, startIdx) + replacement + contents.substring(startIdx + endIdx);
fs.writeFileSync(file, newContents);
console.log(`Patched ${file}`);

process.exit(0);
