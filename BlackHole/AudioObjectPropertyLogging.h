//
//  AudioObjectPropertyLogging.h
//  BlackHole
//
//  Created by Gordon Childs on 11/1/21.
//

#ifndef AudioObjectPropertyLogging_h
#define AudioObjectPropertyLogging_h

#include <CoreAudio/CoreAudio.h>

void dumpAudioObjectProperty(bool get, AudioObjectID inObjectID, const AudioObjectPropertyAddress* inAddress, const void* inQualifierData, UInt32 dataSize, const void* data);


#endif /* AudioObjectPropertyLogging_h */