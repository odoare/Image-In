/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "MapOscillator.h"
#include "LFO.h"
#include "SynthSound.h"
#include "ParameterStructs.h"
#include "FactoryPresets.h"
#include "SynthVoice.h"
#include "BitmapDataManager.h"

// Number of voices for the synth
#define NUM_VOICES 4
#define NUM_METER_CHANNELS 2

struct VoiceDisplayState
{
    bool isActive = false;
    juce::Array<ReaderBase::DrawingInfo> readerInfos;
};


//==============================================================================
/**
*/
class MapSynthAudioProcessor  : public juce::AudioProcessor,
                                public juce::AudioProcessorValueTreeState::Listener
{
public:
    //==============================================================================
    MapSynthAudioProcessor();
    ~MapSynthAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    void parameterChanged (const juce::String& parameterID, float newValue) override;

    ImageBuffer imageBuffer;
    GlobalParameters globalParams; // This now contains all parameter structs
    BitmapDataManager bitmapDataManager { imageBuffer };
    juce::AudioProcessorValueTreeState apvts {*this, nullptr, "Parameters", createParameters()};
    LFO lfo;
    LFO lfo2;
    LFO lfo3;
    LFO lfo4;
    juce::AudioBuffer<float> lfoBuffer;

    std::array<VoiceDisplayState, NUM_VOICES> voiceDisplayStates;
    juce::CriticalSection displayStateLock;

    // OpenGL state management
    void setUseOpenGL (bool shouldUseOpenGL);
    bool getUseOpenGL() const;
    juce::ChangeBroadcaster openGLStateBroadcaster;

    float getSmoothedMaxLevel(const int channel);
    float getMaxLevel(const int channel);

private:
    // Preset Management
    int currentProgram = 0;
    const juce::Array<FactoryPresets::Preset> factoryPresets;
    bool isLoadingPreset = false;
    bool useOpenGL = false;

    double processSampleRate = 44100.0;

    juce::Synthesiser synth;
    juce::LinearSmoothedValue<float> masterLevelSmoother;
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameters();  
    
    void highPassFilter(juce::AudioBuffer<float>& buffer, float cutoffFreq);
    void updateParameters();
    void updateVoices();

    // State for the high-pass filter
    juce::Array<float> hpf_prevInput;
    juce::Array<float> hpf_prevOutput;

    juce::LinearSmoothedValue<float> smoothedMaxLevel[NUM_METER_CHANNELS]; 
    float maxLevel[NUM_METER_CHANNELS];
    float maxDecay{2.f};

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MapSynthAudioProcessor)
};
