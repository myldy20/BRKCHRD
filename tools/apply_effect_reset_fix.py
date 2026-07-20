from pathlib import Path

path = Path(__file__).resolve().parents[1] / "src/synth.cpp"
text = path.read_text()


def once(old: str, new: str, label: str) -> None:
    global text
    count = text.count(old)
    if count != 1:
        raise RuntimeError(f"{label}: expected one match, found {count}")
    text = text.replace(old, new, 1)


once(
    "    std::size_t position = 0U;\n    double lfo = 0.0;",
    "    std::size_t position = 0U;\n    std::size_t valid_samples = 0U;\n    double lfo = 0.0;",
    "valid sample state",
)
once(
    "    void reset() {\n"
    "        std::fill(left.begin(), left.end(), 0.0F);\n"
    "        std::fill(right.begin(), right.end(), 0.0F);\n"
    "        position = 0U;\n"
    "        lfo = 0.0;",
    "    bool readable(std::size_t index) const {\n"
    "        const std::size_t distance = (position + left.size() - index) % left.size();\n"
    "        return distance > 0U && distance <= valid_samples;\n"
    "    }\n"
    "\n"
    "    float read_left(std::size_t index) const { return readable(index) ? left[index] : 0.0F; }\n"
    "    float read_right(std::size_t index) const { return readable(index) ? right[index] : 0.0F; }\n"
    "\n"
    "    void reset() {\n"
    "        position = 0U;\n"
    "        valid_samples = 0U;\n"
    "        lfo = 0.0;",
    "constant time reset",
)
once("            const float dl = state.left[read]; const float dr = state.right[(read + 67U) % state.right.size()];",
     "            const float dl = state.read_left(read); const float dr = state.read_right((read + 67U) % state.right.size());",
     "chorus guarded reads")
once("            const float dl = state.left[read]; const float dr = state.right[read];",
     "            const float dl = state.read_left(read); const float dr = state.read_right(read);",
     "delay guarded reads")
once("            const float dl = state.left[read]; const float dr = state.right[(read + 953U) % state.right.size()];",
     "            const float dl = state.read_left(read); const float dr = state.read_right((read + 953U) % state.right.size());",
     "reverb guarded reads")
once(
    "        state.position = (state.position + 1U) % state.left.size();\n"
    "        if (state.lfo >= 1.0) state.lfo -= 1.0;",
    "        if (settings.type == EffectType::Chorus || settings.type == EffectType::Delay || settings.type == EffectType::Reverb) {\n"
    "            state.valid_samples = std::min(state.valid_samples + 1U, state.left.size());\n"
    "        }\n"
    "        state.position = (state.position + 1U) % state.left.size();\n"
    "        if (state.lfo >= 1.0) state.lfo -= 1.0;",
    "buffer validity advance",
)

path.write_text(text)
print("effect reset made constant-time")
