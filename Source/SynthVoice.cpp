/*
  ==============================================================================

    SynthVoice.cpp
    Created: 16 Sep 2025 10:00:00am
    Author:  Olivier Doaré

  ==============================================================================
*/

#include "SynthVoice.h"
#include "PluginProcessor.h"

SynthVoice::SynthVoice(MapSynthAudioProcessor& p, int index) : processor(p), voiceIndex(index)
{
}

bool SynthVoice::canPlaySound (juce::SynthesiserSound* sound)
{
    return dynamic_cast<SynthSound*> (sound) != nullptr;
}

void SynthVoice::startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition)
{
    const double frequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    for (auto* reader : mapOscillator.getReaders())
    {
        reader->setFrequency(frequency);
    }
    noteVel = velocity;

    adsr.noteOn();
    adsr2.noteOn();
}

void SynthVoice::stopNote (float velocity, bool allowTailOff)
{
    adsr.noteOff();
    adsr2.noteOff();

    if (! allowTailOff || ! isVoiceActive())
        clearCurrentNote();
}

bool SynthVoice::isVoiceActive() const
{
    return adsr.isActive() || adsr2.isActive();
}

void SynthVoice::setCurrentPlaybackSampleRate (double newRate)
{
    mapOscillator.prepareToPlay (getSampleRate());
    adsr.prepareToPlay (getSampleRate());
    adsr2.prepareToPlay (getSampleRate());
}

void SynthVoice::rebuildReaders (const juce::Array<ReaderBase::Type>& types)
{
    mapOscillator.rebuildReaders (types);
}

void SynthVoice::renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    if (! isVoiceActive())
    {
        // Ensure the GUI knows this voice is off
        juce::ScopedLock lock (processor.displayStateLock);
        if (processor.voiceDisplayStates[voiceIndex].isActive)
            processor.voiceDisplayStates[voiceIndex].isActive = false;
        return;
    }
    
    // Update parameters from the processor's pre-filled struct
    adsr.setParameters (processor.globalParams.adsr);
    adsr2.setParameters (processor.globalParams.adsr2);
    mapOscillator.updateParameters (processor.globalParams);

    // Prepare modulator buffer
    modulatorBuffer.setSize (4, numSamples, false, false, true);
    modulatorBuffer.copyFrom (0, 0, processor.lfoBuffer, 0, 0, numSamples); // LFO 1
    modulatorBuffer.copyFrom (1, 0, processor.lfoBuffer, 1, 0, numSamples); // LFO 2

    // Generate ADSR modulator data
    auto* adsr1Writer = modulatorBuffer.getWritePointer (2);
    auto* adsr2Writer = modulatorBuffer.getWritePointer (3);
    for (int i = 0; i < numSamples; ++i)
    {
        adsr1Writer[i] = adsr.process();
        adsr2Writer[i] = adsr2.process();
    }

    // Render audio
    tempRenderBuffer.setSize (outputBuffer.getNumChannels(), numSamples, false, false, true);
    tempRenderBuffer.clear();

    juce::MidiBuffer emptyMidi;
    mapOscillator.processBlock (tempRenderBuffer, emptyMidi, 0, numSamples, processor.imageBuffer, modulatorBuffer);

    tempRenderBuffer.applyGain (0, numSamples, noteVel);

    for (int ch = 0; ch < outputBuffer.getNumChannels(); ++ch)
        outputBuffer.addFrom (ch, startSample, tempRenderBuffer, ch, 0, numSamples);

    // Report state to GUI
    {
        juce::ScopedLock lock (processor.displayStateLock);
        auto& displayState = processor.voiceDisplayStates[voiceIndex];
        displayState.isActive = isVoiceActive();

        const auto& readers = mapOscillator.getReaders();
        displayState.readerInfos.resize (readers.size());
        for (int i = 0; i < readers.size(); ++i)
            displayState.readerInfos.set (i, readers.getUnchecked(i)->lastDrawingInfo);
    }

    if (! isVoiceActive())
    {
        clearCurrentNote();
        // Final update to ensure GUI shows inactive state
        juce::ScopedLock lock (processor.displayStateLock);
        processor.voiceDisplayStates[voiceIndex].isActive = false;
    }
}