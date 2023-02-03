#ifndef Server_hpp
#define Server_hpp

#include <functional>
#include <iostream>
#include <mutex>
#include <regex>
#include <string>
#include <thread>
#include <unordered_map>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

using std::cerr;
using std::cout;
using std::endl;
using std::function;
using std::regex;
using std::string;
using std::thread;
using std::unordered_map;
using std::smatch;
using std::for_each;

class Server;

typedef function<void(Server &, string &message, const string &speaker)>
    handler;

class Server {
public:
  Server(int port, const string &host = "localhost");
  void accept_handler(int server);
  void public_message(const string &message);
  void private_message(const std::string &message, const string &speaker);
  void private_confirm(const string &target, const string &speaker);
  ~Server();

private:
  static regex prefix_pattern;
  static const int max_len;

  void connection_handler(int client);
  void recv_handler();
  void create_private_client(struct sockaddr_in server_addr);
  void register_new_client(string &name, int client);
  void register_psuedo_client(int client);
  void on_client_leave(int client);
  int server;
  int private_client;
  unordered_map<string, int> name2socket;
  unordered_map<int, string> socket2name;
  static unordered_map<string, handler> prefix2handler;
  fd_set recv_set;
  std::mutex mtx;
  int port;
};

#endif /* Server_hpp */