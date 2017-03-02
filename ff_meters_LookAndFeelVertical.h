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

    ff_meters_LookAndFeelVertical.h
    Author:  Daniel Walz

 ==============================================================================
 */

#ifndef FF_METERS_LOOKAND_FEEL_VERTICAL_H_INCLUDED
#define FF_METERS_LOOKAND_FEEL_VERTICAL_H_INCLUDED



class LevelMeterLookAndFeelVertical : public LevelMeterLookAndFeel {
public:
    LevelMeterLookAndFeelVertical ()
    {
    }


    void drawMeters (juce::Graphics& g,
                     const juce::Rectangle<float> bounds,
                     LevelMeterSource* source) override
    {
        juce::Graphics::ScopedSaveState saved (g);

        const int numChannels = source ? source->getNumChannels () : 2;
        drawMetersBackground (g, bounds);

        const float corner = getCornerSize (bounds);
        const float barWidth  = (bounds.getWidth() - 2.0 * corner) / (2 * numChannels - 1);
        const float barHeight = bounds.getHeight() - 2.0 * corner;
        const float y = bounds.getY() + corner;
        for (int channel=0; channel < numChannels; ++channel) {
            const float x = bounds.getX () + corner + 2 * channel * barWidth;

            if (source) {
                drawMeterBar (g, juce::Rectangle<float> (x, y, barWidth, barHeight).reduced (2, 5),
                              source->getMaxLevel (channel),
                              source->getRMSLevel (channel),
                              source->getReductionLevel (channel));
            }
            else {
                drawMeterBar (g, juce::Rectangle<float> (x, y, barWidth, barHeight).reduced (2, 5),
                              0.0f, 0.0f);
            }

            if (channel < numChannels - 1) {
                drawMeterTicks (g, juce::Rectangle<float> (x + barWidth, y, barWidth, barHeight).reduced (2, 5));
            }
        }
    }

    void drawMetersBackground (juce::Graphics& g,
                               const juce::Rectangle<float> bounds) override
    {
        const float corner = getCornerSize (bounds);
        g.setColour (getMeterColour (lmBackgroundColour));
        g.fillRoundedRectangle (bounds, corner);
        g.setColour (getMeterColour (lmTicksColour));
        g.drawRoundedRectangle (bounds.reduced (3.0), corner, 1.0);
    }

    void drawMeterTicks (juce::Graphics& g,
                         const juce::Rectangle<float> bounds) override
    {
        g.setColour (getMeterColour (lmTicksColour));
        for (int i=0; i<11; ++i) {
            g.drawHorizontalLine (bounds.getY() + i * 0.1f * bounds.getHeight(),
                                  bounds.getX() + 4,
                                  bounds.getRight() - 4);
        }
    }

    void drawMeterBar (juce::Graphics& g,
                       const juce::Rectangle<float> bounds,
                       const float max,
                       const float rms,
                       const float reduction = -1.0 ) override
    {
        const float infinity = -50.0f;
        const float maxDb = juce::Decibels::gainToDecibels (max, infinity);
        const float rmsDb = juce::Decibels::gainToDecibels (rms, infinity);

        g.setColour (getMeterColour (lmMeterBackgroundColour));
        g.fillRect  (bounds);

        juce::ColourGradient gradient (getMeterColour (lmMeterGradientLowColour),
                                       bounds.getX(), bounds.getBottom(),
                                       getMeterColour (lmMeterGradientMaxColour),
                                       bounds.getX(), bounds.getY(), false);
        gradient.addColour (0.5, getMeterColour (lmMeterGradientLowColour));
        gradient.addColour (0.75, getMeterColour (lmMeterGradientMidColour));
        g.setGradientFill (gradient);
        g.fillRect (bounds.withTop (bounds.getY() + rmsDb * bounds.getHeight() / infinity));

        if (maxDb > -49.0) {
            g.setColour (getMeterColour ((maxDb > -0.3f) ? lmMeterMaxOverColour :
                                         ((maxDb > -5.0) ? lmMeterMaxWarnColour : lmMeterMaxNormalColour)));
            g.drawHorizontalLine (bounds.getY() + juce::jmax (maxDb * bounds.getHeight() / infinity, 0.0f),
                                  bounds.getX(), bounds.getRight());
        }
        if (reduction >= 0.0) {
            g.setColour (getMeterColour (lmMeterReductionColour));
            g.fillRect (bounds.getX() + bounds.getWidth() * 0.75f,
                        bounds.getY(),
                        bounds.getWidth() * 0.25f,
                        bounds.getHeight() * (1.0f - reduction));
        }
    }
};

#endif /* FF_METERS_LOOKAND_FEEL_VERTICAL_H_INCLUDED */
