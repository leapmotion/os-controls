#import "MenubarController.h"

@interface ApplicationDelegate : NSObject <NSApplicationDelegate> {
@private
  MenubarController* _menubarController;
  BOOL _isInitialized;
  BOOL _wasHidden;
}

@end
