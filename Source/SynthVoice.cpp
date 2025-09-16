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
    isNoteOn = true;
    const double frequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    for (auto* reader : mapOscillator.getReaders())
    {
        reader->setFrequency(frequency);
    }
}

void SynthVoice::stopNote (float velocity, bool allowTailOff)
{
    isNoteOn = false;
    clearCurrentNote();
}

void SynthVoice::setCurrentPlaybackSampleRate (double newRate)
{
    mapOscillator.prepareToPlay (getSampleRate());
    mapOscillator.setLFOs (&processor.lfo, &processor.lfo2);
}

void SynthVoice::renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    if (! isNoteOn)
        return;

    for (auto* readerBase : mapOscillator.getReaders())
    {
        if (auto* lineReader = dynamic_cast<LineReader*> (readerBase))
        {
            lineReader->setCentre (processor.apvts.getRawParameterValue ("LineCX")->load(),
                                   processor.apvts.getRawParameterValue ("LineCY")->load());
            lineReader->setLength (processor.apvts.getRawParameterValue ("LineLength")->load());
            lineReader->setAngle (processor.apvts.getRawParameterValue ("LineAngle")->load());
            lineReader->setVolume (processor.apvts.getRawParameterValue ("LineVolume")->load());

            lineReader->lfoAngleAmount = processor.apvts.getRawParameterValue ("LFO_LineAngle_Amount")->load();
            lineReader->lfoLengthAmount = processor.apvts.getRawParameterValue ("LFO_LineLength_Amount")->load();

            lineReader->lfoAngleSelect = processor.apvts.getRawParameterValue ("LFO_LineAngle_Select")->load();
            lineReader->lfoLengthSelect = processor.apvts.getRawParameterValue ("LFO_LineLength_Select")->load();
        }
        else if (auto* circleReader = dynamic_cast<CircleReader*> (readerBase))
        {
            circleReader->setCentre (processor.apvts.getRawParameterValue ("CX")->load(),
                                     processor.apvts.getRawParameterValue ("CY")->load());
            circleReader->setRadius (processor.apvts.getRawParameterValue ("R")->load());
            circleReader->setVolume (processor.apvts.getRawParameterValue ("CircleVolume")->load());

            circleReader->lfoCxAmount = processor.apvts.getRawParameterValue ("LFO_CX_Amount")->load();
            circleReader->lfoRadiusAmount = processor.apvts.getRawParameterValue ("LFO_R_Amount")->load();

            circleReader->lfoCxSelect = processor.apvts.getRawParameterValue ("LFO_CX_Select")->load();
            circleReader->lfoRadiusSelect = processor.apvts.getRawParameterValue ("LFO_R_Select")->load();
        }
    }

    juce::MidiBuffer emptyMidi;
    mapOscillator.processBlock (outputBuffer, emptyMidi, startSample, numSamples, processor.imageBuffer);
}