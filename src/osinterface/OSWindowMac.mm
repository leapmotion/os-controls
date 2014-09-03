// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "OSWindowMac.h"

OSWindowMac::~OSWindowMac(void)
{
}

bool OSWindowMac::IsValid(void) {
  return true;
}

std::shared_ptr<OSApp> OSWindowMac::GetOwnerApp(void) {
  return nullptr;
}

uint32_t OSWindowMac::GetOwnerPid(void) {
  return 0;
}

void OSWindowMac::GetWindowTexture(ImagePrimitive& texture) {
  return OSWindow::GetWindowTexture(texture);
}

bool OSWindowMac::GetFocus(void) {
  return false;
}

void OSWindowMac::SetFocus(void) {
}

std::vector<std::shared_ptr<OSWindowNode>> OSWindowMac::EnumerateChildren(void) {
  std::vector<std::shared_ptr<OSWindowNode>> retVal;
  return retVal;
}

std::wstring OSWindowMac::GetTitle(void) {
  std::wstring retVal;
  return retVal;
}

OSPoint OSWindowMac::GetPosition(void) {
  OSPoint retVal;
  retVal.x = 0;
  retVal.y = 0;
  return retVal;
}

OSSize OSWindowMac::GetSize(void) {
  OSSize retVal;
  retVal.width = 0;
  retVal.height = 0;
  return retVal;
}

void OSWindowMac::Cloak(void) {

}

void OSWindowMac::Uncloak(void) {
}
