#include "IPlugMaxi.h"
#include "IPlug_include_in_plug_src.h"
#include "resource.h"

static const int pitch[8]={57,57,59,60};//the bassline for the arpeggio
static const int chord[8]={0,0,7,2,5,5,0,0};//the root chords for the arpeggio
//here's the lead line trigger array, followed by the pitches
static const int leadLineTrigger[256]={1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static const int leadLinePitch[15]={69,67,65,64,67,66,64,62,65,64,62,57,55,60,57};

IPlugMaxi::IPlugMaxi(IPlugInstanceInfo instanceInfo)
:	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo), mGain(1.)
{
  TRACE;

  trigger = trigger2 = newnote = 0;
  lastCount = 0;
  currentCount = 0;
  playHead = 0 ;
  currentChord = 0;
  bassout = leadout = delayout = 0.;
  currentPitch = leadPitch = 0.f;
  
  //arguments are: name, defaultVal, minVal, maxVal, step, label
  GetParam(kGain)->InitDouble("Gain", 50., 0., 100.0, 0.01, "%");

  //MakePreset("preset 1", ... );
  MakeDefaultPreset((char *) "-", kNumPrograms);
}

void IPlugMaxi::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{
  double* out1 = outputs[0];
  double* out2 = outputs[1];
  
  for (int s = 0; s < nFrames; s++)
  {
    currentCount = (int) timer.phasor(9); // this sets up a metronome that ticks every so often
    
    if (lastCount != currentCount) // if we have a new timer int this sample, play the sound
    {
      trigger = 1; // play the arpeggiator line
      trigger2 = leadLineTrigger[playHead%256]; // play the lead line
      
      if (trigger2 == 1) // if we are going to play a note
      {
        leadPitch = mtof.mtof(leadLinePitch[newnote]); //get the next pitch val
        newnote++;
        
        if (newnote>14) 
        {
          newnote = 0; // make sure we don't go over the edge of the array
        }
      }
      
      currentPitch = mtof.mtof(pitch[(playHead%4)]+chord[currentChord%8]); // write the frequency val into currentPitch
      
      playHead++;
      
      if (playHead%32 == 0) // wrap every 4 bars
      {
        currentChord++;// change the chord
      }

      lastCount = 0; // set lastCount to 0
    }
    
    bassout = filter2.lores(envelope.adsr(bass.saw(currentPitch*0.5)+sound.pulse(currentPitch*0.5,mod.phasor(1)),1,0.9995, 0.25, 0.9995, 1, trigger),9250,2); // new, simple ADSR. 
    leadout = filter.lores(leadenvelope.ar(lead2.saw(leadPitch*4)+lead.pulse(leadPitch+(leadmod.sinebuf(1.9)*1.5), 0.6), 0.00005, 0.999975, 50000, trigger2),5900,10); // leadline
    
    delayout = (leadout+(delay.dl(leadout, 14000, 0.8)*0.5))/2.; // add some delay
    
    if(trigger != 0) trigger = 0; // set the trigger to off if you want it to trigger immediately next time.
    
    out1[s] = ((bassout+delayout)/2.) * 0.2 * mGain; // sum output
    out2[s] = ((bassout+delayout)/2.) * 0.2 * mGain; // sum output
  }
}

void IPlugMaxi::OnParamChange(int paramIdx)
{
  IMutexLock lock(this);

  switch (paramIdx)
  {
    case kGain:
      mGain = GetParam(kGain)->Value() / 100.;
      break;

    default:
      break;
  }
}
