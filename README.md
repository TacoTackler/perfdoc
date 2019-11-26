
# PowerVR PerfDoc

![banner](PVRPerfDoc.png)

The PowerVR Developer Support team has adapted the ARM PerfDoc validation layer to make it more applicable to the PowerVR architecture. 

We have attempted to modify the original code as little as possible, just adding or removing checks that reflect our own [PowerVR Performance Recommendations](https://docs.imgtec.com/PerfRec/topics/c_PerfRec_introduction.html) more closely.

As with the original PerfDoc, this version evaluates API usage with checks which can be done up-front, and tries to keep code portability as high as possible.

Just like PerfDoc and other Vulkan validation layers, errors are reported either through VK_EXT_debug_report to the application as callbacks, or via a console/logcat if enabled. The layer will run on any Vulkan implementation, so optimisations can be found even when running on non-PowerVR platforms.


## Features

Currently, the layer implements the checks below.
The layer uses this enumeration to pass down a message code to the application.
The debug callback has a message string with more description.

```
enum MessageCodes
{
	MESSAGE_CODE_COMMAND_BUFFER_RESET = 1,
	MESSAGE_CODE_COMMAND_BUFFER_SIMULTANEOUS_USE = 2,
	MESSAGE_CODE_SMALL_ALLOCATION = 3,
	MESSAGE_CODE_SMALL_DEDICATED_ALLOCATION = 4,
	MESSAGE_CODE_TOO_LARGE_SAMPLE_COUNT = 5,
	MESSAGE_CODE_NON_LAZY_MULTISAMPLED_IMAGE = 6,
	MESSAGE_CODE_NON_LAZY_TRANSIENT_IMAGE = 7,
	MESSAGE_CODE_MULTISAMPLED_IMAGE_REQUIRES_MEMORY = 8,
	MESSAGE_CODE_RESOLVE_IMAGE = 9,
	MESSAGE_CODE_FRAMEBUFFER_ATTACHMENT_SHOULD_BE_TRANSIENT = 10,
	MESSAGE_CODE_FRAMEBUFFER_ATTACHMENT_SHOULD_NOT_BE_TRANSIENT = 11,
	MESSAGE_CODE_INDEX_BUFFER_SPARSE = 12,
	MESSAGE_CODE_INDEX_BUFFER_CACHE_THRASHING = 13,
	MESSAGE_CODE_TOO_MANY_INSTANCED_VERTEX_BUFFERS = 14,
	MESSAGE_CODE_DISSIMILAR_WRAPPING = 15,
	MESSAGE_CODE_NO_PIPELINE_CACHE = 16,
	MESSAGE_CODE_DESCRIPTOR_SET_ALLOCATION_CHECKS = 17,
	MESSAGE_CODE_COMPUTE_NO_THREAD_GROUP_ALIGNMENT = 18,
	MESSAGE_CODE_COMPUTE_LARGE_WORK_GROUP = 19,
	MESSAGE_CODE_COMPUTE_POOR_SPATIAL_LOCALITY = 20,
	MESSAGE_CODE_POTENTIAL_PUSH_CONSTANT = 21,
	MESSAGE_CODE_MANY_SMALL_INDEXED_DRAWCALLS = 22,
	MESSAGE_CODE_DEPTH_PRE_PASS = 23,
	MESSAGE_CODE_PIPELINE_BUBBLE = 24,
	MESSAGE_CODE_NOT_FULL_THROUGHPUT_BLENDING = 25,
	MESSAGE_CODE_SAMPLER_LOD_CLAMPING = 26,
	MESSAGE_CODE_SAMPLER_LOD_BIAS = 27,
	MESSAGE_CODE_SAMPLER_BORDER_CLAMP_COLOR = 28,
	MESSAGE_CODE_SAMPLER_UNNORMALIZED_COORDS = 29,
	MESSAGE_CODE_SAMPLER_ANISOTROPY = 30,
	MESSAGE_CODE_TILE_READBACK = 31,
	MESSAGE_CODE_CLEAR_ATTACHMENTS_AFTER_LOAD = 32,
	MESSAGE_CODE_CLEAR_ATTACHMENTS_NO_DRAW_CALL = 33,
	MESSAGE_CODE_REDUNDANT_RENDERPASS_STORE = 34,
	MESSAGE_CODE_REDUNDANT_IMAGE_CLEAR = 35,
	MESSAGE_CODE_INEFFICIENT_CLEAR = 36,
	MESSAGE_CODE_LAZY_TRANSIENT_IMAGE_NOT_SUPPORTED = 37,
	MESSAGE_CODE_UNCOMPRESSED_TEXTURE_USED = 38,
	MESSAGE_CODE_NON_MIPMAPPED_TEXTURE_USED = 39,
	MESSAGE_CODE_NON_INDEXED_DRAW_CALL = 40,
	MESSAGE_CODE_SUBOPTIMAL_TEXTURE_FORMAT = 41,
	MESSAGE_CODE_TEXTURE_LINEAR_TILING = 42,
	MESSAGE_CODE_NO_FBCDC = 43, 
	MESSAGE_CODE_ROBUST_BUFFER_ACCESS_ENABLED = 44,
	MESSAGE_CODE_SUBOPTIMAL_SUBPASS_DEPENDENCY_FLAG = 45,
	MESSAGE_CODE_PARTIAL_CLEAR = 46,
	MESSAGE_CODE_PIPELINE_OPTIMISATION_DISABLED = 47,
	MESSAGE_CODE_WORKGROUP_SIZE_DIVISOR = 48,
	MESSAGE_CODE_POTENTIAL_SUBPASS = 49,
	MESSAGE_CODE_SUBPASS_STENCIL_SELF_DEPENDENCY = 50,
	MESSAGE_CODE_INEFFICIENT_DEPTH_STENCIL_OPS = 51,
	MESSAGE_CODE_QUERY_BUNDLE_TOO_SMALL = 52,

	MESSAGE_CODE_COUNT
};

```

The objectType reported by the layer matches the standard `VK_EXT_debug_report` object types.

## To build

See BUILD.md.

## Config file

**The config file is optional.**

There are certain values in PowerVR PerfDoc which are used as thresholds for heuristics, which can flag potential issues in an application.
Sometimes, these thresholds are somewhat arbitrary and may cause unnecessary false positives for certain applications.
For these scenarios it is possible to provide a config file.
See the sections below for how to enable the config file for Linux/Windows and Android.

Some common options like logging can be overridden directly with environment variables or setprop on Android.
A config file should not be necessary in the common case.

The default config file can be found in `layer/perfdoc-default.cfg`.
This default config file contains all the options available to the layer.
The default config file contains all the default values which are used by the layer if a config file is not present.

## Enabling layers on Linux and Windows

The JSON and binary file must be in the same folder, which is the case after building.

To have the Vulkan loader find the layers, export the following environment variable:

```
VK_LAYER_PATH=/path/to/directory/with/json/and/binary
```

This allows the application to enumerate the layer manually and enable the debug callback from within the app.
The layer name is `VK_LAYER_IMG_powervr_perf_doc`.
The layer should appear in `vkEnumerateInstanceLayerProperties` and `vkEnumerateDeviceLayerProperties`.

### Enabling layers outside the application

To force-enable PowerVR PerfDoc outside the application, some environment variables are needed.

```
VK_LAYER_PATH=/path/to/directory/with/json/and/binary
VK_INSTANCE_LAYERS=VK_LAYER_IMG_powervr_perf_doc
VK_DEVICE_LAYERS=VK_LAYER_IMG_powervr_perf_doc
```

However, without a `VK_EXT_debug_report` debug callback,
you will not get any output, so to add logging to file or console:

```
# Either one of these
POWERVR_PERFDOC_LOG=stdout
POWERVR_PERFDOC_LOG=stderr
POWERVR_PERFDOC_LOG=/path/to/log.txt
POWERVR_PERFDOC_LOG=debug_output # OutputDebugString, Windows only
```

It is also possible to use a config file which supports more options as well as logging output:

```
POWERVR_PERFDOC_CONFIG=/tmp/path/to/config.cfg"
```

## Enabling layers on Android

### ABI (ARMv7 vs. AArch64)

The package contains both ARMv7 binaries and AArch64.
Make sure to use the right version which matches your application.

### Within application

The layer .so must be present in the APKs library directory.
The Android loader will find the layers when enumerating layers, just like the validation layers.

The PowerVR PerfDoc layer must be enabled explicitly by the app in both `vkCreateInstance` and `vkCreateDevice`.
The layer name is `VK_LAYER_IMG_powervr_perf_doc`.
The layer should appear in `vkEnumerateInstanceLayerProperties` and `vkEnumerateDeviceLayerProperties`.

### Outside the application

Vulkan layers can be placed in `/data/local/debug/vulkan` on any device.
Depending on your device, `/data/local/debug/` may be writeable without root access.
It is also possible to place the layer directly inside the application library folder in `/data/data`,
but this will certainly require root.

To force-enable the layer for all Vulkan applications:

```
setprop debug.vulkan.layers VK_LAYER_IMG_powervr_perf_doc:
```

Here is an example for how to enable PowerVR PerfDoc for any Vulkan application:
```
# For ARMv7-A
adb push build-android-armeabi-v7a/layer/libVkLayer_powervr_perf_doc.so /data/local/debug/vulkan/
# For AArch64
adb push build-android-arm64-v8a/layer/libVkLayer_powervr_perf_doc.so /data/local/debug/vulkan/

adb shell

setprop debug.powervr.perfdoc.log logcat
setprop debug.vulkan.layers VK_LAYER_IMG_powervr_perf_doc:

exit
adb logcat -c && adb logcat -s PowerVRPerfDoc
```

#### Enabling logcat/file logging

It is sometimes desirable to use PowerVR PerfDoc from outside an application,
e.g. when debugging random APKs which do not have PowerVR PerfDoc integrated.

There are two ways to enable external logging on Android.
Both of the methods described below can also be used when the layer is embedded in the APK (but not enabled by the app),
but they are most relevant when dealing with arbitrary Vulkan applications.

To filter logcat output, you can use:
```
adb logcat -s PowerVRPerfDoc
```

##### setprop method (Recommended)

To force-enable logging from outside an application, you can set an Android system property:
```
setprop debug.powervr.perfdoc.log logcat
```

To log to a file, replace logcat with a filename. Be aware that system properties on Android
have a very limited number of characters available, so a long path might not be possible to represent.
```
setprop debug.powervr.perfdoc.log /sdcard/path/to/log.txt
```

##### Config file method

An alternative to setprop is via the config file. This method is a bit more cumbersome than setprop,
but might be more convenient if you are already using a config file for other purposes.

Place a config file on the SD card looking like this:

```
loggingFilename logcat
```

or

```
loggingFilename /sdcard/path/to/log.txt
```

Then, point the layer to this config file by typing this into adb shell:

```
setprop debug.powervr.perfdoc.config /sdcard/path/to/perfdoc.cfg
```

Be careful with permissions however. Not all paths on the SD card can be made visible to an application.

