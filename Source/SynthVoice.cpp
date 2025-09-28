/*
  ==============================================================================

    SynthVoice.cpp
    Created: 16 Sep 2025 10:00:00am
    Author:  Olivier Doaré

  ==============================================================================
*/

#include "SynthVoice.h"
#include "PluginProcessor.h"
#include "ParameterStructs.h"

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
    adsr3.noteOn();
}

void SynthVoice::stopNote (float velocity, bool allowTailOff)
{
    adsr.noteOff();
    adsr2.noteOff();
    adsr3.noteOff();

    if (! allowTailOff || ! isVoiceActive())
        clearCurrentNote();
}

bool SynthVoice::isVoiceActive() const
{
    return adsr.isActive() || adsr2.isActive() || adsr3.isActive();
}

void SynthVoice::setCurrentPlaybackSampleRate (double newRate)
{
    juce::SynthesiserVoice::setCurrentPlaybackSampleRate (newRate);
    mapOscillator.prepareToPlay (getSampleRate());
    adsr.prepareToPlay (getSampleRate());
    adsr2.prepareToPlay (getSampleRate());
    adsr3.prepareToPlay (getSampleRate());
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
    adsr3.setParameters (processor.globalParams.adsr3);
    mapOscillator.updateParameters (processor.globalParams);

    // Prepare modulator buffer
    modulatorBuffer.setSize (ModulatorSources::NumModulators, numSamples, false, false, true);
    modulatorBuffer.copyFrom (ModulatorSources::LFO1, 0, processor.lfoBuffer, 0, 0, numSamples); // LFO 1
    modulatorBuffer.copyFrom (ModulatorSources::LFO2, 0, processor.lfoBuffer, 1, 0, numSamples); // LFO 2
    modulatorBuffer.copyFrom (ModulatorSources::LFO3, 0, processor.lfoBuffer, 2, 0, numSamples); // LFO 3
    modulatorBuffer.copyFrom (ModulatorSources::LFO4, 0, processor.lfoBuffer, 3, 0, numSamples); // LFO 4

    // Generate ADSR modulator data
    auto* adsr1Writer = modulatorBuffer.getWritePointer (ModulatorSources::ADSR1);
    auto* adsr2Writer = modulatorBuffer.getWritePointer (ModulatorSources::ADSR2);
    auto* adsr3Writer = modulatorBuffer.getWritePointer (ModulatorSources::ADSR3);
    for (int i = 0; i < numSamples; ++i)
    {
        adsr1Writer[i] = adsr.process();
        adsr2Writer[i] = adsr2.process();
        adsr3Writer[i] = adsr3.process();
    }

    // Generate combined LFO*ADSR modulator data
    auto* lfo1Reader = modulatorBuffer.getReadPointer(ModulatorSources::LFO1);
    auto* lfo2Reader = modulatorBuffer.getReadPointer(ModulatorSources::LFO2);
    auto* lfo3Reader = modulatorBuffer.getReadPointer(ModulatorSources::LFO3);
    auto* lfo4Reader = modulatorBuffer.getReadPointer(ModulatorSources::LFO4);
    auto* adsr1Reader = modulatorBuffer.getReadPointer(ModulatorSources::ADSR1);
    auto* adsr2Reader = modulatorBuffer.getReadPointer(ModulatorSources::ADSR2);
    auto* adsr3Reader = modulatorBuffer.getReadPointer(ModulatorSources::ADSR3);

    auto* lfo1adsr1Writer = modulatorBuffer.getWritePointer(ModulatorSources::LFO1_ADSR1);
    auto* lfo1adsr2Writer = modulatorBuffer.getWritePointer(ModulatorSources::LFO1_ADSR2);
    auto* lfo1adsr3Writer = modulatorBuffer.getWritePointer(ModulatorSources::LFO1_ADSR3);
    auto* lfo2adsr1Writer = modulatorBuffer.getWritePointer(ModulatorSources::LFO2_ADSR1);
    auto* lfo2adsr2Writer = modulatorBuffer.getWritePointer(ModulatorSources::LFO2_ADSR2);
    auto* lfo2adsr3Writer = modulatorBuffer.getWritePointer(ModulatorSources::LFO2_ADSR3);
    auto* lfo3adsr1Writer = modulatorBuffer.getWritePointer(ModulatorSources::LFO3_ADSR1);
    auto* lfo3adsr2Writer = modulatorBuffer.getWritePointer(ModulatorSources::LFO3_ADSR2);
    auto* lfo3adsr3Writer = modulatorBuffer.getWritePointer(ModulatorSources::LFO3_ADSR3);
    auto* lfo4adsr1Writer = modulatorBuffer.getWritePointer(ModulatorSources::LFO4_ADSR1);
    auto* lfo4adsr2Writer = modulatorBuffer.getWritePointer(ModulatorSources::LFO4_ADSR2);
    auto* lfo4adsr3Writer = modulatorBuffer.getWritePointer(ModulatorSources::LFO4_ADSR3);

    for (int i = 0; i < numSamples; ++i)
    {
        lfo1adsr1Writer[i] = lfo1Reader[i] * adsr1Reader[i];
        lfo1adsr2Writer[i] = lfo1Reader[i] * adsr2Reader[i];
        lfo1adsr3Writer[i] = lfo1Reader[i] * adsr3Reader[i];
        lfo2adsr1Writer[i] = lfo2Reader[i] * adsr1Reader[i];
        lfo2adsr2Writer[i] = lfo2Reader[i] * adsr2Reader[i];
        lfo2adsr3Writer[i] = lfo2Reader[i] * adsr3Reader[i];
        lfo3adsr1Writer[i] = lfo3Reader[i] * adsr1Reader[i];
        lfo3adsr2Writer[i] = lfo3Reader[i] * adsr2Reader[i];
        lfo3adsr3Writer[i] = lfo3Reader[i] * adsr3Reader[i];
        lfo4adsr1Writer[i] = lfo4Reader[i] * adsr1Reader[i];
        lfo4adsr2Writer[i] = lfo4Reader[i] * adsr2Reader[i];
        lfo4adsr3Writer[i] = lfo4Reader[i] * adsr3Reader[i];
    }

    // Render audio
    tempRenderBuffer.setSize (outputBuffer.getNumChannels(), numSamples, false, false, true);
    tempRenderBuffer.clear();

    juce::MidiBuffer emptyMidi;
    mapOscillator.processBlock (tempRenderBuffer, emptyMidi, 0, numSamples, processor.bitmapDataManager, modulatorBuffer);

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