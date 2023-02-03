#ifndef Context_hpp
#define Context_hpp

#include <memory>

using namespace std;

class ClientState;

typedef unique_ptr<ClientState> state_ptr;

class Context {
public:
  virtual void set_state(state_ptr state) = 0;
};
#endif /* Context_hpp */
