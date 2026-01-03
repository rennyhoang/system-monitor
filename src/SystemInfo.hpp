#pragma once
#include <string>
#include <vector>

class SystemMonitor {
public:
  SystemMonitor();

  // Refreshes CPU and Memory stats
  void Update();

  // Returns CPU usage as a percentage (0.0 to 100.0)
  double GetCpuUsage() const;

  // Returns a string like "2.4 / 16.0 GB"
  std::string GetMemoryString() const;

  int GetUptime() const;

  // Returns memory usage fraction (0.0 to 1.0) for the gauge
  double GetMemoryFraction() const;

  std::vector<double> GetCpuCores() const;

private:
  // Helper to read /proc/stat
  struct CpuData {
    unsigned long long active;
    unsigned long long total;
  };
  CpuData ReadCpuStats();
  std::vector<CpuData> ReadCoreStats();

  CpuData prev_cpu_;
  CpuData curr_cpu_;
  double cpu_usage_percent_ = 0.0;

  std::vector<CpuData> prev_cores_;
  std::vector<CpuData> curr_cores_;
  std::vector<double> core_usage_percent_{};

  // Memory data
  struct MemData {
    double used_gb;
    double total_gb;
    double fraction;
  };
  MemData mem_data_;
};
