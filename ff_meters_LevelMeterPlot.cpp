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
 
 ff_meters_LevelMeterPlot.cpp
 Created: 13 Jul 2017 21:30:29
 Author:  Daniel Walz
 
 ==============================================================================
 */

#include "../JuceLibraryCode/JuceHeader.h"


//==============================================================================


FFAU::LevelMeterPlot::LevelMeterPlot ()
  : source       (nullptr),
    writePointer (0)
{
    channelColours.push_back (juce::Colours::blue);
    channelColours.push_back (juce::Colours::red);
    channelColours.push_back (juce::Colours::green);
    channelColours.push_back (juce::Colours::yellow);
    channelColours.push_back (juce::Colours::magenta);
    channelColours.push_back (juce::Colours::cyan);
    const int numChannels = 2;
    const int numSamples  = 100;
    levels.resize (numChannels, std::vector<float>(numSamples, 0.0f));
    reduction.resize(numSamples, 1.0f);
    startTimerHz (30);
}

FFAU::LevelMeterPlot::~LevelMeterPlot()
{
    stopTimer();
}


void FFAU::LevelMeterPlot::paint (juce::Graphics& g)
{
    juce::Rectangle<float> bounds = getLocalBounds().toFloat();
    const float step = (getWidth() - 8.0f) / reduction.size();
    const float t    = getHeight() * 0.1f;
    const float h    = getHeight() * 0.8f;
    g.fillAll();
    
    g.setColour (findColour (FFAU::LevelMeter::lmBackgroundColour));
    const float corner = std::min (bounds.getWidth(), bounds.getHeight()) * 0.01;
    g.fillRoundedRectangle (bounds, corner);
    g.setColour (findColour (FFAU::LevelMeter::lmOutlineColour));
    g.drawRoundedRectangle (bounds.reduced (3), corner, 2);

    float infinity = -80.0f;
    for (int c=0; c<levels.size(); ++c) {
        if (levels[c].size() > 0) {
            Path p;
            float x = 4.0f;
            float y = juce::Decibels::gainToDecibels (levels[c][writePointer], infinity) / infinity;
            p.startNewSubPath (x, t + h * y);
            for (int i=1; i<levels[c].size(); ++i) {
                x += step;
                y  = juce::Decibels::gainToDecibels (levels[c][(writePointer + i) % levels[c].size()], infinity) / infinity;
                p.lineTo (x, t + h * y);
            }
            g.setColour (channelColours[c]);
            g.strokePath (p, PathStrokeType(1));
        }
    }

    infinity = -30.0f;
    if (reduction.size() > 0) {
        Path p;
        float x = 4.0f;
        float y = juce::Decibels::gainToDecibels (reduction[writePointer], infinity) / infinity;
        p.startNewSubPath (x, t + h * y);
        for (int i=1; i<reduction.size(); ++i) {
            x += step;
            y  = juce::Decibels::gainToDecibels (reduction[(writePointer + i) % reduction.size()], infinity) / infinity;
            p.lineTo (x, t + h * y);
        }
        g.setColour (juce::Colours::orange);
        g.strokePath (p, PathStrokeType(1));
    }
    
}

void FFAU::LevelMeterPlot::resized()
{
    
}

void FFAU::LevelMeterPlot::timerCallback()
{
    if (source) {
        float r = 1.0;
        for (int i=0; i < juce::jmin (static_cast<int>(levels.size()), source->getNumChannels()); ++i) {
            levels[i][writePointer] = source->getRMSLevel (i);
            r = juce::jmin (r, source->getReductionLevel(i));
        }
        reduction[writePointer] = r;
        writePointer = (writePointer + 1) % reduction.size();
    }
    repaint();
}


void FFAU::LevelMeterPlot::setMeterSource (LevelMeterSource* src)
{
    source = src;
}



