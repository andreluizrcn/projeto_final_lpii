#include "libtslog.h"
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <thread>
#include <unistd.h>

#define PORT 8080

// Classe RAII para gerenciar socket
class SocketGuard {
public:
  explicit SocketGuard(int sock) : sock_(sock) {}
  ~SocketGuard() {
    if (sock_ >= 0) {
      close(sock_);
    }
  }
  SocketGuard(const SocketGuard &) = delete;
  SocketGuard &operator=(const SocketGuard &) = delete;

private:
  int sock_;
};

void handle_client(int client_socket) {
  SocketGuard guard(client_socket);

  char buffer[1024] = {0};
  int valread = read(client_socket, buffer, sizeof(buffer));
  if (valread > 0) {
    std::string msg(buffer, valread);
    TSLogger::instance().info("Mensagem recebida: " + msg);
    send(client_socket, msg.c_str(), msg.size(), 0);
    TSLogger::instance().info("Mensagem enviada de volta: " + msg);
  } else {
    TSLogger::instance().warn("Cliente desconectado ou leitura vazia.");
  }
}

int main() {
  try {
    TSLogger::instance().init("server.log", false);
  } catch (const std::exception &e) {
    std::cerr << "Logger init failed: " << e.what() << '\n';
    return 1;
  }

  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == -1) {
    TSLogger::instance().error("Erro ao criar socket servidor");
    return 1;
  }

  int opt = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  sockaddr_in address{};
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    TSLogger::instance().error("Erro no bind");
    close(server_fd);
    return 1;
  }

  TSLogger::instance().info("Servidor iniciado na porta " +
                            std::to_string(PORT));

  if (listen(server_fd, 5) < 0) {
    TSLogger::instance().error("Erro no listen");
    close(server_fd);
    return 1;
  }

  while (true) {
    sockaddr_in client_addr{};
    socklen_t addrlen = sizeof(client_addr);
    int client_sock =
        accept(server_fd, (struct sockaddr *)&client_addr, &addrlen);

    if (client_sock < 0) {
      TSLogger::instance().warn("Falha ao aceitar cliente.");
      continue;
    }

    TSLogger::instance().info("Novo cliente conectado.");
    std::thread(handle_client, client_sock).detach();
  }

  close(server_fd);
  TSLogger::instance().shutdown();
  return 0;
}
