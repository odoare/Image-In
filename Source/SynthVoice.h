/*
  ==============================================================================

    SynthVoice.h
    Created: 16 Sep 2025 10:00:00am
    Author:  Olivier Doaré

  ==============================================================================
*/

#pragma once

#include "SynthSound.h"
#include "MapOscillator.h"
#include "ADSR.h"

class MapSynthAudioProcessor;
class SynthVoice : public juce::SynthesiserVoice
{
public:
    SynthVoice (MapSynthAudioProcessor& p, int voiceIndex);
    
    bool canPlaySound (juce::SynthesiserSound* sound) override;
    
    void startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition) override;
    
    void stopNote (float velocity, bool allowTailOff) override;
    
    bool isVoiceActive() const override;

    void pitchWheelMoved (int newPitchWheelValue) override {}
    
    void controllerMoved (int controllerNumber, int newControllerValue) override {}
    
    void setCurrentPlaybackSampleRate (double newRate) override;

    void renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

    void rebuildReaders (const juce::Array<ReaderBase::Type>& types);

private:
    MapSynthAudioProcessor& processor;
    MapOscillator mapOscillator;
    ADSR adsr; // Main ADSR for volume
    ADSR adsr2; // Modulation ADSR
    ADSR adsr3; // Modulation ADSR
    juce::AudioBuffer<float> tempRenderBuffer;
    int voiceIndex;
    float noteVel{0.f};

    juce::AudioBuffer<float> modulatorBuffer;
};