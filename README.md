
ff_meters
=========

by Daniel Walz / Foleys Finest Audio UG (haftungsbeschraenkt)
Published under the BSD License (3 clause)

The ff_meters provide an easy to use Component to display a level reading for an
AudioBuffer. It is to be used in the audio framework JUCE (www.juce.com).

Usage
=====

To use it create a LevelMeterSource instance next to the AudioBuffer you want to
display. To update the meter, call LevelMeterSource::measureBlock (buffer) in your
processBlock or getNextAudioBuffer method.

On the Component use LevelMeter::setMeterSource to link to the LevelMeterSource 
instance. The number of channels will be updated automatically.

The orientation is set by choosing the appropriate LookAndFeel class.
There are also a lot of colours you can change in the LevelMeterLookAndFeel.

    // In your Editor's constructor:
    LevelMeterLookAndFeel* lnf = new LevelMeterLookAndFeelVertical();
    // adjust the colours to how you like them
    lnf->setMeterColour (lmMeterGradientLowColour, juce::Colours::green);
    // the meter takes ownership of the LookAndFeel
    meter = new LevelMeter (lnf);
    meter->setMeterSource (processor.getMeterSource());
    addAndMakeVisible (inputMeter);


    // and in the processor:
    public:
        LevelMeterSource* getMeterSource ()
        {
            return &meterSource;
        }

        void processBlock (AudioSampleBuffer& buffer, MidiBuffer&) override
        {
            inputSource.measureBlock (buffer);
            // ...
        }

    private:
        LevelMeterSource meterSource;



********************************************************************************

We hope it is of any use, let us know of any problems or improvements you may 
come up with...

Brighton, 2nd March 2017

********************************************************************************
