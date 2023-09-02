/*
This file is a part of: LinaVG
https://github.com/inanevin/LinaVG

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2022-] [Inan Evin]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/


#include "SponzaPBR.hpp"

using namespace LinaGX::Examples;

#import <Cocoa/Cocoa.h>
#include <CoreVideo/CoreVideo.h>


CVDisplayLinkRef displayLink;

CVReturn MyDisplayLinkCallback(CVDisplayLinkRef displayLink,
                               const CVTimeStamp *inNow,
                               const CVTimeStamp *inOutputTime,
                               CVOptionFlags flagsIn,
                               CVOptionFlags *flagsOut,
                               void *displayLinkContext)
{
    ((LinaGX::Examples::App*)displayLinkContext)->Tick();
    return kCVReturnSuccess;
}


@interface AppDelegate : NSObject <NSApplicationDelegate>
@property (assign) void *myApp;
@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    
    if (self.myApp) {
        LinaGX::Examples::Example *appPointer = static_cast<LinaGX::Examples::Example*>(self.myApp);
        appPointer->Initialize();
        appPointer->Run();
    }
   
    CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
    CVDisplayLinkSetOutputCallback(displayLink, &MyDisplayLinkCallback, self.myApp);
    CVDisplayLinkStart(displayLink);
}

@end

int main(int argc, char* argv[])
{
    Example exampleApp;
    
    @autoreleasepool {
            NSApplication *app = [NSApplication sharedApplication];
            AppDelegate *appDelegate = [[AppDelegate alloc] init];
            appDelegate.myApp = (void *)&exampleApp;
            [app setDelegate:appDelegate];
            [app activateIgnoringOtherApps:YES];
            [app run];
        }
    
    exampleApp.Shutdown();

    return 0;
}

