/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "ParameterStructs.h"
#include "colours.h"
#include "LFOControlComponent.h"

class GlobalControlsComponent  : public juce::Component
{
public:
    GlobalControlsComponent(MapSynthAudioProcessor& p)
        : audioProcessor(p),
          lfo1Controls(p, 1),
          lfo2Controls(p, 2),
          lfo3Controls(p, 3),
          lfo4Controls(p, 4),
          attackKnob (p.apvts, "Attack", juce::Colours::limegreen),
          decayKnob (p.apvts, "Decay", juce::Colours::limegreen),
          sustainKnob (p.apvts, "Sustain", juce::Colours::limegreen),
          releaseKnob (p.apvts, "Release", juce::Colours::limegreen),
          attack2Knob (p.apvts, "Attack2", juce::Colours::cyan),
          decay2Knob (p.apvts, "Decay2", juce::Colours::cyan),
          sustain2Knob (p.apvts, "Sustain2", juce::Colours::cyan),
          release2Knob (p.apvts, "Release2", juce::Colours::cyan)
    {
        // Initialize member FlexBox objects
        globalFb.flexDirection = juce::FlexBox::Direction::column;
        lfoRowsContainer.flexDirection = juce::FlexBox::Direction::column;

        adsrBox.flexDirection = juce::FlexBox::Direction::row;
        adsr2Box.flexDirection = juce::FlexBox::Direction::row;

        addAndMakeVisible(lfo1Controls);
        addAndMakeVisible(lfo2Controls);
        addAndMakeVisible(lfo3Controls);
        addAndMakeVisible(lfo4Controls);

        auto setupKnobAndLabel = [this] (fxme::FxmeKnob& knob)
        {
            addAndMakeVisible (knob);
            knob.slider.setLookAndFeel (&fxmeLookAndFeel);
        };

        setupKnobAndLabel(attackKnob);
        setupKnobAndLabel(decayKnob);
        setupKnobAndLabel(sustainKnob);
        setupKnobAndLabel(releaseKnob);
        setupKnobAndLabel(attack2Knob);
        setupKnobAndLabel(decay2Knob);
        setupKnobAndLabel(sustain2Knob);
        setupKnobAndLabel(release2Knob);
    }

    void resized() override
    {
        auto globalContentBounds = getLocalBounds().reduced(5);

        // Clear items before re-adding to prevent duplicates on resize
        globalFb.items.clear();
        lfoRowsContainer.items.clear();
        adsrBox.items.clear();
        adsr2Box.items.clear();

        // Populate ADSR FlexBoxes
        adsrBox.items.add (juce::FlexItem (attackKnob).withFlex (1.0));
        adsrBox.items.add (juce::FlexItem (decayKnob).withFlex (1.0));
        adsrBox.items.add (juce::FlexItem (sustainKnob).withFlex (1.0));
        adsrBox.items.add (juce::FlexItem (releaseKnob).withFlex (1.0));

        adsr2Box.items.add (juce::FlexItem (attack2Knob).withFlex (1.0));
        adsr2Box.items.add (juce::FlexItem (decay2Knob).withFlex (1.0));
        adsr2Box.items.add (juce::FlexItem (sustain2Knob).withFlex (1.0));
        adsr2Box.items.add (juce::FlexItem (release2Knob).withFlex (1.0));

        // Add LFO rows to the container
        lfoRowsContainer.items.add(juce::FlexItem(lfo1Controls).withFlex(1.0f));
        lfoRowsContainer.items.add(juce::FlexItem(lfo2Controls).withFlex(1.0f));
        lfoRowsContainer.items.add(juce::FlexItem(lfo3Controls).withFlex(1.0f));
        lfoRowsContainer.items.add(juce::FlexItem(lfo4Controls).withFlex(1.0f));

        // Add main containers to globalFb
        globalFb.items.add (juce::FlexItem(lfoRowsContainer).withFlex(3.0));
        globalFb.items.add (juce::FlexItem(adsrBox).withFlex(1.0).withMargin(juce::FlexItem::Margin(5.f, 0, 0, 0)));
        globalFb.items.add (juce::FlexItem(adsr2Box).withFlex(1.0));

        globalFb.performLayout(globalContentBounds);

        lfoRowsContainer.performLayout(globalFb.items[0].currentBounds.toNearestInt());
        adsrBox.performLayout(globalFb.items[1].currentBounds.toNearestInt());
        adsr2Box.performLayout(globalFb.items[2].currentBounds.toNearestInt());
    }

private:
    MapSynthAudioProcessor& audioProcessor;
    fxme::FxmeLookAndFeel fxmeLookAndFeel;

    LFOControlComponent lfo1Controls, lfo2Controls, lfo3Controls, lfo4Controls;

    fxme::FxmeKnob attackKnob, decayKnob, sustainKnob, releaseKnob;
    fxme::FxmeKnob attack2Knob, decay2Knob, sustain2Knob, release2Knob;

    // FlexBox layout objects (now members to ensure lifetime)
    juce::FlexBox globalFb;
    juce::FlexBox lfoRowsContainer;
    juce::FlexBox adsrBox;
    juce::FlexBox adsr2Box;
};

//==============================================================================
MapSynthAudioProcessorEditor::MapSynthAudioProcessorEditor (MapSynthAudioProcessor& p)
    : AudioProcessorEditor (&p), 
      audioProcessor (p),
      ellipseReaderComponent1(p, 1),
      ellipseReaderComponent2(p, 2),
      ellipseReaderComponent3(p, 3)
{
    globalControlsComponent = std::make_unique<GlobalControlsComponent>(p);

    mapDisplayComponentCPU = std::make_unique<MapDisplayComponent>(p);
    addAndMakeVisible(mapDisplayComponentCPU.get());

    mapDisplayComponentGL = std::make_unique<MapDisplayComponent_GL>(p);
    addAndMakeVisible(mapDisplayComponentGL.get());

    addAndMakeVisible(factoryImageLabel);
    factoryImageLabel.setText("Preset Image:", juce::dontSendNotification);
    factoryImageLabel.setJustificationType(juce::Justification::right);
    factoryImageLabel.attachToComponent(&factoryImageSelector, true);
    addAndMakeVisible(factoryImageSelector);

    if (auto* choiceParam = dynamic_cast<juce::AudioParameterChoice*>(audioProcessor.apvts.getParameter("FactoryImage")))
    {
        factoryImageSelector.addItemList(choiceParam->choices, 1);
    }

    factoryImageAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.apvts, "FactoryImage", factoryImageSelector);

    addAndMakeVisible (loadImageButton);
    loadImageButton.setButtonText ("Load...");
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
                if (audioProcessor.imageBuffer.setImage (file))
                {
                    // If user loads an image, set the selector to "Custom"
                    if (auto* param = audioProcessor.apvts.getParameter("FactoryImage"))
                        param->setValueNotifyingHost(0.0f);
                }
                else
                {
                    juce::AlertWindow::showMessageBoxAsync (juce::AlertWindow::WarningIcon, "Image Load Error", "Could not load the image file: " + file.getFileName());
                }
            }
        });
    };

    addAndMakeVisible(useOpenGLButton);
    useOpenGLButton.setButtonText("Use OpenGL");
    useOpenGLButton.onClick = [this]
    {
        audioProcessor.setUseOpenGL (useOpenGLButton.getToggleState());
    };

    addAndMakeVisible(masterVolumeSlider);
    masterVolumeSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    masterVolumeSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    masterVolumeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "Level", masterVolumeSlider);

    addAndMakeVisible(masterVolumeLabel);
    masterVolumeLabel.setText("Master Volume", juce::dontSendNotification);
    masterVolumeLabel.setJustificationType(juce::Justification::centredRight);
    masterVolumeLabel.attachToComponent(&masterVolumeSlider, true);

    addAndMakeVisible(readerTabs);
    readerTabs.addTab("Global / LFO", juce::Colours::transparentBlack, globalControlsComponent.get(), false);
    readerTabs.addTab("Reader 1", juce::Colours::transparentBlack, &ellipseReaderComponent1, false);
    readerTabs.addTab("Reader 2", juce::Colours::transparentBlack, &ellipseReaderComponent2, false);
    readerTabs.addTab("Reader 3", juce::Colours::transparentBlack, &ellipseReaderComponent3, false);

    audioProcessor.openGLStateBroadcaster.addChangeListener (this);
    updateRendererVisibility();

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setResizable(true, true);
    setSize (960, 480);
}

MapSynthAudioProcessorEditor::~MapSynthAudioProcessorEditor()
{
    audioProcessor.openGLStateBroadcaster.removeChangeListener (this);
}

void MapSynthAudioProcessorEditor::changeListenerCallback (juce::ChangeBroadcaster* source)
{
    if (source == &audioProcessor.openGLStateBroadcaster)
    {
        updateRendererVisibility();
    }
}

//==============================================================================
void MapSynthAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    auto diagonale = (getLocalBounds().getTopLeft() - getLocalBounds().getBottomRight()).toFloat();
    auto length = diagonale.getDistanceFromOrigin();
    auto perpendicular = diagonale.rotatedAboutOrigin (juce::degreesToRadians (270.0f)) / length;
    auto height = float (getWidth() * getHeight()) / length;
    auto bluegreengrey = juce::Colour::fromFloatRGBA (0.15f, 0.15f, 0.25f, 1.0f);
    juce::ColourGradient grad (bluegreengrey.darker().darker().darker(), perpendicular * height,
                           bluegreengrey, perpendicular * -height, false);
    g.setGradientFill(grad);
    g.fillAll();

}

void MapSynthAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto bounds = getLocalBounds();
    auto leftPanelArea = bounds.removeFromLeft(juce::jmin(500, bounds.getWidth() / 2));
    auto rightPanel = bounds;

    auto leftPanelPadded = leftPanelArea.reduced(5);
    auto volumeArea = leftPanelPadded.removeFromBottom(30);
    auto buttonArea = leftPanelPadded.removeFromBottom(30);
    readerTabs.setBounds(leftPanelPadded);

    masterVolumeSlider.setBounds(volumeArea);

    factoryImageSelector.setBounds(buttonArea.getX() + 80, buttonArea.getY() + 3, 120, 24);
    loadImageButton.setBounds(factoryImageSelector.getRight() + 10, buttonArea.getY() + 3, 80, 24);
    useOpenGLButton.setBounds(loadImageButton.getRight() + 10, buttonArea.getY() + 3, 100, 24);

    auto mapArea = rightPanel.reduced(5);

    mapDisplayComponentCPU->setBounds(mapArea);
    mapDisplayComponentGL->setBounds(mapArea);
}

void MapSynthAudioProcessorEditor::updateRendererVisibility()
{
    const bool useOpenGL = audioProcessor.getUseOpenGL();

    useOpenGLButton.setToggleState (useOpenGL, juce::dontSendNotification);
    mapDisplayComponentGL->setVisible (useOpenGL);
    mapDisplayComponentCPU->setVisible (!useOpenGL);
}
