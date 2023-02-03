#include "UI.hpp"
#include "ConsoleUI.hpp"

unique_ptr<UI> UI::get_console_ui() { return make_unique<ConsoleUI>(); }

UI::~UI() {}
