/*
 * simple_mud_server.c - Minimal TCP server for AMLP driver testing
 * 
 * A basic telnet-style server that:
 * - Initializes the VM and loads master object
 * - Listens on a configurable port (default 4000)
 * - Accepts multiple clients using select()
 * - Echoes input for initial testing (replace with VM execution later)
 * - Handles graceful shutdown
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "vm.h"
#include "compiler.h"
#include "master_object.h"

#define MAX_CLIENTS 100
#define BUFFER_SIZE 4096
#define DEFAULT_PORT 3000
#define DEFAULT_MASTER_PATH "lib/secure/master.c"

/* Global state for signal handling */
static volatile sig_atomic_t server_running = 1;
static VirtualMachine *global_vm = NULL;

/* Signal handler for clean shutdown */
void handle_shutdown_signal(int sig) {
    (void)sig;  /* Unused parameter */
    fprintf(stderr, "\n[Server] Received shutdown signal, cleaning up...\n");
    server_running = 0;
}

/* Initialize VM and master object */
int initialize_vm(const char *master_path) {
    fprintf(stderr, "[Server] Initializing VM...\n");
    
    global_vm = vm_init();
    if (!global_vm) {
        fprintf(stderr, "[Server] ERROR: Failed to initialize VM\n");
        return -1;
    }
    
    fprintf(stderr, "[Server] Loading master object from: %s\n", master_path);
    
    if (master_object_init(master_path, global_vm) != 0) {
        fprintf(stderr, "[Server] ERROR: Failed to load master object\n");
        vm_free(global_vm);
        global_vm = NULL;
        return -1;
    }
    
    fprintf(stderr, "[Server] VM initialized successfully\n");
    return 0;
}

/* Cleanup VM resources */
void cleanup_vm(void) {
    if (global_vm) {
        fprintf(stderr, "[Server] Cleaning up master object...\n");
        master_object_cleanup();
        
        fprintf(stderr, "[Server] Freeing VM...\n");
        vm_free(global_vm);
        global_vm = NULL;
    }
}

/* Send welcome message to new client */
void send_welcome(int client_fd) {
    const char *welcome = 
        "\r\n"
        "╔═══════════════════════════════════════╗\r\n"
        "║   AMLP Driver - Development Server   ║\r\n"
        "║           Version 0.1.0               ║\r\n"
        "╚═══════════════════════════════════════╝\r\n"
        "\r\n"
        "Type 'help' for commands, 'quit' to disconnect.\r\n"
        "\r\n> ";
    
    send(client_fd, welcome, strlen(welcome), 0);
}

/* Handle client input (echo for now, will execute LPC later) */
void handle_client_input(int client_fd, const char *input, size_t len) {
    /* Strip trailing newline/carriage return */
    char *cleaned = strndup(input, len);
    if (!cleaned) return;
    
    size_t clean_len = len;
    while (clean_len > 0 && (cleaned[clean_len-1] == '\n' || cleaned[clean_len-1] == '\r')) {
        cleaned[--clean_len] = '\0';
    }
    
    /* Handle basic commands */
    if (strcmp(cleaned, "quit") == 0) {
        const char *goodbye = "Goodbye!\r\n";
        send(client_fd, goodbye, strlen(goodbye), 0);
        free(cleaned);
        close(client_fd);
        return;
    }
    
    if (strcmp(cleaned, "help") == 0) {
        const char *help = 
            "Available commands:\r\n"
            "  help  - Show this help\r\n"
            "  quit  - Disconnect\r\n"
            "\r\n"
            "All other input is echoed (VM execution coming soon).\r\n"
            "\r\n> ";
        send(client_fd, help, strlen(help), 0);
        free(cleaned);
        return;
    }
    
    /* Echo the input back (placeholder for VM execution) */
    char response[BUFFER_SIZE];
    int response_len = snprintf(response, sizeof(response), 
                                "Echo: %s\r\n> ", cleaned);
    
    if (response_len > 0 && response_len < BUFFER_SIZE) {
        send(client_fd, response, response_len, 0);
    }
    
    free(cleaned);
}

int main(int argc, char **argv) {
    int port = DEFAULT_PORT;
    const char *master_path = DEFAULT_MASTER_PATH;
    
    /* Parse command line arguments */
    if (argc >= 2) {
        port = atoi(argv[1]);
        if (port <= 0 || port > 65535) {
            fprintf(stderr, "Usage: %s [port] [master_path]\n", argv[0]);
            fprintf(stderr, "  port: 1-65535 (default: %d)\n", DEFAULT_PORT);
            fprintf(stderr, "  master_path: path to master.c (default: %s)\n", DEFAULT_MASTER_PATH);
            return 1;
        }
    }
    
    if (argc >= 3) {
        master_path = argv[2];
    }
    
    /* Setup signal handlers */
    signal(SIGINT, handle_shutdown_signal);
    signal(SIGTERM, handle_shutdown_signal);
    signal(SIGPIPE, SIG_IGN);  /* Ignore broken pipe */
    
    /* Initialize VM before network setup */
    if (initialize_vm(master_path) != 0) {
        return 1;
    }
    
    /* Create listening socket */
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        fprintf(stderr, "[Server] ERROR: Failed to create socket: %s\n", strerror(errno));
        cleanup_vm();
        return 1;
    }
    
    /* Set socket options */
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        fprintf(stderr, "[Server] WARNING: Failed to set SO_REUSEADDR: %s\n", strerror(errno));
    }
    
    /* Bind to port */
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        fprintf(stderr, "[Server] ERROR: Failed to bind to port %d: %s\n", port, strerror(errno));
        close(server_fd);
        cleanup_vm();
        return 1;
    }
    
    /* Start listening */
    if (listen(server_fd, 10) < 0) {
        fprintf(stderr, "[Server] ERROR: Failed to listen: %s\n", strerror(errno));
        close(server_fd);
        cleanup_vm();
        return 1;
    }
    
    fprintf(stderr, "[Server] Listening on port %d\n", port);
    fprintf(stderr, "[Server] Master object: %s\n", master_path);
    fprintf(stderr, "[Server] Ready for connections (Ctrl+C to shutdown)\n\n");
    
    /* Client tracking */
    int client_fds[MAX_CLIENTS];
    for (int i = 0; i < MAX_CLIENTS; i++) {
        client_fds[i] = -1;
    }
    
    /* Main server loop */
    while (server_running) {
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(server_fd, &read_fds);
        
        int max_fd = server_fd;
        
        /* Add client sockets to set */
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (client_fds[i] > 0) {
                FD_SET(client_fds[i], &read_fds);
                if (client_fds[i] > max_fd) {
                    max_fd = client_fds[i];
                }
            }
        }
        
        /* Wait for activity with timeout */
        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        
        int activity = select(max_fd + 1, &read_fds, NULL, NULL, &timeout);
        
        if (activity < 0 && errno != EINTR) {
            fprintf(stderr, "[Server] ERROR: select() failed: %s\n", strerror(errno));
            break;
        }
        
        if (activity < 0) {
            /* Interrupted by signal, check server_running flag */
            continue;
        }
        
        /* Check for new connection */
        if (FD_ISSET(server_fd, &read_fds)) {
            struct sockaddr_in client_addr;
            socklen_t addr_len = sizeof(client_addr);
            int new_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
            
            if (new_fd < 0) {
                fprintf(stderr, "[Server] ERROR: Failed to accept connection: %s\n", strerror(errno));
            } else {
                /* Find free slot */
                int slot = -1;
                for (int i = 0; i < MAX_CLIENTS; i++) {
                    if (client_fds[i] == -1) {
                        slot = i;
                        break;
                    }
                }
                
                if (slot == -1) {
                    fprintf(stderr, "[Server] WARNING: Max clients reached, rejecting connection\n");
                    const char *msg = "Server full, try again later.\r\n";
                    send(new_fd, msg, strlen(msg), 0);
                    close(new_fd);
                } else {
                    client_fds[slot] = new_fd;
                    fprintf(stderr, "[Server] New connection from %s on slot %d (fd %d)\n", 
                           inet_ntoa(client_addr.sin_addr), slot, new_fd);
                    send_welcome(new_fd);
                }
            }
        }
        
        /* Check client sockets for data */
        for (int i = 0; i < MAX_CLIENTS; i++) {
            int fd = client_fds[i];
            if (fd <= 0) continue;
            
            if (FD_ISSET(fd, &read_fds)) {
                char buffer[BUFFER_SIZE];
                ssize_t bytes_read = recv(fd, buffer, sizeof(buffer) - 1, 0);
                
                if (bytes_read <= 0) {
                    /* Client disconnected or error */
                    if (bytes_read == 0) {
                        fprintf(stderr, "[Server] Client on slot %d disconnected\n", i);
                    } else {
                        fprintf(stderr, "[Server] ERROR: recv() on slot %d: %s\n", i, strerror(errno));
                    }
                    close(fd);
                    client_fds[i] = -1;
                } else {
                    /* Process client input */
                    buffer[bytes_read] = '\0';
                    handle_client_input(fd, buffer, bytes_read);
                    
                    /* Check if client was closed during handling */
                    if (client_fds[i] == -1) {
                        fprintf(stderr, "[Server] Client on slot %d closed during command processing\n", i);
                    }
                }
            }
        }
    }
    
    /* Cleanup */
    fprintf(stderr, "\n[Server] Shutting down...\n");
    
    /* Close all client connections */
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_fds[i] > 0) {
            const char *msg = "\r\nServer shutting down...\r\n";
            send(client_fds[i], msg, strlen(msg), 0);
            close(client_fds[i]);
        }
    }
    
    close(server_fd);
    cleanup_vm();
    
    fprintf(stderr, "[Server] Shutdown complete\n");
    return 0;
}
