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

    ff_meters_LevelMeter.cpp
    Created: 5 Apr 2016 9:49:54am
    Author:  Daniel Walz

 ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"


//==============================================================================
LevelMeter::LevelMeter (const MeterType type)
  : source          (nullptr),
    selectedChannel (-1),
    meterType       (type),
    refreshRate     (30)
{
    startTimerHz (refreshRate);
}

LevelMeter::~LevelMeter()
{
    stopTimer();
}

void LevelMeter::setMeterSource (LevelMeterSource* src)
{
    source = src;
}

void LevelMeter::setSelectedChannel (const int c)
{
    selectedChannel = c;
}

void LevelMeter::setRefreshRateHz (const int newRefreshRate)
{
    refreshRate = newRefreshRate;
    startTimerHz (refreshRate);
}

void LevelMeter::paint (Graphics& g)
{
    Graphics::ScopedSaveState saved (g);

    LookAndFeel& l = getLookAndFeel();
    if (LookAndFeelMethods* lnf = dynamic_cast<LookAndFeelMethods*> (&l)) {
        const juce::Rectangle<float> bounds = getLocalBounds().toFloat();
        lnf->drawBackground (g, meterType, bounds);
        lnf->drawMeterBars (g, meterType, bounds, source, selectedChannel);
    }
    else {
        // This LookAndFeel is missing the LevelMeter::LookAndFeelMethods.
        // If you work with the default LookAndFeel, set an instance of
        // LevelMeterLookAndFeel as LookAndFeel of this component.
        //
        // If you write a LookAndFeel from scratch, inherit also
        // LevelMeter::LookAndFeelMethods
        jassertfalse;
    }
}

void LevelMeter::timerCallback ()
{
    repaint();
}

void LevelMeter::clearClipIndicator (const int channel)
{
    if (source) {
        if (channel < 0) {
            source->clearAllClipFlags();
        }
        else {
            source->clearClipFlag (channel);
        }
    }
}

void LevelMeter::clearMaxLevelDisplay (const int channel)
{
    if (source) {
        if (channel < 0) {
            source->clearAllMaxNums();
        }
        else {
            source->clearMaxNum (channel);
        }
    }
}

void LevelMeter::mouseDown (const juce::MouseEvent &event)
{
    if (LookAndFeelMethods* lnf = dynamic_cast<LookAndFeelMethods*> (&getLookAndFeel())) {
        const juce::Rectangle<float> innerBounds = lnf->getMeterInnerBounds (getLocalBounds().toFloat(),
                                                                             meterType);
        if (event.mods.isLeftButtonDown() && source) {
            int hit = lnf->hitTestClipIndicator (event.getPosition(),
                                                 meterType,
                                                 innerBounds,
                                                 source);
            if (hit >= 0) {
                listeners.call (&LevelMeter::Listener::clipLightClicked, hit);
            }
            hit = lnf->hitTestMaxNumber (event.getPosition(),
                                         meterType,
                                         innerBounds,
                                         source);
            if (hit >= 0) {
                listeners.call (&LevelMeter::Listener::maxLevelClicked, hit);
            }
        }
    }
}

void LevelMeter::addListener (LevelMeter::Listener* listener)
{
    listeners.add (listener);
}

void LevelMeter::removeListener (LevelMeter::Listener* listener)
{
    listeners.remove (listener);
}


