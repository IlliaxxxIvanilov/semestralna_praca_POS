#include "client_network.h"
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>

int client_network_connect(const char *host, int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return -1;
    }
    
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    
    if (inet_pton(AF_INET, host, &addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(sock);
        return -1;
    }
    
    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("connect");
        close(sock);
        return -1;
    }
    
    printf("[CLIENT DEBUG] Uspesne pripojeny, socket=%d\n", sock);
    fflush(stdout);
    
    return sock;
}

int client_network_send_str(int sock, const char *buffer, size_t length) {
    if (!buffer || length == 0) return -1;
    
    printf("[CLIENT DEBUG] Posielam %zu bajtov\n", length);
    for (size_t i = 0; i < length && i < 20; i++) {
        printf("%02x ", (unsigned char)buffer[i]);
    }
    printf("\n");
    fflush(stdout);
    
    ssize_t sent = send(sock, buffer, length, 0);
    printf("[CLIENT DEBUG] Odoslanych bajtov: %zd\n", sent);
    fflush(stdout);
    
    return sent == (ssize_t)length ? 0 : -1;
}

/* jednoduché správy bez payloadu */
int client_network_send(int sock, message_type_t type) {
    char buffer[1024];
    
    printf("[CLIENT DEBUG] Serializujem spravu typu %d\n", type);
    fflush(stdout);
    
    size_t size = protocol_serialize_message(
        type,
        NULL,
        0,
        buffer,
        sizeof(buffer)
    );
    
    printf("[CLIENT DEBUG] Serializovana velkost: %zu bajtov\n", size);
    printf("[CLIENT DEBUG] Obsah bufferu: ");
    for (size_t i = 0; i < size && i < 20; i++) {
        printf("%02x ", (unsigned char)buffer[i]);
    }
    printf("\n");
    fflush(stdout);
    
    ssize_t sent = send(sock, buffer, size, 0);
    printf("[CLIENT DEBUG] send() vratil: %zd\n", sent);
    fflush(stdout);
    
    return (int)sent;
}

/* správy s dátami (napr. CREATE_SIMULATION) */
int client_network_send_with_payload(
    int sock,
    message_type_t type,
    const void *payload,
    size_t payload_size
) {
    char buffer[1024];
    
    printf("[CLIENT DEBUG] Serializujem spravu typu %d s payloadom %zu bajtov\n", 
           type, payload_size);
    fflush(stdout);
    
    size_t size = protocol_serialize_message(
        type,
        payload,
        payload_size,
        buffer,
        sizeof(buffer)
    );
    
    printf("[CLIENT DEBUG] Celkova velkost: %zu bajtov\n", size);
    fflush(stdout);
    
    ssize_t sent = send(sock, buffer, size, 0);
    printf("[CLIENT DEBUG] send() vratil: %zd\n", sent);
    fflush(stdout);
    
    return (int)sent;
}

void client_network_close(int sock) {
    if (sock >= 0) {
        printf("[CLIENT DEBUG] Zatváram socket %d\n", sock);
        fflush(stdout);
        close(sock);
    }
}

int client_network_receive(int sock, char *buffer, size_t buffer_size) {
    if (!buffer || buffer_size == 0) return -1;
    
    // Nastav timeout na 5 sekúnd
    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    
    printf("[CLIENT DEBUG] Cakam na recv()...\n");
    fflush(stdout);
    
    ssize_t n = recv(sock, buffer, buffer_size, 0);
    
    printf("[CLIENT DEBUG] recv() vratil: %zd\n", n);
    fflush(stdout);
    
    if (n <= 0) {
        if (n == 0) {
            printf("[CLIENT DEBUG] Server zatvoril spojenie\n");
        } else {
            perror("[CLIENT DEBUG] recv error");
        }
        return -1;
    }
    
    printf("[CLIENT DEBUG] Prijatych %zd bajtov: ", n);
    for (ssize_t i = 0; i < n && i < 20; i++) {
        printf("%02x ", (unsigned char)buffer[i]);
    }
    printf("\n");
    fflush(stdout);
    
    buffer[n] = '\0';
    return (int)n;
}
