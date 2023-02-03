#ifndef Client_hpp
#define Client_hpp

#include <algorithm>
#include <functional>
#include <iostream>
#include <mutex>
#include <regex>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include "Context.hpp"

using namespace std;

class Server;
class ClientState;
class UI;

typedef unique_ptr<ClientState> state_ptr;
typedef function<void(Server &, string &, const string &speaker)> handler;
typedef std::vector<int> socket_vect;
typedef unique_ptr<UI> ui_ptr;
typedef std::function<ui_ptr(void)> t_ui_getter;

class Client : public Context {
public:
  Client(const t_ui_getter &ui_getter, int port = 3000);
  virtual void set_state(state_ptr state);
  ~Client();

private:
  static const int max_len;
  void recv_handler();
  void send_handler();
  state_ptr state;
  int server;
  mutex mtx;
  ui_ptr ui;
};

#endif /* Client_hpp */
