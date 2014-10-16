#pragma once
#include <autowiring/autowiring.h>

struct VRShellContext {};

int VRShell_main(int argc, char **argv);

class VRShell :
  public DispatchQueue,
  public ExceptionFilter
{
public:
  VRShell(void);
  ~VRShell(void);

  void Main(void);
  void Filter(void) override;
};
