#!/bin/bash
# SPDX-License-Identifier: GPL-3.0-or-later

XDG_DATA_HOME=${XDG_DATA_HOME:-$HOME/.local/share}

if [ -d "/opt/system/Tools/PortMaster/" ]; then
  controlfolder="/opt/system/Tools/PortMaster"
elif [ -d "/opt/tools/PortMaster/" ]; then
  controlfolder="/opt/tools/PortMaster"
elif [ -d "$XDG_DATA_HOME/PortMaster/" ]; then
  controlfolder="$XDG_DATA_HOME/PortMaster"
else
  controlfolder="/roms/ports/PortMaster"
fi

source "$controlfolder/control.txt"
[ -f "${controlfolder}/mod_${CFW_NAME}.txt" ] && source "${controlfolder}/mod_${CFW_NAME}.txt"
get_controls

GAMEDIR="/$directory/ports/brkchrd"
CONFDIR="$GAMEDIR/conf"
BIN="$GAMEDIR/brkchrd-sdl.${DEVICE_ARCH}"

mkdir -p "$CONFDIR"
cd "$GAMEDIR" || exit 1

export XDG_CONFIG_HOME="$CONFDIR"
export XDG_DATA_HOME="$CONFDIR"
export SDL_GAMECONTROLLERCONFIG="$sdl_controllerconfig"
export LD_LIBRARY_PATH="$GAMEDIR/libs.${DEVICE_ARCH}:$LD_LIBRARY_PATH"

if [ ! -x "$BIN" ]; then
  echo "BRKCHRD binary is missing: $BIN" > "$CONFDIR/brkchrd.log"
  exit 1
fi

$GPTOKEYB "brkchrd-sdl.${DEVICE_ARCH}" &
pm_platform_helper "$BIN"
"$BIN" > "$CONFDIR/brkchrd.log" 2>&1
app_status=$?
pm_finish
exit "$app_status"
