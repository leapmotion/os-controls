#pragma once

/// <summary>
/// Stores application-wide settings on a per-user basis
/// </summary>
class Config {
public:
  Config(void);

public:
  std::chrono::microseconds GetFrameRate(void) const;
};