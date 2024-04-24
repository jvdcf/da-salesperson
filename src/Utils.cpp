#include <cmath>
#include <cstdint>
#include <exception>
#include <iostream>
#include <ostream>
#include <string>
#include "Utils.h"



[[noreturn]] void panic(std::string s) {
  auto c = Color(255,100,100);
  std::cerr << c.foreground() << "[CRITICAL ERR] " << c.clear() << s << std::endl;
  std::exit(1);
}

void error(std::string s) {
  auto c = Color(255,100,0);
  std::cerr << c.foreground() << "[ERROR] " << c.clear() << s << std::endl;
}

void info(std::string s) {
  auto c = Color(0,235,235);
  std::cerr << c.foreground() << "[INFO] " << c.clear() << s << std::endl;
}

void warning(std::string s) {
  auto c = Color(255,255,15);
  std::cerr << c.foreground() <<"[WARNING] " << c.clear() << s << std::endl;
}

// TODO