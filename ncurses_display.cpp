#define _XOPEN_SOURCE_EXTENDED
#include <ncursesw/ncurses.h>
#include "ncurses_display.h"

#include <chrono>
#include <string>
#include <thread>
#include <vector>

#include "format.h"
#include "system.h"

using std::string;
using std::to_string;

// Generate dynamic progress bar (used for CPU & Memory)
std::string NCursesDisplay::ProgressBar(float percent) {
  std::string result{"["};
  int size{50};
  int bars = static_cast<int>(percent * size);
  for (int i = 0; i < size; ++i) {
    result += (i < bars) ? '|' : ' ';
  }
  result += "] " + to_string(static_cast<int>(percent * 100)) + "%";
  return result;
}

// Display overall system information
void NCursesDisplay::DisplaySystem(System& system, WINDOW* window) {
  int row{1};
  wattron(window, COLOR_PAIR(3) | A_BOLD);
  mvwprintw(window, row++, 2, "=== SYSTEM INFORMATION ===");
  wattroff(window, COLOR_PAIR(3) | A_BOLD);

  mvwprintw(window, row++, 2, ("OS: " + system.OperatingSystem()).c_str());
  mvwprintw(window, row++, 2, ("Kernel: " + system.Kernel()).c_str());

  wattron(window, COLOR_PAIR(1));
  mvwprintw(window, row++, 2, "CPU Usage:");
  mvwprintw(window, row++, 4, ProgressBar(system.Cpu().Utilization()).c_str());
  wattroff(window, COLOR_PAIR(1));

  wattron(window, COLOR_PAIR(2));
  mvwprintw(window, row++, 2, "Memory Usage:");
  mvwprintw(window, row++, 4, ProgressBar(system.MemoryUtilization()).c_str());
  wattroff(window, COLOR_PAIR(2));

  mvwprintw(window, row++, 2,
            ("Total Processes: " + to_string(system.TotalProcesses())).c_str());
  mvwprintw(window, row++, 2,
            ("Running Processes: " + to_string(system.RunningProcesses())).c_str());
  mvwprintw(window, row++, 2,
            ("Uptime: " + Format::ElapsedTime(system.UpTime())).c_str());

  wattron(window, COLOR_PAIR(4));
  mvwprintw(window, row + 1, 2, "Press 'q' to quit");
  wattroff(window, COLOR_PAIR(4));

  box(window, 0, 0);
  wrefresh(window);
}

// Display top processes in tabular form
void NCursesDisplay::DisplayProcesses(std::vector<Process>& processes,
                                      WINDOW* window, int n) {
  werase(window);
  box(window, 0, 0);
  int row{1};
  int const pid_column{2};
  int const user_column{8};
  int const cpu_column{20};
  int const ram_column{30};
  int const time_column{40};
  int const command_column{52};

  wattron(window, COLOR_PAIR(3) | A_BOLD);
  mvwprintw(window, row++, pid_column, "PID");
  mvwprintw(window, row - 1, user_column, "USER");
  mvwprintw(window, row - 1, cpu_column, "CPU%");
  mvwprintw(window, row - 1, ram_column, "RAM(MB)");
  mvwprintw(window, row - 1, time_column, "TIME+");
  mvwprintw(window, row - 1, command_column, "COMMAND");
  wattroff(window, COLOR_PAIR(3) | A_BOLD);

  for (int i = 0; i < n && i < (int)processes.size(); ++i) {
    mvwprintw(window, row, pid_column, to_string(processes[i].Pid()).c_str());
    mvwprintw(window, row, user_column, processes[i].User().c_str());
    float cpu = processes[i].CpuUtilization() * 100;
    mvwprintw(window, row, cpu_column, to_string(cpu).substr(0, 4).c_str());
    mvwprintw(window, row, ram_column, processes[i].Ram().c_str());
    mvwprintw(window, row, time_column,
              Format::ElapsedTime(processes[i].UpTime()).c_str());
    mvwprintw(window, row, command_column,
              processes[i].Command().substr(0, 25).c_str());
    row++;
  }

  wrefresh(window);
}

// Main monitor loop — runs until 'q' key is pressed
void NCursesDisplay::Display(System& system, int n) {
  initscr();
  noecho();
  cbreak();
  curs_set(0);
  start_color();

  // Color pairs (theme)
  init_pair(1, COLOR_BLUE, COLOR_BLACK);
  init_pair(2, COLOR_GREEN, COLOR_BLACK);
  init_pair(3, COLOR_YELLOW, COLOR_BLACK);
  init_pair(4, COLOR_CYAN, COLOR_BLACK);

  int height_sys = 12;
  int height_proc = n + 5;
  int width = COLS - 2;

  WINDOW* system_window = newwin(height_sys, width, 0, 1);
  WINDOW* process_window = newwin(height_proc, width, height_sys + 1, 1);

  nodelay(stdscr, TRUE);  // Non-blocking keyboard input
  keypad(stdscr, TRUE);

  while (true) {
    werase(system_window);
    werase(process_window);

    DisplaySystem(system, system_window);
    DisplayProcesses(system.Processes(), process_window, n);

    refresh();

    // Exit when 'q' or 'Q' is pressed
    int ch = getch();
    if (ch == 'q' || ch == 'Q') {
      break;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(700));
  }

  delwin(system_window);
  delwin(process_window);
  endwin();
}
