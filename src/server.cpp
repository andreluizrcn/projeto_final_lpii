#include "libtslog.h"
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <thread>
#include <unistd.h>

#define PORT 8080

void handle_client(int client_socket) {
  char buffer[1024] = {0};
  int valread = read(client_socket, buffer, 1024);
  if (valread > 0) {
    std::string msg(buffer, valread);
    TSLogger::instance().info("Mensagem recebida: " + msg);

    // Echo back
    send(client_socket, msg.c_str(), msg.size(), 0);
    TSLogger::instance().info("Mensagem enviada de volta: " + msg);
  }
  close(client_socket);
}

int main() {
  try {
    TSLogger::instance().init("server.log", false);
  } catch (const std::exception &e) {
    std::cerr << "Logger init failed: " << e.what() << '\n';
    return 1;
  }

  int server_fd;
  struct sockaddr_in address{};
  int addrlen = sizeof(address);

  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    TSLogger::instance().error("Erro ao criar socket");
    return 1;
  }

  int opt = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    TSLogger::instance().error("Erro no bind");
    return 1;
  }

  TSLogger::instance().info("Servidor iniciado na porta " +
                            std::to_string(PORT));

  if (listen(server_fd, 5) < 0) {
    TSLogger::instance().error("Erro no listen");
    return 1;
  }

  while (true) {
    int new_socket =
        accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
    if (new_socket < 0) {
      TSLogger::instance().error("Erro no accept");
      continue;
    }
    TSLogger::instance().info("Novo cliente conectado");

    std::thread t(handle_client, new_socket);
    t.detach();
  }

  TSLogger::instance().shutdown();
  return 0;
}
