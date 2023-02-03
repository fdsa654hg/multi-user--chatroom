#include "Client.hpp"
#include "UI.hpp"
#include <codecvt>
#include <iostream>
#include <locale>
#include <memory>
#include <regex>
#include <string>

using namespace std;

int main() { Client client(&UI::get_console_ui); }
