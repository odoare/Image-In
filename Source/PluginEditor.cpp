/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

class GlobalControlsComponent  : public juce::Component
{
public:
    GlobalControlsComponent(MapSynthAudioProcessor& p)
        : lfoFreqKnob (p.apvts, "LFOFreq", juce::Colours::hotpink),
          lfo2FreqKnob (p.apvts, "LFO2Freq", juce::Colours::hotpink),
          lfo3FreqKnob (p.apvts, "LFO3Freq", juce::Colours::hotpink),
          lfo4FreqKnob (p.apvts, "LFO4Freq", juce::Colours::hotpink),
          lfo1PhaseKnob (p.apvts, "LFO1Phase", juce::Colours::hotpink),
          lfo2PhaseKnob (p.apvts, "LFO2Phase", juce::Colours::hotpink),
          lfo3PhaseKnob (p.apvts, "LFO3Phase", juce::Colours::hotpink),
          lfo4PhaseKnob (p.apvts, "LFO4Phase", juce::Colours::hotpink),
          attackKnob (p.apvts, "Attack", juce::Colours::limegreen),
          decayKnob (p.apvts, "Decay", juce::Colours::limegreen),
          sustainKnob (p.apvts, "Sustain", juce::Colours::limegreen),
          releaseKnob (p.apvts, "Release", juce::Colours::limegreen),
          attack2Knob (p.apvts, "Attack2", juce::Colours::cyan),
          decay2Knob (p.apvts, "Decay2", juce::Colours::cyan),
          sustain2Knob (p.apvts, "Sustain2", juce::Colours::cyan),
          release2Knob (p.apvts, "Release2", juce::Colours::cyan)
    {
        addAndMakeVisible(globalControlsGroup);
        globalControlsGroup.setText("Global / LFO");

        addAndMakeVisible (lfoFreqKnob.slider);
        lfoFreqKnob.slider.setLookAndFeel (&fxmeLookAndFeel);

        addAndMakeVisible (lfo2FreqKnob.slider);
        lfo2FreqKnob.slider.setLookAndFeel (&fxmeLookAndFeel);

        addAndMakeVisible (lfo3FreqKnob.slider);
        lfo3FreqKnob.slider.setLookAndFeel (&fxmeLookAndFeel);
        addAndMakeVisible (lfo4FreqKnob.slider);
        lfo4FreqKnob.slider.setLookAndFeel (&fxmeLookAndFeel);

        addAndMakeVisible (lfo1PhaseKnob.slider);
        lfo1PhaseKnob.slider.setLookAndFeel (&fxmeLookAndFeel);
        addAndMakeVisible (lfo2PhaseKnob.slider);
        lfo2PhaseKnob.slider.setLookAndFeel (&fxmeLookAndFeel);
        addAndMakeVisible (lfo3PhaseKnob.slider);
        lfo3PhaseKnob.slider.setLookAndFeel (&fxmeLookAndFeel);
        addAndMakeVisible (lfo4PhaseKnob.slider);
        lfo4PhaseKnob.slider.setLookAndFeel (&fxmeLookAndFeel);

        addAndMakeVisible (attackKnob.slider);
        attackKnob.slider.setLookAndFeel (&fxmeLookAndFeel);
        addAndMakeVisible (decayKnob.slider);
        decayKnob.slider.setLookAndFeel (&fxmeLookAndFeel);
        addAndMakeVisible (sustainKnob.slider);
        sustainKnob.slider.setLookAndFeel (&fxmeLookAndFeel);
        addAndMakeVisible (releaseKnob.slider);
        releaseKnob.slider.setLookAndFeel (&fxmeLookAndFeel);

        addAndMakeVisible (attack2Knob.slider);
        attack2Knob.slider.setLookAndFeel (&fxmeLookAndFeel);
        addAndMakeVisible (decay2Knob.slider);
        decay2Knob.slider.setLookAndFeel (&fxmeLookAndFeel);
        addAndMakeVisible (sustain2Knob.slider);
        sustain2Knob.slider.setLookAndFeel (&fxmeLookAndFeel);
        addAndMakeVisible (release2Knob.slider);
        release2Knob.slider.setLookAndFeel (&fxmeLookAndFeel);
    }

    void resized() override
    {
        globalControlsGroup.setBounds(getLocalBounds());
        auto globalContentBounds = globalControlsGroup.getBounds().reduced(5).withTrimmedTop(15);

        juce::FlexBox globalFb;
        globalFb.flexDirection = juce::FlexBox::Direction::column;

        juce::FlexBox lfo12Box;
        lfo12Box.flexDirection = juce::FlexBox::Direction::row;
        lfo12Box.items.add (juce::FlexItem (lfoFreqKnob.flex()).withFlex (1.0));
        lfo12Box.items.add (juce::FlexItem (lfo1PhaseKnob.flex()).withFlex (1.0));
        lfo12Box.items.add (juce::FlexItem (lfo2FreqKnob.flex()).withFlex (1.0));
        lfo12Box.items.add (juce::FlexItem (lfo2PhaseKnob.flex()).withFlex (1.0));

        juce::FlexBox lfo34Box;
        lfo34Box.flexDirection = juce::FlexBox::Direction::row;
        lfo34Box.items.add (juce::FlexItem (lfo3FreqKnob.flex()).withFlex (1.0));
        lfo34Box.items.add (juce::FlexItem (lfo3PhaseKnob.flex()).withFlex (1.0));
        lfo34Box.items.add (juce::FlexItem (lfo4FreqKnob.flex()).withFlex (1.0));
        lfo34Box.items.add (juce::FlexItem (lfo4PhaseKnob.flex()).withFlex (1.0));

        juce::FlexBox adsrBox;
        adsrBox.flexDirection = juce::FlexBox::Direction::row;
        adsrBox.items.add (juce::FlexItem (attackKnob.flex()).withFlex (1.0));
        adsrBox.items.add (juce::FlexItem (decayKnob.flex()).withFlex (1.0));
        adsrBox.items.add (juce::FlexItem (sustainKnob.flex()).withFlex (1.0));
        adsrBox.items.add (juce::FlexItem (releaseKnob.flex()).withFlex (1.0));

        juce::FlexBox adsr2Box;
        adsr2Box.flexDirection = juce::FlexBox::Direction::row;
        adsr2Box.items.add (juce::FlexItem (attack2Knob.flex()).withFlex (1.0));
        adsr2Box.items.add (juce::FlexItem (decay2Knob.flex()).withFlex (1.0));
        adsr2Box.items.add (juce::FlexItem (sustain2Knob.flex()).withFlex (1.0));
        adsr2Box.items.add (juce::FlexItem (release2Knob.flex()).withFlex (1.0));

        globalFb.items.add (juce::FlexItem(lfo12Box).withFlex(1.0));
        globalFb.items.add (juce::FlexItem(lfo34Box).withFlex(1.0));
        globalFb.items.add (juce::FlexItem(adsrBox).withFlex(1.0));
        globalFb.items.add (juce::FlexItem(adsr2Box).withFlex(1.0));

        globalFb.performLayout(globalContentBounds);
    }

private:
    fxme::FxmeLookAndFeel fxmeLookAndFeel;
    juce::GroupComponent globalControlsGroup;
    fxme::FxmeKnob lfoFreqKnob, lfo2FreqKnob, lfo3FreqKnob, lfo4FreqKnob;
    fxme::FxmeKnob lfo1PhaseKnob, lfo2PhaseKnob, lfo3PhaseKnob, lfo4PhaseKnob;
    fxme::FxmeKnob attackKnob, decayKnob, sustainKnob, releaseKnob;
    fxme::FxmeKnob attack2Knob, decay2Knob, sustain2Knob, release2Knob;
};

//==============================================================================
MapSynthAudioProcessorEditor::MapSynthAudioProcessorEditor (MapSynthAudioProcessor& p)
    : AudioProcessorEditor (&p), 
      audioProcessor (p),
      lineReaderComponent(p), 
      circleReaderComponent(p)
{
    globalControlsComponent = std::make_unique<GlobalControlsComponent>(p);

    mapDisplayComponentCPU = std::make_unique<MapDisplayComponent>(p);
    addAndMakeVisible(mapDisplayComponentCPU.get());

    mapDisplayComponentGL = std::make_unique<MapDisplayComponent_GL>(p);
    addAndMakeVisible(mapDisplayComponentGL.get());

    addAndMakeVisible (loadImageButton);
    loadImageButton.setButtonText ("Load Image");
    loadImageButton.onClick = [this]
    {
        fileChooser = std::make_unique<juce::FileChooser> ("Select an image file...",
                                                           juce::File{},
                                                           "*.png,*.jpg,*.jpeg,*.gif");

        auto chooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

        fileChooser->launchAsync (chooserFlags, [this] (const juce::FileChooser& fc)
        {
            auto file = fc.getResult();

            if (file != juce::File{})
            {
                if (! audioProcessor.imageBuffer.setImage (file))
                {
                    juce::AlertWindow::showMessageBoxAsync (juce::AlertWindow::WarningIcon, "Image Load Error", "Could not load the image file: " + file.getFileName());
                }
            }
        });
    };

    addAndMakeVisible(useOpenGLButton);
    useOpenGLButton.setButtonText("Use OpenGL");
    useOpenGLAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "UseOpenGL", useOpenGLButton);
    
    addAndMakeVisible(readerTabs);
    readerTabs.addTab("Global / LFO", juce::Colours::transparentBlack, globalControlsComponent.get(), false);
    readerTabs.addTab("Line Reader", juce::Colours::transparentBlack, &lineReaderComponent, false);
    readerTabs.addTab("Circle Reader", juce::Colours::transparentBlack, &circleReaderComponent, false);

    audioProcessor.apvts.addParameterListener("UseOpenGL", this);
    updateRendererVisibility();

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setResizable(true, true);
    setSize (900, 450);
}

MapSynthAudioProcessorEditor::~MapSynthAudioProcessorEditor()
{
    audioProcessor.apvts.removeParameterListener("UseOpenGL", this);
}

void MapSynthAudioProcessorEditor::parameterChanged (const juce::String& parameterID, float newValue)
{
    if (parameterID == "UseOpenGL")
    {
        updateRendererVisibility();
    }
}

//==============================================================================
void MapSynthAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)

    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId).darker (0.2f));
}

void MapSynthAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto bounds = getLocalBounds();
    auto leftPanelArea = bounds.removeFromLeft(juce::jmin(500, bounds.getWidth() / 2));
    auto rightPanel = bounds;

    auto leftPanelPadded = leftPanelArea.reduced(5);
    auto buttonArea = leftPanelPadded.removeFromTop(30);
    readerTabs.setBounds(leftPanelPadded);

    loadImageButton.setBounds(buttonArea.getX(), buttonArea.getY(), 80, 24);
    useOpenGLButton.setBounds(loadImageButton.getRight() + 10, buttonArea.getY(), 100, 24);

    auto mapArea = rightPanel.reduced(5);
    mapDisplayComponentCPU->setBounds(mapArea);
    mapDisplayComponentGL->setBounds(mapArea);
}

void MapSynthAudioProcessorEditor::updateRendererVisibility()
{
    const bool useOpenGL = audioProcessor.apvts.getRawParameterValue("UseOpenGL")->load() > 0.5f;

    mapDisplayComponentGL->setVisible (useOpenGL);
    mapDisplayComponentCPU->setVisible (!useOpenGL);
}
