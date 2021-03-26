// Debounce
const debounce = (callback, wait, immediate) => {
    let timeout;
    return (...args) => {
            const callNow = immediate && !timeout
            const next = () => callback(...args)
            clearTimeout(timeout)
            timeout = setTimeout(next, wait)
            if (callNow) { next() }
        }
    }

// Handle resizing
function onResize() {
    if (window._js_on_resize_callback) {
        let canvas = document.getElementById('canvas');
        if (canvas) {
            canvas.width = window.innerWidth;
            canvas.height = window.innerHeight;
        }
        debounce(function what() {
            window._js_on_resize_callback(window.innerWidth, window.innerHeight);
        }, 100)();
    } else {
        console.error('Failed to call the resize callback!');
    }
}

// Handle fullscreen
window.addEventListener('keydown', (e) => {
    console.log(e.code);
    if (e.code === 'F11') {
        let fsHook = document.getElementById('canvas');
        fsHook.requestFullscreen();
    }
});

window.addEventListener('resize', onResize);
window.addEventListener('orientationchange', onResize);
// window.addEventListener('fullscreenchange', onResize);

// Init the module
var statusElement = document.getElementById('status');
var progressElement = document.getElementById('progress');
var spinnerElement = document.getElementById('spinner');

var Module = {
  preRun: [],
  postRun: [],
  print: (function() {
    var element = document.getElementById('output');
    if (element) element.value = ''; // clear browser cache
    return function(text) {
      if (arguments.length > 1) text = Array.prototype.slice.call(arguments).join(' ');
      // These replacements are necessary if you render to raw HTML
      //text = text.replace(/&/g, "&amp;");
      //text = text.replace(/</g, "&lt;");
      //text = text.replace(/>/g, "&gt;");
      //text = text.replace('\n', '<br>', 'g');
      console.log(text);
      if (element) {
        element.value += text + "\n";
        element.scrollTop = element.scrollHeight; // focus on bottom
      }
    };
  })(),
  printErr: function(text) {
    if (arguments.length > 1) text = Array.prototype.slice.call(arguments).join(' ');
    console.error(text);
  },
  canvas: (function() {
    var canvas = document.getElementById('canvas');

    // As a default initial behavior, pop up an alert when webgl context is lost. To make your
    // application robust, you may want to override this behavior before shipping!
    // See http://www.khronos.org/registry/webgl/specs/latest/1.0/#5.15.2
    canvas.addEventListener("webglcontextlost", function(e) { alert('WebGL context lost. You will need to reload the page.'); e.preventDefault(); }, false);

    return canvas;
  })(),
  setStatus: function(text) {
    if (!Module.setStatus.last) Module.setStatus.last = { time: Date.now(), text: '' };
    if (text === Module.setStatus.last.text) return;
    var m = text.match(/([^(]+)\((\d+(\.\d+)?)\/(\d+)\)/);
    var now = Date.now();
    if (m && now - Module.setStatus.last.time < 30) return; // if this is a progress update, skip it if too soon
    Module.setStatus.last.time = now;
    Module.setStatus.last.text = text;
    if (m) {
      text = m[1];
      progressElement.value = parseInt(m[2])*100;
      progressElement.max = parseInt(m[4])*100;
      progressElement.hidden = false;
      spinnerElement.hidden = false;
    } else {
      progressElement.value = null;
      progressElement.max = null;
      progressElement.hidden = true;
      if (!text) spinnerElement.hidden = true;
    }
    statusElement.innerHTML = text;
  },
  totalDependencies: 0,
  monitorRunDependencies: function(left) {
    this.totalDependencies = Math.max(this.totalDependencies, left);
    Module.setStatus(left ? 'Preparing... (' + (this.totalDependencies-left) + '/' + this.totalDependencies + ')' : 'All downloads complete.');
  }
};
Module.setStatus('Downloading...');
window.onerror = function() {
  Module.setStatus('Exception thrown, see JavaScript console');
  spinnerElement.style.display = 'none';
  Module.setStatus = function(text) {
    if (text) Module.printErr('[post-exception status] ' + text);
  };
};

// Load the files
const wasmSupported = (() => {
    try {
        if (typeof WebAssembly === "object"
            && typeof WebAssembly.instantiate === "function") {
            const module = new WebAssembly.Module(Uint8Array.of(0x0, 0x61, 0x73, 0x6d, 0x01, 0x00, 0x00, 0x00));
            if (module instanceof WebAssembly.Module)
                return new WebAssembly.Instance(module) instanceof WebAssembly.Instance;
        }
    } catch (e) {
    }
    return false;
})();

function loadScript(src, onLoadCallback = () => {}) {
    const script = document.createElement('script');
    script.addEventListener('load', onLoadCallback);
    script.src = src;
    script.async = true;
    document.body.appendChild(script);
}

window.notify_startingMainloop = () => {
  onResize();
}

const WASM_FILE = '/mazer.js';
const JS_FALLBACK = '/mazer-fallback.js';

if (wasmSupported) {
    loadScript(WASM_FILE);
} else {
    loadScript(JS_FALLBACK);
}
