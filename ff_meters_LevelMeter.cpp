/*
 ==============================================================================
 Copyright (c) 2017 - 2019 Foleys Finest Audio Ltd. - Daniel Walz
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


namespace foleys
{

//==============================================================================
LevelMeter::LevelMeter (std::unique_ptr<LevelMeter::LookAndFeelMethods> lookAndFeel)
  : lookAndFeelMethods (std::move (lookAndFeel))
{
    onMaxLevelClicked = [](LevelMeter& meter, int channel, juce::ModifierKeys mods)
    {
        // default clear all indicators. Overwrite this lambda to change the behaviour
        juce::ignoreUnused (channel);
        juce::ignoreUnused (mods);
        meter.clearMaxLevelDisplay();
        meter.clearClipIndicator();
    };

    onClipLightClicked = [](LevelMeter& meter, int channel, juce::ModifierKeys mods)
    {
        // default clear all indicators. Overwrite this lambda to change the behaviour
        juce::ignoreUnused (channel);
        juce::ignoreUnused (mods);
        meter.clearMaxLevelDisplay();
        meter.clearClipIndicator();
    };

    startTimerHz (refreshRate);
}

LevelMeter::~LevelMeter()
{
    stopTimer();
}

void LevelMeter::setMeterSource (LevelMeterSource* src)
{
    source = src;
    repaint();
}

void LevelMeter::setSelectedChannel (const int c)
{
    selectedChannel = c;
}

void LevelMeter::setFixedNumChannels (const int numChannels)
{
    selectedNumChannels = numChannels;
}

void LevelMeter::setRefreshRateHz (const int newRefreshRate)
{
    refreshRate = newRefreshRate;
    startTimerHz (refreshRate);
}

void LevelMeter::paint (juce::Graphics& g)
{
    juce::Graphics::ScopedSaveState saved (g);
    lookAndFeelMethods->drawMeter (g, *this, getLocalBounds().toFloat());
}

void LevelMeter::setMeterColour (LevelMeter::ColourIds id, juce::Colour c)
{
    lookAndFeelMethods->setMeterColour (id, c);
}

void LevelMeter::timerCallback ()
{
    if (source)
    {
        source->decayIfNeeded();

        if (source->checkNewDataFlag())
        {
            source->resetNewDataFlag();
            repaint();
        }
    }
}

void LevelMeter::clearClipIndicator (const int channel)
{
    if (source == nullptr)
        return;

    if (channel < 0)
        source->clearAllClipFlags();
    else
        source->clearClipFlag (channel);
}

void LevelMeter::clearMaxLevelDisplay (const int channel)
{
    if (source == nullptr)
        return;

    if (channel < 0)
        source->clearAllMaxNums();
    else
        source->clearMaxNum (channel);
}

void LevelMeter::mouseDown (const juce::MouseEvent &event)
{
    if (source == nullptr)
        return;

    if (event.mods.isLeftButtonDown())
    {
        auto channel = lookAndFeelMethods->hitTestClipIndicator (*this,
                                                                 event.position,
                                                                 getLocalBounds().toFloat());
        if (channel >= 0)
        {
            listeners.call (&LevelMeter::Listener::clipLightClicked, this, channel, event.mods);
            if (onClipLightClicked)
                onClipLightClicked (*this, channel, event.mods);
        }

        channel = lookAndFeelMethods->hitTestMaxNumber (*this,
                                                        event.position,
                                                        getLocalBounds().toFloat());
        if (channel >= 0)
        {
            listeners.call (&LevelMeter::Listener::maxLevelClicked, this, channel, event.mods);
            if (onMaxLevelClicked)
                onMaxLevelClicked (*this, channel, event.mods);
        }

        repaint();
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

LevelMeterSource* LevelMeter::getMeterSource()
{
    return source;
}

const LevelMeterSource* LevelMeter::getMeterSource() const
{
    return source;
}

int LevelMeter::getFirstChannel() const
{
    return std::max (0, selectedChannel);
}

int LevelMeter::getNumChannels() const
{
    if (selectedNumChannels > 0)
        return selectedNumChannels;

    if (source != nullptr)
        return source->getNumChannels();

    return 0;
}

void LevelMeter::LookAndFeelMethods::setMeterColour (LevelMeter::ColourIds id, juce::Colour c)
{
    colourMap [id] = c;
}

void LevelMeter::LookAndFeelMethods::setupDefaultMeterColours()
{
    colourMap [LevelMeter::BackgroundColour] = juce::Colour (0xff333333);
    colourMap [LevelMeter::TextColour] = juce::Colours::lightgrey;
    colourMap [LevelMeter::TextClipColour] = juce::Colours::red;
    colourMap [LevelMeter::TicksColour] = juce::Colours::silver;
    colourMap [LevelMeter::OutlineColour] = juce::Colours::silver;
    colourMap [LevelMeter::ClipOffColour] = juce::Colours::darkgrey;
    colourMap [LevelMeter::ClipClippedColour] = juce::Colours::red;
    colourMap [LevelMeter::MeterMaxNormalColour] = juce::Colours::silver;
    colourMap [LevelMeter::MeterMaxWarnColour] = juce::Colours::orange;
    colourMap [LevelMeter::MeterMaxOverColour] = juce::Colours::red;
    colourMap [LevelMeter::MeterBackgroundColour] = juce::Colours::darkgrey;
    colourMap [LevelMeter::MeterGradientLowColour] = juce::Colours::green;
    colourMap [LevelMeter::MeterGradientMidColour] = juce::Colours::lightgoldenrodyellow;
    colourMap [LevelMeter::MeterGradientMaxColour] = juce::Colours::red;

    colourMap [LevelMeter::MeterReductionColour] = juce::Colours::orange;
}


} // namespace foleys
