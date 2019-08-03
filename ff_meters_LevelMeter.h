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

    ff_meters_LevelMeter.h
    Created: 5 Apr 2016 9:49:54am
    Author:  Daniel Walz

 ==============================================================================
*/

#pragma once

namespace foleys
{

/** @addtogroup ff_meters */
/*@{*/

//==============================================================================
/*
 \class LevelMeter
 \brief A component to display live gain and RMS readings

 This class is used to display a level reading. It supports max and RMS levels.
 You can also set a reduction value to display, the definition of that value is up to you.
*/
class LevelMeter    : public juce::Component, private juce::Timer
{
public:

    enum MeterFlags
    {
        Default         = 0x0000, /**< Default is showing all channels in the LevelMeterSource without a border */
        Horizontal      = 0x0001, /**< Displays the level bars horizontally */
        Vintage         = 0x0002, /**< Switches to a special mode of old school meters (to come) */
        SingleChannel   = 0x0004, /**< Display only one channel meter. \see setSelectedChannel */
        HasBorder       = 0x0008, /**< Displays a rounded border around the meter. This is used with the default constructor */
        Reduction       = 0x0010, /**< This turns the bar into a reduction bar.
                                   The additional reduction bar is automatically added, as soon a reduction value < 1.0 is set
                                   in the LevelMeterSource. \see LevelMeterSource::setReductionLevel */
        Minimal         = 0x0020, /**< For a stereo meter, this tries to save space by showing only one line tickmarks in the middle and no max numbers */
        MaxNumber       = 0x0040  /**< To add level meter to Minimal, set this flag */
    };

    enum ColourIds
    {
        BackgroundColour = 0x2200001, /**< Background colour */
        TextColour,                   /**< Colour for the numbers etc. */
        TextClipColour,               /**< Colour to print the max number if it has clipped */
        TicksColour,                  /**< Colour for the tick marks */
        OutlineColour,                /**< Colour for the frame around all */
        ClipOffColour,
        ClipClippedColour,
        MeterBackgroundColour,
        MeterGradientLowColour,       /**< Colour for the meter bar under the warn threshold */
        MeterGradientMidColour,       /**< Colour for the meter bar in the warn area */
        MeterGradientMaxColour,       /**< Colour for the meter bar at the clip threshold */
        MeterMaxNormalColour,         /**< Text colour for the max number, if under warn threshold */
        MeterMaxWarnColour,           /**< Text colour for the max number, if between warn threshold and clip threshold */
        MeterMaxOverColour,           /**< Text colour for the max number, if above the clip threshold */

//        BackgroundClipColour,     /**< This is the colour of the clip indicator if it has clipped */
//        MeterForegroundColour,    /**< Unused, will eventually be removed */
//        MeterOutlineColour,       /**< Colour for the outlines of meter bars etc. */
//        MeterBackgroundColour,    /**< Background colour for the actual meter bar and the max number */
        MeterReductionColour      /**< Colour for the reduction meter displayed within the meter */
    };

    /**
     These methods define a interface for the LookAndFeel class of juce.
     The LevelMeter needs a LookAndFeel, that implements these methods.
     There is a default implementation to be included in your custom LookAndFeel class, \see LookAndFeelMethods.h
     */
    class LookAndFeelMethods
    {
    public:
        LookAndFeelMethods()
        {
            setupDefaultMeterColours();
        }

        virtual ~LookAndFeelMethods() = default;

        virtual void drawMeter (juce::Graphics& g,
                                const LevelMeter& meter,
                                juce::Rectangle<float> bounds) = 0;

        /** This is called by the frontend to check, if the clip indicator was clicked (e.g. for reset) */
        virtual int hitTestClipIndicator (const LevelMeter& meter,
                                          const juce::Point<float> position,
                                          const juce::Rectangle<float> bounds) const = 0;

        /** This is called by the frontend to check, if the maximum level number was clicked (e.g. for reset) */
        virtual int hitTestMaxNumber (const LevelMeter& meter,
                                      const juce::Point<float> position,
                                      const juce::Rectangle<float> bounds) const = 0;

        void setMeterColour (LevelMeter::ColourIds id, juce::Colour c);

        virtual void setupDefaultMeterColours();

    protected:
        std::map<ColourIds, juce::Colour> colourMap;

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LookAndFeelMethods)
    };

    /**
     To create a LevelMeter supply a LookAndFeel. This will define the behaviour,
     e.g. horizontal, vertical etc. A few defaults are provided, you can create your own
     by inheriting from FFAU::LevelMeter::LookAndFeelMethods.
     */
    LevelMeter (std::unique_ptr<LevelMeter::LookAndFeelMethods> lookAndFeel);
    ~LevelMeter ();

    void paint (juce::Graphics&) override;

    void timerCallback () override;

    /**
     Set a LevelMeterSource to display. This separation is used, so the source can work in the processing and the
     GUI can display the values.
     */
    void setMeterSource (LevelMeterSource* source);

    /**
     Set a specific channel to display. This is only useful, if MeterFlags::SingleChannel is set.
     */
    void setSelectedChannel (const int c);

    /**
     If you don't know, how many channels will be in the processblock, you can set this number to avoid stretching
     the width of the channels.
     */
    void setFixedNumChannels (const int numChannels);

    /** This returns the offset of the first channel to display */
    int getFirstChannel() const;

    /** Returns the number of channels to display. This can be different from the number in LevelMeterSource */
    int getNumChannels() const;

    void setMeterColour (LevelMeter::ColourIds id, juce::Colour c);

    void setRefreshRateHz (const int newRefreshRate);

    /**
     Unset the clip indicator flag for a channel. Use -1 to reset all clip indicators.
     */
    void clearClipIndicator (const int channel=-1);

    /**
     Set the max level display back to -inf for a channel. Use -1 to reset all max levels.
     */
    void clearMaxLevelDisplay (const int channel=-1);

    /**
     This lambda is called when the user clicks on a clip light. It is initially set to clear all clip lights
     and max level numbers.
     */
    std::function<void(LevelMeter& meter, int channel, juce::ModifierKeys mods)> onClipLightClicked;

    /**
     This lambda is called when the user clicks on a max level display. It is initially set to clear all clip lights
     and max level numbers.
     */
    std::function<void(LevelMeter& meter, int channel, juce::ModifierKeys mods)> onMaxLevelClicked;

    /**
     \internal
    */
    void mouseDown (const juce::MouseEvent& event) override;

    /**
     DEPRECATED: Instead of using the Listener, use the new lambdas:
     \see onMaxLevelClicked, onClipLightClicked

     This Listener interface is meant to implement behaviour if either the clip indicator or the max level text
     is clicked.

     An example implementation could look like this (+alt means clear all, else clear the clicked number):
     \code{.cpp}
     void clipLightClicked (LevelMeter* clickedMeter, const int channel, ModifierKeys mods) override
     {
         clickedMeter->clearClipIndicator (mods.isAltDown() ? -1 : channel);
     }

     void maxLevelClicked (LevelMeter* clickedMeter, const int channel, ModifierKeys mods) override
     {
         clickedMeter->clearMaxLevelDisplay (mods.isAltDown() ? -1 : channel);
     }
     \endcode
     */
    class Listener
    {
    public:
        virtual ~Listener() = default;
        /**
         This is called, when the user clicks a clip indicator. It can be used to reset the clip indicator.
         To allow different behaviour, e.g. resetting only one indicator or even all meters spread over the UI.
         \see clearClipIndicator, maxLevelClicked
         */
        virtual void clipLightClicked (LevelMeter* meter, const int channel, juce::ModifierKeys mods) = 0;
        /**
         This is called, when the user clicks a max level text. It can be used to reset the max number.
         \see clearMaxLevelDisplay, clipLightClicked
         */
        virtual void maxLevelClicked (LevelMeter* meter, const int channel, juce::ModifierKeys mods)  = 0;
    };

    void addListener (LevelMeter::Listener*);

    void removeListener (LevelMeter::Listener*);

    /**
     Returns the LevelMeterSource this meter component is connected to.
     */
    LevelMeterSource* getMeterSource();
    const LevelMeterSource* getMeterSource() const;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LevelMeter)

    juce::WeakReference<LevelMeterSource> source;

    /** This is just the part necessary for the LevelMeter, it doesn't have to be a complete juce::LookAndFeel. */
    std::unique_ptr<LevelMeter::LookAndFeelMethods> lookAndFeelMethods;

    /** If the juce::LookAndFeel contains the FFAU::LevelMeter::LookAndFeelMethods, this will point to the juce::LookAndFeel */
    LookAndFeelMethods& currentLookAndFeelMethods = *lookAndFeelMethods.get();

    int                                   selectedChannel  = -1;
    int                                   selectedNumChannels = -1;
    int                                   refreshRate = 30;

    juce::ListenerList<LevelMeter::Listener> listeners;
};

inline LevelMeter::MeterFlags operator|(LevelMeter::MeterFlags a, LevelMeter::MeterFlags b)
{return static_cast<LevelMeter::MeterFlags>(static_cast<int>(a) | static_cast<int>(b));}

/*@}*/

} // end namespace foleys
