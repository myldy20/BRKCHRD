from pathlib import Path
from urllib.request import urlopen

PINNED_SOURCE = (
    "https://raw.githubusercontent.com/myldy20/BRKCHRD/"
    "2f1a713990e12fcb80199a16eb3ff0e6f75d8ed6/"
    "tools/apply-audio-path-hardening.py"
)

source = urlopen(PINNED_SOURCE, timeout=30).read().decode("utf-8")
start_marker = '''sdl = replace_once(
    sdl,
    "    std::cerr << \\"audio backend=\\"'''
start = source.find(start_marker)
if start < 0:
    raise RuntimeError("cannot locate brittle obtained-sample-rate replacement")
end = source.find("\n\nsdl = replace_once(", start + len(start_marker))
if end < 0:
    raise RuntimeError("cannot locate end of obtained-sample-rate replacement")
replacement = '''sdl = replace_once(
    sdl,
    "    SDL_PauseAudioDevice(audio, 0);",
    "    audio_state.sample_rate = obtained.freq;\\n"
    "    SDL_PauseAudioDevice(audio, 0);",
    "obtained sample rate",
)'''
source = source[:start] + replacement + source[end:]
exec(compile(source, "apply-audio-path-hardening-core.py", "exec"), {
    "__file__": str(Path(__file__).resolve()),
    "__name__": "__main__",
})
