#include "Client.hpp"
#include "ClientState.hpp"
#include "UI.hpp"

const int Client::max_len = 1024;

void Client::set_state(state_ptr state) {
  this->state = move(state);
  ui->notify(this->state->ui_text());
}

void Client::send_handler() {
  string message;
  while (true) {
    message = ui->input_from_user();
    if (message.size() > 0) {
      mtx.lock();
      message = state->_process_send(message);
      send(server, (char *)message.c_str(),
           message.length() * sizeof(message[0]), 0);
      mtx.unlock();
    }
  }
}

void Client::recv_handler() {
  char buf[max_len];
  uint32_t count_buf[1];
  while (true) {
    memset(count_buf, 0, 4);
    memset(buf, 0, max_len);
    auto r = recv(server, (char *)count_buf, 4, 0);
    if (r == 0) {
      ui->disconnect();
      break;
    }
    uint32_t count = ntohl(count_buf[0]);
    recv(server, (char *)buf, count, 0);
    string message(buf);

    mtx.lock();
    string response = state->_process_recv(message);
    ui->output_to_user(response);
    mtx.unlock();
  }
}

Client::Client(const t_ui_getter &ui_getter, int port) {

  ui = ui_getter();
  ui->output_to_user("enter server ip:");
  string ip = ui->input_from_user();
  string server_ip;
  if (ip.empty())
    server_ip = "127.0.0.1";

  cout << server_ip << endl;
  ui->notify("connecting...");

  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr)); // initializes
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr.s_addr = INADDR_ANY;
  inet_pton(AF_INET, server_ip.c_str(), &(server_addr.sin_addr));

  server = socket(PF_INET, SOCK_STREAM, 0);
  if (server < 0) {
    cerr << "Fail to create a socket." << endl;
    exit(-1);
  }

  if (connect(server, (const struct sockaddr *)&server_addr,
              sizeof(server_addr)) < 0) {
    cerr << "Connect error" << endl;
    exit(-1);
  }

  set_state(ClientState::get_initial_state(*this));
  thread t_send = thread(&Client::send_handler, this);
  t_send.detach();
  thread t_recv = thread(&Client::recv_handler, this);
  t_recv.join();
}

Client::~Client() { close(server); }
