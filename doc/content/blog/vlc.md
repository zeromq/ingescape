+++
title = "VLC Media Player as an ingeScape agent : an example of how ingeScape integrates in complex existing software architectures"
date = "2018-04-23T11:36:41+02:00"
categories = ["agents"]
banner = "img/banners/vlc.jpg"
+++

IngeScape is able to integrate with minimal effort into complex existing software so that these software become ingeScape agents accessible in any ingeScape platform. To concretely illustrate this, we decided to use the famous [VLC](https://www.videolan.org) project as an example. This is a very large software project that runs on almost any software platform with its own complex (and very flexible) architecture and its own compilation scripts based on autoconf & friends.

Our challenge is to keep the VLC architecture unchanged and to minimally edit the source code and compilation scripts to integrate the ingeScape library, to transform VLC into a fully-fledged ingeScape agent, that would be, if possible, usable on any operating system already supported by VLC.

This is how we achieved the challenge...

## Adaptation on macOS
VLC is a multi-platform software. By commodity, we decided to explore it using macOS. The next chapter of this article presents the approach for Windows and Linux.


### Getting the code and checking compilation
We first asked for a git checkout on the VLC github repository:

{{< highlight bash >}}
git clone git://git.videolan.org/vlc.git
{{< / highlight >}}

Then we followed the compilation instructions for macOS provided [here](https://wiki.videolan.org/OSXCompile) on the VideoLAN Wiki to check that everything was OK before changing anything. The compilation process is easy and straightforward.

### Programming language
VLC is a significant multi-OS open source project. There was no doubt that it would be developed mostly in C to take advantage of the portability. Depending on the OS, it may use other languages. For instance to take advantage of native UI technologies on macOS, which is the first OS we will try to work on, Objective-C is used in addition to the C language to handle the UI.

To be consistent with the VLC source code, we will use the C API for ingeScape as well. But we already know that the various ingeScape wrappers (C#, Python, etc.) are available and could  be used if needed, around and complementarily to the C API. Said otherwise, ingeScape supports software projects that use different programming languages combined together.

### Starting and stopping our VLC agent properly
This is often the first step to integrate with legacy or external software. We need to figure out how this software is initialized and stopped to add a few lines of ingeScape code at the right places.

VLC is a graphical application. Inspecting the UI code is thus a good idea to determine how the software is started and stopped. In our case, and running on macOS, we opened the XCode project provided with the VLC source code. 

NB: we modified this project so that it includes the ingeScape headers path (i.e. */usr/local/include*) to benefit from the live code compilation in XCode and see possible errors when we will edit the code, even if XCode is not used to compile VLC on macOS.

This inspection led us to *bin/darwinvlc.m* which contains the *main* function starting the VLC UI on macOS. This function deals with interruption signals and command line parameters. For VLC itself, it seems that there are only a few specific lines.

Initialization in *bin/darwinvlc.m* is achieved like this:

{{< highlight c >}}
    /* Initialize libvlc */
    libvlc_instance_t *vlc = libvlc_new(argc, argv);
    if (vlc == NULL)
        return 1;

    int ret = 1;
    libvlc_set_exit_handler(vlc, vlc_terminate, NULL);
    libvlc_set_app_id(vlc, "org.VideoLAN.VLC", PACKAGE_VERSION, PACKAGE_NAME);
    libvlc_set_user_agent(vlc, "VLC media player", "VLC/"PACKAGE_VERSION);

    libvlc_add_intf(vlc, "hotkeys,none");

    if (libvlc_add_intf(vlc, NULL)) {
        fprintf(stderr, "VLC cannot start any interface. Exiting.\n");
        goto out;
    }
    libvlc_playlist_play(vlc);
{{< / highlight >}}
<br>

And termination, after the Cocoa mainloop is stopped, is achieved like this:

{{< highlight c >}}
    libvlc_release(vlc);
{{< / highlight >}}
<br>

Clearly, the VLC UI seems to rely on the VLC library itself and this is actually a great news : this means that the VLC team designed the software so that the UIs for the supported operating systems all rely on the library which forms a sort of unified core. This design choice pushes us to investigate the library in order to start and stop what will become our VLC ingeScape agent. The code here above points to two relevant functions : *libvlc_new* and *libvlc_release*.

These two functions are defined in *lib/core.c* whose name suggest that we are right where we want to be to include the ingeScape code. And indeed, inspecting these two functions quickly helps us to define how to include our ingescape code with the existing one.

First, we include the ingeScape header:
{{< highlight c >}}
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <ingescape/ingescape.h>

#include "libvlc_internal.h"
{{< / highlight >}}
<br>

Then we add the initialization code - at this stage, only a simple agent name and an empty definition declaration - complemented by the ingeScape start function, for now with hardcoded parameters:
{{< highlight c >}}
libvlc_instance_t * libvlc_new( int argc, const char *const *argv )
{
    igs_setAgentName("igsVLC");
    igs_setDefinitionName("igsVLC");
    igs_setDefinitionVersion("1.0");
    igs_setDefinitionDescription("Definition for our VLC agent");
    igs_startWithDevice("en7", 5670);
    
    libvlc_threads_init ();

    libvlc_instance_t *p_new = malloc (sizeof (*p_new));
    if (unlikely(p_new == NULL))
        return NULL;
    //...
{{< / highlight >}}
<br>

Finally, we add the ingeScape stop function and the end of *libvlc_release*:
{{< highlight c >}}
void libvlc_release( libvlc_instance_t *p_instance )
{
    vlc_mutex_t *lock = &p_instance->instance_lock;
    int refs;

    vlc_mutex_lock( lock );
    assert( p_instance->ref_count > 0 );
    refs = --p_instance->ref_count;
    vlc_mutex_unlock( lock );

    if( refs == 0 )
    {
        vlc_mutex_destroy( lock );
        libvlc_Quit( p_instance->p_libvlc_int );
        libvlc_InternalCleanup( p_instance->p_libvlc_int );
        libvlc_InternalDestroy( p_instance->p_libvlc_int );
        free( p_instance );
        libvlc_threads_deinit ();
    }
    
    igs_stop();
}
{{< / highlight >}}
<br>


### Compiling after adding ingeScape
Now that we have added some ingeScape code in VLC, it is necessary to edit the compilation scripts so that ingeScape is properly included and linked.

VLC uses automake to generate its Makefiles. This means that we need to inspect and modify the Makefile.am instances. Because we are integrated inside the VLC library itself and there is a *lib* folder in the source code repository, we inspected *lib/Makefile.am* first and this was a good intuition : this file contains all the parameters to compile the VLC library as an independent library that is then linked by the modules, the UIs, etc. Thanks again to the VLC team for making things easy here.

We added two sections in *lib/Makefile.am*, first to make the ingeScape header available to GCC by modifying the CFLAGS and then to enable to link the ingeScape library by modifying the LDFLAGS. At this stage, being on macOS only, we did not try to take advantage of the various OS-specific sections of *lib/Makefile.am* and added our information at the easiest places.

Here is the modification for CFLAGS, supposing that ingeScape headers are installed in */usr/local/include/*:

{{< highlight bash >}}
AM_CFLAGS = $(CFLAGS_libvlc) -I/usr/local/include/
{{< / highlight >}}
<br>

Here is the modification for LDFLAGS, supposing that the ingeScape library are installed in */usr/local/lib/*:

{{< highlight bash >}}
libvlc_la_LDFLAGS = \
	$(LDFLAGS_libvlc) \
	-no-undefined \
	-version-info 12:0:0 \
	-export-symbols $(srcdir)/libvlc.sym \
	-L/usr/local/lib/ -lingescape
{{< / highlight >}}
<br>

Then we recompile VLC, still following the instructions for macOS provided [here](https://wiki.videolan.org/OSXCompile) on the VideoLAN Wiki. And we get a brand new VLC app running and stopping as an ingeScape agent, using the hardcoded network parameters we passed to the start function added in *lib/core.c*. When stopping the VLC UI, we noted that the agent stopped properly as well.

Basically, it required less than an hour and **the edition or addition of exactly 9 lines of code to transform VLC into a proper ingeScape agent**.

Now is time to make this VLC agent useful...

### Designing VLC as an ingeScape agent
VLC is a great media player. We want to use it to play videos and sound files, and possibly video streams that we will select remotely. When playing videos, we want to control and toggle the *Full screen* mode, *play/pause* and *audio volume*.

NB: VLC already offers many ways to be controlled remotely. But we keep in mind that the objective of this article is to show a VLC/ingeScape integration.

From an ingeScape point of view, the VLC agent would have only inputs to control the media player. Here are the proposed inputs:

- *file* (string) : path of the media file to be opened
- *stream* (string) : url of the string to be opened
- *playPause* (impulsion) : play/pause toggle control
- *fullScreen* (impulsion) : full screen toggle control
- *volume* (boolean) : sound volume, false will go one step down and true will go one step up.

These inputs must first be declared in the VLC source code. Each declaration will require one line of code. They could be placed in *lib/core.c* just before starting the agent or in other places in the code.

Then, these inputs need to be observed to create the callbacks that will actually use the existing VLC code to modify the media player. To find the best places to add these callbacks in the VLC source code, we need to investigate the VLC source code once again. As we did before, starting from the UI seems relevant as what we want to do with our inputs looks like what users would do directly on the UI.


#### Loading a file
In the UI, when opening a file from the menu, it adds the file in the playlist and starts playing it right away. This is the behavior we want when feeding our agent's *file* input. The inspection of the VLC XCode project and more specifically of the part of the UI in charge of loading a file indicates that when using the *open* action in the VLC app menu on macOS, the *intfOpenFile:* method in *modules/gui/macosx/VLCMainMenu.m* is called. This method contains a tricky part of code transforming an array of file paths into an array of more complex structures.

It then calls *addPlaylistItems:* in *modules/gui/macosx/VLCPlaylist.m* using these more complex structures, which, in turn calls *addPlaylistItems:withParentItemId:atPos:startPlayback:* in the same file. This last method is called with specific values regarding the parent item id and the current position, both set to *-1*. The parameter corresponding to *startPlayback:* depends on the player default configuration. We will be able to overload it if necessary.

At this stage we still are in Objective-C code, in the macos-specific part of the VLC UI. However, there is already a heavy mix between specific VLC code and native macOS code so that, when we reach *addPlaylistItems:withParentItemId:atPos:startPlayback:* it seems difficult to install our ingeScape observe function for the *file* input directly in the platform-independent VLC code. In this case, the VLC team made the choice not to separate things as clearly as they enabled us to for the start/stop functions.

The conclusion is that we will have to add our observe callback in the Objective-C code and thus partially lose our objective of minimal full portability. If we want to reach full portability, we will have to add callbacks in each UI platform-specific part of the VLC code. The good news is that each section of code we will have to add will be significantly simpler that a shared one because we will be able to rely on the existing native code, already doing most the work.

In the case of macOS, most things happen in *modules/gui/macosx/VLCMainMenu.m* that we already inspected. In addition, we need to edit *modules/gui/macosx/Makefile.am* to edit CFLAGS and LDFLAGS, just like with did for the core VLC library.

At the beginning of *modules/gui/macosx/VLCMainMenu.m* we include the missing headers :

{{< highlight m >}}
#import <vlc_url.h>
#include <ingescape/ingescape.h>
{{< / highlight >}}
<br>

Then comes the callback itself, placed between the @interface and the @implementation sections of VLCMainMenu:

{{< highlight m >}}
void observeFile(iop_t iopType, const char *name, iopType_t valueType, void* value, size_t valueSize, void *myData){
    printf("observeFile: %s changed\n", name);
    char *cPath = (char *)value;
    dispatch_async(dispatch_get_main_queue(), ^{
        NSString *path = [NSString stringWithCString:cPath encoding:NSUTF8StringEncoding];
        NSMutableArray *array = [NSMutableArray arrayWithCapacity:1];
        NSDictionary *dictionary;
        char *psz_uri = vlc_path2uri([path UTF8String], "file");
        if (psz_uri){
            dictionary = [NSDictionary dictionaryWithObject:toNSStr(psz_uri) forKey:@"ITEM_URL"];
            free(psz_uri);
            [array addObject: dictionary];
            [[[VLCMain sharedInstance] playlist] addPlaylistItems:array];
        }
    });
}
{{< / highlight >}}

The callback code is directly inspired from the existing code used to handle file opening.

<br>The callback is registered once at object creation. Here, it is in *awakeFromNib*:

{{< highlight m >}}
- (void)awakeFromNib
{
    igs_observeInput("file", observeFile, NULL);
    
    _timeSelectionPanel = [[VLCTimeSelectionPanelController alloc] init];
{{< / highlight >}}
<br>

And finally, here are the modifications for *modules/gui/macosx/Makefile.am* CFLAGS and LDFLAGS to include and link ingeScape:

{{< highlight bash >}}
libmacosx_plugin_la_OBJCFLAGS = $(AM_OBJCFLAGS) -fobjc-exceptions -fobjc-arc -I/usr/local/include/
libmacosx_plugin_la_LDFLAGS = $(AM_LDFLAGS) -rpath '$(guidir)' \
	-Wl,-framework,Cocoa -Wl,-framework,CoreServices \
	-Wl,-framework,AVFoundation -Wl,-framework,CoreMedia -Wl,-framework,IOKit \
	-Wl,-framework,AddressBook -Wl,-framework,WebKit -Wl,-framework,CoreAudio \
	-Wl,-framework,SystemConfiguration -Wl,-framework,ScriptingBridge \
	-Wl,-framework,QuartzCore -Wl,-framework,MediaPlayer \
	-L/usr/local/lib/ -lingescape
{{< / highlight >}}
<br>

After a new compilation, using the ingeScape editor to write the *file* input of our VLC agent with a path on the computer pointing to an actual video properly loads and plays the video. Our modification is thus functional and complete !


#### Loading a stream

Following the same strategy as for the *file* input, opening a stream from the main menu leads to the *intfOpenNet* method still in *modules/gui/macosx/VLCMainMenu.m*. This method opens a dialog window whose Xib file, describing the UI, is *Open.xib*. Inspecting this UI description leads to the *Open* button which is attached to the *panelOk* function in *modules/gui/macosx/VLCOpenWindowController.m*. On macOS with Cocoa, the window to open stream URLs is coded using a modal approach. When the window is opened, the parent function is blocked and when closed, the parent function continues. This parent function is *openTarget*, still in the same class.

The blocking line in *modules/gui/macosx/VLCOpenWindowController.m* is :

{{< highlight m >}}
NSModalResponse i_result = [NSApp runModalForWindow: self.window];
{{< / highlight >}}
<br>

Then, in the case of a simple stream URL, a dictionary is created, containing the URL and an empty options array, passed to the playlist with this code :

{{< highlight m >}}
NSMutableDictionary *itemOptionsDictionary;
NSMutableArray *options = [NSMutableArray array];

itemOptionsDictionary = [NSMutableDictionary dictionaryWithObject: [self MRL] forKey: @"ITEM_URL"];

//..

/* apply the options to our item(s) */
[itemOptionsDictionary setObject: (NSArray *)[options copy] forKey: @"ITEM_OPTIONS"];
[[[VLCMain sharedInstance] playlist] addPlaylistItems:[NSArray arrayWithObject:itemOptionsDictionary]];
{{< / highlight >}}
<br>

This code is exactly what we have to use in our observe callback dedicated to the *stream* input, replacing the MRL property with our own URL.

The *VLCOpenWindowController* is created only when the popup window is displayed, whereas the main menu exists as long as the VLC application is running. If we placed the call to *igs_observeInput* in the *VLCOpenWindowController*, it might be called multiple times and would be called for the first time only if a user actually displays the pop-up window. Therefore, we have to find another place to safely declare and call *igs_observeInput*. Our chance is that the observe callback for the *stream* input does not require the *VLCOpenWindowController* context at all. We can add it wherever necessary. And the easiest place to do so is the same as for the *file* input, in *modules/gui/macosx/VLCMainMenu.m*, because the *VLCMainMenu* class is instantiated once and last as long as the application is running.


Here is the callback itself, placed between the @interface and the @implementation sections of VLCMainMenu, near the existing *observeFile* callback:

{{< highlight m >}}
void observeStream(iop_t iopType, const char *name, iopType_t valueType, void* value, size_t valueSize, void *myData){
    printf("observeStream: %s changed\n", name);
    char *cURL = (char *)value;
    dispatch_async(dispatch_get_main_queue(), ^{
        NSString *url = [NSString stringWithCString:cURL encoding:NSUTF8StringEncoding];
        NSMutableDictionary *itemOptionsDictionary;
        NSMutableArray *options = [NSMutableArray array];
        itemOptionsDictionary = [NSMutableDictionary dictionaryWithObject: url forKey: @"ITEM_URL"];
        [itemOptionsDictionary setObject: (NSArray *)[options copy] forKey: @"ITEM_OPTIONS"];
        [[[VLCMain sharedInstance] playlist] addPlaylistItems:[NSArray arrayWithObject:itemOptionsDictionary]];
    });
}
{{< / highlight >}}
<br>

The callback is registered once at object creation, still in *awakeFromNib*:

{{< highlight m >}}
- (void)awakeFromNib
{
    igs_observeInput("file", observeFile, NULL);
    igs_observeInput("stream", observeStream, NULL);
    
    _timeSelectionPanel = [[VLCTimeSelectionPanelController alloc] init];
{{< / highlight >}}
<br>

Because we already did the work for the *file* input, there is no need to adapt Makefile.am to include and link ingeScape again.

After a new compilation, a test of writing the *stream* input of our VLC agent with the ingeScape editor properly loads and plays the video we provided the URL to. Our modification is thus functional and complete !


#### Toggling play/pause and full screen, control volume
Another code inspection starting in the UI shows that play/pause, full screen and sound volume are also controlled from the main menu. The corresponding functions, which are *play*, *toggleFullscreen*, *volumeUp* and *volumeDown* all rely on core VLC methods and can be called easily from anywhere in the code. Because we already installed code in *modules/gui/macosx/VLCMainMenu.m*.

We need to observe three new ingeScape inputs, which are *playPause*, *fullScreen* and *volume*:

{{< highlight m >}}
- (void)awakeFromNib
{
    igs_observeInput("file", observeFile, NULL);
    igs_observeInput("stream", observeStream, NULL);
    igs_observeInput("playPause", observePlayPause, NULL);
    igs_observeInput("fullScreen", observeFullScreen, NULL);
    igs_observeInput("volume", observeVolume, NULL);
{{< / highlight >}}
<br>

And we need to add three new observe callbacks:

{{< highlight m >}}
void observePlayPause(iop_t iopType, const char *name, iopType_t valueType, void* value, size_t valueSize, void *myData){
    dispatch_async(dispatch_get_main_queue(), ^{
        [[VLCCoreInteraction sharedInstance] playOrPause];
    });
}

void observeFullScreen(iop_t iopType, const char *name, iopType_t valueType, void* value, size_t valueSize, void *myData){
    dispatch_async(dispatch_get_main_queue(), ^{
        [[VLCCoreInteraction sharedInstance] toggleFullscreen];
    });
}

void observeVolume(iop_t iopType, const char *name, iopType_t valueType, void* value, size_t valueSize, void *myData){
    bool flag = *(bool *)(value);
    dispatch_async(dispatch_get_main_queue(), ^{
        if (flag){
            [[VLCCoreInteraction sharedInstance] volumeUp];
        }else{
            [[VLCCoreInteraction sharedInstance] volumeDown];
        }
    });
}
{{< / highlight >}}

Once again, actions created in the ingeScape editor enable to check that our new inputs work properly.
<br><br>

#### Stopping the app properly
When stopped by the user, the VLC app already notifies its ingeScape platform that it is leaving. But when stopped from the ingeScape editor this thread-rich application does not terminate properly when an interruption signal (SIGINT) is received from the ingeScape library. We thus need to add an *observe stop* callback to make it do so. Everything happens in *modules/gui/macosx/VLCMainMenu.m*.

Here is the callback registration:

{{< highlight m >}}
    igs_observeInput("volume", observeVolume, NULL);
    
    igs_observeForcedStop(observeForcedStop, NULL);
    
    _timeSelectionPanel = [[VLCTimeSelectionPanelController alloc] init];
{{< / highlight >}}
<br>

And here is the callback itself:

{{< highlight m >}}
void observeForcedStop(void *myData){
    NSLog(@"STOP received");
    dispatch_async(dispatch_get_main_queue(), ^{
        igs_stop();
        [NSApp terminate:nil];
    });
}
{{< / highlight >}}
<br>


## Adaptation for Windows and Linux : making VLC cross-platform again
While writing this article on a Mac, we used the macOS environment to investigate the code and compile it after adding our ingeScape code. During our analysis, we found out that the macOS implementation of the GUI makes it uneasy to keep all of our modifications compliant with the VLC cross-platform philosophy and to locate all the changes in the VLC library. Most of our changes were done in the *modules/gui/macosx* folder and more specifically in *modules/gui/macosx/VLCMainMenu.m*.

On Windows and Linux, the default VLC UI is developed using Qt, which is a great multi-platform industrial framework that we used to develop the ingeScape editor.

In order to make our modifications compatible also for Linux and Windows, we now need to investigate the code in *modules/gui/qt* or in *modules/gui/skins2*,which also relies on Qt.

### Compiling on Linux and Windows
The Videolan Wiki provides a page for [Unix Compilation](https://wiki.videolan.org/UnixCompile/). For Windows, the Videolan Wiki provides this [page](https://wiki.videolan.org/Win32Compile/).

**Linux**

We are using a Debian Buster distribution because it is well [documented](https://wiki.videolan.org/UnixCompile/) for VLC compilation and all dependencies can be fetched easily. This Debian version is compliant with the minimal Qt version required by VLC (>5.9.0).

We used the following commands to prepare, compile and run vlc:

{{< highlight bash >}}
#as root
apt-get build-dep vlc
apt-get install libxcb-xkb-dev

#as user in the vlc folder
./bootstrap
./configure --enable-skins2
make
./vlc
{{< / highlight >}}
<br>

**Windows**

We are using the cross-compilation from our Debian Buster distribution. 

Following the [compilation guide](https://wiki.videolan.org/Win32Compile/), just before the third-party *bootstrap*, we had to uninstall the standard libprotobuf package and to download (from github) and instal Protocol Buffer version 3.1.0 instead of the 3.0.0 version provided by default. The compilation process is the usual, with *autogen*, *configure*, *make* and *make install*. Just do not forget to type *ldconfig* after installing protobuf.

We also had to disable the compilation of the bluray module, which was not working for some *java-ish* reason. We did that by renaming *contrib/src/bluray/rules.mak* so that it is ignored. We agree this is a dirty hack but the bluray module if of no use in our experiment.

For libx264, we had to install the nasm package:

{{< highlight bash >}}
apt-get install nasm
{{< / highlight >}}
<br>

Note that compiling all the dependencies is extremely long: several hours, depending on your computer.


### Transforming VLC into an ingeScape agent using the Qt GUI
The modifications we did in lib/core.c for the macOS version are applied in the same way here. Real work starts with the analysis of the GUI code based on Qt or Skins2 to integrate the ingeScape observe callbacks properly.



