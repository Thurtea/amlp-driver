#!/bin/bash

# AMLP-MUD Control Script
# Usage: ./mud.ctl {start|stop|restart|status|reload|log}

MUDLIB_DIR="$(cd "$(dirname "$0")" && pwd)"
SERVER_BIN="$MUDLIB_DIR/build/mud_server"
PID_FILE="$MUDLIB_DIR/mud.pid"
LOG_FILE="$MUDLIB_DIR/lib/log/server.log"
DEFAULT_PORT=3000

cd "$MUDLIB_DIR" || exit 1

start_server() {
    if [ -f "$PID_FILE" ]; then
        PID=$(cat "$PID_FILE")
        if kill -0 "$PID" 2>/dev/null; then
            echo "Server already running (PID: $PID)"
            return 1
        else
            echo "Removing stale PID file..."
            rm -f "$PID_FILE"
        fi
    fi
    
    if [ ! -x "$SERVER_BIN" ]; then
        echo "Error: Server binary not found or not executable"
        echo "Run: mkdir -p build && make"
        return 1
    fi
    
    mkdir -p "$(dirname "$LOG_FILE")"
    
    echo "Starting AMLP-MUD server on port $DEFAULT_PORT..."
    nohup "$SERVER_BIN" "$DEFAULT_PORT" >> "$LOG_FILE" 2>&1 &
    SERVER_PID=$!
    echo "$SERVER_PID" > "$PID_FILE"
    
    sleep 1
    if kill -0 "$SERVER_PID" 2>/dev/null; then
        echo "Server started successfully (PID: $SERVER_PID)"
        echo "Log file: $LOG_FILE"
        return 0
    else
        echo "Server failed to start. Check $LOG_FILE"
        rm -f "$PID_FILE"
        return 1
    fi
}

stop_server() {
    if [ ! -f "$PID_FILE" ]; then
        echo "Server not running (no PID file)"
        
        # Check for orphaned processes
        ORPHAN=$(pgrep -f "mud_server.*$DEFAULT_PORT" | head -1)
        if [ -n "$ORPHAN" ]; then
            echo "Found orphaned server process (PID: $ORPHAN)"
            echo "Killing orphaned process..."
            kill -TERM "$ORPHAN" 2>/dev/null
            sleep 1
            if kill -0 "$ORPHAN" 2>/dev/null; then
                kill -KILL "$ORPHAN" 2>/dev/null
            fi
            echo "Orphaned process terminated"
        fi
        return 0
    fi
    
    PID=$(cat "$PID_FILE")
    if ! kill -0 "$PID" 2>/dev/null; then
        echo "Server not running (stale PID: $PID)"
        rm -f "$PID_FILE"
        return 0
    fi
    
    echo "Stopping server (PID: $PID)..."
    kill -TERM "$PID" 2>/dev/null
    
    # Wait up to 10 seconds for graceful shutdown
    for i in {1..10}; do
        if ! kill -0 "$PID" 2>/dev/null; then
            echo "Server stopped"
            rm -f "$PID_FILE"
            return 0
        fi
        sleep 1
    done
    
    echo "Server did not stop gracefully, forcing..."
    kill -KILL "$PID" 2>/dev/null
    rm -f "$PID_FILE"
    echo "Server killed"
    return 0
}

status_server() {
    if [ -f "$PID_FILE" ]; then
        PID=$(cat "$PID_FILE")
        if kill -0 "$PID" 2>/dev/null; then
            echo "Server is running (PID: $PID)"
            echo "Port: $DEFAULT_PORT"
            echo "Uptime: $(ps -p "$PID" -o etime= | tr -d ' ')"
            return 0
        else
            echo "Server not running (stale PID: $PID)"
            return 1
        fi
    else
        echo "Server is not running"
        
        # Check for orphaned processes
        ORPHAN=$(pgrep -f "mud_server.*$DEFAULT_PORT" | head -1)
        if [ -n "$ORPHAN" ]; then
            echo "WARNING: Found orphaned server process (PID: $ORPHAN)"
            echo "Run: ./mud.ctl stop"
        fi
        return 1
    fi
}

restart_server() {
    echo "Restarting server..."
    stop_server
    sleep 2
    start_server
}

reload_server() {
    echo "Reloading server (full restart)..."
    restart_server
}

tail_log() {
    if [ -f "$LOG_FILE" ]; then
        tail -f "$LOG_FILE"
    else
        echo "No log file found at: $LOG_FILE"
        return 1
    fi
}

case "$1" in
    start)
        start_server
        ;;
    stop)
        stop_server
        ;;
    restart)
        restart_server
        ;;
    reload)
        reload_server
        ;;
    status)
        status_server
        ;;
    log)
        tail_log
        ;;
    *)
        echo "AMLP-MUD Control Script"
        echo "Usage: $0 {start|stop|restart|reload|status|log}"
        echo ""
        echo "Commands:"
        echo "  start    - Start the MUD server"
        echo "  stop     - Stop the MUD server (kills orphaned processes)"
        echo "  restart  - Restart the MUD server"
        echo "  reload   - Reload the server (alias for restart)"
        echo "  status   - Check if server is running"
        echo "  log      - Tail the server log file"
        exit 1
        ;;
esac
