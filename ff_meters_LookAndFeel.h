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


class LevelMeterLookAndFeel : public juce::LookAndFeel_V3, public LevelMeter::LookAndFeelMethods
{
public:
    LevelMeterLookAndFeel ()
    {
        setColour (LevelMeter::lmTextColour,             juce::Colours::lightgrey);
        setColour (LevelMeter::lmTextClipColour,         juce::Colours::white);
        setColour (LevelMeter::lmTextDeactiveColour,     juce::Colours::darkgrey);
        setColour (LevelMeter::lmTicksColour,            juce::Colours::orange);
        setColour (LevelMeter::lmOutlineColour,          juce::Colours::orange);
        setColour (LevelMeter::lmBackgroundColour,       juce::Colour (0xff050a29));
        setColour (LevelMeter::lmBackgroundClipColour,   juce::Colours::red);
        setColour (LevelMeter::lmMeterForegroundColour,  juce::Colours::green);
        setColour (LevelMeter::lmMeterOutlineColour,     juce::Colours::lightgrey);
        setColour (LevelMeter::lmMeterBackgroundColour,  juce::Colours::darkgrey);
        setColour (LevelMeter::lmMeterMaxNormalColour,   juce::Colours::lightgrey);
        setColour (LevelMeter::lmMeterMaxWarnColour,     juce::Colours::orange);
        setColour (LevelMeter::lmMeterMaxOverColour,     juce::Colours::darkred);
        setColour (LevelMeter::lmMeterGradientLowColour, juce::Colours::green);
        setColour (LevelMeter::lmMeterGradientMidColour, juce::Colours::lightgoldenrodyellow);
        setColour (LevelMeter::lmMeterGradientMaxColour, juce::Colours::red);
        setColour (LevelMeter::lmMeterReductionColour,   juce::Colours::orange);
    }

    virtual ~LevelMeterLookAndFeel() {}

    virtual juce::Rectangle<float> getMeterInnerBounds (const juce::Rectangle<float> bounds,
                                                        const LevelMeter::MeterFlags meterType) const override
    {
        if (meterType & LevelMeter::HasBorder) {
            const float corner = std::min (bounds.getWidth(), bounds.getHeight()) * 0.01;
            return bounds.reduced (3 + corner);
        }
        return bounds;
    }

    juce::Rectangle<float> getMeterBounds (const juce::Rectangle<float> bounds,
                                           const LevelMeter::MeterFlags meterType,
                                           const int numChannels,
                                           const int channel) const override
    {
        if (meterType & LevelMeter::SingleChannel) {
            return bounds;
        }
        else {
            if (meterType & LevelMeter::Horizontal) {
                const float h = bounds.getHeight() / numChannels;
                return bounds.withHeight (h).withY (bounds.getY() + channel * h);
            }
            else {
                const float w = bounds.getWidth() / numChannels;
                return bounds.withWidth (w).withX (bounds.getX() + channel * w);
            }
        }
        return juce::Rectangle<float> ();
    }

    /** Override this callback to define the placement of the actual meter bar. */
    juce::Rectangle<float> getMeterBarBounds (const juce::Rectangle<float> bounds,
                                              const LevelMeter::MeterFlags meterType) const override
    {
        if (meterType & LevelMeter::Minimal) {
            if (meterType & LevelMeter::Horizontal) {
                const float margin = bounds.getHeight() * 0.05;
                const float h      = bounds.getHeight() - 2.0 * margin;
                const float left   = bounds.getX() + margin;
                const float right  = bounds.getRight() - (4.0 * margin + h * 0.5);
                return juce::Rectangle<float>(bounds.getX() + margin,
                                              bounds.getY() + margin,
                                              right - left,
                                              h);
            }
            else {
                const float margin = bounds.getWidth() * 0.05;
                const float top    = bounds.getY() + 2.0 * margin + bounds.getWidth() * 0.5;
                const float bottom = bounds.getBottom() - margin;
                return juce::Rectangle<float>(bounds.getX() + margin, top,
                                              bounds.getWidth() - margin * 2.0, bottom - top);
            }
        }
        else if (meterType & LevelMeter::Vintage) {
            return bounds;
        }
        else {
            if (meterType & LevelMeter::Horizontal) {
                const float margin = bounds.getHeight() * 0.05;
                const float h      = bounds.getHeight() * 0.5 - 2.0 * margin;
                const float left   = 60.0 + 3.0 * margin;
                const float right  = bounds.getRight() - (4.0 * margin + h * 0.5);
                return juce::Rectangle<float>(bounds.getX() + left,
                                              bounds.getY() + margin,
                                              right - left,
                                              h);
            }
            else {
                const float margin = bounds.getWidth() * 0.05;
                const float w      = bounds.getWidth() * 0.45;
                const float top    = bounds.getY() + 2.0 * margin + w * 0.5;
                const float bottom = bounds.getBottom() - (2.0 * margin + 25.0);
                return juce::Rectangle<float>(bounds.getX() + margin, top, w, bottom - top);
            }
        }
        return juce::Rectangle<float> ();
    }

    /** Override this callback to define the placement of the tickmarks.
     To disable this feature return an empty rectangle. */
    juce::Rectangle<float> getMeterTickmarksBounds (const juce::Rectangle<float> bounds,
                                                    const LevelMeter::MeterFlags meterType) const override
    {
        if (meterType & LevelMeter::Minimal) {
            if (meterType & LevelMeter::Horizontal) {
                return getMeterBarBounds(bounds, meterType).reduced (0.0, 2.0);
            }
            else {
                return getMeterBarBounds(bounds, meterType).reduced (2.0, 0.0);
            }
        }
        else if (meterType & LevelMeter::Vintage) {
            return bounds;
        } else {
            if (meterType & LevelMeter::Horizontal) {
                const float margin = bounds.getHeight() * 0.05;
                const float h      = bounds.getHeight() * 0.5 - 2.0 * margin;
                const float left   = 60.0 + 3.0 * margin;
                const float right  = bounds.getRight() - (4.0 * margin + h * 0.5);
                return juce::Rectangle<float>(bounds.getX() + left,
                                              bounds.getCentreY() + margin,
                                              right - left,
                                              h);
            }
            else {
                const float margin = bounds.getWidth() * 0.05;
                const float w      = bounds.getWidth() * 0.45;
                const float top    = bounds.getY() + 2.0 * margin + w * 0.5 + 2.0;
                const float bottom = bounds.getBottom() - (2.0 * margin + 25.0 + 2.0);
                return juce::Rectangle<float>(bounds.getCentreX(), top, w, bottom - top);
            }
        }
        return juce::Rectangle<float> ();
    }

    /** Override this callback to define the placement of the clip indicator light.
     To disable this feature return an empty rectangle. */
    juce::Rectangle<float> getMeterClipIndicatorBounds (const juce::Rectangle<float> bounds,
                                                        const LevelMeter::MeterFlags meterType) const override
    {
        if (meterType & LevelMeter::Minimal) {
            if (meterType & LevelMeter::Horizontal) {
                const float margin = bounds.getHeight() * 0.05;
                const float h      = bounds.getHeight() - 2.0 * margin;
                return juce::Rectangle<float>(bounds.getRight() - (margin + h * 0.5),
                                              bounds.getY() + margin,
                                              h * 0.5,
                                              h);
            }
            else {
                const float margin = bounds.getWidth() * 0.05;
                const float w      = bounds.getWidth() - margin * 2.0;
                return juce::Rectangle<float>(bounds.getX() + margin,
                                              bounds.getY() + margin,
                                              w,
                                              w * 0.5);
            }
        }
        else if (meterType & LevelMeter::Vintage) {
            return bounds;
        }
        else {
            if (meterType & LevelMeter::Horizontal) {
                const float margin = bounds.getHeight() * 0.05;
                const float h      = bounds.getHeight() * 0.5 - 2.0 * margin;
                return juce::Rectangle<float>(bounds.getRight() - (margin + h * 0.5),
                                              bounds.getY() + margin,
                                              h * 0.5,
                                              h);
            }
            else {
                const float margin = bounds.getWidth() * 0.05;
                const float w      = bounds.getWidth() * 0.45;
                return juce::Rectangle<float>(bounds.getX() + margin,
                                              bounds.getY() + margin,
                                              w,
                                              w * 0.5);
            }
        }
        return juce::Rectangle<float> ();
    }

    /** Override this callback to define the placement of the max level.
     To disable this feature return an empty rectangle. */
    juce::Rectangle<float> getMeterMaxNumberBounds (const juce::Rectangle<float> bounds,
                                                    const LevelMeter::MeterFlags meterType) const override
    {
        if (meterType & LevelMeter::Minimal) {
            return juce::Rectangle<float>();
        }
        else if (meterType & LevelMeter::Vintage) {
            return bounds;
        }
        else {
            if (meterType & LevelMeter::Horizontal) {
                const float margin = bounds.getHeight() * 0.05;
                return juce::Rectangle<float>(bounds.getX() + margin,
                                              bounds.getCentreY() + margin,
                                              60,
                                              bounds.getHeight() * 0.5 - margin * 2.0);
            }
            else {
                const float margin = bounds.getWidth() * 0.05;
                return juce::Rectangle<float>(bounds.getX() + margin,
                                              bounds.getBottom() - (margin + 25),
                                              bounds.getWidth() - 2 * margin,
                                              25.0);
            }
        }
    }

    juce::Rectangle<float> drawBackground (juce::Graphics& g,
                                           const LevelMeter::MeterFlags meterType,
                                           const juce::Rectangle<float> bounds) override
    {
        g.setColour (findColour (LevelMeter::lmBackgroundColour));
        if (meterType & LevelMeter::HasBorder) {
            const float corner = std::min (bounds.getWidth(), bounds.getHeight()) * 0.01;
            g.fillRoundedRectangle (bounds, corner);
            g.setColour (findColour (LevelMeter::lmOutlineColour));
            g.drawRoundedRectangle (bounds.reduced (3), corner, 2);
            return bounds.reduced (3 + corner);
        }
        else {
            g.fillRect (bounds);
            return bounds;
        }
    }

    void drawMeterBars (juce::Graphics& g,
                        const LevelMeter::MeterFlags meterType,
                        const juce::Rectangle<float> bounds,
                        const LevelMeterSource* source,
                        const int selectedChannel=-1) override
    {
        const juce::Rectangle<float> innerBounds = getMeterInnerBounds (bounds, meterType);
        if (source) {
            const int numChannels = source->getNumChannels();
            if (meterType & LevelMeter::Minimal) {
                if (meterType & LevelMeter::Horizontal) {
                    const float height = innerBounds.getHeight() / (2 * numChannels - 1);
                    juce::Rectangle<float> meter = innerBounds.withHeight (height);
                    for (int channel=0; channel < numChannels; ++channel) {
                        meter.setY (height * channel * 2);
                        drawMeterBar (g, meterType, getMeterBarBounds (meter, meterType),
                                      source->getRMSLevel (channel),
                                      source->getMaxLevel (channel));
                        juce::Rectangle<float> clip = getMeterClipIndicatorBounds (meter, meterType);
                        if (! clip.isEmpty())
                            drawClipIndicator (g, meterType, clip, source->getClipFlag (channel));
                        if (channel < numChannels-1) {
                            meter.setY (height * (channel * 2 + 1));
                            juce::Rectangle<float> ticks = getMeterTickmarksBounds (meter, meterType);
                            if (! ticks.isEmpty())
                                drawTickMarks (g, meterType, ticks);
                        }
                    }
                }
                else {
                    const float width = innerBounds.getWidth() / (2 * numChannels - 1);
                    juce::Rectangle<float> meter = innerBounds.withWidth(width);
                    for (int channel=0; channel < numChannels; ++channel) {
                        meter.setX (width * channel * 2);
                        drawMeterBar (g, meterType, getMeterBarBounds (meter, meterType),
                                      source->getRMSLevel (channel),
                                      source->getMaxLevel (channel));
                        juce::Rectangle<float> clip = getMeterClipIndicatorBounds (meter, meterType);
                        if (! clip.isEmpty())
                            drawClipIndicator (g, meterType, clip, source->getClipFlag (channel));
                        if (channel < numChannels-1) {
                            meter.setX (width * (channel * 2 + 1));
                            juce::Rectangle<float> ticks = getMeterTickmarksBounds (meter, meterType);
                            if (! ticks.isEmpty())
                                drawTickMarks (g, meterType, ticks);
                        }
                    }
                }
            }
            else if (meterType & LevelMeter::SingleChannel) {
                if (selectedChannel >= 0) {
                    drawMeterChannel (g, meterType, innerBounds, source, selectedChannel);
                }
            }
            else {
                const int numChannels = source->getNumChannels();
                for (int channel=0; channel < numChannels; ++channel) {
                    drawMeterChannel (g, meterType,
                                      getMeterBounds (innerBounds, meterType, numChannels, channel),
                                      source, channel);
                }
            }
        }
    }

    void drawMeterChannel (juce::Graphics& g,
                           const LevelMeter::MeterFlags meterType,
                           const juce::Rectangle<float> bounds,
                           const LevelMeterSource* source,
                           const int selectedChannel) override
    {
        if (source) {
            juce::Rectangle<float> meter = getMeterBarBounds (bounds, meterType);
            if (! meter.isEmpty()) {
                if (meterType & LevelMeter::Reduction) {
                    drawMeterBar (g, meterType, meter,
                                  source->getReductionLevel (selectedChannel),
                                  0.0f);
                }
                else {
                    drawMeterBar (g, meterType, meter,
                                  source->getRMSLevel (selectedChannel),
                                  source->getMaxLevel (selectedChannel));
                }
            }
            juce::Rectangle<float> clip = getMeterClipIndicatorBounds (bounds, meterType);
            if (! clip.isEmpty())
                drawClipIndicator (g, meterType, clip, source->getClipFlag (selectedChannel));
            juce::Rectangle<float> ticks = getMeterTickmarksBounds (bounds, meterType);
            if (! ticks.isEmpty())
                drawTickMarks (g, meterType, ticks);
            juce::Rectangle<float> maxes = getMeterMaxNumberBounds (bounds, meterType);
            if (! maxes.isEmpty()) {
                if (meterType & LevelMeter::Reduction) {
                    drawMaxNumber (g, meterType, maxes, source->getReductionLevel (selectedChannel));
                }
                else {
                    drawMaxNumber (g, meterType, maxes, source->getMaxOverallLevel(selectedChannel));
                }
            }
        }
    }

    void drawMeterBar (juce::Graphics& g,
                       const LevelMeter::MeterFlags meterType,
                       const juce::Rectangle<float> bounds,
                       const float rms, const float peak) override
    {
        const float infinity = meterType & LevelMeter::Reduction ? -30.0f :  -80.0f;
        const float rmsDb  = juce::Decibels::gainToDecibels (rms,  infinity);
        const float peakDb = juce::Decibels::gainToDecibels (peak, infinity);
        g.setColour (findColour (LevelMeter::lmMeterBackgroundColour));
        g.fillRect  (bounds);

        if (meterType & LevelMeter::Vintage) {
            // TODO
        }
        else if (meterType & LevelMeter::Reduction) {
            const float limitDb = juce::Decibels::gainToDecibels (rms, infinity);
            g.setColour (findColour (LevelMeter::lmMeterReductionColour));
            if (meterType & LevelMeter::Horizontal) {
                g.fillRect (bounds.withLeft (bounds.getX() + limitDb * bounds.getWidth() / infinity));
            }
            else {
                g.fillRect (bounds.withBottom (bounds.getY() + limitDb * bounds.getHeight() / infinity));
            }
        }
        else {
            if (meterType & LevelMeter::Horizontal) {
                juce::ColourGradient gradient (findColour (LevelMeter::lmMeterGradientLowColour),
                                               bounds.getX(), bounds.getY(),
                                               findColour (LevelMeter::lmMeterGradientMaxColour),
                                               bounds.getRight(), bounds.getY(), false);
                gradient.addColour (0.5, findColour (LevelMeter::lmMeterGradientLowColour));
                gradient.addColour (0.75, findColour (LevelMeter::lmMeterGradientMidColour));
                g.setGradientFill (gradient);
                g.fillRect (bounds.withRight (bounds.getX() - rmsDb * bounds.getWidth() / infinity));

                if (peakDb > -49.0) {
                    g.setColour (findColour ((peakDb > -0.3f) ? LevelMeter::lmMeterMaxOverColour :
                                             ((peakDb > -5.0) ? LevelMeter::lmMeterMaxWarnColour :
                                              LevelMeter::lmMeterMaxNormalColour)));
                    g.drawVerticalLine (bounds.getRight() - juce::jmax (peakDb * bounds.getWidth() / infinity, 0.0f),
                                        bounds.getY(), bounds.getBottom());
                }
            }
            else {
                juce::ColourGradient gradient (findColour (LevelMeter::lmMeterGradientLowColour),
                                               bounds.getX(), bounds.getBottom(),
                                               findColour (LevelMeter::lmMeterGradientMaxColour),
                                               bounds.getX(), bounds.getY(), false);
                gradient.addColour (0.5, findColour (LevelMeter::lmMeterGradientLowColour));
                gradient.addColour (0.75, findColour (LevelMeter::lmMeterGradientMidColour));
                g.setGradientFill (gradient);
                g.fillRect (bounds.withTop (bounds.getY() + rmsDb * bounds.getHeight() / infinity));

                if (peakDb > -49.0) {
                    g.setColour (findColour ((peakDb > -0.3f) ? LevelMeter::lmMeterMaxOverColour :
                                             ((peakDb > -5.0) ? LevelMeter::lmMeterMaxWarnColour :
                                              LevelMeter::lmMeterMaxNormalColour)));
                    g.drawHorizontalLine (bounds.getY() + juce::jmax (peakDb * bounds.getHeight() / infinity, 0.0f),
                                          bounds.getX(), bounds.getRight());
                }
            }
        }
        g.setColour (findColour (LevelMeter::lmMeterOutlineColour));
        g.drawRect (bounds, 1.0);
    }

    void drawTickMarks (juce::Graphics& g,
                        const LevelMeter::MeterFlags meterType,
                        const juce::Rectangle<float> bounds) override
    {
        const float infinity = meterType & LevelMeter::Reduction ? -30.0f :  -80.0f;

        g.setColour (findColour (LevelMeter::lmTicksColour));
        if (meterType & LevelMeter::Minimal) {
            if (meterType & LevelMeter::Horizontal) {
                for (int i=0; i<11; ++i)
                    g.drawVerticalLine (bounds.getX() + i * 0.1f * bounds.getWidth(),
                                        bounds.getY() + 4,
                                        bounds.getBottom() - 4);
            }
            else {
                const float h = (bounds.getHeight() - 2.0) * 0.1;
                g.setFont (h * 0.8f);
                for (int i=0; i<11; ++i) {
                    const float y = bounds.getY() + i * h;
                    g.drawHorizontalLine (y + 1,
                                          bounds.getX() + 4,
                                          bounds.getRight());
                    if (i < 10) {
                        g.drawFittedText (juce::String (i * 0.05 * infinity),
                                          bounds.getX(), y + 4, bounds.getWidth(), h * 0.6,
                                          juce::Justification::centredTop, 1);
                    }
                }
            }
        }
        else if (meterType & LevelMeter::Vintage) {
            // TODO
        }
        else {
            if (meterType & LevelMeter::Horizontal) {
                for (int i=0; i<11; ++i)
                    g.drawVerticalLine (bounds.getX() + i * 0.1f * bounds.getWidth(),
                                        bounds.getY() + 4,
                                        bounds.getBottom() - 4);
            }
            else {
                const float h = (bounds.getHeight() - 2.0) * 0.05;
                g.setFont (h * 0.8f);
                for (int i=0; i<21; ++i) {
                    const float y = bounds.getY() + i * h;
                    if (i % 2 == 0) {
                        g.drawHorizontalLine (y + 1,
                                              bounds.getX() + 4,
                                              bounds.getRight());
                        if (i < 20) {
                            g.drawFittedText (juce::String (i * 0.05 * infinity),
                                              bounds.getX(), y + 4, bounds.getWidth(), h * 0.6,
                                              juce::Justification::topRight, 1);
                        }
                    }
                    else {
                        g.drawHorizontalLine (y + 2,
                                              bounds.getX() + 4,
                                              bounds.getCentreX());
                    }
                }
            }
        }
    }

    void drawClipIndicator (juce::Graphics& g,
                            const LevelMeter::MeterFlags meterType,
                            const juce::Rectangle<float> bounds,
                            const bool hasClipped) override
    {
        g.setColour (findColour (hasClipped ? LevelMeter::lmBackgroundClipColour : LevelMeter::lmMeterBackgroundColour));
        g.fillRect (bounds);
        g.setColour (findColour (LevelMeter::lmMeterOutlineColour));
        g.drawRect (bounds, 1.0);
    }

    void drawMaxNumber (juce::Graphics& g,
                        const LevelMeter::MeterFlags meterType,
                        const juce::Rectangle<float> bounds,
                        const float maxGain) override
    {
        g.setColour (findColour (LevelMeter::lmMeterBackgroundColour));
        g.fillRect (bounds);
        const float maxDb = juce::Decibels::gainToDecibels (maxGain, -80.0f);
        g.setColour (findColour (maxDb > 0.0 ? LevelMeter::lmTextClipColour : LevelMeter::lmTextColour));
        g.drawFittedText (juce::String (maxDb, 1) + " dB",
                          bounds.reduced (2.0).toNearestInt(),
                          juce::Justification::centred, 1);
        g.setColour (findColour (LevelMeter::lmMeterOutlineColour));
        g.drawRect (bounds, 1.0);
    }

    int hitTestClipIndicator (const juce::Point<int> position,
                              const LevelMeter::MeterFlags meterType,
                              const juce::Rectangle<float> bounds,
                              const LevelMeterSource* source) const override
    {
        if (source) {
            const int numChannels = source->getNumChannels();
            for (int i=0; i < numChannels; ++i) {
                if (getMeterClipIndicatorBounds (getMeterBounds
                                                 (bounds, meterType, source->getNumChannels(), i), meterType)
                    .contains (position.toFloat())) {
                    return i;
                }
            }
        }
        return -1;
    }

    int hitTestMaxNumber (const juce::Point<int> position,
                          const LevelMeter::MeterFlags meterType,
                          const juce::Rectangle<float> bounds,
                          const LevelMeterSource* source) const override
    {
        if (source) {
            const int numChannels = source->getNumChannels();
            for (int i=0; i < numChannels; ++i) {
                if (getMeterMaxNumberBounds (getMeterBounds
                                             (bounds, meterType, source->getNumChannels(), i), meterType)
                    .contains (position.toFloat())) {
                    return i;
                }
            }
        }
        return -1;
    }

};

#endif /* FF_METERS_LOOKAND_FEEL_H_INCLUDED */
