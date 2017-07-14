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
 
 ff_meters_LevelMeterPlot.h
 Created: 13 Jul 2017 21:30:29
 Author:  Daniel Walz
 
 ==============================================================================
 */

#pragma once

#include <vector>

/** @addtogroup ff_meters */
/*@{*/

//==============================================================================
/*
 \class LevelMeterPlot
 \brief A component to display live gain and RMS as a plot graph
 
 This class is used to display a level reading. It supports max and RMS levels.
 You can also set a reduction value to display, the definition of that value is up to you.
 */
class LevelMeterPlot : public juce::Component,
                       public juce::Timer
{
public:
    LevelMeterPlot();
    
    virtual ~LevelMeterPlot();
    
    void paint (juce::Graphics& g) override;
    
    void resized() override;
    
    void timerCallback() override;
    
    /**
     Set a LevelMeterSource to display. This separation is used, so the source can work in the processing and the
     GUI can display the values.
     */
    void setMeterSource (FFAU::LevelMeterSource* source);
    

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LevelMeterPlot)
    
    juce::WeakReference<FFAU::LevelMeterSource> source;
    
    std::vector<std::vector<float> > levels;
    std::vector<float>               reduction;
    int                              writePointer;

    std::vector<juce::Colour> channelColours;
    
};

/*@}*/
