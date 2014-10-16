#pragma once
#include <autowiring/autowiring.h>

struct ShortcutsContext {};

class Shortcuts :
  public DispatchQueue,
  public ExceptionFilter
{
public:
  Shortcuts(void);
  ~Shortcuts(void);

public:
  void Main(void);
  void Filter(void) override;
};
