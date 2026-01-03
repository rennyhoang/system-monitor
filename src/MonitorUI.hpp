#pragma once
#include "SystemInfo.hpp"
#include <ftxui/dom/elements.hpp>

class MonitorUI {
public:
  // Pure function: Takes data, returns a UI element
  static ftxui::Element Render(const SystemMonitor &monitor);
  template <typename T> static ftxui::Element RenderList(const T &values) {
    ftxui::Elements entries;
    for (const auto &val : values) {
      entries.push_back(ftxui::gaugeUp(val) | ftxui::border);
    }
    return hbox(std::move(entries));
  }
};
