#!/usr/bin/env bash
set -euo pipefail

# Simple wrapper to ensure the built server binary is available
MUDLIB_DIR="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="$MUDLIB_DIR/build"
DRIVER_BIN="$BUILD_DIR/driver"
MUD_SERVER_SYM="$BUILD_DIR/mud_server"

cd "$MUDLIB_DIR" || exit 1

ensure_server_binary() {
    mkdir -p "$BUILD_DIR"
    if [ ! -x "$MUD_SERVER_SYM" ]; then
        if [ -x "$DRIVER_BIN" ]; then
            ln -sf "$(readlink -f "$DRIVER_BIN")" "$MUD_SERVER_SYM"
            chmod +x "$MUD_SERVER_SYM"
        else
            echo "Server binary not found. Run: (cd $MUDLIB_DIR && make)" >&2
            return 1
        fi
    fi
    return 0
}

CMD="${1:-start}"

case "$CMD" in
  start|stop|restart|reload|status|log)
    ensure_server_binary || exit 1
    exec "$MUDLIB_DIR/mud.ctl" "$CMD"
    ;;
  *)
    echo "Usage: $0 {start|stop|restart|reload|status|log}" >&2
    exit 2
    ;;
esac
