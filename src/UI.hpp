#ifndef UI_hpp
#define UI_hpp
#include <memory>
#include <string>
using namespace std;

class UI {
public:
  virtual string input_from_user() = 0;
  virtual void output_to_user(const string &message) = 0;
  static unique_ptr<UI> get_console_ui();
  virtual void notify(const string &text) = 0;
  virtual void disconnect() = 0;
  virtual ~UI() = 0;
};

#endif /* UI_hpp */
