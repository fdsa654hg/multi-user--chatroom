#ifndef ConcreteClientState_hpp
#define ConcreteClientState_hpp

#include "ClientState.hpp"
#include <string>
using namespace std;

class PublicMessageState : public ClientState {
public:
  PublicMessageState(Context &context);
  virtual string process_send(const string &message);
  virtual string ui_text();

protected:
  virtual string process_recv(const string &message);

private:
  string target;
};

class PrivateMessageState : public ClientState {
public:
  PrivateMessageState(Context &context, const string &target);
  virtual string process_send(const string &message);
  virtual string ui_text();

protected:
  virtual string process_recv(const string &message);

private:
  string target;
  string prefix;
};

class UserNameState : public ClientState {
public:
  UserNameState(Context &context);
  virtual string process_send(const string &message);
  virtual string ui_text();

protected:
  virtual string process_recv(const string &message);
};

#endif
