/*
 ==============================================================================
 Copyright (c) 2017 Filmstro Ltd. / Foleys Finest Audio UG - Daniel Walz
 All rights reserved.

 Redistribution and use in source and binary forms, with or without modification,
 are permitted provided that the following conditions are met:
 1. Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
 3. Neither the name of the copyright holder nor the names of its contributors
    may be used to endorse or promote products derived from this software without
    specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 OF THE POSSIBILITY OF SUCH DAMAGE.
 ==============================================================================

    ff_meters_LevelMeter.h
    Created: 5 Apr 2016 9:49:54am
    Author:  Daniel Walz

 ==============================================================================
*/

#ifndef LEVELMETER_H_INCLUDED
#define LEVELMETER_H_INCLUDED



//==============================================================================
/*
 This class is used to display a level reading. It supports max and RMS levels.
 You can also set a reduction value to display, the definition of that value is up to you.
*/
class LevelMeter    : public juce::Component, private juce::Timer
{
public:
    LevelMeter (LevelMeterLookAndFeel* lnf = new LevelMeterLookAndFeelVertical ());
    ~LevelMeter ();

    void paint (juce::Graphics&) override;
    void timerCallback () override;
    
    void setMeterSource (LevelMeterSource* source);

    void setRefreshRateHz (const int newRefreshRate);

    void setLookAndFeel (LevelMeterLookAndFeel* lnf);

    void mouseDown (const juce::MouseEvent& event) override;

    void mouseDoubleClick (const juce::MouseEvent& event) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LevelMeter)
    
    juce::WeakReference<LevelMeterSource>       source;

    juce::ScopedPointer<LevelMeterLookAndFeel>  lookAndFeel;

    int refreshRate;
};


#endif  // LEVELMETER_H_INCLUDED
