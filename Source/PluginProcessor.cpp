/*
  ==============================================================================

    PluginProcessor.h
    Created: 16 Sep 2025 10:00:00am
    Author:  Olivier Doaré

    Part of Image-In project

    Licenced under the LGPLv3

  ==============================================================================
*/


#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "SynthSound.h"
#include "FactoryPresets.h"

namespace ParameterHelpers
{
    void addEllipseParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, int index, const juce::StringArray& modulatorChoices, const juce::StringArray& filterTypeChoices)
    {
        juce::String idPrefix = "Ellipse" + juce::String(index) + "_";
        juce::String namePrefix = "Ellipse " + juce::String(index) + " ";

        float defaultVolume = (index == 1) ? 1.0f : 0.0f;
        float defaultR1 = 0.4f - (index - 1) * 0.1f;
        float defaultR2 = 0.2f - (index - 1) * 0.05f;

        layout.add(std::make_unique<juce::AudioParameterFloat>(idPrefix + "CX", namePrefix + "CX", juce::NormalisableRange<float>(0.f, 1.f, .01f, 1.f), 0.5f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(idPrefix + "CY", namePrefix + "CY", juce::NormalisableRange<float>(0.f, 1.f, .01f, 1.f), 0.5f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(idPrefix + "R1", namePrefix + "R1", juce::NormalisableRange<float>(0.f, 0.5f, .01f, 1.f), defaultR1));
        layout.add(std::make_unique<juce::AudioParameterFloat>(idPrefix + "R2", namePrefix + "R2", juce::NormalisableRange<float>(0.f, 0.5f, .01f, 1.f), defaultR2));
        layout.add(std::make_unique<juce::AudioParameterFloat>(idPrefix + "Angle", namePrefix + "Angle", juce::NormalisableRange<float>(0.f, juce::MathConstants<float>::twoPi, .01f, 1.f), 0.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(idPrefix + "Volume", namePrefix + "Volume", juce::NormalisableRange<float>(0.f, 1.f, .01f, 1.f), defaultVolume));
        layout.add(std::make_unique<juce::AudioParameterFloat>(idPrefix + "Pan", namePrefix + "Pan", juce::NormalisableRange<float>(-1.0f, 1.0f, 0.01f), 0.0f));

        // Modulation
        layout.add(std::make_unique<juce::AudioParameterFloat>("Mod_" + idPrefix + "CX_Amount", "Mod->" + namePrefix + "CX", juce::NormalisableRange<float>(-1.f, 1.f, .01f), 0.0f));
        layout.add(std::make_unique<juce::AudioParameterChoice>("Mod_" + idPrefix + "CX_Select", "Mod Select", modulatorChoices, 0));
        layout.add(std::make_unique<juce::AudioParameterFloat>("Mod_" + idPrefix + "CY_Amount", "Mod->" + namePrefix + "CY", juce::NormalisableRange<float>(-1.f, 1.f, .01f), 0.0f));
        layout.add(std::make_unique<juce::AudioParameterChoice>("Mod_" + idPrefix + "CY_Select", "Mod Select", modulatorChoices, 0));
        layout.add(std::make_unique<juce::AudioParameterFloat>("Mod_" + idPrefix + "R1_Amount", "Mod->" + namePrefix + "R1", juce::NormalisableRange<float>(-1.f, 1.f, .01f), 0.0f));
        layout.add(std::make_unique<juce::AudioParameterChoice>("Mod_" + idPrefix + "R1_Select", "Mod Select", modulatorChoices, 0));
        layout.add(std::make_unique<juce::AudioParameterFloat>("Mod_" + idPrefix + "R2_Amount", "Mod->" + namePrefix + "R2", juce::NormalisableRange<float>(-1.f, 1.f, .01f), 0.0f));
        layout.add(std::make_unique<juce::AudioParameterChoice>("Mod_" + idPrefix + "R2_Select", "Mod Select", modulatorChoices, 0));
        layout.add(std::make_unique<juce::AudioParameterFloat>("Mod_" + idPrefix + "Angle_Amount", "Mod->" + namePrefix + "Angle", juce::NormalisableRange<float>(-1.f, 1.f, .01f), 0.0f));
        layout.add(std::make_unique<juce::AudioParameterChoice>("Mod_" + idPrefix + "Angle_Select", "Mod Select", modulatorChoices, 0));
        layout.add(std::make_unique<juce::AudioParameterFloat>("Mod_" + idPrefix + "Volume_Amount", "Mod->" + namePrefix + "Vol", juce::NormalisableRange<float>(-1.f, 1.f, .01f), 1.0f));
        layout.add(std::make_unique<juce::AudioParameterChoice>("Mod_" + idPrefix + "Volume_Select", "Mod Select", modulatorChoices, 4)); // Default to ADSR1
        layout.add(std::make_unique<juce::AudioParameterFloat>("Mod_" + idPrefix + "Pan_Amount", "Mod->" + namePrefix + "Pan", juce::NormalisableRange<float>(-1.f, 1.f, .01f), 0.0f));
        layout.add(std::make_unique<juce::AudioParameterChoice>("Mod_" + idPrefix + "Pan_Select", "Mod Select", modulatorChoices, 0));
        layout.add(std::make_unique<juce::AudioParameterFloat>("Mod_" + idPrefix + "Freq_Amount", "Mod->" + namePrefix + "Freq", juce::NormalisableRange<float>(-1.f, 1.f, .01f), 0.0f));
        layout.add(std::make_unique<juce::AudioParameterChoice>("Mod_" + idPrefix + "Freq_Select", "Mod Select", modulatorChoices, 0));

        // Filter
        layout.add(std::make_unique<juce::AudioParameterChoice>(idPrefix + "FilterType", namePrefix + "Filter Type", filterTypeChoices, 0));
        layout.add(std::make_unique<juce::AudioParameterFloat>(idPrefix + "FilterFreq", namePrefix + "Filter Freq", juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.3f), 20000.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(idPrefix + "FilterQuality", namePrefix + "Filter Q", juce::NormalisableRange<float>(0.1f, 18.0f, 0.01f), 1.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(idPrefix + "Detune", namePrefix + "Detune", juce::NormalisableRange<float>(-12.f, 12.f, 0.01f), 0.0f));
        layout.add(std::make_unique<juce::AudioParameterFloat>("Mod_" + idPrefix + "FilterFreq_Amount", "Mod->" + namePrefix + "FltFreq", juce::NormalisableRange<float>(-1.f, 1.f, .01f), 0.0f));
        layout.add(std::make_unique<juce::AudioParameterChoice>("Mod_" + idPrefix + "FilterFreq_Select", "Mod Select", modulatorChoices, 0));
        layout.add(std::make_unique<juce::AudioParameterFloat>("Mod_" + idPrefix + "FilterQuality_Amount", "Mod->" + namePrefix + "FltQ", juce::NormalisableRange<float>(-1.f, 1.f, .01f), 0.0f));
        layout.add(std::make_unique<juce::AudioParameterChoice>("Mod_" + idPrefix + "FilterQuality_Select", "Mod Select", modulatorChoices, 0));
    }
}

namespace ImageResourceHelper
{
    // A struct to hold information about our image resources
    struct ImageResource
    {
        juce::String friendlyName;
        const char* resourceName; // The variable name in BinaryData
    };

    // This function now returns a list of valid image resources
    static juce::Array<ImageResource> getFactoryImageResources()
    {
        juce::Array<ImageResource> resources;
        for (int i = 0; i < BinaryData::namedResourceListSize; ++i)
        {
            juce::String filename(BinaryData::originalFilenames[i]);
            if (filename.endsWithIgnoreCase(".png") || filename.endsWithIgnoreCase(".jpg") || filename.endsWithIgnoreCase(".jpeg"))
            {
                resources.add({
                    filename.fromFirstOccurrenceOf("_", false, false).upToLastOccurrenceOf(".", false, false),
                    BinaryData::namedResourceList[i]
                });
            }
        }
        return resources;
    }
}

//==============================================================================
MapSynthAudioProcessor::MapSynthAudioProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), factoryPresets(FactoryPresets::getAvailablePresets())
{
    synth.addSound (new SynthSound());
    for (int i = 0; i < NUM_VOICES; ++i)
    {
        synth.addVoice (new SynthVoice (*this, i));
    }

    // This must be called after voices are created.
    updateVoices();

    // Add this as a listener to all parameters to detect when the user
    // modifies the state, so we can mark the current program as "dirty".
    for (auto* param : getParameters())
    {
        if (auto* paramWithID = dynamic_cast<juce::AudioProcessorParameterWithID*> (param))
            apvts.addParameterListener (paramWithID->getParameterID(), this);
    }

    // Manually trigger initial state from the parameter's default value
    parameterChanged("FactoryImage", apvts.getRawParameterValue("FactoryImage")->load());

    // Load the first preset by default when the plugin is instantiated.
    setCurrentProgram (0);
}

MapSynthAudioProcessor::~MapSynthAudioProcessor()
{
    // Remove listeners
    for (auto* param : getParameters())
    {
        if (auto* paramWithID = dynamic_cast<juce::AudioProcessorParameterWithID*> (param))
            if (apvts.getParameter (paramWithID->getParameterID()) != nullptr)
                apvts.removeParameterListener (paramWithID->getParameterID(), this);
    }
}

//==============================================================================
const juce::String MapSynthAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MapSynthAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool MapSynthAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool MapSynthAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double MapSynthAudioProcessor::getTailLengthSeconds() const
{
    return 5.0;
}

int MapSynthAudioProcessor::getNumPrograms()
{
    // N factory presets + 1 slot for "user" or "dirty" states.
    return factoryPresets.size() + 1;
}

int MapSynthAudioProcessor::getCurrentProgram()
{
    return currentProgram;
}

void MapSynthAudioProcessor::setCurrentProgram (int index)
{
    const int userProgramIndex = factoryPresets.size();

    // If the index is for a user preset, we just update the index and do nothing else.
    // The state is assumed to be managed by the host via setStateInformation.
    if (index >= userProgramIndex)
    {
        currentProgram = userProgramIndex;
        return;
    }

    // If it's a factory preset, load it.
    if (! juce::isPositiveAndBelow (index, userProgramIndex))
        return;

    // Use a flag to prevent parameterChanged from thinking the user edited the preset.
    juce::ScopedValueSetter<bool> loading (isLoadingPreset, true);

    currentProgram = index;

    // Get the XML data for the preset.
    const char* presetData = factoryPresets.getUnchecked(index).data;

    // Create an XML element from the data.
    std::unique_ptr<juce::XmlElement> xmlState = juce::XmlDocument::parse (presetData);

    // Load the state from the XML.
    if (xmlState != nullptr)
    {
        if (xmlState->hasTagName (apvts.state.getType())) {
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
        }
    }
}

const juce::String MapSynthAudioProcessor::getProgramName (int index)
{
    if (juce::isPositiveAndBelow (index, factoryPresets.size()))
        return factoryPresets.getUnchecked(index).name;

    if (index == factoryPresets.size())
        return "User Preset";

    return {};
}

void MapSynthAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    // When the user saves a preset in the host, this might be called.
    // We can take this as a hint that the current state is now a "user" preset.
    // This isn't guaranteed to be called by all hosts, but it's a good fallback.
    currentProgram = factoryPresets.size();
}

//==============================================================================
void MapSynthAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    synth.setCurrentPlaybackSampleRate (sampleRate);

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = (juce::uint32) samplesPerBlock;
    spec.numChannels = (juce::uint32) getTotalNumOutputChannels();

    masterLevelSmoother.reset(sampleRate, 0.05);

    lfoBuffer.setSize (4, samplesPerBlock);

    lfo.prepareToPlay (sampleRate);
    lfo2.prepareToPlay (sampleRate);
    lfo3.prepareToPlay (sampleRate);
    lfo4.prepareToPlay (sampleRate);

    // Initialise high-pass filter states
    hpf_prevInput.clear();
    hpf_prevOutput.clear();
    hpf_prevInput.insertMultiple(0, 0.0f, getTotalNumOutputChannels());
    hpf_prevOutput.insertMultiple(0, 0.0f, getTotalNumOutputChannels());

    processSampleRate = sampleRate;
}

void MapSynthAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MapSynthAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void MapSynthAudioProcessor::setUseOpenGL (bool shouldUseOpenGL)
{
    if (useOpenGL != shouldUseOpenGL)
    {
        useOpenGL = shouldUseOpenGL;
        openGLStateBroadcaster.sendChangeMessage();
    }
}

bool MapSynthAudioProcessor::getUseOpenGL() const { return useOpenGL; }

float MapSynthAudioProcessor::getMaxLevel(const int channel)
{
  return maxLevel[channel];
}

float MapSynthAudioProcessor::getSmoothedMaxLevel(const int channel)
{
  return smoothedMaxLevel[channel].getCurrentValue();
}


void MapSynthAudioProcessor::updateVoices()
{
    juce::Array<ReaderBase::Type> types;
    for (int i = 0; i < 3; ++i)
        types.add (ReaderBase::Type::Ellipse);
    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<SynthVoice*> (synth.getVoice (i)))
        {
            voice->rebuildReaders (types);
        }
    }
}

void MapSynthAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
    if (parameterID == "FactoryImage")
    {
        const int choiceIndex = (int)newValue;
        if (choiceIndex > 0) // 0 is "Custom"
        {
            const auto imageResources = ImageResourceHelper::getFactoryImageResources();
            const int resourceIndex = choiceIndex - 1; // Adjust for "Custom" item

            if (juce::isPositiveAndBelow(resourceIndex, imageResources.size()))
            {
                const char* resourceName = imageResources.getUnchecked(resourceIndex).resourceName;
                int dataSize = 0;
                const char* resourceData = BinaryData::getNamedResource(resourceName, dataSize);

                if (resourceData != nullptr && dataSize > 0)
                    imageBuffer.setImage(juce::ImageFileFormat::loadFrom(resourceData, (size_t)dataSize));
            }
        }
    }

    // Any parameter change makes the preset "dirty" (a user preset).
    // We check the isLoadingPreset flag to avoid this when loading a preset.
    if (!isLoadingPreset)
    {
        if (currentProgram < factoryPresets.size()) {
            currentProgram = factoryPresets.size();
        }
    }
}

void MapSynthAudioProcessor::highPassFilter(juce::AudioBuffer<float>& buffer, float cutoffFreq)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    // Ensure state arrays are the correct size
    jassert(hpf_prevInput.size() == numChannels);
    jassert(hpf_prevOutput.size() == numChannels);

    // Filter coefficients (RC filter)
    float RC = 1.0f / (juce::MathConstants<float>::twoPi * cutoffFreq);
    float dt = 1.0f / processSampleRate;
    float alpha = RC / (RC + dt);

    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        float prevInput = hpf_prevInput[channel];
        float prevOutput = hpf_prevOutput[channel];

        for (int n = 0; n < numSamples; ++n)
        {
            float input = channelData[n];
            channelData[n] = alpha * (prevOutput + input - prevInput);
            prevOutput = channelData[n];
            prevInput = input;
        }
        hpf_prevInput.set(channel, prevInput);
        hpf_prevOutput.set(channel, prevOutput);
    }
}

void MapSynthAudioProcessor::updateParameters()
{
    for (int i = 0; i < 3; ++i)
    {
        auto& ellipseParams = globalParams.ellipses[i];
        juce::String prefix = "Ellipse" + juce::String(i + 1) + "_";

        ellipseParams.cx = apvts.getRawParameterValue(prefix + "CX")->load();
        ellipseParams.cy = apvts.getRawParameterValue(prefix + "CY")->load();
        ellipseParams.r1 = apvts.getRawParameterValue(prefix + "R1")->load();
        ellipseParams.r2 = apvts.getRawParameterValue(prefix + "R2")->load();
        ellipseParams.angle = apvts.getRawParameterValue(prefix + "Angle")->load();
        ellipseParams.volume = apvts.getRawParameterValue(prefix + "Volume")->load();
        ellipseParams.detune = apvts.getRawParameterValue(prefix + "Detune")->load();
        ellipseParams.detune = apvts.getRawParameterValue(prefix + "Detune")->load();
        ellipseParams.pan = apvts.getRawParameterValue(prefix + "Pan")->load();

        ellipseParams.modCxAmount = apvts.getRawParameterValue("Mod_" + prefix + "CX_Amount")->load();
        ellipseParams.modCxSelect = (int)apvts.getRawParameterValue("Mod_" + prefix + "CX_Select")->load();
        ellipseParams.modCyAmount = apvts.getRawParameterValue("Mod_" + prefix + "CY_Amount")->load();
        ellipseParams.modCySelect = (int)apvts.getRawParameterValue("Mod_" + prefix + "CY_Select")->load();
        ellipseParams.modR1Amount = apvts.getRawParameterValue("Mod_" + prefix + "R1_Amount")->load();
        ellipseParams.modR1Select = (int)apvts.getRawParameterValue("Mod_" + prefix + "R1_Select")->load();
        ellipseParams.modR2Amount = apvts.getRawParameterValue("Mod_" + prefix + "R2_Amount")->load();
        ellipseParams.modR2Select = (int)apvts.getRawParameterValue("Mod_" + prefix + "R2_Select")->load();
        ellipseParams.modAngleAmount = apvts.getRawParameterValue("Mod_" + prefix + "Angle_Amount")->load();
        ellipseParams.modAngleSelect = (int)apvts.getRawParameterValue("Mod_" + prefix + "Angle_Select")->load();
        ellipseParams.modVolumeAmount = apvts.getRawParameterValue("Mod_" + prefix + "Volume_Amount")->load();
        ellipseParams.modVolumeSelect = (int)apvts.getRawParameterValue("Mod_" + prefix + "Volume_Select")->load();
        ellipseParams.modPanAmount = apvts.getRawParameterValue("Mod_" + prefix + "Pan_Amount")->load();
        ellipseParams.modPanSelect = (int)apvts.getRawParameterValue("Mod_" + prefix + "Pan_Select")->load();
        ellipseParams.modFreqAmount = apvts.getRawParameterValue("Mod_" + prefix + "Freq_Amount")->load();
        ellipseParams.modFreqSelect = (int)apvts.getRawParameterValue("Mod_" + prefix + "Freq_Select")->load();

        ellipseParams.filter.type = (int)apvts.getRawParameterValue(prefix + "FilterType")->load();
        ellipseParams.filter.frequency = apvts.getRawParameterValue(prefix + "FilterFreq")->load();
        ellipseParams.filter.quality = apvts.getRawParameterValue(prefix + "FilterQuality")->load();
        ellipseParams.filter.modFreqAmount = apvts.getRawParameterValue("Mod_" + prefix + "FilterFreq_Amount")->load();
        ellipseParams.filter.modFreqSelect = (int)apvts.getRawParameterValue("Mod_" + prefix + "FilterFreq_Select")->load();
        ellipseParams.filter.modQualityAmount = apvts.getRawParameterValue("Mod_" + prefix + "FilterQuality_Amount")->load();
        ellipseParams.filter.modQualitySelect = (int)apvts.getRawParameterValue("Mod_" + prefix + "FilterQuality_Select")->load();
    }

    // ADSR
    globalParams.adsr.attack = apvts.getRawParameterValue ("Attack")->load();
    globalParams.adsr.decay = apvts.getRawParameterValue ("Decay")->load();
    globalParams.adsr.sustain = apvts.getRawParameterValue ("Sustain")->load();
    globalParams.adsr.release = apvts.getRawParameterValue ("Release")->load();

    // ADSR 2
    globalParams.adsr2.attack = apvts.getRawParameterValue ("Attack2")->load();
    globalParams.adsr2.decay = apvts.getRawParameterValue ("Decay2")->load();
    globalParams.adsr2.sustain = apvts.getRawParameterValue ("Sustain2")->load();
    globalParams.adsr2.release = apvts.getRawParameterValue ("Release2")->load();

    // ADSR 3
    globalParams.adsr3.attack = apvts.getRawParameterValue ("Attack3")->load();
    globalParams.adsr3.decay = apvts.getRawParameterValue ("Decay3")->load();
    globalParams.adsr3.sustain = apvts.getRawParameterValue ("Sustain3")->load();
    globalParams.adsr3.release = apvts.getRawParameterValue ("Release3")->load();
}

float getRateMultiplier(int choice)
{
    // These are multipliers for a base frequency of 1 beat (a quarter note)
    switch (choice)
    {
        case 0:  return 8.0f;        // 1/32
        case 1:  return 4.0f * 1.5f; // 1/16T
        case 2:  return 4.0f;        // 1/16
        case 3:  return 4.0f / 1.5f; // 1/16D
        case 4:  return 2.0f * 1.5f; // 1/8T
        case 5:  return 2.0f;        // 1/8
        case 6:  return 2.0f / 1.5f; // 1/8D
        case 7:  return 1.0f * 1.5f; // 1/4T
        case 8:  return 1.0f;        // 1/4
        case 9:  return 1.0f / 1.5f; // 1/4D
        case 10: return 0.5f * 1.5f; // 1/2T
        case 11: return 0.5f;        // 1/2
        case 12: return 0.5f / 1.5f; // 1/2D
        case 13: return 0.25f;       // 1 Bar
        default: return 1.0f;
    }
}

void MapSynthAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    masterLevelSmoother.setTargetValue(juce::Decibels::decibelsToGain(apvts.getRawParameterValue("Level")->load()));

    updateParameters();

    double bpm = 120.0;
    if (auto* playHead = getPlayHead())
    {
        if (auto positionInfo = playHead->getPosition())
        {
            if (positionInfo->getBpm() && *positionInfo->getBpm() > 0)
            {
                bpm = *positionInfo->getBpm();
            }
                
        }
    }

    auto getLfoFreq = [&] (const char* syncId, const char* rateId, const char* freqId)
    {
        auto* syncParam = apvts.getRawParameterValue(syncId);
        jassert (syncParam != nullptr); // This will fire in a debug build if the syncId is wrong

        if (syncParam != nullptr && syncParam->load() > 0.5f)
        {
            auto* rateParam = apvts.getRawParameterValue(rateId);
            jassert (rateParam != nullptr); // This will fire if the rateId is wrong

            if (rateParam == nullptr)
                return 1.0f; // Fallback to prevent crash in release build

            const int rateIndex = (int)rateParam->load();
            const float multiplier = getRateMultiplier(rateIndex);
            return (float) (bpm / 60.0 * multiplier);
        }
        
        auto* freqParam = apvts.getRawParameterValue(freqId);
        jassert (freqParam != nullptr); // This will fire if the freqId is wrong

        if (freqParam != nullptr)
            return freqParam->load();
        
        return 1.0f; // Fallback
    };

    // Set up the LFOs

    lfo.setWaveform((LFO::Waveform)(int)apvts.getRawParameterValue("LFO1Wave")->load());
    lfo.setFrequency(getLfoFreq("LFO1Sync", "LFO1Rate", "LFOFreq"));
    lfo.setPhaseOffset(apvts.getRawParameterValue("LFO1Phase")->load());

    lfo2.setWaveform((LFO::Waveform)(int)apvts.getRawParameterValue("LFO2Wave")->load());
    lfo2.setFrequency(getLfoFreq("LFO2Sync", "LFO2Rate", "LFO2Freq"));
    lfo2.setPhaseOffset(apvts.getRawParameterValue("LFO2Phase")->load());

    lfo3.setWaveform((LFO::Waveform)(int)apvts.getRawParameterValue("LFO3Wave")->load());
    lfo3.setFrequency(getLfoFreq("LFO3Sync", "LFO3Rate", "LFO3Freq"));
    lfo3.setPhaseOffset(apvts.getRawParameterValue("LFO3Phase")->load());

    lfo4.setWaveform((LFO::Waveform)(int)apvts.getRawParameterValue("LFO4Wave")->load());
    lfo4.setFrequency(getLfoFreq("LFO4Sync", "LFO4Rate", "LFO4Freq"));
    lfo4.setPhaseOffset(apvts.getRawParameterValue("LFO4Phase")->load());

    // Process LFOs for the block
    auto* lfo1Data = lfoBuffer.getWritePointer (0);
    auto* lfo2Data = lfoBuffer.getWritePointer (1);
    auto* lfo3Data = lfoBuffer.getWritePointer (2);
    auto* lfo4Data = lfoBuffer.getWritePointer (3);

    for (int i = 0; i < buffer.getNumSamples(); ++i)
    {
        lfo1Data[i] = lfo.process();
        lfo2Data[i] = lfo2.process();
        lfo3Data[i] = lfo3.process();
        lfo4Data[i] = lfo4.process();
    }

    buffer.clear();
    synth.renderNextBlock (buffer, midiMessages, 0, buffer.getNumSamples());

    masterLevelSmoother.applyGain(buffer, buffer.getNumSamples());

    highPassFilter(buffer, 15.0f);

    // Vu-Meter
    for (int i=0; i<2; ++i)
    {
        smoothedMaxLevel[i].skip(buffer.getNumSamples());
        maxLevel[i] = juce::Decibels::gainToDecibels(buffer.getMagnitude(i,0,buffer.getNumSamples()));
        if (maxLevel[i] < smoothedMaxLevel[i].getCurrentValue())
            smoothedMaxLevel[i].setTargetValue(maxLevel[i]);
        else
            smoothedMaxLevel[i].setCurrentAndTargetValue(maxLevel[i]);

    }
    
}

//==============================================================================
bool MapSynthAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MapSynthAudioProcessor::createEditor()
{
    return new MapSynthAudioProcessorEditor (*this);
}

//==============================================================================
void MapSynthAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());

    // Store the current program index so the host can recall it.
    xml->setAttribute ("currentProgram", currentProgram);

    // Also store the OpenGL state, but outside the main parameter block
    xml->setAttribute ("useOpenGL", useOpenGL);

    // If the current image is a custom user-loaded one, save its path.
    // The "FactoryImage" parameter will be at index 0 ("Custom").
    if (static_cast<int>(apvts.getRawParameterValue("FactoryImage")->load()) == 0)
    {
        auto imageFile = imageBuffer.getFile();
        if (imageFile.existsAsFile())
        {
            xml->setAttribute ("imagePath", imageFile.getFullPathName());
        }
    }

    copyXmlToBinary (*xml, destData);
}

void MapSynthAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState != nullptr)
    {
        if (xmlState->hasTagName (apvts.state.getType()))
        {
            // Restore OpenGL state. Default to false if not present.
            setUseOpenGL (xmlState->getBoolAttribute ("useOpenGL", false));

            // Use a flag to prevent parameterChanged from marking this as a user edit.
            juce::ScopedValueSetter<bool> loading (isLoadingPreset, true);

            // Restore the program index before restoring the full state.
            currentProgram = xmlState->getIntAttribute ("currentProgram", factoryPresets.size());

            // Restore parameters. This will trigger parameterChanged for FactoryImage if it's not "Custom".
            apvts.replaceState (juce::ValueTree::fromXml (*xmlState));

            if (xmlState->hasAttribute ("imagePath"))
            {
                auto imagePath = xmlState->getStringAttribute ("imagePath");
                // This will fail silently if file not found, which is acceptable.
                imageBuffer.setImage (juce::File (imagePath));
            }

            // This is useful for creating new factory presets.
            std::cout << (apvts.copyState().createXml()->toString()) << std::endl;
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MapSynthAudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout MapSynthAudioProcessor::createParameters()
{
    // Get the list of image resources
    static const auto imageResources = ImageResourceHelper::getFactoryImageResources();
    
    // Create the string array for the ComboBox
    juce::StringArray factoryImageChoices;
    factoryImageChoices.add("Custom");
    for (const auto& resource : imageResources)
        factoryImageChoices.add(resource.friendlyName);

    juce::AudioProcessorValueTreeState::ParameterLayout layout;
        
    layout.add(std::make_unique<juce::AudioParameterChoice>("FactoryImage", "Factory Image", factoryImageChoices, 1));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Level","Level",juce::NormalisableRange<float>(-60.f,12.f,1e-2f,1.f),0.f));
    layout.add(std::make_unique<juce::AudioParameterBool>("OscilloscopeEnabled", "Enable Oscilloscope", true));

    layout.add(std::make_unique<juce::AudioParameterFloat>("LFOFreq", "LFO 1 Freq", juce::NormalisableRange<float>(0.01f, 200.0f, 0.01f, 0.3f), 1.0f));
    layout.add(std::make_unique<juce::AudioParameterBool>("LFO1Sync", "LFO 1 Sync", false));
    layout.add(std::make_unique<juce::AudioParameterChoice>("LFO1Wave", "LFO 1 Wave", lfoWaveformChoices, 0));
    layout.add(std::make_unique<juce::AudioParameterChoice>("LFO1Rate", "LFO 1 Rate", tempoSyncRateChoices, 8)); // Default to 1/4
    layout.add(std::make_unique<juce::AudioParameterFloat>("LFO2Freq", "LFO 2 Freq", juce::NormalisableRange<float>(0.01f, 200.0f, 0.01f, 0.3f), 1.0f));
    layout.add(std::make_unique<juce::AudioParameterBool>("LFO2Sync", "LFO 2 Sync", false));
    layout.add(std::make_unique<juce::AudioParameterChoice>("LFO2Wave", "LFO 2 Wave", lfoWaveformChoices, 0));
    layout.add(std::make_unique<juce::AudioParameterChoice>("LFO2Rate", "LFO 2 Rate", tempoSyncRateChoices, 8));
    layout.add(std::make_unique<juce::AudioParameterFloat>("LFO3Freq", "LFO 3 Freq", juce::NormalisableRange<float>(0.01f, 200.0f, 0.01f, 0.3f), 1.0f));
    layout.add(std::make_unique<juce::AudioParameterBool>("LFO3Sync", "LFO 3 Sync", false));
    layout.add(std::make_unique<juce::AudioParameterChoice>("LFO3Wave", "LFO 3 Wave", lfoWaveformChoices, 0));
    layout.add(std::make_unique<juce::AudioParameterChoice>("LFO3Rate", "LFO 3 Rate", tempoSyncRateChoices, 8));
    layout.add(std::make_unique<juce::AudioParameterFloat>("LFO4Freq", "LFO 4 Freq", juce::NormalisableRange<float>(0.01f, 200.0f, 0.01f, 0.3f), 1.0f));
    layout.add(std::make_unique<juce::AudioParameterBool>("LFO4Sync", "LFO 4 Sync", false));
    layout.add(std::make_unique<juce::AudioParameterChoice>("LFO4Wave", "LFO 4 Wave", lfoWaveformChoices, 0));
    layout.add(std::make_unique<juce::AudioParameterChoice>("LFO4Rate", "LFO 4 Rate", tempoSyncRateChoices, 8));
    layout.add(std::make_unique<juce::AudioParameterFloat>("LFO1Phase", "LFO 1 Phase", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("LFO2Phase", "LFO 2 Phase", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("LFO3Phase", "LFO 3 Phase", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("LFO4Phase", "LFO 4 Phase", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("Attack", "Attack", juce::NormalisableRange<float>(0.0f, 5.0f, 0.01f, 0.5f), 0.1f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Decay", "Decay", juce::NormalisableRange<float>(0.0f, 5.0f, 0.01f, 0.5f), 0.1f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Sustain", "Sustain", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 1.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Release", "Release", juce::NormalisableRange<float>(0.0f, 5.0f, 0.01f, 0.5f), 0.4f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("Attack2", "Attack 2", juce::NormalisableRange<float>(0.0f, 5.0f, 0.01f, 0.5f), 0.1f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Decay2", "Decay 2", juce::NormalisableRange<float>(0.0f, 5.0f, 0.01f, 0.5f), 0.1f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Sustain2", "Sustain 2", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 1.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Release2", "Release 2", juce::NormalisableRange<float>(0.0f, 5.0f, 0.01f, 0.5f), 0.4f));

    layout.add(std::make_unique<juce::AudioParameterFloat>("Attack3", "Attack 3", juce::NormalisableRange<float>(0.0f, 5.0f, 0.01f, 0.5f), 0.1f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Decay3", "Decay 3", juce::NormalisableRange<float>(0.0f, 5.0f, 0.01f, 0.5f), 0.1f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Sustain3", "Sustain 3", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 1.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Release3", "Release 3", juce::NormalisableRange<float>(0.0f, 5.0f, 0.01f, 0.5f), 0.4f));

    for (int i = 1; i <= 3; ++i)
    {
        ParameterHelpers::addEllipseParameters(layout, i, modulatorChoices, filterTypeChoices);
    }

    return layout;
}
