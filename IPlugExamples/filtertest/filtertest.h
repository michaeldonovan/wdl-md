#ifndef __FILTERTEST__
#define __FILTERTEST__

#include "IPlug_include_in_plug_hdr.h"
#include "LinkwitzRiley.h"

class filtertest : public IPlug
{
public:
  filtertest(IPlugInstanceInfo instanceInfo);
  ~filtertest();

  LinkwitzRiley* filter;
  
  void Reset();
  void OnParamChange(int paramIdx);
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);

private:
  double mGain;
};

#endif
