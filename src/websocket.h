/*
 * websocket.h - WebSocket Protocol Support for AMLP MUD Driver
 * 
 * Implements RFC 6455 WebSocket protocol for web client connectivity.
 * Supports:
 *   - WebSocket handshake and upgrade
 *   - Text and binary frame encoding/decoding
 *   - Ping/pong heartbeat
 *   - Connection close handling
 * 
 * Usage:
 *   1. Detect HTTP upgrade request in raw socket data
 *   2. Call ws_handle_handshake() to complete upgrade
 *   3. Use ws_decode_frame() for incoming data
 *   4. Use ws_encode_frame() for outgoing data
 */

#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include <stdint.h>
#include <stddef.h>

/* WebSocket frame opcodes */
#define WS_OPCODE_CONTINUATION  0x0
#define WS_OPCODE_TEXT          0x1
#define WS_OPCODE_BINARY        0x2
#define WS_OPCODE_CLOSE         0x8
#define WS_OPCODE_PING          0x9
#define WS_OPCODE_PONG          0xA

/* WebSocket close codes */
#define WS_CLOSE_NORMAL         1000
#define WS_CLOSE_GOING_AWAY     1001
#define WS_CLOSE_PROTOCOL_ERROR 1002
#define WS_CLOSE_INVALID_DATA   1003
#define WS_CLOSE_TOO_LARGE      1009

/* Maximum frame/message sizes */
#define WS_MAX_FRAME_SIZE       65536
#define WS_MAX_HEADER_SIZE      14

/* WebSocket connection states */
typedef enum {
    WS_STATE_CONNECTING,    /* Waiting for HTTP upgrade request */
    WS_STATE_OPEN,          /* WebSocket connection established */
    WS_STATE_CLOSING,       /* Close frame sent, waiting for response */
    WS_STATE_CLOSED         /* Connection closed */
} WSState;

/* WebSocket frame structure */
typedef struct {
    int fin;                /* Final fragment flag */
    int opcode;             /* Frame opcode */
    int masked;             /* Is payload masked? */
    uint64_t payload_len;   /* Payload length */
    uint8_t mask_key[4];    /* Masking key (if masked) */
    uint8_t *payload;       /* Payload data (allocated) */
} WSFrame;

/* WebSocket handshake result */
typedef struct {
    int success;            /* 1 = handshake successful */
    char *response;         /* HTTP response to send (allocated) */
    size_t response_len;    /* Response length */
    char client_key[64];    /* Client's Sec-WebSocket-Key */
    char protocol[64];      /* Requested sub-protocol (if any) */
} WSHandshake;

/*
 * Check if incoming data looks like an HTTP WebSocket upgrade request.
 * Returns 1 if it starts with "GET " and contains "Upgrade: websocket"
 */
int ws_is_upgrade_request(const char *data, size_t len);

/*
 * Parse HTTP upgrade request and generate handshake response.
 * Caller must free result->response when done.
 * 
 * Parameters:
 *   request     - Raw HTTP request data
 *   request_len - Length of request
 *   result      - Output handshake result
 * 
 * Returns: 0 on success, -1 on error
 */
int ws_handle_handshake(const char *request, size_t request_len, WSHandshake *result);

/*
 * Decode a WebSocket frame from raw data.
 * Caller must free frame->payload when done.
 * 
 * Parameters:
 *   data       - Raw WebSocket frame data
 *   data_len   - Length of input data
 *   frame      - Output frame structure
 *   consumed   - Output: number of bytes consumed from data
 * 
 * Returns: 
 *   0  - Successfully decoded complete frame
 *   1  - Need more data (incomplete frame)
 *   -1 - Error (invalid frame)
 */
int ws_decode_frame(const uint8_t *data, size_t data_len, WSFrame *frame, size_t *consumed);

/*
 * Encode data into a WebSocket frame.
 * Caller must free the returned buffer.
 * 
 * Parameters:
 *   opcode     - Frame opcode (WS_OPCODE_TEXT, WS_OPCODE_BINARY, etc.)
 *   payload    - Data to encode
 *   payload_len - Length of payload
 *   output_len  - Output: length of encoded frame
 * 
 * Returns: Allocated buffer containing WebSocket frame, or NULL on error
 */
uint8_t *ws_encode_frame(int opcode, const uint8_t *payload, size_t payload_len, size_t *output_len);

/*
 * Convenience function to encode a text message.
 */
uint8_t *ws_encode_text(const char *text, size_t *output_len);

/*
 * Create a WebSocket close frame.
 * 
 * Parameters:
 *   code       - Close status code (e.g., WS_CLOSE_NORMAL)
 *   reason     - Optional close reason (can be NULL)
 *   output_len - Output: length of encoded frame
 * 
 * Returns: Allocated buffer containing close frame
 */
uint8_t *ws_encode_close(uint16_t code, const char *reason, size_t *output_len);

/*
 * Create a WebSocket pong frame (response to ping).
 * 
 * Parameters:
 *   ping_payload - Payload from the ping frame
 *   payload_len  - Length of ping payload
 *   output_len   - Output: length of encoded pong
 * 
 * Returns: Allocated buffer containing pong frame
 */
uint8_t *ws_encode_pong(const uint8_t *ping_payload, size_t payload_len, size_t *output_len);

/*
 * Free resources in a WSFrame structure.
 */
void ws_frame_free(WSFrame *frame);

/*
 * Free resources in a WSHandshake structure.
 */
void ws_handshake_free(WSHandshake *handshake);

/*
 * Convert ANSI color codes to simple text or HTML.
 * Strips or converts ANSI escape sequences for web display.
 * Caller must free returned string.
 * 
 * Parameters:
 *   text   - Text containing ANSI codes
 *   mode   - 0=strip, 1=convert to HTML <span> tags
 * 
 * Returns: Allocated string with ANSI codes processed
 */
char *ws_convert_ansi(const char *text, int mode);

/*
 * Convert line endings for web clients.
 * Converts \r\n and \n to appropriate format.
 * Windows: keeps \r\n
 * Web: converts to \n (JavaScript handles display)
 */
char *ws_normalize_line_endings(const char *text);

#endif /* WEBSOCKET_H */
