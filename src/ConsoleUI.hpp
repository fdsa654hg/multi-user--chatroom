#ifndef ConsoleUI_hpp
#define ConsoleUI_hpp

#include <iostream>
#include <string>
using namespace std;
#include "UI.hpp"

class ConsoleUI : public UI {
public:
  virtual string input_from_user();
  virtual void output_to_user(const string &message);
  virtual void notify(const string &text);
  virtual void disconnect();
  ~ConsoleUI();
};

#endif /* ConsoleUI_hpp */
