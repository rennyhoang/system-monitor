#include "SystemInfo.hpp"
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/sysinfo.h>

SystemMonitor::SystemMonitor() {
  // Initialize initial CPU stats so the first update has a baseline
  prev_cpu_ = ReadCpuStats();
  curr_cpu_ = prev_cpu_;

  prev_cores_ = ReadCoreStats();
  curr_cores_ = prev_cores_;
  core_usage_percent_.resize(curr_cores_.size());
  std::fill(begin(core_usage_percent_), end(core_usage_percent_), 0);
}

void SystemMonitor::Update() {
  // Update CPU
  curr_cpu_ = ReadCpuStats();
  curr_cores_ = ReadCoreStats();

  unsigned long long active_diff = curr_cpu_.active - prev_cpu_.active;
  unsigned long long total_diff = curr_cpu_.total - prev_cpu_.total;

  std::vector<unsigned long long> active_diffs, total_diffs;
  for (std::size_t i = 0; i < curr_cores_.size(); ++i) {
    active_diffs.push_back(curr_cores_[i].active - prev_cores_[i].active);
    total_diffs.push_back(curr_cores_[i].total - prev_cores_[i].total);
    core_usage_percent_[i] = static_cast<double>(active_diffs[i]) /
                             static_cast<double>(total_diffs[i]);
  }

  if (total_diff > 0) {
    cpu_usage_percent_ =
        (static_cast<double>(active_diff) / static_cast<double>(total_diff)) *
        100.0;
  } else {
    cpu_usage_percent_ = 0.0;
  }

  // Prepare for next delta
  prev_cpu_ = curr_cpu_;
  prev_cores_ = curr_cores_;

  // Update Memory
  struct sysinfo info;
  if (sysinfo(&info) == 0) {
    unsigned long long total_ram = info.totalram;
    unsigned long long free_ram = info.freeram;
    unsigned long long mem_unit = info.mem_unit;

    // sysinfo's freeram doesn't count buffers/cache, but for a simple monitor
    // this is a sufficient approximation of "available" memory vs "used".
    unsigned long long used_ram = total_ram - free_ram;

    // Convert to GB
    const double GB = 1024.0 * 1024.0 * 1024.0;
    mem_data_.total_gb = static_cast<double>(total_ram * mem_unit) / GB;
    mem_data_.used_gb = static_cast<double>(used_ram * mem_unit) / GB;
    mem_data_.fraction =
        static_cast<double>(used_ram) / static_cast<double>(total_ram);
  }
}

double SystemMonitor::GetCpuUsage() const { return cpu_usage_percent_; }

std::string SystemMonitor::GetMemoryString() const {
  std::stringstream ss;
  ss << std::fixed << std::setprecision(1) << mem_data_.used_gb << " / "
     << mem_data_.total_gb << " GB";
  return ss.str();
}

double SystemMonitor::GetMemoryFraction() const { return mem_data_.fraction; }

SystemMonitor::CpuData SystemMonitor::ReadCpuStats() {
  std::ifstream file("/proc/stat");
  std::string line;
  std::string label;

  // The first line is "cpu  user nice system idle iowait irq softirq steal
  // guest guest_nice"
  if (std::getline(file, line)) {
    std::stringstream ss(line);
    ss >> label; // "cpu"

    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;
    ss >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;

    unsigned long long active = user + nice + system + irq + softirq + steal;
    unsigned long long total = active + idle + iowait;

    return {active, total};
  }
  return {0, 0};
}

int SystemMonitor::GetUptime() const {
  std::ifstream file("/proc/uptime");
  std::string line;
  std::string uptime;
  if (std::getline(file, line)) {
    std::stringstream ss(line);
    ss >> uptime;

    return stoi(uptime);
  }
  return 0;
}

std::vector<SystemMonitor::CpuData> SystemMonitor::ReadCoreStats() {
  std::ifstream file("/proc/stat");
  std::string line;
  std::string label;
  std::vector<SystemMonitor::CpuData> core_usage{};
  while (std::getline(file, line)) {
    std::stringstream ss(line);
    ss >> label;

    if (label.find("cpu") == std::string::npos || label.size() <= 3)
      continue;

    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;
    ss >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;

    unsigned long long active = user + nice + system + irq + softirq + steal;
    unsigned long long total = active + idle + iowait;
    core_usage.push_back({active, total});
  }
  return core_usage;
}

std::vector<double> SystemMonitor::GetCpuCores() const {
  return core_usage_percent_;
}
