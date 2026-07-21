from pathlib import Path

script_path = Path(__file__).with_name("apply-ui-themes.py")
source = script_path.read_text()
marker = 'workflow_path = ROOT / ".github" / "workflows" / "build.yml"\n'
if marker not in source:
    raise RuntimeError("workflow migration marker missing")

source = source[:source.index(marker)] + r'''workflow_path = ROOT / ".github" / "workflows" / "build.yml"
workflow = workflow_path.read_text()

old_config = "printf 'language 1\\nchorddpad 1\\n' > /tmp/brkchrd-ru/brkchrd.cfg"
new_config = "printf 'language 1\\nchorddpad 1\\nuitheme 1\\n' > /tmp/brkchrd-ru/brkchrd.cfg"
workflow = replace_once(workflow, old_config, new_config, "theme config smoke")

render_marker = "      - name: Render sixteen-preset demo\n"
theme_smoke = r'''      - name: SDL startup smoke test for all UI palettes
        run: |
          for theme in 0 1 2 3 4; do
            dir="/tmp/brkchrd-theme-$theme"
            mkdir -p "$dir"
            printf 'uitheme %s\n' "$theme" > "$dir/brkchrd.cfg"
            set +e
            XDG_CONFIG_HOME="$dir" BRKCHRD_SKIP_SPLASH=1 SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy timeout 2s build/brkchrd-sdl
            status=$?
            set -e
            test "$status" -eq 0 -o "$status" -eq 124
          done
'''
workflow = replace_once(workflow, render_marker, theme_smoke + render_marker, "theme smoke insertion")
workflow = replace_once(workflow,
    "          grep -q 'CHORD DPAD' docs/controls.en.md\n          grep -q 'DPAD АККОРД' docs/controls.ru.md\n",
    "          grep -q 'CHORD DPAD' docs/controls.en.md\n          grep -q 'DPAD АККОРД' docs/controls.ru.md\n          grep -q 'UI PALETTE' docs/controls.en.md\n          grep -q 'ПАЛИТРА UI' docs/controls.ru.md\n          grep -q 'kUiThemes' src/sdl_main.cpp\n          grep -q 'uitheme' src/sdl_main.cpp\n",
    "theme CI checks")
workflow_path.write_text(workflow)
'''

namespace = {"__file__": str(script_path), "__name__": "__main__"}
exec(compile(source, str(script_path), "exec"), namespace)
