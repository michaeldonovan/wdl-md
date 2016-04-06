#define PLUG_MFR "OliLarkin"
#define PLUG_NAME "MidiMessages"

#define PLUG_CLASS_NAME MidiMessages

#define BUNDLE_MFR "OliLarkin"
#define BUNDLE_NAME "MidiMessages"

#define PLUG_ENTRY MidiMessages_Entry
#define PLUG_VIEW_ENTRY MidiMessages_ViewEntry

#define PLUG_ENTRY_STR "MidiMessages_Entry"
#define PLUG_VIEW_ENTRY_STR "MidiMessages_ViewEntry"

#define VIEW_CLASS MidiMessages_View
#define VIEW_CLASS_STR "MidiMessages_View"

// Format        0xMAJR.MN.BG - in HEX! so version 10.1.5 would be 0x000A0105
#define PLUG_VER 0x00010000
#define VST3_VER_STR "1.0.0"

// http://service.steinberg.de/databases/plugin.nsf/plugIn?openForm
// 4 chars, single quotes. At least one capital letter
#define PLUG_UNIQUE_ID 'm4uC'
// make sure this is not the same as BUNDLE_MFR
#define PLUG_MFR_ID 'oliL'

// ProTools stuff
#define PLUG_MFR_DIGI "OliLarkin\nOliLarkin\nAcme\n"
#define PLUG_NAME_DIGI "MidiMessages\nIPEF"
#define EFFECT_TYPE_DIGI "Effect"

#define PLUG_CHANNEL_IO "2-2" // only works on stereo -> stereo track
#define PLUG_LATENCY 0
#define PLUG_IS_INST 0

// if this is 0 RTAS can't get tempo info
#define PLUG_DOES_MIDI 1
#define PLUG_DOES_STATE_CHUNKS 0

#ifdef SA_API
  #ifndef OS_IOS
    #include "app_wrapper/app_resource.h"
  #endif
#endif

// vst3 stuff
#define MFR_URL "www.olilarkin.co.uk"
#define MFR_EMAIL "spam@me.com"
#define EFFECT_TYPE_VST3 kFx
