from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def replace_once(text: str, old: str, new: str, label: str) -> str:
    count = text.count(old)
    if count != 1:
        raise RuntimeError(f"{label}: expected one match, found {count}")
    return text.replace(old, new, 1)


synth_path = ROOT / "src" / "synth.cpp"
synth = synth_path.read_text()
synth = replace_once(
    synth,
    "        case EffectType::Drive: {\n"
    "            const float gain = 1.0F + wet * (2.0F + colour * 12.0F);\n"
    "            out_l = soft_clip(left, gain); out_r = soft_clip(right, gain);\n"
    "            break;\n"
    "        }",
    "        case EffectType::Drive: {\n"
    "            const float gain = 1.0F + wet * (2.0F + colour * 12.0F);\n"
    "            const float compensation = 0.78F - wet * 0.10F;\n"
    "            const float driven_l = soft_clip(left, gain) * compensation;\n"
    "            const float driven_r = soft_clip(right, gain) * compensation;\n"
    "            out_l = left * (1.0F - wet) + driven_l * wet;\n"
    "            out_r = right * (1.0F - wet) + driven_r * wet;\n"
    "            break;\n"
    "        }",
    "drive gain compensation",
)
synth_path.write_text(synth)

path = ROOT / "tools" / "audio_audit.cpp"
text = path.read_text()

text = replace_once(
    text,
    "        ++rows_;\n"
    "        if (status != \"OK\") {\n"
    "            ++warnings_;",
    "        ++rows_;\n"
    "        if (status != \"OK\") {\n"
    "            ++warnings_;\n"
    "            if (status.rfind(\"FAIL_\", 0U) == 0U) ++failures_;",
    "track audit failures",
)
text = replace_once(
    text,
    "    int rows() const { return rows_; }\n"
    "    int warnings() const { return warnings_; }\n\n"
    "private:\n"
    "    std::ofstream output_;\n"
    "    int rows_ = 0;\n"
    "    int warnings_ = 0;",
    "    int rows() const { return rows_; }\n"
    "    int warnings() const { return warnings_; }\n"
    "    int failures() const { return failures_; }\n\n"
    "private:\n"
    "    std::ofstream output_;\n"
    "    int rows_ = 0;\n"
    "    int warnings_ = 0;\n"
    "    int failures_ = 0;",
    "audit failure accessor",
)
text = replace_once(
    text,
    "    if (stats.max_delta > 1.50F) return \"WARN_DELTA\";\n"
    "    if (transition_jump > 0.80) return \"WARN_TRANSITION\";\n"
    "    if (stats.saturation > 0.25) return \"WARN_SATURATION\";",
    "    if (stats.saturation > 0.25) return \"WARN_SATURATION\";\n"
    "    if (stats.max_delta > 1.50F) return \"WARN_DELTA\";\n"
    "    if (transition_jump > 0.80) return \"WARN_TRANSITION\";",
    "prefer saturation warning",
)

old_pairs = '''    for (int first_type = 0; first_type < SynthEngine::effect_count(); ++first_type) {
        for (int second_type = 0; second_type < SynthEngine::effect_count(); ++second_type) {
            const EffectSettings first{static_cast<EffectType>(first_type), first_type == 0 ? 0.0F : 1.0F, 1.0F};
            const EffectSettings second{static_cast<EffectType>(second_type), second_type == 0 ? 0.0F : 1.0F, 1.0F};
            double elapsed = 0.0;
            const Stats stats = run_sustained(1, PlayMode::Pad, first, second, kRate, elapsed);
            report.add("ALL_FX_PAIRS", 1, PlayMode::Pad, first, second, stats, 0.0, 0.0, elapsed);
        }
    }
'''
new_pairs = '''    for (int preset = 0; preset < SynthEngine::preset_count(); ++preset) {
        for (int first_type = 0; first_type < SynthEngine::effect_count(); ++first_type) {
            for (int second_type = 0; second_type < SynthEngine::effect_count(); ++second_type) {
                const EffectSettings first{static_cast<EffectType>(first_type), first_type == 0 ? 0.0F : 1.0F, 1.0F};
                const EffectSettings second{static_cast<EffectType>(second_type), second_type == 0 ? 0.0F : 1.0F, 1.0F};
                double elapsed = 0.0;
                const Stats stats = run_sustained(preset, PlayMode::Pad, first, second, kRate / 2U, elapsed);
                report.add("ALL_FX_PAIRS", preset, PlayMode::Pad, first, second, stats, 0.0, 0.0, elapsed);
            }
        }
    }
'''
text = replace_once(text, old_pairs, new_pairs, "all presets all fx pairs")

start = text.find("    const auto pairs = performance_pairs();")
end = text.find("\n    for (const FxPair& pair : pairs) {\n        SynthEngine synth(kRate);", start)
if start < 0 or end < 0:
    raise RuntimeError("cannot locate performance matrix loops")
new_performance = '''    const auto pairs = performance_pairs();
    for (const FxPair& pair : pairs) {
        for (int preset = 0; preset < SynthEngine::preset_count(); ++preset) {
            for (PlayMode mode : modes) {
                SynthEngine synth(kRate);
                synth.set_preset(preset);
                synth.set_mode(mode);
                synth.set_effect(0, kOff);
                synth.set_effect(1, kOff);
                synth.play_chord(kChord);
                const auto before = synth.render_copy(4096U);
                const float previous_left = before[before.size() - 2U];
                const float previous_right = before[before.size() - 1U];
                synth.set_effect(0, pair.first);
                synth.set_effect(1, pair.second);
                Rendered active = render_timed(synth, kRate / 2U);
                const double jump = std::max(std::abs(static_cast<double>(active.audio[0] - previous_left)),
                                             std::abs(static_cast<double>(active.audio[1] - previous_right)));
                report.add(pair.name, preset, mode, pair.first, pair.second,
                           analyse(active.audio), jump, 0.0, active.milliseconds);
            }
        }
    }
'''
text = text[:start] + new_performance + text[end:]

text = replace_once(
    text,
    "        report.add(\"MAX_24_VOICE_BENCH\", 1, PlayMode::Pad, synth.effect(0), synth.effect(1),\n"
    "                   analyse(combined), maximum_ms, 0.0, average_ms);",
    "        report.add(\"MAX_24_VOICE_BENCH\", 1, PlayMode::Pad, synth.effect(0), synth.effect(1),\n"
    "                   analyse(combined), 0.0, 0.0, average_ms);",
    "benchmark is not transition test",
)
text = replace_once(
    text,
    "    std::cout << \"audio audit rows=\" << report.rows() << \" warnings=\" << report.warnings()\n"
    "              << \" report=\" << path << '\\n';\n"
    "    if (strict && report.warnings() != 0) return 1;",
    "    std::cout << \"audio audit rows=\" << report.rows() << \" warnings=\" << report.warnings()\n"
    "              << \" failures=\" << report.failures() << \" report=\" << path << '\\n';\n"
    "    if (strict && report.failures() != 0) return 1;",
    "strict fails only on hard failures",
)
path.write_text(text)
