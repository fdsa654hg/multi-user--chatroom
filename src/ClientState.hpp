
#ifndef ClientState_hpp
#define ClientState_hpp
#include "Context.hpp"
#include <iostream>
#include <string>
using namespace std;
class ClientState;
typedef unique_ptr<ClientState> state_ptr;

class ClientState {
public:
  ClientState(Context &context);
  string _process_send(const string &message);
  string _process_recv(const string &message);
  static state_ptr get_initial_state(Context &context);
  virtual string ui_text() = 0;
  virtual ~ClientState(){};

protected:
  Context &context;
  virtual string process_send(const string &message) = 0;
  virtual string process_recv(const string &message) = 0;
  string response_private;
};
#endif /* ClientState_hpp */
