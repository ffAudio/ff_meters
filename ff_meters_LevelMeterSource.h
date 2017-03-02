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

    ff_meters_LevelMeterSource.h
    Created: 5 Apr 2016 9:49:54am
    Author:  Daniel Walz

 ==============================================================================
 */

#ifndef FF_METER_LEVEL_METER_SOURCE_H_INCLUDED
#define FF_METER_LEVEL_METER_SOURCE_H_INCLUDED

#include <atomic>
#include <vector>

/**
 \class LevelMeterSource

 To get a nice meter GUI create a LevelMeterSource in your audioProcessor
 or whatever instance processes an AudioBuffer.
 Then call LevelMeterSource::measureBlock (AudioBuffer<float>& buf) to 
 create the readings.
 */
class LevelMeterSource
{
private:
    class ChannelData {
    public:
        ChannelData () :
        max (),
        rms (),
        reduction (-1.0f),
        hold () {}

        ChannelData (const ChannelData& other) :
        max       (other.max.load() ),
        rms       (other.rms.load() ),
        reduction (other.reduction.load()),
        hold      (other.hold.load())
        {}
        std::atomic<float>       max;
        std::atomic<float>       rms;
        std::atomic<float>       reduction;
        std::atomic<juce::int64> hold;
    };

public:
    LevelMeterSource () : holdMSecs   (500) {}

    void measureBlock (const juce::AudioBuffer<float>& buffer)
    {
        const juce::int64 time = juce::Time::currentTimeMillis();
        const int         numChannels = buffer.getNumChannels ();
        const int         numSamples  = buffer.getNumSamples ();

        levels.resize (numChannels);

        for (int channel=0; channel < numChannels; ++channel) {
            setLevels (time, channel,
                       buffer.getMagnitude (channel, 0, numSamples),
                       buffer.getRMSLevel  (channel, 0, numSamples));
        }
    }

    void setLevels (const juce::int64 time, const int channel, const float max, const float rms)
    {
        jassert (max <= 1.0);
        jassert (rms <= 1.0);

        if (max >= levels [channel].max) {
            levels [channel].max = max;
            levels [channel].hold = time + holdMSecs;
        }
        else if (time > levels [channel].hold) {
            levels [channel].max = max;
        }
        levels [channel].rms = rms;
    }

    void setReductionLevel (const int channel, const float reduction)
    {
        if (juce::isPositiveAndBelow (channel, static_cast<int> (levels.size ())))
            levels [channel].reduction = reduction;
    }

    float getReductionLevel (const int channel) const
    {
        if (juce::isPositiveAndBelow (channel, static_cast<int> (levels.size ())))
            return levels [channel].reduction;
        return -1.0f;
    }

    float getMaxLevel (const int channel) const
    {
        return levels.at (channel).max;
    }

    float getRMSLevel (const int channel) const
    {
        return levels.at (channel).rms;
    }

    int getNumChannels () const
    {
        return levels.size();
    }

private:

    std::vector<ChannelData> levels;

    juce::int64 holdMSecs;

};

#endif /* FF_METER_LEVEL_METER_SOURCE_H_INCLUDED */

