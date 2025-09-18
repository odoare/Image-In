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
    mapOscillator.addLineReader();
    mapOscillator.addCircleReader();
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

void SynthVoice::renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    if (! isVoiceActive())
        return;

    juce::ADSR::Parameters adsrParams;
    adsrParams.attack  = processor.apvts.getRawParameterValue ("Attack")->load();
    adsrParams.decay   = processor.apvts.getRawParameterValue ("Decay")->load();
    adsrParams.sustain = processor.apvts.getRawParameterValue ("Sustain")->load();
    adsrParams.release = processor.apvts.getRawParameterValue ("Release")->load();
    adsr.setParameters (adsrParams);

    for (auto* readerBase : mapOscillator.getReaders())
        readerBase->updateParameters (processor.apvts);

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