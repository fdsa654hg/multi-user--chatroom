#include "Server.hpp"
#define MAX_CLIENT_NAME 20
const int Server::max_len = 1024;

Server::Server(int port, const string &host) : port(port) {

  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr)); // initializes
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  server = socket(PF_INET, SOCK_STREAM, 0);
  if (server < 0) {
    cerr << "Fail to create a socket." << endl;
    exit(-1);
  }
  int option = 1;
  setsockopt(server, SOL_SOCKET, SO_REUSEADDR, (char *)&option, sizeof(option));
  if (bind(server, (const struct sockaddr *)&server_addr, sizeof(server_addr)) <
      0) {
    cerr << "Bind error" << endl;
    exit(-1);
  }

  if (listen(server, SOMAXCONN) < 0) {
    cerr << "Listen error" << endl;
    exit(-1);
  }

  FD_ZERO(&recv_set);
  cout << "Server " << inet_ntoa(server_addr.sin_addr)
            << ntohs(server_addr.sin_port) << " --- Ready\n"
            << endl;
  thread t_accept(&Server::accept_handler, this, server);
  t_accept.detach();
  create_private_client(server_addr);
}

regex Server::prefix_pattern = regex("<(.*?)>.*");

void _send(int client, const string &message) {
  cout << "_send " << message << endl;
  uint32_t count = message.length() * sizeof(message[0]);
  cout << "sended message" << message << endl;
  count = htonl(count);
  send(client, (char *)&count, 4, 0);
  send(client, (char *)message.c_str(), message.length() * sizeof(message[0]),
       0);
}

void Server::public_message(const string &message) {
  mtx.lock();
  cout << "public message" << endl;
  string complete_message = string("<message>") + message;
  for_each(name2socket.begin(), name2socket.end(),
                [complete_message](auto name_socket) {
                  _send(name_socket.second, complete_message);
                });
  mtx.unlock();
}

void Server::register_new_client(string &name, int client) {
  name2socket[name] = client;
  socket2name[client] = name;
  FD_SET(client, &recv_set);
  cout << "user name" << name.c_str() << " accepted" << endl;
  _send(client, "confirmed");
  public_message(name + " has entered the room");
  send(private_client, "1", 1, 0);
  return;
}

void Server::register_psuedo_client(int client) {
  private_client = client;
  private_client = client;
  FD_SET(client, &recv_set);
  thread t_recv(&Server::recv_handler, this);
  t_recv.detach();
  return;
}

void Server::connection_handler(int client) {
  string message("username");
  _send(client, message);
  char recv_buf[max_len];
  memset(recv_buf, 0, max_len);
  while (true) {
    cout << "recving" << endl;
    auto r = recv(client, (char *)recv_buf, max_len, 0);
    if (r == 0) {
      cout << "disconnected during username" << endl;
      return;
    }
    string name(recv_buf);
    cout << "received " << name << endl;
    if (name == string("psuedo client")) {
      register_psuedo_client(client);
      return;
    }
    if (name2socket.find(name) == name2socket.end()) {
      register_new_client(name, client);
      return;
    } else {
      _send(client, "conflict");
      cout << "user name refused" << endl;
    }
  }
}

void Server::recv_handler() {
  while (true) {
    select(FD_SETSIZE, &recv_set, NULL, NULL, NULL);
    for (auto name_socket : name2socket) {
      int client = name_socket.second;
      if (FD_ISSET(client, &recv_set)) {
        char buff[1024];
        memset(buff, 0, 1024);
        auto r = recv(client, (char *)buff, 1024, 0);
        cout << "got message" << endl;
        if (r == 0) {
          on_client_leave(client);
          break;
        }
        string message(buff);
        smatch match;
        regex_match(message, match, prefix_pattern);
        if (match.size() < 2) {
          cout << "no matched prefix to " << message << endl;
          break;
        }
        cout << "got prefix:" << match[1] << endl;
        prefix2handler[match[1]](*this, message, socket2name[client]);
      }
    }
    for_each(name2socket.begin(), name2socket.end(),
                  [this](auto name_socket) {
                    FD_SET(name_socket.second, &this->recv_set);
                  });
  }
}

void Server::accept_handler(int server) {
  while (true) {
    struct sockaddr_in clientAddr;
    socklen_t client_len = sizeof(clientAddr);
    auto client = accept(server, (struct sockaddr *)&clientAddr, &client_len);
    thread newCon(&Server::connection_handler, this, client);
    newCon.detach();
    FD_SET(client, &recv_set);
    cout << "Accept connect request from [ " << inet_ntoa(clientAddr.sin_addr)
         << ":" << ntohs(clientAddr.sin_port) << "]\n";
  }
}

void Server::create_private_client(struct sockaddr_in server_addr) {
  int private_server = socket(PF_INET, SOCK_STREAM, 0);
  auto *localhost = "127.0.0.1";
  struct sockaddr_in locahost_addr;
  locahost_addr.sin_family = AF_INET;
  locahost_addr.sin_port = htons(port);
  inet_pton(AF_INET, localhost, &(locahost_addr.sin_addr));

  auto result = connect(private_server, (struct sockaddr *)&locahost_addr,
                        sizeof(locahost_addr));
  if (result < 0) {
    cerr << "connect error" << endl;
    exit(-1);
  }
  char buff[1024];
  while (true) {
    recv(private_server, buff, max_len, 0);
    send(private_server, (char *)"psuedo client", max_len, 0);
  }
}

void Server::on_client_leave(int client) {
  cout << "client leave" << endl;
  string name = socket2name[client];
  socket2name.erase(socket2name.find(client));
  name2socket.erase(name2socket.find(name));
  public_message(name + string(" has leaved the room"));
}

void Server::private_confirm(const string &target, const string &speaker) {
  int client = name2socket[speaker];
  cout << "confirming " << target << endl;
  if (name2socket.find(target) != name2socket.end())
    return _send(client, "confirmed");
  else
    return _send(client, "no user");
}

void Server::private_message(const string &message,
                             const string &speaker) {
  //<private><target>message
  mtx.lock();
  smatch match;
  regex_search(message.cbegin() + 9, message.cend(), match, prefix_pattern);
  cout << message << endl;
  string target = match[1];
  if (name2socket.find(target) == name2socket.end())
    _send(name2socket[speaker], "disconnected");

  auto displace = match[1].second - message.cbegin() + 1;
  cout << "disaplce:" << displace << endl;
  // <message><private user> says:
  string complete_message = string("<message><private ") + speaker + ">" +
                            string(" says:") + message.substr(displace);
  cout << complete_message << endl;
  _send(name2socket[target], complete_message);
  mtx.unlock();
}

Server::~Server() { close(server); }
