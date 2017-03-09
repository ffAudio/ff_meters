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

 BEGIN_JUCE_MODULE_DECLARATION

 ID:            ff_meters
 vendor:        Foleys Finest Audio UG / Filmstro Ltd.
 version:       0.9.0
 name:          Meters with GUI and LookAndFeel
 description:   Audio helper classes of general purpose
 dependencies:  juce_audio_basics, juce_audio_formats, juce_audio_devices, juce_audio_processors
 website:       http://www.foleysfinest.com/
 license:       BSD V2 3-clause

 END_JUCE_MODULE_DECLARATION

 ==============================================================================
 */


#ifndef FF_AUDIO_METERS_INCLUDED_H
#define FF_AUDIO_METERS_INCLUDED_H

#ifndef USE_FF_AUDIO_METERS
#define USE_FF_AUDIO_METERS 1
#endif

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_devices/juce_audio_devices.h>

#include <ff_meters/ff_meters_LevelMeterSource.h>
#include <ff_meters/ff_meters_LookAndFeel.h>
#include <ff_meters/ff_meters_LookAndFeelVertical.h>
#include <ff_meters/ff_meters_LookAndFeelHorizontal.h>
#include <ff_meters/ff_meters_LookAndFeelVintage.h>
#include <ff_meters/ff_meters_LevelMeter.h>

#endif /* FF_AUDIO_METERS_INCLUDED_H */

