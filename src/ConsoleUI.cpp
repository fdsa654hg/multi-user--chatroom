#include "ConsoleUI.hpp"

string ConsoleUI::input_from_user() {
  string line;
  getline(cin, line);
  return line;
}

void ConsoleUI::output_to_user(const string &message) {
  if (!message.empty())
    cout << message << endl;
}

void ConsoleUI::notify(const string &text) { cout << text << endl; }

void ConsoleUI::disconnect() { cout << L"server disconnected" << endl; }

ConsoleUI::~ConsoleUI() {}
