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

    ff_meters_LevelMeter.h
    Created: 5 Apr 2016 9:49:54am
    Author:  Daniel Walz

 ==============================================================================
*/

#ifndef LEVELMETER_H_INCLUDED
#define LEVELMETER_H_INCLUDED


//==============================================================================
/*
 This class is used to display a level reading. It supports max and RMS levels.
 You can also set a reduction value to display, the definition of that value is up to you.
*/
class LevelMeter    : public juce::Component, private juce::Timer
{
public:

    enum MeterType {
        VerticalBars = 0,
        VerticalBarSingle,
        HorizontalBars,
        HorizontalBarSingle,
        VerticalMinimal,
        HorizontalMinimal,
        AnalogMeterSingle,
        CustomMeterTypes = 256
    };

    enum ColourIds {
        lmTextColour = 0x2200001,
        lmTextDeactiveColour,
        lmTextClipColour,
        lmTicksColour,
        lmOutlineColour,
        lmBackgroundColour,
        lmBackgroundClipColour,
        lmMeterForegroundColour,
        lmMeterOutlineColour,
        lmMeterBackgroundColour,
        lmMeterMaxNormalColour,
        lmMeterMaxWarnColour,
        lmMeterMaxOverColour,
        lmMeterGradientLowColour,
        lmMeterGradientMidColour,
        lmMeterGradientMaxColour,
        lmMeterReductionColour
    };

    class LookAndFeelMethods {
    public:
        virtual ~LookAndFeelMethods() {}

        /** Override this to change the inner rectangle in case you want to paint a border e.g. */
        virtual juce::Rectangle<float> getMeterInnerBounds (const juce::Rectangle<float> bounds,
                                                            const MeterType meterType) const = 0;

        /** Override this callback to define the placement of a meter channel. */
        virtual juce::Rectangle<float> getMeterBounds (const juce::Rectangle<float> bounds,
                                                       const MeterType meterType,
                                                       const int numChannels,
                                                       const int channel) const = 0;

        /** Override this callback to define the placement of the actual meter bar. */
        virtual juce::Rectangle<float> getMeterBarBounds (const juce::Rectangle<float> bounds,
                                                          const MeterType meterType) const = 0;

        /** Override this callback to define the placement of the tickmarks.
         To disable this feature return an empty rectangle. */
        virtual juce::Rectangle<float> getMeterTickmarksBounds (const juce::Rectangle<float> bounds,
                                                                const MeterType meterType) const = 0;

        /** Override this callback to define the placement of the clip indicator light.
         To disable this feature return an empty rectangle. */
        virtual juce::Rectangle<float> getMeterClipIndicatorBounds (const juce::Rectangle<float> bounds,
                                                                    const MeterType meterType) const = 0;

        /** Override this callback to define the placement of the max level.
         To disable this feature return an empty rectangle. */
        virtual juce::Rectangle<float> getMeterMaxNumberBounds (const juce::Rectangle<float> bounds,
                                                                const MeterType meterType) const = 0;

        /** Override this to draw background and if wanted a frame. If the frame takes space away, 
         it should return the reduced bounds */
        virtual juce::Rectangle<float> drawBackground (juce::Graphics&,
                                                       const MeterType meterType,
                                                       const juce::Rectangle<float> bounds) = 0;

        virtual void drawMeterBars (juce::Graphics&,
                                    const MeterType meterType,
                                    const juce::Rectangle<float> bounds,
                                    const LevelMeterSource* source,
                                    const int selectedChannel=-1) = 0;

        virtual void drawMeterChannel (juce::Graphics&,
                                       const MeterType meterType,
                                       const juce::Rectangle<float> bounds,
                                       const LevelMeterSource* source,
                                       const int selectedChannel) = 0;

        virtual void drawMeterBar (juce::Graphics&,
                                   const MeterType meterType,
                                   const juce::Rectangle<float> bounds,
                                   const float rms, const float peak) = 0;

        virtual void drawTickMarks (juce::Graphics&,
                                    const MeterType meterType,
                                    const juce::Rectangle<float> bounds) = 0;

        virtual void drawClipIndicator (juce::Graphics&,
                                        const MeterType meterType,
                                        const juce::Rectangle<float> bounds,
                                        const bool hasClipped) = 0;

        virtual void drawMaxNumber (juce::Graphics&,
                                    const MeterType meterType,
                                    const juce::Rectangle<float> bounds,
                                    const float maxGain) = 0;

        virtual int hitTestClipIndicator (const juce::Point<int> position,
                                          const MeterType meterType,
                                          const juce::Rectangle<float> bounds,
                                          const LevelMeterSource* source) const = 0;

        virtual int hitTestMaxNumber (const juce::Point<int> position,
                                      const MeterType meterType,
                                      const juce::Rectangle<float> bounds,
                                      const LevelMeterSource* source) const = 0;
    };

    class LookAndFeelDefaultMethods;

    LevelMeter (const MeterType type = VerticalBars);
    ~LevelMeter ();

    void paint (juce::Graphics&) override;

    void timerCallback () override;

    void setMeterSource (LevelMeterSource* source);

    void setSelectedChannel (const int c);

    void setRefreshRateHz (const int newRefreshRate);

    void clearClipIndicator (const int channel=-1);

    void clearMaxLevelDisplay (const int channel=-1);

    void mouseDown (const juce::MouseEvent& event) override;

    class Listener {
    public:
        virtual ~Listener () {}
        virtual void clipLightClicked (const int channel) = 0;
        virtual void maxLevelClicked (const int channel)  = 0;
    };

    void addListener (Listener*);

    void removeListener (Listener*);

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LevelMeter)
    
    juce::WeakReference<LevelMeterSource> source;

    int                                   selectedChannel;

    MeterType                             meterType;

    int                                   refreshRate;

    juce::ListenerList<LevelMeter::Listener> listeners;
};


#endif  // LEVELMETER_H_INCLUDED
