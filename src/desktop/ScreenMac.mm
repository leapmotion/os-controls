// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "Screen.h"

#include <AppKit/NSScreen.h>
#include <AppKit/NSGraphicsContext.h>
#include <AppKit/NSImage.h>
#include <AppKit/NSWorkspace.h>
#include <Foundation/NSKeyValueCoding.h>
#include <Foundation/NSValue.h>

namespace leap {

void Screen::Update()
{
  m_bounds = CGDisplayBounds(m_screenID);
  m_isPrimary = CGDisplayIsMain(m_screenID);
}

void Screen::GetBackgroundImage() const
{
  @autoreleasepool {
    NSScreen* screen = nil;
    for (NSScreen* item in [NSScreen screens]) {
      NSNumber* number = [[item deviceDescription] valueForKey:@"NSScreenNumber"];
      if (m_screenID == [number unsignedIntValue]) {
        screen = item;
        break;
      }
    }
    if (!screen) {
      return;
    }
    const size_t width = static_cast<size_t>(Width());
    const size_t height = static_cast<size_t>(Height());
    const size_t bytesPerRow = width*4;
    const size_t totalBytes = bytesPerRow*height;

    uint8_t* dstBytes = new uint8_t[totalBytes];
    if (!dstBytes) {
      return;
    }
    ::memset(dstBytes, 0, totalBytes);

    NSImage* nsImage = [[NSImage alloc] initWithContentsOfURL:[[NSWorkspace sharedWorkspace] desktopImageURLForScreen:screen]];
    if (nsImage) {
      CGColorSpaceRef rgb = CGColorSpaceCreateDeviceRGB();
      CGContextRef cgContextRef =
        CGBitmapContextCreate(dstBytes, width, height, 8, bytesPerRow, rgb, kCGImageAlphaPremultipliedLast);
      NSGraphicsContext* gc = [NSGraphicsContext graphicsContextWithGraphicsPort:cgContextRef flipped:NO];
      [NSGraphicsContext saveGraphicsState];
      [NSGraphicsContext setCurrentContext:gc];

      const NSSize imageSize = [nsImage size];
      const CGFloat scaleX = width/imageSize.width;
      const CGFloat scaleY = height/imageSize.height;
      const CGFloat scale = (scaleX >= scaleY) ? scaleX : scaleY;
      const NSSize scaledImageSize = NSMakeSize(imageSize.width * scale, imageSize.height * scale);
      const CGFloat xoffset = (imageSize.width*scaleX - scaledImageSize.width)/2.0;
      const CGFloat yoffset = (imageSize.height*scaleY - scaledImageSize.height)/2.0;
      const NSRect rect = NSMakeRect(xoffset, yoffset, scaledImageSize.width, scaledImageSize.height);

      [nsImage drawInRect:rect fromRect:NSZeroRect operation:NSCompositeSourceOver fraction:1.0];
      [NSGraphicsContext restoreGraphicsState];
      CGContextRelease(cgContextRef);
      CGColorSpaceRelease(rgb);
    }
    delete [] dstBytes;
  }
}

}
