/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "SynthSound.h"

//==============================================================================
MapSynthAudioProcessor::MapSynthAudioProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
{
    imageBuffer.setImage (juce::ImageCache::getFromMemory (BinaryData::world_png, BinaryData::world_pngSize));

    // Set up the initial readers
    readerTypes.add (ReaderBase::Type::Line);
    readerTypes.add (ReaderBase::Type::Circle);

    synth.addSound (new SynthSound());
    for (int i = 0; i < NUM_VOICES; ++i)
    {
        synth.addVoice (new SynthVoice (*this, i));
    }

    // This must be called after voices are created.
    updateVoices();
}

MapSynthAudioProcessor::~MapSynthAudioProcessor()
{
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
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int MapSynthAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MapSynthAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String MapSynthAudioProcessor::getProgramName (int index)
{
    return {};
}

void MapSynthAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void MapSynthAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    synth.setCurrentPlaybackSampleRate (sampleRate);

    lfoBuffer.setSize (4, samplesPerBlock);

    lfo.prepareToPlay (sampleRate);
    lfo2.prepareToPlay (sampleRate);
    lfo3.prepareToPlay (sampleRate);
    lfo4.prepareToPlay (sampleRate);
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

void MapSynthAudioProcessor::addReader (ReaderBase::Type type)
{
    readerTypes.add (type);
    updateVoices();
}

void MapSynthAudioProcessor::removeReader (int index)
{
    if (juce::isPositiveAndBelow (index, readerTypes.size()))
    {
        readerTypes.remove (index);
        updateVoices();
    }
}

void MapSynthAudioProcessor::updateVoices()
{
    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<SynthVoice*> (synth.getVoice (i)))
        {
            voice->rebuildReaders (readerTypes);
        }
    }
}

void MapSynthAudioProcessor::updateParameters()
{
    // Line Reader
    globalParams.line.cx = apvts.getRawParameterValue ("LineCX")->load();
    globalParams.line.cy = apvts.getRawParameterValue ("LineCY")->load();
    globalParams.line.length = apvts.getRawParameterValue ("LineLength")->load();
    globalParams.line.angle = apvts.getRawParameterValue ("LineAngle")->load();
    globalParams.line.volume = apvts.getRawParameterValue ("LineVolume")->load();
    globalParams.line.modCxAmount = apvts.getRawParameterValue ("Mod_LineCX_Amount")->load();
    globalParams.line.modCxSelect = (int) apvts.getRawParameterValue ("Mod_LineCX_Select")->load();
    globalParams.line.modCyAmount = apvts.getRawParameterValue ("Mod_LineCY_Amount")->load();
    globalParams.line.modCySelect = (int) apvts.getRawParameterValue ("Mod_LineCY_Select")->load();
    globalParams.line.modAngleAmount = apvts.getRawParameterValue ("Mod_LineAngle_Amount")->load();
    globalParams.line.modAngleSelect = (int) apvts.getRawParameterValue ("Mod_LineAngle_Select")->load();
    globalParams.line.modLengthAmount = apvts.getRawParameterValue ("Mod_LineLength_Amount")->load();
    globalParams.line.modLengthSelect = (int) apvts.getRawParameterValue ("Mod_LineLength_Select")->load();
    globalParams.line.modVolumeAmount = apvts.getRawParameterValue ("Mod_LineVolume_Amount")->load();
    globalParams.line.modVolumeSelect = (int) apvts.getRawParameterValue ("Mod_LineVolume_Select")->load();
    globalParams.line.filter.type = (int)apvts.getRawParameterValue("LineFilterType")->load();
    globalParams.line.filter.frequency = apvts.getRawParameterValue("LineFilterFreq")->load();
    globalParams.line.filter.quality = apvts.getRawParameterValue("LineFilterQuality")->load();
    globalParams.line.filter.modFreqAmount = apvts.getRawParameterValue("Mod_LineFilterFreq_Amount")->load();
    globalParams.line.filter.modFreqSelect = (int)apvts.getRawParameterValue("Mod_LineFilterFreq_Select")->load();
    globalParams.line.filter.modQualityAmount = apvts.getRawParameterValue("Mod_LineFilterQuality_Amount")->load();
    globalParams.line.filter.modQualitySelect = (int)apvts.getRawParameterValue("Mod_LineFilterQuality_Select")->load();
    globalParams.line.pan = apvts.getRawParameterValue("LinePan")->load();
    globalParams.line.modPanAmount = apvts.getRawParameterValue("Mod_LinePan_Amount")->load();
    globalParams.line.modPanSelect = (int)apvts.getRawParameterValue("Mod_LinePan_Select")->load();
    globalParams.line.modFreqAmount = apvts.getRawParameterValue("Mod_LineFreq_Amount")->load();
    globalParams.line.modFreqSelect = (int)apvts.getRawParameterValue("Mod_LineFreq_Select")->load();

    // Circle Reader
    globalParams.circle.cx = apvts.getRawParameterValue ("CX")->load();
    globalParams.circle.cy = apvts.getRawParameterValue ("CY")->load();
    globalParams.circle.radius = apvts.getRawParameterValue ("R")->load();
    globalParams.circle.volume = apvts.getRawParameterValue ("CircleVolume")->load();
    globalParams.circle.modCxAmount = apvts.getRawParameterValue ("Mod_CircleCX_Amount")->load();
    globalParams.circle.modCxSelect = (int) apvts.getRawParameterValue ("Mod_CircleCX_Select")->load();
    globalParams.circle.modCyAmount = apvts.getRawParameterValue ("Mod_CircleCY_Amount")->load();
    globalParams.circle.modCySelect = (int) apvts.getRawParameterValue ("Mod_CircleCY_Select")->load();
    globalParams.circle.modRadiusAmount = apvts.getRawParameterValue ("Mod_CircleRadius_Amount")->load();
    globalParams.circle.modRadiusSelect = (int) apvts.getRawParameterValue ("Mod_CircleRadius_Select")->load();
    globalParams.circle.modVolumeAmount = apvts.getRawParameterValue ("Mod_CircleVolume_Amount")->load();
    globalParams.circle.modVolumeSelect = (int) apvts.getRawParameterValue ("Mod_CircleVolume_Select")->load();
    globalParams.circle.filter.type = (int)apvts.getRawParameterValue("CircleFilterType")->load();
    globalParams.circle.filter.frequency = apvts.getRawParameterValue("CircleFilterFreq")->load();
    globalParams.circle.filter.quality = apvts.getRawParameterValue("CircleFilterQuality")->load();
    globalParams.circle.filter.modFreqAmount = apvts.getRawParameterValue("Mod_CircleFilterFreq_Amount")->load();
    globalParams.circle.filter.modFreqSelect = (int)apvts.getRawParameterValue("Mod_CircleFilterFreq_Select")->load();
    globalParams.circle.filter.modQualityAmount = apvts.getRawParameterValue("Mod_CircleFilterQuality_Amount")->load();
    globalParams.circle.filter.modQualitySelect = (int)apvts.getRawParameterValue("Mod_CircleFilterQuality_Select")->load();
    globalParams.circle.pan = apvts.getRawParameterValue("CirclePan")->load();
    globalParams.circle.modPanAmount = apvts.getRawParameterValue("Mod_CirclePan_Amount")->load();
    globalParams.circle.modPanSelect = (int)apvts.getRawParameterValue("Mod_CirclePan_Select")->load();
    globalParams.circle.modFreqAmount = apvts.getRawParameterValue("Mod_CircleFreq_Amount")->load();
    globalParams.circle.modFreqSelect = (int)apvts.getRawParameterValue("Mod_CircleFreq_Select")->load();

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
    // Get the parameters' state from APVTS
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());

    // Get the source file for the current image
    auto imageFile = imageBuffer.getFile();

    if (imageFile.existsAsFile())
    {
        // If we have a valid file, save its path
        xml->setAttribute ("imagePath", imageFile.getFullPathName());
    }
    else
    {
        // Otherwise (e.g. for the default image), save the raw image data as a fallback
        auto image = imageBuffer.getImage();
        if (image.isValid())
        {
            juce::MemoryOutputStream memoryStream;
            juce::PNGImageFormat pngFormat;
            if (pngFormat.writeImageToStream (image, memoryStream))
            {
                xml->setAttribute ("imageData", memoryStream.getMemoryBlock().toBase64Encoding());
            }
        }
    }
    
    // Store the combined XML state in the memory block
    copyXmlToBinary (*xml, destData);
}

void MapSynthAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // Restore the state from XML
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState != nullptr)
    {
        if (xmlState->hasTagName (apvts.state.getType()))
        {
            // First, try to restore from a file path
            if (xmlState->hasAttribute ("imagePath"))
            {
                auto imagePath = xmlState->getStringAttribute ("imagePath");
                if (! imageBuffer.setImage (juce::File (imagePath)))
                {
                    // If loading from path fails, do nothing, keep current/default image.
                    // You could show an error message here if you wanted.
                }
            }
            // If no path, try to restore from embedded data (for older presets or default state)
            else if (xmlState->hasAttribute ("imageData"))
            {
                auto base64Data = xmlState->getStringAttribute ("imageData");
                juce::MemoryBlock imageData;
                if (imageData.fromBase64Encoding (base64Data))
                {
                    juce::Image loadedImage = juce::ImageFileFormat::loadFrom (imageData.getData(), imageData.getSize());
                    imageBuffer.setImage (loadedImage);
                }
            }

            // Then, restore the parameters using the APVTS
            apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
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
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
        
    layout.add(std::make_unique<juce::AudioParameterFloat>("Level","Level",juce::NormalisableRange<float>(-60.f,12.f,1e-2f,1.f),0.f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("LineCX", "LineCX", juce::NormalisableRange<float>(0.f, 1.f, .01f, 1.f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("LineCY", "LineCY", juce::NormalisableRange<float>(0.f, 1.f, .01f, 1.f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("LineLength", "LineLength", juce::NormalisableRange<float>(0.f, 1.f, .01f, 1.f), 0.6f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("LineAngle", "LineAngle", juce::NormalisableRange<float>(0.f, juce::MathConstants<float>::twoPi, .01f, 1.f), 0.0f));    
    layout.add(std::make_unique<juce::AudioParameterFloat>("CX", "CX", juce::NormalisableRange<float>(0.f, 1.f, .01f, 1.f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("CY", "CY", juce::NormalisableRange<float>(0.f, 1.f, .01f, 1.f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("R", "R", juce::NormalisableRange<float>(0.f, 0.5f, .01f, 1.f), 0.25f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("LineVolume", "LineVolume", juce::NormalisableRange<float>(0.f, 1.f, .01f, 1.f), 1.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("CircleVolume", "CircleVolume", juce::NormalisableRange<float>(0.f, 1.f, .01f, 1.f), 1.0f));
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

    layout.add(std::make_unique<juce::AudioParameterFloat>("Mod_LineCX_Amount", "Mod->LineCX", juce::NormalisableRange<float>(-1.f, 1.f, .01f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterChoice>("Mod_LineCX_Select", "Mod Select", modulatorChoices, 0));

    layout.add(std::make_unique<juce::AudioParameterFloat>("Mod_LineCY_Amount", "Mod->LineCY", juce::NormalisableRange<float>(-1.f, 1.f, .01f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterChoice>("Mod_LineCY_Select", "Mod Select", modulatorChoices, 0));

    layout.add(std::make_unique<juce::AudioParameterFloat>("Mod_LineAngle_Amount", "Mod->Angle", juce::NormalisableRange<float>(-1.f, 1.f, .01f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterChoice>("Mod_LineAngle_Select", "Mod Select", modulatorChoices, 0));

    layout.add(std::make_unique<juce::AudioParameterFloat>("Mod_LineLength_Amount", "Mod->Length", juce::NormalisableRange<float>(-1.f, 1.f, .01f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterChoice>("Mod_LineLength_Select", "Mod Select", modulatorChoices, 0));

    layout.add(std::make_unique<juce::AudioParameterFloat>("Mod_LineVolume_Amount", "Mod->LineVol", juce::NormalisableRange<float>(-1.f, 1.f, .01f), 1.0f));
    layout.add(std::make_unique<juce::AudioParameterChoice>("Mod_LineVolume_Select", "Mod Select", modulatorChoices, 4)); // Default to ADSR1

    layout.add(std::make_unique<juce::AudioParameterFloat>("Mod_CircleCX_Amount", "Mod->CircleCX", juce::NormalisableRange<float>(-1.f, 1.f, .01f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterChoice>("Mod_CircleCX_Select", "Mod Select", modulatorChoices, 0));

    layout.add(std::make_unique<juce::AudioParameterFloat>("Mod_CircleCY_Amount", "Mod->CircleCY", juce::NormalisableRange<float>(-1.f, 1.f, .01f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterChoice>("Mod_CircleCY_Select", "Mod Select", modulatorChoices, 0));

    layout.add(std::make_unique<juce::AudioParameterFloat>("Mod_CircleRadius_Amount", "Mod->Radius", juce::NormalisableRange<float>(-1.f, 1.f, .01f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterChoice>("Mod_CircleRadius_Select", "Mod Select", modulatorChoices, 0));

    layout.add(std::make_unique<juce::AudioParameterFloat>("Mod_CircleVolume_Amount", "Mod->CircleVol", juce::NormalisableRange<float>(-1.f, 1.f, .01f), 1.0f));
    layout.add(std::make_unique<juce::AudioParameterChoice>("Mod_CircleVolume_Select", "Mod Select", modulatorChoices, 4)); // Default to ADSR1

    // Line Reader Filter
    layout.add(std::make_unique<juce::AudioParameterChoice>("LineFilterType", "Line Filter Type", filterTypeChoices, 0));
    layout.add(std::make_unique<juce::AudioParameterFloat>("LineFilterFreq", "Line Filter Freq", juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.3f), 20000.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("LineFilterQuality", "Line Filter Q", juce::NormalisableRange<float>(0.1f, 18.0f, 0.01f), 1.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Mod_LineFilterFreq_Amount", "Mod->LineFltFreq", juce::NormalisableRange<float>(-1.f, 1.f, .01f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterChoice>("Mod_LineFilterFreq_Select", "Mod Select", modulatorChoices, 0));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Mod_LineFilterQuality_Amount", "Mod->LineFltQ", juce::NormalisableRange<float>(-1.f, 1.f, .01f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterChoice>("Mod_LineFilterQuality_Select", "Mod Select", modulatorChoices, 0));

    // Circle Reader Filter
    layout.add(std::make_unique<juce::AudioParameterChoice>("CircleFilterType", "Circle Filter Type", filterTypeChoices, 0));
    layout.add(std::make_unique<juce::AudioParameterFloat>("CircleFilterFreq", "Circle Filter Freq", juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.3f), 20000.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("CircleFilterQuality", "Circle Filter Q", juce::NormalisableRange<float>(0.1f, 18.0f, 0.01f), 1.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Mod_CircleFilterFreq_Amount", "Mod->CircFltFreq", juce::NormalisableRange<float>(-1.f, 1.f, .01f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterChoice>("Mod_CircleFilterFreq_Select", "Mod Select", modulatorChoices, 0));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Mod_CircleFilterQuality_Amount", "Mod->CircFltQ", juce::NormalisableRange<float>(-1.f, 1.f, .01f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterChoice>("Mod_CircleFilterQuality_Select", "Mod Select", modulatorChoices, 0));

    // Line Reader Pan
    layout.add(std::make_unique<juce::AudioParameterFloat>("LinePan", "Line Pan", juce::NormalisableRange<float>(-1.0f, 1.0f, 0.01f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Mod_LinePan_Amount", "Mod->LinePan", juce::NormalisableRange<float>(-1.f, 1.f, .01f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterChoice>("Mod_LinePan_Select", "Mod Select", modulatorChoices, 0));

    // Circle Reader Pan
    layout.add(std::make_unique<juce::AudioParameterFloat>("CirclePan", "Circle Pan", juce::NormalisableRange<float>(-1.0f, 1.0f, 0.01f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>("Mod_CirclePan_Amount", "Mod->CircPan", juce::NormalisableRange<float>(-1.f, 1.f, .01f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterChoice>("Mod_CirclePan_Select", "Mod Select", modulatorChoices, 0));

    // Line Freq Mod
    layout.add(std::make_unique<juce::AudioParameterFloat>("Mod_LineFreq_Amount", "Mod->LineFreq", juce::NormalisableRange<float>(-1.f, 1.f, .01f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterChoice>("Mod_LineFreq_Select", "Mod Select", modulatorChoices, 0));

    // Circle Freq Mod
    layout.add(std::make_unique<juce::AudioParameterFloat>("Mod_CircleFreq_Amount", "Mod->CircFreq", juce::NormalisableRange<float>(-1.f, 1.f, .01f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterChoice>("Mod_CircleFreq_Select", "Mod Select", modulatorChoices, 0));

    layout.add(std::make_unique<juce::AudioParameterBool>("UseOpenGL", "Use OpenGL", true));

    return layout;
}
