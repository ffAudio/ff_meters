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

#pragma once
#include <atomic>
#include <vector>
#include <numeric>

namespace FFAU
{

/** @addtogroup ff_meters */
/*@{*/

/**
 \class LevelMeterSource

 To get a meter GUI create a LevelMeterSource in your AudioProcessor
 or whatever instance processes an AudioBuffer.
 Then call LevelMeterSource::measureBlock (AudioBuffer<float>& buf) to
 create the readings.
 */
class LevelMeterSource
{
private:
    class ChannelData
    {
    public:
        ChannelData (const int rmsWindow = 8) :
        max (),
        maxOverall (),
        clip (false),
        reduction (1.0f),
        hold (0),
        rmsHistory ((size_t) rmsWindow, 0.0),
        rmsSum (0.0),
        rmsPtr (0)
        {}

        ChannelData (const ChannelData& other) :
        max       (other.max.load() ),
        maxOverall(other.maxOverall.load() ),
        clip      (other.clip.load() ),
        reduction (other.reduction.load()),
        hold      (other.hold.load()),
        rmsHistory (8, 0.0),
        rmsSum    (0.0),
        rmsPtr    (0)
        {}

        ChannelData& operator=(const ChannelData& other)
        {
            max.store         (other.max.load());
            maxOverall.store  (other.maxOverall.load());
            clip.store        (other.clip.load());
            reduction.store   (other.reduction.load());
            hold.store        (other.hold.load());
            rmsHistory.resize (other.rmsHistory.size(), 0.0);
            rmsSum = 0.0;
            rmsPtr = 0;
            return (*this);
        }

        std::atomic<float>       max;
        std::atomic<float>       maxOverall;
        std::atomic<bool>        clip;
        std::atomic<float>       reduction;

        float getAvgRMS () const
        {
            if (rmsHistory.size() > 0)
                return std::sqrt (std::accumulate (rmsHistory.begin(), rmsHistory.end(), 0.0f) / rmsHistory.size());

            return (float) std::sqrt (rmsSum);
        }

        void setLevels (const juce::int64 time, const float newMax, const float newRms, const juce::int64 newHoldMSecs)
        {
            if (newMax > 1.0 || newRms > 1.0)
                clip = true;

            maxOverall = fmaxf (maxOverall, newMax);
            if (newMax >= max)
            {
                max = std::min (1.0f, newMax);
                hold = time + newHoldMSecs;
            }
            else if (time > hold)
            {
                max = std::min (1.0f, newMax);
            }
            pushNextRMS (std::min (1.0f, newRms));
        }

        void setRMSsize (const int numBlocks)
        {
            rmsHistory.assign ((size_t) numBlocks, 0.0f);
            rmsSum  = 0.0;
            if (numBlocks > 1)
                rmsPtr %= rmsHistory.size();
            else
                rmsPtr = 0;
        }
    private:
        void pushNextRMS (const float newRMS)
        {
            const double squaredRMS = std::min (newRMS * newRMS, 1.0f);
            if (rmsHistory.size() > 0)
            {
                rmsHistory [(size_t) rmsPtr] = squaredRMS;
                rmsPtr = (rmsPtr + 1) % (int) rmsHistory.size();
            }
            else
            {
                rmsSum = squaredRMS;
            }
        }

        std::atomic<juce::int64> hold;
        std::vector<double>      rmsHistory;
        std::atomic<double>      rmsSum;
        int                      rmsPtr;
    };

public:
    LevelMeterSource () :
    holdMSecs       (500),
    lastMeasurement (0),
    suspended       (false)
    {}

    ~LevelMeterSource ()
    {
        masterReference.clear();
    }

    /**
     Resize the meters data containers. Set the
     \param numChannels to the number of channels. If you don't do this in prepareToPlay,
            it will be done when calling measureBlock, but a few bytes will be allocated
            on the audio thread, so be aware.
     \param rmsWindow is the number of rms values to gather. Keep that aligned with
            the sampleRate and the blocksize to get reproducable results.
            e.g. `rmsWindow = msecs * 0.001f * sampleRate / blockSize;`
     \FIXME: don't call this when measureBlock is processing
     */
    void resize (const int channels, const int rmsWindow)
    {
        levels.resize ((size_t) channels, ChannelData (rmsWindow));
        for (ChannelData& l : levels) {
            l.setRMSsize (rmsWindow);
        }

        newDataFlag = true;
    }

    /**
     Call this method to measure a block af levels to be displayed in the meters
     */
    template<typename FloatType>
    void measureBlock (const juce::AudioBuffer<FloatType>& buffer)
    {
        lastMeasurement = juce::Time::currentTimeMillis();
        if (! suspended)
        {
            const int         numChannels = buffer.getNumChannels ();
            const int         numSamples  = buffer.getNumSamples ();

#if FF_AUDIO_ALLOW_ALLOCATIONS_IN_MEASURE_BLOCK
#warning The use of levels.resize() is not realtime safe. Please call resize from the message thread and set this config setting to 0 via Projucer.
            levels.resize ((size_t) numChannels);
#endif

            for (int channel=0; channel < std::min (numChannels, int (levels.size())); ++channel) {
                levels [(size_t) channel].setLevels (lastMeasurement,
                                            buffer.getMagnitude (channel, 0, numSamples),
                                            buffer.getRMSLevel  (channel, 0, numSamples),
                                            holdMSecs);
            }
        }

        newDataFlag = true;
    }

    /**
     This is called from the GUI. If processing was stalled, this will pump zeroes into the buffer,
     until the readings return to zero.
     */
    void decayIfNeeded()
    {
        juce::int64 time = juce::Time::currentTimeMillis();
        if (time - lastMeasurement < 100)
            return;

        lastMeasurement = time;
        for (size_t channel=0; channel < levels.size(); ++channel)
        {
            levels [channel].setLevels (lastMeasurement, 0.0f, 0.0f, holdMSecs);
            levels [channel].reduction = 1.0f;
        }

        newDataFlag = true;
    }

    /**
     With the reduction level you can add an extra bar do indicate, by what amount the level was reduced.
     This will be printed on top of the bar with half the width.
     @param channel the channel index, that was reduced
     @param reduction the factor for the reduction applied to the channel, 1.0=no reduction, 0.0=block completely
     */
    void setReductionLevel (const int channel, const float reduction)
    {
        if (juce::isPositiveAndBelow (channel, static_cast<int> (levels.size ())))
            levels [(size_t) channel].reduction = reduction;
    }

    /**
     With the reduction level you can add an extra bar do indicate, by what amount the level was reduced.
     This will be printed on top of the bar with half the width.
     @param reduction the factor for the reduction applied to all channels, 1.0=no reduction, 0.0=muted completely
     */
    void setReductionLevel (const float reduction)
    {
        for (auto& channel : levels)
            channel.reduction = reduction;
    }

    /**
     Set the timeout, how long the peak line will be displayed, before it resets to the
     current peak
     */
    void setMaxHoldMS (const juce::int64 millis)
    {
        holdMSecs = millis;
    }

    /**
     Returns the reduction level. This value is not computed but can be set
     manually via \see setReductionLevel.
     */
    float getReductionLevel (const int channel) const
    {
        if (juce::isPositiveAndBelow (channel, static_cast<int> (levels.size ())))
            return levels [(size_t) channel].reduction;

        return -1.0f;
    }

    /**
     This is the max level as displayed by the little line above the RMS bar.
     It is reset by \see setMaxHoldMS.
     */
    float getMaxLevel (const int channel) const
    {
        return levels.at ((size_t) channel).max;
    }

    /**
     This is the max level as displayed under the bar as number.
     It will stay up until \see clearMaxNum was called.
     */
    float getMaxOverallLevel (const int channel) const
    {
        return levels.at ((size_t) channel).maxOverall;
    }

    /**
     This is the RMS level that the bar will indicate. It is
     summed over rmsWindow number of blocks/measureBlock calls.
     */
    float getRMSLevel (const int channel) const
    {
        return levels.at ((size_t) channel).getAvgRMS();
    }

    /**
     Returns the status of the clip flag.
     */
    bool getClipFlag (const int channel) const
    {
        return levels.at ((size_t) channel).clip;
    }

    /**
     Reset the clip flag to reset the indicator in the meter
     */
    void clearClipFlag (const int channel)
    {
        levels.at ((size_t) channel).clip = false;
    }

    void clearAllClipFlags ()
    {
        for (ChannelData& l : levels) {
            l.clip = false;
        }
    }

    /**
     Reset the max number to minus infinity
     */
    void clearMaxNum (const int channel)
    {
        levels.at ((size_t) channel).maxOverall = -80.0f;
    }

    /**
     Reset all max numbers
     */
    void clearAllMaxNums ()
    {
        for (ChannelData& l : levels) {
            l.maxOverall = -80.0f;
        }
    }

    /**
     Get the number of channels to be displayed
     */
    int getNumChannels () const
    {
        return static_cast<int> (levels.size());
    }

    /**
     The measure can be suspended, e.g. to save CPU when no meter is displayed.
     In this case, the \see measureBlock will return immediately
     */
    void setSuspended (const bool shouldBeSuspended)
    {
        suspended = shouldBeSuspended;
    }

    bool checkNewDataFlag() const
    {
        return newDataFlag;
    }

    void resetNewDataFlag()
    {
        newDataFlag = false;
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LevelMeterSource)
    juce::WeakReference<LevelMeterSource>::Master masterReference;
    friend class juce::WeakReference<LevelMeterSource>;

    std::vector<ChannelData> levels;

    juce::int64 holdMSecs;

    std::atomic<juce::int64> lastMeasurement;

    bool newDataFlag = true;

    bool suspended;
};

/*@}*/

} // end namespace FFAU
