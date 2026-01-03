#include <atomic>
#include <chrono>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <thread>

#include "MonitorUI.hpp"
#include "SystemInfo.hpp"

int main() {
  using namespace ftxui;

  SystemMonitor monitor;
  auto screen = ScreenInteractive::Fullscreen();

  // State to control the update loop
  std::atomic<bool> running{true};

  // Create a thread to update data every second
  std::thread updater([&]() {
    while (running) {
      std::this_thread::sleep_for(std::chrono::seconds(1));

      // 1. Fetch new data
      monitor.Update();

      // 2. Trigger a redraw.
      // Post(Event::Custom) is thread-safe and wakes up the main loop.
      screen.Post(Event::Custom);
    }
  });

  // Define the component that catches events and renders the UI
  auto component = Renderer([&] { return MonitorUI::Render(monitor); });

  // Add a 'Catch' to handle the 'q' key for quitting
  component = CatchEvent(component, [&](Event event) {
    if (event == Event::Character('q')) {
      screen.Exit();
      running = false;
      return true;
    }
    return false;
  });

  // Start the event loop
  screen.Loop(component);

  // Cleanup
  running = false;
  if (updater.joinable()) {
    updater.join();
  }

  return 0;
}
