#define PLUG_MFR "OliLarkin"
#define PLUG_NAME "IPlugMaxi"

#define PLUG_CLASS_NAME IPlugMaxi

#define BUNDLE_MFR "OliLarkin"
#define BUNDLE_NAME "IPlugMaxi"

#define PLUG_ENTRY IPlugMaxi_Entry
#define PLUG_VIEW_ENTRY IPlugMaxi_ViewEntry

#define PLUG_ENTRY_STR "IPlugMaxi_Entry"
#define PLUG_VIEW_ENTRY_STR "IPlugMaxi_ViewEntry"

#define VIEW_CLASS IPlugMaxi_View
#define VIEW_CLASS_STR "IPlugMaxi_View"

// Format        0xMAJR.MN.BG - in HEX! so version 10.1.5 would be 0x000A0105
#define PLUG_VER 0x00010000
#define VST3_VER_STR "1.0.0"

// http://service.steinberg.de/databases/plugin.nsf/plugIn?openForm
// 4 chars, single quotes. At least one capital letter
#define PLUG_UNIQUE_ID 'm4uD'
// make sure this is not the same as BUNDLE_MFR
#define PLUG_MFR_ID 'oliL'

// ProTools stuff
#define PLUG_MFR_DIGI "OliLarkin\nOliLarkin\nAcme\n"
#define PLUG_NAME_DIGI "IPlugMaxi\nIPEF"
#define EFFECT_TYPE_DIGI "Effect" // valid options "None" "EQ" "Dynamics" "PitchShift" "Reverb" "Delay" "Modulation" "Harmonic" "NoiseReduction" "Dither" "SoundField" "Effect" instrument determined by PLUG _IS _INST

#define PLUG_CHANNEL_IO "1-1 2-2"

#define PLUG_LATENCY 0
#define PLUG_IS_INST 0

// if this is 0 RTAS can't get tempo info
#define PLUG_DOES_MIDI 0
#define PLUG_DOES_STATE_CHUNKS 0

#define GUI_WIDTH 300
#define GUI_HEIGHT 300

// on MSVC, you must define SA_API in the resource editor preprocessor macros as well as the c++ ones
#ifdef SA_API
  #ifndef OS_IOS
    #include "app_wrapper/app_resource.h"
  #endif
#endif

// vst3 stuff
#define MFR_URL "www.olilarkin.co.uk"
#define MFR_EMAIL "spam@me.com"
#define EFFECT_TYPE_VST3 kFx

/* kFxAnalyzer, kFxDelay, kFxDistortion, kFxDynamics, kFxEQ, kFxFilter,
kFx, kFxInstrument, kFxInstrumentExternal, kFxSpatial, kFxGenerator,
kFxMastering, kFxModulation, kFxPitchShift, kFxRestoration, kFxReverb,
kFxSurround, kFxTools, kInstrument, kInstrumentDrum, kInstrumentSampler,
kInstrumentSynth, kInstrumentSynthSample, kInstrumentExternal, kSpatial,
kSpatialFx, kOnlyRealTime, kOnlyOfflineProcess, kMono, kStereo,
kSurround
*/
