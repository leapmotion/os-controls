// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "Screen.h"

namespace leap {

void Screen::Update()
{
  m_bounds = CGDisplayBounds(m_screenID);
  m_isPrimary = CGDisplayIsMain(m_screenID);
}

void Screen::GetBackgroundImage() const
{
  @autoreleasepool {
#if 0
    int scaledWindowWidth = static_cast<int>(Globals::windowWidth*SCALE_FACTOR);
    int scaledWindowHeight = static_cast<int>(Globals::windowHeight*SCALE_FACTOR);

    ci::Surface8u surface = ci::Surface8u(scaledWindowWidth, scaledWindowHeight, true, ci::SurfaceChannelOrder::RGBA);
    surface.setPremultiplied(true);
    unsigned char* dstBytes = surface.getData();
    ::memset(dstBytes, 0, scaledWindowWidth*scaledWindowHeight*4);

    NSScreen* screen = getDisplay()->getNsScreen();
    NSImage* nsImage = [[NSImage alloc] initWithContentsOfURL:[[NSWorkspace sharedWorkspace] desktopImageURLForScreen:screen]];
    if (nsImage) {
      CGColorSpaceRef rgb = CGColorSpaceCreateDeviceRGB();
      CGContextRef cgContextRef =
        CGBitmapContextCreate(dstBytes, scaledWindowWidth, scaledWindowHeight, 8, 4*scaledWindowWidth, rgb, kCGImageAlphaPremultipliedLast);
      NSGraphicsContext* gc = [NSGraphicsContext graphicsContextWithGraphicsPort:cgContextRef flipped:NO];
      [NSGraphicsContext saveGraphicsState];
      [NSGraphicsContext setCurrentContext:gc];

      const NSSize imageSize = [nsImage size];
      const CGFloat scaleX = scaledWindowWidth/imageSize.width;
      const CGFloat scaleY = scaledWindowHeight/imageSize.height;
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
#endif
  }
}

}
