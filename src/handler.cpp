#include "Server.hpp"
auto public_handler = [](Server &socket_server, string &message,
                         const string &speaker) {
  cout << "public_handler" << endl;
  socket_server.public_message(speaker + string(" says:") + message.substr(8));
};

auto private_handler = [](Server &socket_server, string &message,
                          const string &speaker) {
  cout << "private_handler" << endl;
  socket_server.private_message(message, speaker);
};

auto ignore_handler = [](Server &socket_server, string &message,
                         const string &speaker) { cout << "ignore" << endl; };

auto private_confirm_handler = [](Server &socket_server, string &message,
                                  const string &speaker) {
  cout << "private_confirm_handler" << endl;
  socket_server.private_confirm(message.substr(17), speaker);
};

unordered_map<string, handler> Server::prefix2handler{
    {"public", public_handler},
    {"private", private_handler},
    {"private confirm", private_confirm_handler},
    {"ignore", ignore_handler}

};