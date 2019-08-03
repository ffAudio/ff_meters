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

    \file ff_meters_LookAndFeel.h
    Author:  Daniel Walz

 ==============================================================================
 */

#pragma once

namespace foleys
{

class VerticalMultiChannelMeter : public LevelMeter::LookAndFeelMethods
{
public:
    VerticalMultiChannelMeter() = default;

    void drawMeter (juce::Graphics& g, const LevelMeter& meter, juce::Rectangle<float> bounds) override
    {
        g.setColour (colourMap [LevelMeter::BackgroundColour]);
        g.fillRect (bounds);

        auto* source = meter.getMeterSource();
        const auto offset = meter.getFirstChannel();
        const auto numChannels = meter.getNumChannels();
        if (source == nullptr || numChannels < 1)
            return;

        for (int index = 0; index < numChannels; ++index)
        {
            const auto area  = getChannelArea (bounds, numChannels, index);
            const auto clip  = getClipIndicatorBounds (area);
            const auto max   = getMeterMaxNumberBounds (area);
            const auto bar   = getMeterBarBounds (area);

            g.setColour (colourMap [LevelMeter::MeterBackgroundColour]);
            g.fillRect (max);
            g.fillRect (bar);

            const auto currentRMS = juce::Decibels::gainToDecibels (source->getRMSLevel (index + offset), infinity);
            const auto currentMax = juce::Decibels::gainToDecibels (source->getMaxLevel (index + offset), infinity);
            const auto overallMax = juce::Decibels::gainToDecibels (source->getMaxOverallLevel (index + offset), infinity);

            const auto hasClipped = source->getClipFlag (index + offset);
            g.setColour (hasClipped ? colourMap [LevelMeter::ClipClippedColour]
                                    : colourMap [LevelMeter::ClipOffColour]);
            g.fillRect (clip);
            g.setColour (colourMap [LevelMeter::OutlineColour]);
            g.drawRect (clip);
            g.drawRect (bar);
            g.drawRect (max);

            g.setColour (hasClipped ? colourMap [LevelMeter::TextClipColour]
                                    : colourMap [LevelMeter::TextColour]);
            g.drawFittedText (juce::String (overallMax, 1) + " dB",
                              max.toNearestInt().reduced (1), juce::Justification::centred, 1);

            g.setColour (colourMap [LevelMeter::TicksColour]);
            const auto ticks = juce::Rectangle<float> (bar.getRight() + 1, bar.getY(),
                                                       max.getRight() - (bar.getRight() + 1), bar.getHeight());
            const auto h = (ticks.getHeight() - 2.0f) * 0.05f;
            g.setFont (h * 0.8f);
            for (int i=0; i<21; ++i) {
                const float y = ticks.getY() + i * h;
                if (i % 2 == 0)
                {
                    g.drawHorizontalLine (juce::roundToInt (y + 1),
                                          ticks.getX() + 4,
                                          ticks.getRight());
                    if (i < 20)
                    {
                        auto box = ticks.withY (y + 4).withHeight (h * 0.6f);
                        g.drawFittedText (juce::String (i * 0.05f * infinity),
                                          box.getSmallestIntegerContainer(),
                                          juce::Justification::topRight, 1);
                    }
                }
                else
                {
                    g.drawHorizontalLine (juce::roundToInt (y + 2),
                                          ticks.getX() + 4,
                                          ticks.getCentreX());
                }
            }

            juce::ColourGradient gradient (colourMap [LevelMeter::MeterGradientLowColour],
                                           bar.getX(), bar.getBottom(),
                                           colourMap [LevelMeter::MeterGradientMaxColour],
                                           bar.getX(), bar.getY(), false);
            gradient.addColour (0.5,  colourMap [LevelMeter::MeterGradientLowColour]);
            gradient.addColour (0.75, colourMap [LevelMeter::MeterGradientMidColour]);
            g.setGradientFill (gradient);

            g.fillRect (bar.withTop (juce::jmap (currentRMS,
                                                 infinity, 0.0f, bar.getBottom(), bar.getY())));

            g.setColour (currentMax > -0.3f ? colourMap [LevelMeter::MeterMaxOverColour]
                                            : colourMap [LevelMeter::MeterMaxNormalColour]);
            g.drawHorizontalLine (juce::roundToInt (juce::jmap (std::min (currentMax, 0.0f),
                                                                infinity, 0.0f, bar.getBottom(), bar.getY())),
                                  bar.getX(), bar.getRight());

        }
    }

    /** This is called by the frontend to check, if the clip indicator was clicked (e.g. for reset) */
    int hitTestClipIndicator (const LevelMeter& meter,
                              const juce::Point<float> position,
                              const juce::Rectangle<float> bounds) const override
    {
        const auto numChannels = meter.getNumChannels();

        for (int index = 0; index < meter.getNumChannels(); ++index)
            if (getClipIndicatorBounds (getChannelArea (bounds, numChannels, index)).contains (position))
                return index - meter.getFirstChannel();

        return -1;
    }

    /** This is called by the frontend to check, if the maximum level number was clicked (e.g. for reset) */
    int hitTestMaxNumber (const LevelMeter& meter,
                          const juce::Point<float> position,
                          const juce::Rectangle<float> bounds) const override
    {
        const auto numChannels = meter.getNumChannels();

        for (int index = 0; index < meter.getNumChannels(); ++index)
            if (getMeterMaxNumberBounds (getChannelArea (bounds, numChannels, index)).contains (position))
                return index - meter.getFirstChannel();

        return -1;
    }

private:

    static constexpr float infinity = -100.0f;

    juce::Rectangle<float> getChannelArea (juce::Rectangle<float> bounds, int numChannels, int index) const
    {
        const auto width = bounds.getWidth() / numChannels;
        return { bounds.getX() + width * index, bounds.getY(), width, bounds.getHeight() };
    }

    juce::Rectangle<float> getClipIndicatorBounds(juce::Rectangle<float> bounds) const
    {
        const auto margin = bounds.getWidth() * 0.05f;
        const auto w      = bounds.getWidth() * 0.35f;
        return juce::Rectangle<float>(bounds.getX() + margin,
                                      bounds.getY() + margin,
                                      w,
                                      w * 0.5f);
    }

    juce::Rectangle<float> getMeterMaxNumberBounds (const juce::Rectangle<float> bounds) const
    {
        const auto margin = bounds.getWidth() * 0.05f;
        return juce::Rectangle<float>(bounds.getX() + margin,
                                      bounds.getBottom() - (margin + 25),
                                      bounds.getWidth() - 2 * margin,
                                      25.0f);
    }

    juce::Rectangle<float> getMeterBarBounds(juce::Rectangle<float> bounds) const
    {
        const auto margin = bounds.getWidth() * 0.05f;
        const auto w      = bounds.getWidth() * 0.35f;
        return {
            bounds.getX() + margin,
            bounds.getY() + 2.0f * margin + w * 0.5f,
            w,
            bounds.getHeight() - (4.0f * margin + w * 0.5f + 25.0f)
        };
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VerticalMultiChannelMeter)
};

class VerticalMinimalMultiChannelMeter : public LevelMeter::LookAndFeelMethods
{
public:

    VerticalMinimalMultiChannelMeter() = default;

    void drawMeter (juce::Graphics& g, const LevelMeter& meter, juce::Rectangle<float> bounds) override
    {
        g.setColour (colourMap [LevelMeter::BackgroundColour]);
        g.fillRect (bounds);

        auto* source = meter.getMeterSource();
        const auto offset = meter.getFirstChannel();
        const auto numChannels = meter.getNumChannels();
        if (source == nullptr || numChannels < 1)
            return;

        auto bars = bounds;
        auto clips = bars.removeFromTop (12).reduced (2, 0);
        bars = bars.reduced (2);

        const auto w = bars.getWidth() / (numChannels + 1);

        drawTicks (g, bars.removeFromRight (w).reduced (2));
        clips.removeFromRight (w);

        for (int index = 0; index < numChannels; ++index)
        {
            const auto currentRMS = juce::Decibels::gainToDecibels (source->getRMSLevel (index + offset), infinity);
            const auto currentMax = juce::Decibels::gainToDecibels (source->getMaxLevel (index + offset), infinity);

            const auto clip  = clips.removeFromLeft (w).reduced (2);
            const auto bar   = bars.removeFromLeft (w).reduced (2);

            g.setColour (colourMap [LevelMeter::MeterBackgroundColour]);
            g.fillRect (bar);

            const auto hasClipped = source->getClipFlag (index + offset);
            g.setColour (hasClipped ? colourMap [LevelMeter::ClipClippedColour]
                         : colourMap [LevelMeter::ClipOffColour]);
            g.fillRect (clip);

            drawVerticalBar (g, bar, currentRMS, currentMax);

            g.setColour (colourMap [LevelMeter::OutlineColour]);
            g.drawRect (clip);
            g.drawRect (bar);

        }
    }

    /** This is called by the frontend to check, if the clip indicator was clicked (e.g. for reset) */
    int hitTestClipIndicator (const LevelMeter& meter,
                              const juce::Point<float> position,
                              const juce::Rectangle<float> bounds) const override
    {
        const auto numChannels = meter.getNumChannels();

        for (int index = 0; index < meter.getNumChannels(); ++index)
            if (getClipIndicatorBounds (getChannelArea (bounds, numChannels, index)).contains (position))
                return index - meter.getFirstChannel();

        return -1;
    }

    /** This is called by the frontend to check, if the maximum level number was clicked (e.g. for reset) */
    int hitTestMaxNumber (const LevelMeter& meter,
                          const juce::Point<float> position,
                          const juce::Rectangle<float> bounds) const override
    {
        const auto numChannels = meter.getNumChannels();

        for (int index = 0; index < meter.getNumChannels(); ++index)
            if (getMeterMaxNumberBounds (getChannelArea (bounds, numChannels, index)).contains (position))
                return index - meter.getFirstChannel();

        return -1;
    }

private:

    static constexpr float infinity = -100.0f;

    void drawTicks (juce::Graphics& g, juce::Rectangle<float> ticks)
    {
        g.setColour (colourMap [LevelMeter::TicksColour]);

        const auto h = (ticks.getHeight() - 2.0f) * 0.05f;
        g.setFont (h * 0.8f);
        for (int i=0; i<21; ++i) {
            const float y = ticks.getY() + i * h;
            if (i % 2 == 0)
            {
                g.drawHorizontalLine (juce::roundToInt (y + 1),
                                      ticks.getX() + 4,
                                      ticks.getRight());
                if (i < 20)
                {
                    auto box = ticks.withY (y + 4).withHeight (h * 0.6f);
                    g.drawFittedText (juce::String (i * 0.05f * infinity),
                                      box.getSmallestIntegerContainer(),
                                      juce::Justification::topRight, 1);
                }
            }
            else
            {
                g.drawHorizontalLine (juce::roundToInt (y + 2),
                                      ticks.getX() + 4,
                                      ticks.getCentreX());
            }
        }
    }

    void drawVerticalBar (juce::Graphics& g, juce::Rectangle<float> bar, float currentRMS, float currentMax)
    {
        juce::ColourGradient gradient (colourMap [LevelMeter::MeterGradientLowColour],
                                       bar.getX(), bar.getBottom(),
                                       colourMap [LevelMeter::MeterGradientMaxColour],
                                       bar.getX(), bar.getY(), false);
        gradient.addColour (0.5,  colourMap [LevelMeter::MeterGradientLowColour]);
        gradient.addColour (0.75, colourMap [LevelMeter::MeterGradientMidColour]);
        g.setGradientFill (gradient);

        g.fillRect (bar.withTop (juce::jmap (currentRMS,
                                             infinity, 0.0f, bar.getBottom(), bar.getY())));

        g.setColour (currentMax > -0.3f ? colourMap [LevelMeter::MeterMaxOverColour]
                     : colourMap [LevelMeter::MeterMaxNormalColour]);
        g.drawHorizontalLine (juce::roundToInt (juce::jmap (std::min (currentMax, 0.0f),
                                                            infinity, 0.0f, bar.getBottom(), bar.getY())),
                              bar.getX(), bar.getRight());
    }

    juce::Rectangle<float> getChannelArea (juce::Rectangle<float> bounds, int numChannels, int index) const
    {
        const auto width = bounds.getWidth() / numChannels;
        return { bounds.getX() + width * index, bounds.getY(), width, bounds.getHeight() };
    }

    juce::Rectangle<float> getClipIndicatorBounds(juce::Rectangle<float> bounds) const
    {
        const auto margin = bounds.getWidth() * 0.05f;
        const auto w      = bounds.getWidth() * 0.35f;
        return juce::Rectangle<float>(bounds.getX() + margin,
                                      bounds.getY() + margin,
                                      w,
                                      w * 0.5f);
    }

    juce::Rectangle<float> getMeterMaxNumberBounds (const juce::Rectangle<float>) const
    {
        return {};
    }

    juce::Rectangle<float> getMeterBarBounds(juce::Rectangle<float> bounds) const
    {
        const auto margin = bounds.getWidth() * 0.05f;
        const auto w      = bounds.getWidth() * 0.35f;
        return {
            bounds.getX() + margin,
            bounds.getY() + 2.0f * margin + w * 0.5f,
            w,
            bounds.getHeight() - (4.0f * margin + w * 0.5f)
        };
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VerticalMinimalMultiChannelMeter)
};



} // end namespace foleys
