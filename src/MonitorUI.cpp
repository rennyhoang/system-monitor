#include "MonitorUI.hpp"
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

Element MonitorUI::Render(const SystemMonitor &monitor) {
  // 1. CPU Section
  // gauge(1.0) is full, gauge(0.0) is empty.
  float cpu_val = static_cast<float>(monitor.GetCpuUsage() / 100.0);

  // 2. Memory Section
  float mem_val = static_cast<float>(monitor.GetMemoryFraction());
  std::string mem_label = monitor.GetMemoryString();

  int uptime = monitor.GetUptime();
  std::vector<float> cpu_core_usage = [monitor]() {
    auto tmp = monitor.GetCpuCores();
    return std::vector<float>(begin(tmp), end(tmp));
  }();

  return vbox({hbox({text("System Monitor") | bold | flex,
                     text("Uptime: " + std::to_string(uptime) + "s") |
                         align_right}),
               separator(),

               // CPU Row
               hbox({text("CPU Usage: ") | size(WIDTH, EQUAL, 12),
                     gauge(cpu_val) | flex,
                     text(" " + std::to_string(int(monitor.GetCpuUsage())) +
                          "%")}),
               RenderList<std::vector<float>>(cpu_core_usage), separator(),

               // Memory Row
               hbox({text("Memory:    ") | size(WIDTH, EQUAL, 12),
                     gauge(mem_val) | flex, text(" " + mem_label)}),

               separator(), text("Press 'q' to quit.") | dim | hcenter}) |
         border | size(WIDTH, GREATER_THAN, 60);
}
