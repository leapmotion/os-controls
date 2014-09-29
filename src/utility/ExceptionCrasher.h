#pragma once
#include <autowiring/ExceptionFilter.h>

//An exception filter to override our nice shutdown behavior and crash the program so we can get crash dumps.
class Object;

class ExceptionCrasher : public ExceptionFilter {
public:
  void Filter() override { throw; }
  void Filter(const JunctionBoxBase* pJunctionBox, Object* pRecipient) override { throw; };

};