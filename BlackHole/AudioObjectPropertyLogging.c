//
//  AudioObjectPropertyLogging.c
//  BlackHole
//
//  Created by Gordon Childs on 11/1/21.
//

#include "AudioObjectPropertyLogging.h"
#include <CoreAudio/AudioServerPlugIn.h> // kAudioPlugInProperty*

#include <sys/syslog.h>
#define    DebugMsg(inFormat, ...)    syslog(LOG_NOTICE, inFormat, ## __VA_ARGS__)

static void
dumpAudioObjectArray(UInt32* outDataSize, void* outData, const char* s) {
    UInt32 n = *outDataSize / sizeof(UInt32);
    AudioObjectID * x = (AudioObjectID*)(outData);

    DebugMsg(" --> Num %ss: %i", s, n);

    for (UInt32 i = 0; i < n; i++) {
        DebugMsg("%s: %i", s, x[i]);
    }
}

void
dumpGetProperty(AudioObjectID inObjectID, const AudioObjectPropertyAddress* inAddress, const void* inQualifierData, UInt32* outDataSize, void* outData) {
    AudioObjectPropertySelector a = inAddress->mSelector;
    AudioObjectPropertySelector b = inAddress->mScope;
    DebugMsg("GetPropertyData result: object id: %u address: (%c%c%c%c, %c%c%c%c, %u)", inObjectID, a>>24, a>>16, a>>8, a, b>>24, b>>16, b>>8, b, inAddress->mElement);

    if (kAudioDevicePropertyTransportType == inAddress->mSelector) {
        UInt32 x = *(UInt32*)(outData);
        DebugMsg(" --> %c%c%c%c (0x%x)", x>>24, x>>16, x>>8, x, x);
    } else if (kAudioPlugInPropertyDeviceList == inAddress->mSelector) {
        UInt32 n = *outDataSize / sizeof(UInt32);
        UInt32 * x = (UInt32*)(outData);
        DebugMsg("Num Device: %i", n);

        for (UInt32 i = 0; i < n; i++) {
            DebugMsg("Device: %i", x[i]);
        }
    } else if (kAudioDevicePropertyNominalSampleRate == inAddress->mSelector) {
        Float64 c =  *(Float64*)(outData);

        DebugMsg(" --> %lf", c);
    } else if(kAudioObjectPropertyClass == inAddress->mSelector || kAudioObjectPropertyBaseClass == inAddress->mSelector) {
        AudioClassID x = *(AudioClassID *)(outData);
//        DebugMsg(" --> %u", (unsigned int)x);
        // WTF bracketed part missing sometimes
        DebugMsg("~~ --> (0x%x) %c%c%c%c", x, x>>24, x>>16, x>>8, x);
    } else if(kAudioDevicePropertyDeviceUID == inAddress->mSelector || kAudioObjectPropertyName == inAddress->mSelector || kAudioSelectorControlPropertyItemName == inAddress->mSelector || kAudioBoxPropertyBoxUID == inAddress->mSelector) {
        CFStringRef x = *(CFStringRef *)(outData);
        
        if (kAudioSelectorControlPropertyItemName == inAddress->mSelector) {
            UInt32 y = 0;
            if (inQualifierData) y = *(UInt32 *)(inQualifierData);
            DebugMsg("[%p]: 0x%x --> ", inQualifierData, y);
        }
        
        
        const char* s = CFStringGetCStringPtr(x, kCFStringEncodingUTF8);
        if (s) DebugMsg(" --> %s", s);
        else DebugMsg(" --> [ptr] %p", x);
        
//        CFRelease(x); // some yes some no?
    } else if (kAudioDevicePropertyStreams == inAddress->mSelector) {
        dumpAudioObjectArray(outDataSize, outData, "stream");
    } else if (kAudioStreamPropertyAvailablePhysicalFormats == inAddress->mSelector) {
        UInt32 n = *outDataSize / sizeof(AudioStreamRangedDescription);
        AudioStreamRangedDescription * x = (AudioStreamRangedDescription*)(outData);

        DebugMsg(" --> Num formats: %i", n);

        for (UInt32 i = 0; i < n; i++) {
            AudioStreamRangedDescription s = x[i];
            AudioFormatID fmt = s.mFormat.mFormatID;
            DebugMsg("format[%i]: %lf, %c%c%c%c, 0x%x, bpp: %i, fpp: %i, bpf: %i, ch: %i, bitspc: %i", i, s.mFormat.mSampleRate, fmt>>24, fmt>>16, fmt>>8, fmt, s.mFormat.mFormatFlags, s.mFormat.mBytesPerPacket, s.mFormat.mFramesPerPacket, s.mFormat.mBytesPerFrame, s.mFormat.mChannelsPerFrame, s.mFormat.mBitsPerChannel);
            DebugMsg("sr[%i]: [%f, %f]", i, s.mSampleRateRange.mMinimum, s.mSampleRateRange.mMinimum);
//            DebugMsg("format: %i", x[i]);

        }
    } else if(kAudioStreamPropertyPhysicalFormat == inAddress->mSelector) {
        AudioStreamBasicDescription as = *(AudioStreamBasicDescription *)(outData);
        AudioFormatID fmt = as.mFormatID;

        DebugMsg("format: %lf, %c%c%c%c, 0x%x, bpp: %i, fpp: %i, bpf: %i, ch: %i, bitspc: %i", as.mSampleRate, fmt>>24, fmt>>16, fmt>>8, fmt, as.mFormatFlags, as.mBytesPerPacket, as.mFramesPerPacket, as.mBytesPerFrame, as.mChannelsPerFrame, as.mBitsPerChannel);
    } else if (kAudioObjectPropertyControlList == inAddress->mSelector) {
        dumpAudioObjectArray(outDataSize, outData, "control");
    } else if (kAudioSelectorControlPropertyCurrentItem == inAddress->mSelector) {
        dumpAudioObjectArray(outDataSize, outData, "current control");
    } else if(kAudioDevicePropertyZeroTimeStampPeriod == inAddress->mSelector || kAudioDevicePropertyIsHidden == kAudioDevicePropertyZeroTimeStampPeriod == inAddress->mSelector || kAudioStreamPropertyStartingChannel == inAddress->mSelector || kAudioDevicePropertyIsHidden == inAddress->mSelector || kAudioDevicePropertyDeviceCanBeDefaultDevice == inAddress->mSelector || kAudioDevicePropertyDeviceCanBeDefaultSystemDevice == inAddress->mSelector || kAudioBooleanControlPropertyValue == inAddress->mSelector || kAudioDevicePropertySafetyOffset == inAddress->mSelector || kAudioDevicePropertySafetyOffset == inAddress->mSelector || kAudioDevicePropertyDeviceIsRunning == inAddress->mSelector) {
        UInt32 x = *(UInt32*)(outData);
        DebugMsg(" --> %i", x);
    } else if (kAudioStreamPropertyTerminalType == inAddress->mSelector) {
        UInt32 x = *(UInt32*)(outData);
        DebugMsg(" --> %c%c%c%c (0x%x)", x>>24, x>>16, x>>8, x, x);
    } else if (kAudioControlPropertyScope == inAddress->mSelector) {
        AudioObjectPropertyScope x = *(AudioObjectPropertyScope*)(outData);
        DebugMsg(" --> %c%c%c%c (0x%x)", x>>24, x>>16, x>>8, x, x);

    } else if (kAudioControlPropertyElement == inAddress->mSelector) {
        AudioObjectPropertyElement x = *(AudioObjectPropertyElement*)(outData);
        DebugMsg(" -->  (0x%x) %c%c%c%c", x, x>>24, x>>16, x>>8, x);
    } else if (kAudioLevelControlPropertyScalarValue == inAddress->mSelector || kAudioLevelControlPropertyDecibelValue == inAddress->mSelector) {
        Float32 x = *(Float32*)(outData);
        DebugMsg(" --> %f", x);
    } else if (kAudioLevelControlPropertyDecibelRange == inAddress->mSelector) {
        AudioValueRange x = *(AudioValueRange *)outData;
        DebugMsg(" --> [%lf, %lf]", x.mMinimum, x.mMaximum);
    } else if (kAudioDevicePropertySafetyOffset == inAddress->mSelector) {
        dumpAudioObjectArray(outDataSize, outData, "box");
    }
}
