/*
 * websocket.c - WebSocket Protocol Implementation for AMLP MUD Driver
 * 
 * Implements RFC 6455 WebSocket protocol.
 * 
 * Key features:
 *   - HTTP upgrade handshake with SHA-1 key validation
 *   - Frame encoding/decoding with masking support
 *   - Text, binary, ping/pong, and close frames
 *   - ANSI color code conversion for web clients
 */

#include "websocket.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Simple Base64 encoding table */
static const char base64_table[] = 
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/* WebSocket magic GUID from RFC 6455 */
static const char *WS_GUID = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

/* Forward declarations for internal functions */
static void sha1_hash(const uint8_t *data, size_t len, uint8_t *output);
static char *base64_encode(const uint8_t *data, size_t len);
static char *find_header_value(const char *request, const char *header);
static void str_trim(char *str);

/*
 * Simple SHA-1 implementation
 * Based on FIPS 180-4 specification
 */
static uint32_t sha1_rol(uint32_t value, int bits) {
    return ((value << bits) | (value >> (32 - bits)));
}

static void sha1_hash(const uint8_t *data, size_t len, uint8_t *output) {
    uint32_t h0 = 0x67452301;
    uint32_t h1 = 0xEFCDAB89;
    uint32_t h2 = 0x98BADCFE;
    uint32_t h3 = 0x10325476;
    uint32_t h4 = 0xC3D2E1F0;
    
    /* Pre-processing: adding padding bits */
    size_t msg_len = len + 1;
    while ((msg_len % 64) != 56) msg_len++;
    msg_len += 8;  /* 64-bit length */
    
    uint8_t *msg = calloc(msg_len, 1);
    if (!msg) return;
    
    memcpy(msg, data, len);
    msg[len] = 0x80;  /* Append bit '1' to message */
    
    /* Append original length in bits as 64-bit big-endian */
    uint64_t bits_len = len * 8;
    for (int i = 0; i < 8; i++) {
        msg[msg_len - 1 - i] = (uint8_t)(bits_len >> (i * 8));
    }
    
    /* Process each 512-bit chunk */
    for (size_t chunk = 0; chunk < msg_len; chunk += 64) {
        uint32_t w[80];
        
        /* Break chunk into sixteen 32-bit big-endian words */
        for (int i = 0; i < 16; i++) {
            w[i] = ((uint32_t)msg[chunk + i*4] << 24) |
                   ((uint32_t)msg[chunk + i*4 + 1] << 16) |
                   ((uint32_t)msg[chunk + i*4 + 2] << 8) |
                   ((uint32_t)msg[chunk + i*4 + 3]);
        }
        
        /* Extend the sixteen 32-bit words into eighty 32-bit words */
        for (int i = 16; i < 80; i++) {
            w[i] = sha1_rol(w[i-3] ^ w[i-8] ^ w[i-14] ^ w[i-16], 1);
        }
        
        /* Initialize working variables */
        uint32_t a = h0, b = h1, c = h2, d = h3, e = h4;
        
        /* Main loop */
        for (int i = 0; i < 80; i++) {
            uint32_t f, k;
            if (i < 20) {
                f = (b & c) | ((~b) & d);
                k = 0x5A827999;
            } else if (i < 40) {
                f = b ^ c ^ d;
                k = 0x6ED9EBA1;
            } else if (i < 60) {
                f = (b & c) | (b & d) | (c & d);
                k = 0x8F1BBCDC;
            } else {
                f = b ^ c ^ d;
                k = 0xCA62C1D6;
            }
            
            uint32_t temp = sha1_rol(a, 5) + f + e + k + w[i];
            e = d;
            d = c;
            c = sha1_rol(b, 30);
            b = a;
            a = temp;
        }
        
        /* Add this chunk's hash to result */
        h0 += a;
        h1 += b;
        h2 += c;
        h3 += d;
        h4 += e;
    }
    
    free(msg);
    
    /* Produce the final hash value (big-endian) */
    output[0] = (uint8_t)(h0 >> 24);
    output[1] = (uint8_t)(h0 >> 16);
    output[2] = (uint8_t)(h0 >> 8);
    output[3] = (uint8_t)(h0);
    output[4] = (uint8_t)(h1 >> 24);
    output[5] = (uint8_t)(h1 >> 16);
    output[6] = (uint8_t)(h1 >> 8);
    output[7] = (uint8_t)(h1);
    output[8] = (uint8_t)(h2 >> 24);
    output[9] = (uint8_t)(h2 >> 16);
    output[10] = (uint8_t)(h2 >> 8);
    output[11] = (uint8_t)(h2);
    output[12] = (uint8_t)(h3 >> 24);
    output[13] = (uint8_t)(h3 >> 16);
    output[14] = (uint8_t)(h3 >> 8);
    output[15] = (uint8_t)(h3);
    output[16] = (uint8_t)(h4 >> 24);
    output[17] = (uint8_t)(h4 >> 16);
    output[18] = (uint8_t)(h4 >> 8);
    output[19] = (uint8_t)(h4);
}

/*
 * Base64 encode data
 */
static char *base64_encode(const uint8_t *data, size_t len) {
    size_t out_len = ((len + 2) / 3) * 4;
    char *output = malloc(out_len + 1);
    if (!output) return NULL;
    
    size_t i, j;
    for (i = 0, j = 0; i < len; i += 3, j += 4) {
        uint32_t n = ((uint32_t)data[i]) << 16;
        if (i + 1 < len) n |= ((uint32_t)data[i+1]) << 8;
        if (i + 2 < len) n |= data[i+2];
        
        output[j] = base64_table[(n >> 18) & 0x3F];
        output[j+1] = base64_table[(n >> 12) & 0x3F];
        output[j+2] = (i + 1 < len) ? base64_table[(n >> 6) & 0x3F] : '=';
        output[j+3] = (i + 2 < len) ? base64_table[n & 0x3F] : '=';
    }
    
    output[out_len] = '\0';
    return output;
}

/*
 * Find header value in HTTP request (case-insensitive header match)
 * Returns allocated string that caller must free, or NULL if not found
 */
static char *find_header_value(const char *request, const char *header) {
    size_t header_len = strlen(header);
    const char *p = request;
    
    while (*p) {
        /* Find start of line */
        const char *line_start = p;
        
        /* Check if this line starts with our header (case-insensitive) */
        if (strncasecmp(line_start, header, header_len) == 0) {
            const char *value_start = line_start + header_len;
            
            /* Skip colon and whitespace */
            if (*value_start == ':') {
                value_start++;
                while (*value_start == ' ' || *value_start == '\t') {
                    value_start++;
                }
                
                /* Find end of value (CR or LF) */
                const char *value_end = value_start;
                while (*value_end && *value_end != '\r' && *value_end != '\n') {
                    value_end++;
                }
                
                /* Copy and return value */
                size_t value_len = value_end - value_start;
                char *value = malloc(value_len + 1);
                if (value) {
                    memcpy(value, value_start, value_len);
                    value[value_len] = '\0';
                    str_trim(value);
                }
                return value;
            }
        }
        
        /* Move to next line */
        while (*p && *p != '\n') p++;
        if (*p == '\n') p++;
    }
    
    return NULL;
}

/*
 * Trim whitespace from string in-place
 */
static void str_trim(char *str) {
    if (!str) return;
    
    /* Trim leading whitespace */
    char *start = str;
    while (*start && isspace((unsigned char)*start)) start++;
    
    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }
    
    /* Trim trailing whitespace */
    char *end = str + strlen(str) - 1;
    while (end >= str && isspace((unsigned char)*end)) {
        *end = '\0';
        end--;
    }
}

/*
 * Check if data looks like WebSocket upgrade request
 */
int ws_is_upgrade_request(const char *data, size_t len) {
    if (len < 100) return 0;  /* Too short */
    
    /* Must start with GET */
    if (strncmp(data, "GET ", 4) != 0) return 0;
    
    /* Must contain Upgrade header */
    if (strcasestr(data, "Upgrade:") == NULL) return 0;
    if (strcasestr(data, "websocket") == NULL) return 0;
    
    return 1;
}

/*
 * Handle WebSocket handshake
 */
int ws_handle_handshake(const char *request, size_t request_len, WSHandshake *result) {
    if (!request || !result) return -1;
    
    memset(result, 0, sizeof(WSHandshake));
    
    /* Verify it's a valid HTTP upgrade request */
    if (!ws_is_upgrade_request(request, request_len)) {
        return -1;
    }
    
    /* Get Sec-WebSocket-Key header */
    char *key = find_header_value(request, "Sec-WebSocket-Key");
    if (!key || strlen(key) == 0) {
        if (key) free(key);
        return -1;
    }
    
    strncpy(result->client_key, key, sizeof(result->client_key) - 1);
    
    /* Get optional protocol */
    char *protocol = find_header_value(request, "Sec-WebSocket-Protocol");
    if (protocol) {
        strncpy(result->protocol, protocol, sizeof(result->protocol) - 1);
        free(protocol);
    }
    
    /* Calculate accept key: SHA1(key + GUID) -> Base64 */
    size_t concat_len = strlen(key) + strlen(WS_GUID);
    char *concat = malloc(concat_len + 1);
    if (!concat) {
        free(key);
        return -1;
    }
    
    sprintf(concat, "%s%s", key, WS_GUID);
    free(key);
    
    uint8_t sha1_result[20];
    sha1_hash((uint8_t*)concat, concat_len, sha1_result);
    free(concat);
    
    char *accept_key = base64_encode(sha1_result, 20);
    if (!accept_key) {
        return -1;
    }
    
    /* Build HTTP response */
    char response[1024];
    int len = snprintf(response, sizeof(response),
        "HTTP/1.1 101 Switching Protocols\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Accept: %s\r\n"
        "%s%s%s"
        "\r\n",
        accept_key,
        result->protocol[0] ? "Sec-WebSocket-Protocol: " : "",
        result->protocol[0] ? result->protocol : "",
        result->protocol[0] ? "\r\n" : "");
    
    free(accept_key);
    
    result->response = malloc(len + 1);
    if (!result->response) {
        return -1;
    }
    
    memcpy(result->response, response, len);
    result->response[len] = '\0';
    result->response_len = len;
    result->success = 1;
    
    return 0;
}

/*
 * Decode a WebSocket frame
 */
int ws_decode_frame(const uint8_t *data, size_t data_len, WSFrame *frame, size_t *consumed) {
    if (!data || !frame || !consumed) return -1;
    if (data_len < 2) return 1;  /* Need more data */
    
    memset(frame, 0, sizeof(WSFrame));
    *consumed = 0;
    
    size_t pos = 0;
    
    /* Byte 0: FIN + RSV + Opcode */
    frame->fin = (data[pos] & 0x80) != 0;
    frame->opcode = data[pos] & 0x0F;
    pos++;
    
    /* Byte 1: MASK + Payload length */
    frame->masked = (data[pos] & 0x80) != 0;
    uint64_t payload_len = data[pos] & 0x7F;
    pos++;
    
    /* Extended payload length */
    if (payload_len == 126) {
        if (data_len < pos + 2) return 1;  /* Need more data */
        payload_len = ((uint64_t)data[pos] << 8) | data[pos + 1];
        pos += 2;
    } else if (payload_len == 127) {
        if (data_len < pos + 8) return 1;  /* Need more data */
        payload_len = 0;
        for (int i = 0; i < 8; i++) {
            payload_len = (payload_len << 8) | data[pos + i];
        }
        pos += 8;
    }
    
    frame->payload_len = payload_len;
    
    /* Validate size */
    if (payload_len > WS_MAX_FRAME_SIZE) {
        return -1;  /* Frame too large */
    }
    
    /* Masking key (if masked) */
    if (frame->masked) {
        if (data_len < pos + 4) return 1;  /* Need more data */
        memcpy(frame->mask_key, data + pos, 4);
        pos += 4;
    }
    
    /* Payload data */
    if (data_len < pos + payload_len) return 1;  /* Need more data */
    
    if (payload_len > 0) {
        frame->payload = malloc(payload_len + 1);  /* +1 for null terminator */
        if (!frame->payload) return -1;
        
        memcpy(frame->payload, data + pos, payload_len);
        
        /* Unmask if needed */
        if (frame->masked) {
            for (uint64_t i = 0; i < payload_len; i++) {
                frame->payload[i] ^= frame->mask_key[i % 4];
            }
        }
        
        frame->payload[payload_len] = '\0';  /* Null terminate for text frames */
    }
    
    *consumed = pos + payload_len;
    return 0;
}

/*
 * Encode a WebSocket frame
 */
uint8_t *ws_encode_frame(int opcode, const uint8_t *payload, size_t payload_len, size_t *output_len) {
    if (!output_len) return NULL;
    
    /* Calculate header size */
    size_t header_size = 2;
    if (payload_len >= 126 && payload_len <= 65535) {
        header_size += 2;
    } else if (payload_len > 65535) {
        header_size += 8;
    }
    
    /* Server-to-client frames are not masked */
    *output_len = header_size + payload_len;
    
    uint8_t *frame = malloc(*output_len);
    if (!frame) return NULL;
    
    size_t pos = 0;
    
    /* Byte 0: FIN (1) + RSV (000) + Opcode */
    frame[pos++] = 0x80 | (opcode & 0x0F);
    
    /* Byte 1+: Payload length (no mask from server) */
    if (payload_len < 126) {
        frame[pos++] = (uint8_t)payload_len;
    } else if (payload_len <= 65535) {
        frame[pos++] = 126;
        frame[pos++] = (uint8_t)(payload_len >> 8);
        frame[pos++] = (uint8_t)(payload_len & 0xFF);
    } else {
        frame[pos++] = 127;
        for (int i = 7; i >= 0; i--) {
            frame[pos++] = (uint8_t)(payload_len >> (i * 8));
        }
    }
    
    /* Payload */
    if (payload && payload_len > 0) {
        memcpy(frame + pos, payload, payload_len);
    }
    
    return frame;
}

/*
 * Convenience: encode text frame
 */
uint8_t *ws_encode_text(const char *text, size_t *output_len) {
    size_t len = text ? strlen(text) : 0;
    return ws_encode_frame(WS_OPCODE_TEXT, (const uint8_t*)text, len, output_len);
}

/*
 * Create close frame
 */
uint8_t *ws_encode_close(uint16_t code, const char *reason, size_t *output_len) {
    size_t reason_len = reason ? strlen(reason) : 0;
    size_t payload_len = 2 + reason_len;
    
    uint8_t *payload = malloc(payload_len);
    if (!payload) return NULL;
    
    payload[0] = (uint8_t)(code >> 8);
    payload[1] = (uint8_t)(code & 0xFF);
    
    if (reason && reason_len > 0) {
        memcpy(payload + 2, reason, reason_len);
    }
    
    uint8_t *frame = ws_encode_frame(WS_OPCODE_CLOSE, payload, payload_len, output_len);
    free(payload);
    
    return frame;
}

/*
 * Create pong frame (response to ping)
 */
uint8_t *ws_encode_pong(const uint8_t *ping_payload, size_t payload_len, size_t *output_len) {
    return ws_encode_frame(WS_OPCODE_PONG, ping_payload, payload_len, output_len);
}

/*
 * Free frame resources
 */
void ws_frame_free(WSFrame *frame) {
    if (frame && frame->payload) {
        free(frame->payload);
        frame->payload = NULL;
    }
}

/*
 * Free handshake resources
 */
void ws_handshake_free(WSHandshake *handshake) {
    if (handshake && handshake->response) {
        free(handshake->response);
        handshake->response = NULL;
    }
}

/*
 * Convert ANSI codes for web display
 * Mode 0: Strip all ANSI codes
 * Mode 1: Convert to HTML <span> tags
 */
char *ws_convert_ansi(const char *text, int mode) {
    if (!text) return NULL;
    
    size_t len = strlen(text);
    /* Allocate enough for potential expansion (HTML mode) */
    size_t out_size = mode == 1 ? len * 4 : len + 1;
    char *output = malloc(out_size);
    if (!output) return NULL;
    
    size_t out_pos = 0;
    size_t in_pos = 0;
    int in_span = 0;  /* Track if we have an open <span> */
    
    while (in_pos < len) {
        /* Check for ANSI escape sequence: ESC [ ... m */
        if (text[in_pos] == '\033' && in_pos + 1 < len && text[in_pos + 1] == '[') {
            /* Find the 'm' that ends the sequence */
            size_t seq_start = in_pos + 2;
            size_t seq_end = seq_start;
            
            while (seq_end < len && text[seq_end] != 'm' && seq_end - seq_start < 20) {
                seq_end++;
            }
            
            if (seq_end < len && text[seq_end] == 'm') {
                if (mode == 1) {
                    /* Convert to HTML */
                    /* Parse ANSI code */
                    char code_str[32];
                    size_t code_len = seq_end - seq_start;
                    if (code_len > 0 && code_len < sizeof(code_str)) {
                        memcpy(code_str, text + seq_start, code_len);
                        code_str[code_len] = '\0';
                        
                        int code = atoi(code_str);
                        
                        /* Close any open span */
                        if (in_span && out_pos + 7 < out_size) {
                            memcpy(output + out_pos, "</span>", 7);
                            out_pos += 7;
                            in_span = 0;
                        }
                        
                        /* Map ANSI to CSS class */
                        const char *css_class = NULL;
                        switch (code) {
                            case 0: css_class = NULL; break;  /* Reset */
                            case 1: css_class = "bold"; break;
                            case 30: css_class = "fg-black"; break;
                            case 31: css_class = "fg-red"; break;
                            case 32: css_class = "fg-green"; break;
                            case 33: css_class = "fg-yellow"; break;
                            case 34: css_class = "fg-blue"; break;
                            case 35: css_class = "fg-magenta"; break;
                            case 36: css_class = "fg-cyan"; break;
                            case 37: css_class = "fg-white"; break;
                            case 90: css_class = "fg-bright-black"; break;
                            case 91: css_class = "fg-bright-red"; break;
                            case 92: css_class = "fg-bright-green"; break;
                            case 93: css_class = "fg-bright-yellow"; break;
                            case 94: css_class = "fg-bright-blue"; break;
                            case 95: css_class = "fg-bright-magenta"; break;
                            case 96: css_class = "fg-bright-cyan"; break;
                            case 97: css_class = "fg-bright-white"; break;
                        }
                        
                        if (css_class && out_pos + 32 < out_size) {
                            int written = snprintf(output + out_pos, out_size - out_pos,
                                "<span class=\"%s\">", css_class);
                            out_pos += written;
                            in_span = 1;
                        }
                    }
                }
                /* Skip the ANSI sequence in either mode */
                in_pos = seq_end + 1;
                continue;
            }
        }
        
        /* Copy regular character */
        if (out_pos < out_size - 1) {
            output[out_pos++] = text[in_pos];
        }
        in_pos++;
    }
    
    /* Close any open span */
    if (mode == 1 && in_span && out_pos + 7 < out_size) {
        memcpy(output + out_pos, "</span>", 7);
        out_pos += 7;
    }
    
    output[out_pos] = '\0';
    return output;
}

/*
 * Normalize line endings for web clients
 */
char *ws_normalize_line_endings(const char *text) {
    if (!text) return NULL;
    
    size_t len = strlen(text);
    char *output = malloc(len + 1);
    if (!output) return NULL;
    
    size_t out_pos = 0;
    for (size_t i = 0; i < len; i++) {
        if (text[i] == '\r') {
            /* Skip \r, we'll just use \n */
            continue;
        }
        output[out_pos++] = text[i];
    }
    
    output[out_pos] = '\0';
    return output;
}
