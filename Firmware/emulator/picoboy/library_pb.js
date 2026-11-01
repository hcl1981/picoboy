// Schmale Bruecke zum Worker des Studios (globalThis.__pb*).
mergeInto(LibraryManager.library, {
    pb_js_jetzt_us: () => performance.now() * 1000,
    pb_js_hook: (n) => globalThis.__pb_hook(n) | 0,
    pb_js_aus: (p, n) => { globalThis.__pb_aus(p, n); },
    pb_js_warte: (ms) => { globalThis.__pb_warte(ms); },
    pb_js_ruf: (name, a, b, c, d) =>
        globalThis.__pb[UTF8ToString(name)](a, b, c, d) | 0,
});
