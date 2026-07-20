#pragma once

#include "brkchrd/music.hpp"
#include "brkchrd/synth.hpp"

#include <array>
#include <cstdint>
#include <string>
#include <string_view>

namespace brkchrd_ui {

enum class Language { English = 0, Russian = 1 };
enum class ChordDpadStyle { Toggle = 0, Hold = 1 };

inline bool russian(Language language) { return language == Language::Russian; }
inline std::string tr(Language language, std::string_view english, std::string_view russian_value) {
    return std::string(russian(language) ? russian_value : english);
}

inline std::uint32_t next_codepoint(std::string_view value, std::size_t& index) {
    if (index >= value.size()) return 0;
    const auto first = static_cast<unsigned char>(value[index++]);
    if (first < 0x80U) return first;
    if ((first & 0xE0U) == 0xC0U && index < value.size()) {
        const auto second = static_cast<unsigned char>(value[index++]);
        return static_cast<std::uint32_t>((first & 0x1FU) << 6U) | (second & 0x3FU);
    }
    if ((first & 0xF0U) == 0xE0U && index + 1U < value.size()) {
        const auto second = static_cast<unsigned char>(value[index++]);
        const auto third = static_cast<unsigned char>(value[index++]);
        return static_cast<std::uint32_t>((first & 0x0FU) << 12U) |
               static_cast<std::uint32_t>((second & 0x3FU) << 6U) | (third & 0x3FU);
    }
    if ((first & 0xF8U) == 0xF0U && index + 2U < value.size()) {
        const auto second = static_cast<unsigned char>(value[index++]);
        const auto third = static_cast<unsigned char>(value[index++]);
        const auto fourth = static_cast<unsigned char>(value[index++]);
        return static_cast<std::uint32_t>((first & 0x07U) << 18U) |
               static_cast<std::uint32_t>((second & 0x3FU) << 12U) |
               static_cast<std::uint32_t>((third & 0x3FU) << 6U) | (fourth & 0x3FU);
    }
    return '?';
}

inline int codepoint_count(std::string_view value) {
    int count = 0;
    std::size_t index = 0;
    while (index < value.size()) { next_codepoint(value, index); ++count; }
    return count;
}

inline std::size_t byte_offset(std::string_view value, int characters) {
    std::size_t index = 0;
    for (int count = 0; count < characters && index < value.size(); ++count) next_codepoint(value, index);
    return index;
}

inline std::string fit_utf8(std::string value, int characters) {
    if (characters < 1) return {};
    const int length = codepoint_count(value);
    if (length <= characters) return value;
    if (characters <= 3) return value.substr(0, byte_offset(value, characters));
    return value.substr(0, byte_offset(value, characters - 3)) + "...";
}

inline std::array<std::uint8_t, 7> cyrillic_glyph(std::uint32_t codepoint) {
    if (codepoint >= 0x0430U && codepoint <= 0x044FU) codepoint -= 0x20U;
    if (codepoint == 0x0451U) codepoint = 0x0401U;
    switch (codepoint) {
    case 0x0410: return {0x0E,0x11,0x11,0x1F,0x11,0x11,0x11}; // А
    case 0x0411: return {0x1F,0x10,0x10,0x1E,0x11,0x11,0x1E}; // Б
    case 0x0412: return {0x1E,0x11,0x11,0x1E,0x11,0x11,0x1E}; // В
    case 0x0413: return {0x1F,0x10,0x10,0x10,0x10,0x10,0x10}; // Г
    case 0x0414: return {0x0E,0x0A,0x0A,0x0A,0x12,0x1F,0x11}; // Д
    case 0x0415: return {0x1F,0x10,0x10,0x1E,0x10,0x10,0x1F}; // Е
    case 0x0401: return {0x0A,0x00,0x1F,0x10,0x1E,0x10,0x1F}; // Ё
    case 0x0416: return {0x15,0x15,0x0E,0x04,0x0E,0x15,0x15}; // Ж
    case 0x0417: return {0x0E,0x11,0x01,0x06,0x01,0x11,0x0E}; // З
    case 0x0418: return {0x11,0x13,0x15,0x19,0x11,0x11,0x11}; // И
    case 0x0419: return {0x0A,0x04,0x11,0x13,0x15,0x19,0x11}; // Й
    case 0x041A: return {0x11,0x12,0x14,0x18,0x14,0x12,0x11}; // К
    case 0x041B: return {0x07,0x09,0x09,0x11,0x11,0x11,0x11}; // Л
    case 0x041C: return {0x11,0x1B,0x15,0x15,0x11,0x11,0x11}; // М
    case 0x041D: return {0x11,0x11,0x11,0x1F,0x11,0x11,0x11}; // Н
    case 0x041E: return {0x0E,0x11,0x11,0x11,0x11,0x11,0x0E}; // О
    case 0x041F: return {0x1F,0x11,0x11,0x11,0x11,0x11,0x11}; // П
    case 0x0420: return {0x1E,0x11,0x11,0x1E,0x10,0x10,0x10}; // Р
    case 0x0421: return {0x0E,0x11,0x10,0x10,0x10,0x11,0x0E}; // С
    case 0x0422: return {0x1F,0x04,0x04,0x04,0x04,0x04,0x04}; // Т
    case 0x0423: return {0x11,0x11,0x0A,0x04,0x08,0x10,0x0E}; // У
    case 0x0424: return {0x04,0x0E,0x15,0x15,0x0E,0x04,0x04}; // Ф
    case 0x0425: return {0x11,0x11,0x0A,0x04,0x0A,0x11,0x11}; // Х
    case 0x0426: return {0x11,0x11,0x11,0x11,0x11,0x1F,0x01}; // Ц
    case 0x0427: return {0x11,0x11,0x11,0x0F,0x01,0x01,0x01}; // Ч
    case 0x0428: return {0x15,0x15,0x15,0x15,0x15,0x15,0x1F}; // Ш
    case 0x0429: return {0x15,0x15,0x15,0x15,0x15,0x1F,0x01}; // Щ
    case 0x042A: return {0x18,0x08,0x0E,0x09,0x09,0x09,0x0E}; // Ъ
    case 0x042B: return {0x11,0x11,0x1D,0x15,0x15,0x15,0x1D}; // Ы
    case 0x042C: return {0x10,0x10,0x1E,0x11,0x11,0x11,0x1E}; // Ь
    case 0x042D: return {0x0E,0x11,0x01,0x07,0x01,0x11,0x0E}; // Э
    case 0x042E: return {0x12,0x15,0x15,0x1D,0x15,0x15,0x12}; // Ю
    case 0x042F: return {0x0F,0x11,0x11,0x0F,0x05,0x09,0x11}; // Я
    default: return {0x1F,0x11,0x02,0x04,0x00,0x04,0x00};
    }
}

inline std::string on_off(Language language, bool value) {
    if (!russian(language)) return value ? "ON" : "OFF";
    return value ? "ВКЛ" : "ВЫКЛ";
}

inline std::string ui_motion(Language language, int value) {
    if (!russian(language)) return value == 0 ? "OFF" : value == 1 ? "LOW" : "FULL";
    return value == 0 ? "ВЫКЛ" : value == 1 ? "НИЗК" : "ПОЛН";
}

inline std::string play_mode(Language language, brkchrd::PlayMode mode) {
    if (!russian(language)) return brkchrd::mode_name(mode);
    switch (mode) {
    case brkchrd::PlayMode::Pad: return "ПЭД";
    case brkchrd::PlayMode::Strum: return "БОЙ";
    case brkchrd::PlayMode::Arp: return "АРП";
    case brkchrd::PlayMode::Pulse: return "ПУЛЬС";
    }
    return "ПЭД";
}

inline std::string palette(Language language, int value) {
    if (!russian(language)) return brkchrd::palette_name(value);
    return value == 1 ? "РАСШИР." : value == 2 ? "ТЕМНАЯ" : "КЛАССИКА";
}

inline std::string bank(Language language, int value) {
    if (!russian(language)) return brkchrd::bank_name(value);
    return value == 1 ? "ДИАТОНИКА+" : value == 2 ? "ЗАИМСТВ." : "ОСНОВА";
}

inline std::string direction(Language language, int palette_value, brkchrd::Direction value) {
    const std::string english = brkchrd::direction_label(palette_value, value);
    if (!russian(language)) return english;
    if (english == "BASE") return "ОСНОВА";
    if (english == "FLIP") return "СМЕНА";
    if (english == "DARK") return "ТЕМНЫЙ";
    if (english == "DIM") return "УМЕН.";
    if (english == "AUG") return "УВЕЛ.";
    if (english == "HALFDIM") return "ПОЛУУМ.";
    if (english == "SIXTH") return "СЕКСТА";
    if (english == "LYDIAN") return "ЛИДИЙ";
    if (english == "ELEVENTH") return "11";
    if (english == "POWER") return "КВИНТА";
    if (english == "CRUNCH") return "ХРУСТ";
    if (english == "QUARTAL") return "КВАРТА";
    if (english == "TENSION") return "НАПРЯЖ.";
    if (english == "OPEN") return "ОТКРЫТ.";
    if (english == "TRITONE") return "ТРИТОН";
    return english;
}

inline std::string effect(Language language, brkchrd::EffectType value) {
    if (!russian(language)) return brkchrd::SynthEngine::effect_name(value);
    switch (value) {
    case brkchrd::EffectType::Off: return "ВЫКЛ";
    case brkchrd::EffectType::Chorus: return "ХОРУС";
    case brkchrd::EffectType::Phaser: return "ФЭЙЗЕР";
    case brkchrd::EffectType::Tremolo: return "ТРЕМОЛО";
    case brkchrd::EffectType::Drive: return "ДРАЙВ";
    case brkchrd::EffectType::Crusher: return "КРАШЕР";
    case brkchrd::EffectType::Delay: return "ДИЛЭЙ";
    case brkchrd::EffectType::Reverb: return "РЕВЕРБ";
    }
    return "ВЫКЛ";
}

inline std::string chord_dpad(Language language, ChordDpadStyle value) {
    if (!russian(language)) return value == ChordDpadStyle::Hold ? "HOLD" : "TOGGLE";
    return value == ChordDpadStyle::Hold ? "УДЕРЖ." : "НАЖАТИЕ";
}

} // namespace brkchrd_ui
