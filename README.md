
ff_meters
=========

by Daniel Walz / Foleys Finest Audio Ltd. (haftungsbeschraenkt)
Published under the BSD License (3 clause)

The ff_meters provide an easy to use Component to display a level reading for an
AudioBuffer. It is to be used in the audio framework JUCE (www.juce.com).

Find the API documentation here: https://foleysfinest.com/ff_meters/


Usage
=====

LevelMeter
----------

To use it create a LevelMeterSource instance next to the AudioBuffer you want to
display. To update the meter, call LevelMeterSource::measureBlock (buffer) in your
processBlock or getNextAudioBuffer method.

On the Component use LevelMeter::setMeterSource to link to the LevelMeterSource 
instance. The number of channels will be updated automatically.

You can pull the drawing into your LookAndFeel by inheriting LevelMeter::LookAndFeelMethods
and inlining the default implementation from LevelMeterLookAndFeel in 
ff_meters_LookAndFeelMethods.h into a public section of your class declaration. To
setup the default colour scheme, call setupDefaultMeterColours() in your constructor.

Or you can use the LevelMeterLookAndFeel directly because it inherits from juce::LookAndFeel_V3 
for your convenience. You can set it as default LookAndFeel, if you used the default, 
or set it only to the meters, if you don't want it to interfere.

All classes are in the namespace FFAU to avoid collisions. You can either prefix each symbol, 
or import the namespace.

    // In your Editor
    public:
        PluginEditor()
        {
            meter = std::make_unique<foleys::LevelMeter>(std::make_unique<foleys::VerticalMultiChannelMeter>());
            meter->setMeterSource (&processor.getMeterSource());
            addAndMakeVisible (meter);
            // ...
        }
    private:
        std::unique_ptr<FFAU::LevelMeter> meter;

    // and in the processor:
    public:
        foleys::LevelMeterSource& getMeterSource()
        {
            return meterSource;
        }

        void processBlock (AudioSampleBuffer& buffer, MidiBuffer&) override
        {
            meterSource.measureBlock (buffer);
            // ...
        }

    private:
        foleys::LevelMeterSource meterSource;


OutlineBuffer
-------------

Another class is capable of reducing the samples going through into min and max blocks. This
way you can see the outline of a signal running through. It can be used very similar:

    // in your processor
    private:
    foleys::OutlineBuffer outline;

    // in prepareToPlay
    outline.setSize (getTotalNumInputChannels(), 1024);

    // in processBlock
    outline.pushBlock (buffer, buffer.getNumSamples());

    // and in the editor's component:
    const Rectangle<float> plotFrame (10.0, 320.0, 580, 80);
    g.setColour (Colours::lightgreen);
    g.fillRect (plotFrame);

    Path plot;
    processor.getChannelOutline (plot, plotFrame, 1000);
    g.setColour (Colours::grey);
    g.fillPath (plot);
    g.setColour (Colours::black);
    g.strokePath (plot, PathStrokeType (1.0f));


********************************************************************************

We hope it is of any use, let us know of any problems or improvements you may 
come up with...

Brighton, 2nd March 2017

********************************************************************************
