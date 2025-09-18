/*
  ==============================================================================

    SynthVoice.cpp
    Created: 16 Sep 2025 10:00:00am
    Author:  Olivier Doaré

  ==============================================================================
*/

#include "SynthVoice.h"
#include "PluginProcessor.h"

SynthVoice::SynthVoice(MapSynthAudioProcessor& p) : processor(p)
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
}

void SynthVoice::stopNote (float velocity, bool allowTailOff)
{
    adsr.noteOff();

    if (! allowTailOff || ! adsr.isActive())
        clearCurrentNote();
}

bool SynthVoice::isVoiceActive() const
{
    return adsr.isActive();
}

void SynthVoice::setCurrentPlaybackSampleRate (double newRate)
{
    mapOscillator.prepareToPlay (getSampleRate());
    adsr.prepareToPlay (getSampleRate());
}

void SynthVoice::rebuildReaders (const juce::Array<ReaderBase::Type>& types)
{
    mapOscillator.rebuildReaders (types);
}

void SynthVoice::renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    if (! isVoiceActive())
        return;
    
    // Update ADSR and reader parameters from the processor's pre-filled struct
    adsr.setParameters (processor.globalParams.adsr);
    mapOscillator.updateParameters (processor.globalParams);

    tempRenderBuffer.setSize (outputBuffer.getNumChannels(), numSamples, false, false, true);
    tempRenderBuffer.clear();

    juce::MidiBuffer emptyMidi;
    mapOscillator.processBlock (tempRenderBuffer, emptyMidi, 0, numSamples, processor.imageBuffer, processor.lfoBuffer);

    adsr.applyEnvelopeToBuffer (tempRenderBuffer, 0, numSamples);
    tempRenderBuffer.applyGain (0, numSamples, noteVel);

    for (int ch = 0; ch < outputBuffer.getNumChannels(); ++ch)
        outputBuffer.addFrom (ch, startSample, tempRenderBuffer, ch, 0, numSamples);

    if (! adsr.isActive())
    {
        clearCurrentNote();
    }
}