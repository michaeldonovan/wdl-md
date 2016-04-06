/*	Gamma - Generic processing library
	See COPYRIGHT file for authors and license information
	
	Example:		Generator / Oscillator / LFO
	Description:	Using an LFO as an oscillator source.
*/

#include "../examples.h"

LFO<> osc(220);
LFO<> mod(0.5);

void audioCB(AudioIOData& io){

	while(io()){
	
		mod.mod(3./4);
		float f = mod.line2()*0.5+0.5;
	
		osc.freq(f*220);
		float s = osc.cos();
		
		io.sum(s * 0.2f, 0,1);
	}
}


RUN(audioCB);
