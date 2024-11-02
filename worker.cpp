#include <iostream>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <thread>
#include <chrono>

struct Task {
    double start;
    double end;
    double step;
};

double computeFunction(double x) {
    return x * x;
}

double computeIntegral(const Task& task) {
    double result = 0.0;
    for (double x = task.start; x < task.end; x += task.step) {
        result += computeFunction(x) * task.step;
    }
    return result;
}

void handleDiscovery(int discover_port) {
    int discover_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (discover_sock < 0) {
        std::cerr << "Ошибка создания UDP сокета" << std::endl;
        return;
    }

    int reuse = 1;
    if (setsockopt(discover_sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        std::cerr << "Ошибка установки SO_REUSEADDR" << std::endl;
        close(discover_sock);
        return;
    }

    struct sockaddr_in discover_addr;
    discover_addr.sin_family = AF_INET;
    discover_addr.sin_addr.s_addr = INADDR_ANY;
    discover_addr.sin_port = htons(discover_port);

    if (bind(discover_sock, reinterpret_cast<struct sockaddr*>(&discover_addr), 
             sizeof(discover_addr)) < 0) {
        std::cerr << "Ошибка привязки UDP сокета" << std::endl;
        close(discover_sock);
        return;
    }

    char buffer[256];
    while (true) {
        struct sockaddr_in master_addr;
        socklen_t addr_len = sizeof(master_addr);
        std::cout << "Ожидание broadcast сообщения..." << std::endl;
        ssize_t n = recvfrom(discover_sock, buffer, sizeof(buffer)-1, 0,
                            reinterpret_cast<struct sockaddr*>(&master_addr), &addr_len);
        if (n > 0) {
            buffer[n] = '\0';
            std::cout << "Получено сообщение: " << buffer << std::endl;
            if (strcmp(buffer, "DISCOVER") == 0) {
                const char* response = "AVAILABLE";
                sendto(discover_sock, response, strlen(response), 0,
                      reinterpret_cast<struct sockaddr*>(&master_addr), addr_len);
                std::cout << "Отправлен ответ AVAILABLE" << std::endl;
            }
        }
    }
}

void handleTasks(int task_port) {
    int task_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (task_sock < 0) {
        std::cerr << "Ошибка создания TCP сокета" << std::endl;
        return;
    }

    int reuse = 1;
    if (setsockopt(task_sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        std::cerr << "Ошибка установки SO_REUSEADDR" << std::endl;
        close(task_sock);
        return;
    }

    struct sockaddr_in task_addr;
    task_addr.sin_family = AF_INET;
    task_addr.sin_addr.s_addr = INADDR_ANY;
    task_addr.sin_port = htons(task_port);

    if (bind(task_sock, reinterpret_cast<struct sockaddr*>(&task_addr),
             sizeof(task_addr)) < 0) {
        std::cerr << "Ошибка привязки TCP сокета" << std::endl;
        close(task_sock);
        return;
    }

    if (listen(task_sock, 5) < 0) {
        std::cerr << "Ошибка при вызове listen()" << std::endl;
        close(task_sock);
        return;
    }

    while (true) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        
        int client_sock = accept(task_sock, reinterpret_cast<struct sockaddr*>(&client_addr), &addr_len);
        if (client_sock < 0) {
            std::cerr << "Ошибка при accept()" << std::endl;
            continue;
        }

        Task task;
        ssize_t n = recv(client_sock, &task, sizeof(task), 0);
        
        if (n == sizeof(Task)) {
            std::cout << "Получена задача: start=" << task.start 
                      << ", end=" << task.end 
                      << ", step=" << task.step << std::endl;
            
            double result = computeIntegral(task);
            std::cout << "Вычислен результат: " << result << std::endl;
            
            send(client_sock, &result, sizeof(result), 0);
            std::cout << "Результат отправлен" << std::endl;
        }
        
        close(client_sock);
    }
}

void runWorker(int discover_port, int task_port) {
    std::thread discovery_thread(handleDiscovery, discover_port);
    std::thread task_thread(handleTasks, task_port);

    discovery_thread.join();
    task_thread.join();
}

int main() {    
    int discover_port = 12345;
    int task_port = 12346;

    std::cout << "Запуск воркера на портах " << discover_port << " и " << task_port << std::endl;
    runWorker(discover_port, task_port);
    return 0;
}