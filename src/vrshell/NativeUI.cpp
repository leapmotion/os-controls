#include "stdafx.h"
#include "NativeUI.h"

#include <autowiring/Autowired.h>

void NativeUI::OnQuit() {
  GetContext()->SignalShutdown(false);
}
