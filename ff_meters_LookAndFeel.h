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

    ff_meters_LookAndFeel.h
    Author:  Daniel Walz

 ==============================================================================
 */



#ifndef FF_METERS_LOOKAND_FEEL_H_INCLUDED
#define FF_METERS_LOOKAND_FEEL_H_INCLUDED

class LevelMeterSource;

class LevelMeterLookAndFeel
{
public:

    enum LMColours {
        lmTextColour = 0,
        lmTextDeactiveColour,
        lmTicksColour,
        lmBackgroundColour,
        lmMeterForegroundColour,
        lmMeterBackgroundColour,
        lmMeterMaxNormalColour,
        lmMeterMaxWarnColour,
        lmMeterMaxOverColour,
        lmMeterGradientLowColour,
        lmMeterGradientMidColour,
        lmMeterGradientMaxColour,
        lmMeterReductionColour
    };

    LevelMeterLookAndFeel ()
    {
        lmColoursMap.set (lmTextColour,             juce::Colours::green);
        lmColoursMap.set (lmTextDeactiveColour,     juce::Colours::darkgrey);
        lmColoursMap.set (lmTicksColour,            juce::Colours::orange);
        lmColoursMap.set (lmBackgroundColour,       juce::Colour (0xff050a29));
        lmColoursMap.set (lmMeterForegroundColour,  juce::Colours::green);
        lmColoursMap.set (lmMeterBackgroundColour,  juce::Colours::darkgrey);
        lmColoursMap.set (lmMeterMaxNormalColour,   juce::Colours::lightgrey);
        lmColoursMap.set (lmMeterMaxWarnColour,     juce::Colours::orange);
        lmColoursMap.set (lmMeterMaxOverColour,     juce::Colours::darkred);
        lmColoursMap.set (lmMeterGradientLowColour, juce::Colours::green);
        lmColoursMap.set (lmMeterGradientMidColour, juce::Colours::lightgoldenrodyellow);
        lmColoursMap.set (lmMeterGradientMaxColour, juce::Colours::red);
        lmColoursMap.set (lmMeterReductionColour,   juce::Colours::orange);
    }

    virtual ~LevelMeterLookAndFeel () {}

    virtual float getCornerSize (const juce::Rectangle<float> bounds)
    {
        return juce::jmax (bounds.getWidth(), bounds.getHeight()) * 0.01;
    }

    virtual void drawMetersBackground (juce::Graphics&,
                                       const juce::Rectangle<float> bounds) = 0;

    virtual void drawMeterTicks (juce::Graphics&,
                                 const juce::Rectangle<float> bounds) = 0;

    virtual void drawMeters   (juce::Graphics&,
                               const juce::Rectangle<float> bounds,
                               LevelMeterSource* source) = 0;

    virtual void drawMeterBar (juce::Graphics& g,
                               const juce::Rectangle<float> bounds,
                               const float max,
                               const float rms,
                               const float reduction = -1.0) = 0;

    juce::Colour getMeterColour (const LMColours type) const
    {
        return lmColoursMap [type];
    }

    void setMeterColour (const LMColours type, const juce::Colour& colour)
    {
        lmColoursMap.set (type, colour);
    }

private:
    juce::HashMap<int, juce::Colour> lmColoursMap;

};

#endif /* FF_METERS_LOOKAND_FEEL_H_INCLUDED */
