#include "ncurses_display.h"
#include "system.h"
#include <csignal>
#include <thread>
#include <chrono>
#include <iostream>

bool running = true;

void signalHandler(int signum) {
  running = false;
  endwin();
  std::cout << "\nExiting System Monitor...\n";
  exit(signum);
}

int main() {
  signal(SIGINT, signalHandler);
  System system;
  std::thread displayThread([&]() {
    NCursesDisplay::Display(system, 15);
  });
  while (running) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  displayThread.join();
  return 0;
}
