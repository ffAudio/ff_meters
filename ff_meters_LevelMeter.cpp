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
FFAU::LevelMeter::LevelMeter (const MeterFlags type)
  : source          (nullptr),
    selectedChannel (-1),
    fixedNumChannels(-1),
    meterType       (type),
    refreshRate     (30),
    useBackgroundImage (false),
    backgroundNeedsRepaint (true)
{
    startTimerHz (refreshRate);
}

FFAU::LevelMeter::~LevelMeter()
{
    stopTimer();
}

void FFAU::LevelMeter::setMeterFlags (const MeterFlags type)
{
    meterType = type;
}

void FFAU::LevelMeter::setMeterSource (LevelMeterSource* src)
{
    source = src;
}

void FFAU::LevelMeter::setSelectedChannel (const int c)
{
    selectedChannel = c;
}

void FFAU::LevelMeter::setFixedNumChannels (const int numChannels)
{
    fixedNumChannels = numChannels;
}

void FFAU::LevelMeter::setRefreshRateHz (const int newRefreshRate)
{
    refreshRate = newRefreshRate;
    startTimerHz (refreshRate);
}

void FFAU::LevelMeter::paint (Graphics& g)
{
    Graphics::ScopedSaveState saved (g);

    LookAndFeel& l = getLookAndFeel();
    if (LookAndFeelMethods* lnf = dynamic_cast<LookAndFeelMethods*> (&l)) {
        const juce::Rectangle<float> bounds = getLocalBounds().toFloat();
        int numChannels = source ? source->getNumChannels() : 1;
        if (useBackgroundImage) {
            // This seems to only speed up, if you use complex drawings on the background. For
            // "normal" vector graphics the image approach seems actually slower
            if (backgroundNeedsRepaint) {
                backgroundImage = Image (Image::ARGB, getWidth(), getHeight(), true);
                Graphics backGraphics (backgroundImage);
                lnf->drawBackground (backGraphics, meterType, bounds);
                lnf->drawMeterBarsBackground (backGraphics, meterType, bounds, numChannels, fixedNumChannels);
                backgroundNeedsRepaint = false;
            }
            g.drawImageAt (backgroundImage, 0, 0);
            lnf->drawMeterBars (g, meterType, bounds, source, fixedNumChannels, selectedChannel);
        }
        else {
            lnf->drawBackground (g, meterType, bounds);
            lnf->drawMeterBarsBackground (g, meterType, bounds, numChannels, fixedNumChannels);
            lnf->drawMeterBars (g, meterType, bounds, source, fixedNumChannels, selectedChannel);
        }
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

    if (source)
        source->decayIfNeeded();
}

void FFAU::LevelMeter::resized ()
{
    LookAndFeel& l = getLookAndFeel();
    if (LookAndFeelMethods* lnf = dynamic_cast<LookAndFeelMethods*> (&l)) {
        lnf->updateMeterGradients();
    }

    backgroundNeedsRepaint = true;
}
void FFAU::LevelMeter::visibilityChanged ()
{
    backgroundNeedsRepaint = true;
}

void FFAU::LevelMeter::timerCallback ()
{
    LookAndFeel& l = getLookAndFeel();
    if (LookAndFeelMethods* lnf = dynamic_cast<LookAndFeelMethods*> (&l)) {
        int numChannels = source ? source->getNumChannels() : 1;
        for (int i=0; i < numChannels; ++i) {
            auto channelBounds = lnf->getMeterBounds (getLocalBounds().toFloat(), meterType, numChannels, i);
            repaint (lnf->getMeterBarBounds (channelBounds, meterType).toNearestInt());
            repaint (lnf->getMeterMaxNumberBounds (channelBounds, meterType).toNearestInt());
            repaint (lnf->getMeterClipIndicatorBounds (channelBounds, meterType).toNearestInt());
        }
    }
    else
        repaint();
}

void FFAU::LevelMeter::clearClipIndicator (const int channel)
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

void FFAU::LevelMeter::clearMaxLevelDisplay (const int channel)
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

void FFAU::LevelMeter::mouseDown (const juce::MouseEvent &event)
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
                listeners.call (&LevelMeter::Listener::clipLightClicked, this, hit, event.mods);
            }
            hit = lnf->hitTestMaxNumber (event.getPosition(),
                                         meterType,
                                         innerBounds,
                                         source);
            if (hit >= 0) {
                listeners.call (&LevelMeter::Listener::maxLevelClicked, this, hit, event.mods);
            }
        }
    }
}

void FFAU::LevelMeter::addListener (FFAU::LevelMeter::Listener* listener)
{
    listeners.add (listener);
}

void FFAU::LevelMeter::removeListener (FFAU::LevelMeter::Listener* listener)
{
    listeners.remove (listener);
}


