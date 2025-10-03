#include "libtslog.h"
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <unistd.h>

#define PORT 8080
#define SERVER "127.0.0.1"

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "Uso: " << argv[0] << " <mensagem>\n";
    return 1;
  }

  try {
    TSLogger::instance().init("client.log", true);
  } catch (const std::exception &e) {
    std::cerr << "Logger init failed: " << e.what() << '\n';
    return 1;
  }

  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    TSLogger::instance().error("Erro ao criar socket");
    return 1;
  }

  struct sockaddr_in serv_addr{};
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);

  if (inet_pton(AF_INET, SERVER, &serv_addr.sin_addr) <= 0) {
    TSLogger::instance().error("Endereço inválido");
    return 1;
  }

  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    TSLogger::instance().error("Conexão falhou");
    return 1;
  }

  std::string msg = argv[1];
  send(sock, msg.c_str(), msg.size(), 0);
  TSLogger::instance().info("Mensagem enviada: " + msg);

  char buffer[1024] = {0};
  int valread = read(sock, buffer, 1024);
  if (valread > 0) {
    std::string response(buffer, valread);
    TSLogger::instance().info("Resposta recebida: " + response);
    std::cout << "Resposta: " << response << '\n';
  }

  close(sock);
  TSLogger::instance().shutdown();
  return 0;
}
