#include "ClientState.hpp"
#include "ConcreteClientState.hpp"

ClientState::ClientState(Context &context) : context(context) {}

string ClientState::_process_send(const string &message) {
  if (message.find("/r") == 0 && !response_private.empty()) {
    auto private_state =
        make_unique<PrivateMessageState>(context, response_private);
    string response_message = private_state->process_send(message);
    context.set_state(move(private_state));
    return response_message;
  } else {
    return process_send(message);
  }
}

string ClientState::_process_recv(const string &message) {
  if (message.find("<private") == 9) {
    int sender_begin = 18;
    int sender_end = message.find("> : ");
    string sender = message.substr(sender_begin, sender_end - sender_begin);
    response_private = sender;
  }
  if (message.find("<message>") == 0) {
    return message.substr(9);
  } else {
    return process_recv(message);
  }
}

PublicMessageState::PublicMessageState(Context &context)
    : ClientState(context) {}

string PublicMessageState::process_send(const string &message) {
  if (target.empty() && (message.find("/t ") == 0)) {
    target = message.substr(3);
    return "<private confirm>" + message.substr(3);
  }
  return "<public>" + message;
}

string PublicMessageState::ui_text() { return "<public mode>: "; }

string PublicMessageState::process_recv(const string &message) {
  if (!target.empty() && message == string("confirmed")) {
    context.set_state(make_unique<PrivateMessageState>(context, target));
    return string();
  } else if (!target.empty()) {
    string response = target + " not found";
    target = string();
    return response;
  }
  return message;
}

PrivateMessageState::PrivateMessageState(Context &context, const string &target)
    : ClientState(context), target(target) {
  prefix = string("<private><") + target + string(">");
}

string PrivateMessageState::process_send(const string &message) {
  if (message.find("/s") == 0) {
    context.set_state(make_unique<PublicMessageState>(context));
    return "<ignore>";
  }
  return prefix + message;
}

string PrivateMessageState::process_recv(const string &message) {
  if (message == string("disconnected")) {
    context.set_state(make_unique<PublicMessageState>(context));
    return string(" user " + target + "has diconnected");
  } else {
    return message;
  }
}

string PrivateMessageState::ui_text() {
  return "<private mode to " + target + ">:";
}

UserNameState::UserNameState(Context &context) : ClientState(context) {}

string UserNameState::process_send(const string &message) {
  return message;
}
string UserNameState::process_recv(const string &message) {
  if (message == string("confirmed")) {
    context.set_state(make_unique<PublicMessageState>(context));
    return string("username confirmed");
  } else if (message == string("conflict")) {
    return string("username conflicted");
  } else
    return string();
}

string UserNameState::ui_text() { return "enter yor username: "; }

state_ptr ClientState::get_initial_state(Context &context) {
  return make_unique<UserNameState>(context);
}
