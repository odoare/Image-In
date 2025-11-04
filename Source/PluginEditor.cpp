/*
  ==============================================================================

    PluginEditor.h
    Created: 16 Sep 2025 10:00:00am
    Author:  Olivier Doaré

    Part of Image-In project

    Licenced under the LGPLv3

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "ParameterStructs.h"
#include "colours.h"
#include "LFOControlComponent.h"


namespace
{
    /** A helper component to manage displaying the plugin editor in a fullscreen window. */
    class FullscreenComponent  : public juce::Component
    {
    public:
        FullscreenComponent (juce::Component* content)
            : contentToShow (content)
        {
            originalParent = content->getParentComponent();
            originalBounds = content->getBounds();

            addAndMakeVisible (content);

            setOpaque (true);
            addToDesktop (juce::ComponentPeer::windowIsTemporary);
            setAlwaysOnTop (true);
        }

        ~FullscreenComponent() override
        {
            // When this component is destroyed, it puts the editor back where it was.
            if (originalParent != nullptr)
                originalParent->addAndMakeVisible (contentToShow);

            contentToShow->setBounds (originalBounds);
        }

        void resized() override
        {
            if (contentToShow != nullptr)
                contentToShow->setBounds (getLocalBounds());
        }

        void paint (juce::Graphics& g) override { g.fillAll (juce::Colours::black); }
        void mouseDown (const juce::MouseEvent&) override { delete this; }

    private:
        juce::Component* contentToShow;
        juce::Component* originalParent;
        juce::Rectangle<int> originalBounds;
    };
};

class LFOsComponent : public juce::Component
{
public:
    LFOsComponent(MapSynthAudioProcessor& p)
        : audioProcessor(p),
          lfo1Controls(p, 1),
          lfo2Controls(p, 2),
          lfo3Controls(p, 3),
          lfo4Controls(p, 4)
    {
        lfoRow1.flexDirection = juce::FlexBox::Direction::row;
        lfoRow2.flexDirection = juce::FlexBox::Direction::row;
        lfoCol.flexDirection = juce::FlexBox::Direction::column;

        addAndMakeVisible(lfo1Controls);
        addAndMakeVisible(lfo2Controls);
        addAndMakeVisible(lfo3Controls);
        addAndMakeVisible(lfo4Controls);
    }

    void resized() override
    {
        auto bounds = getLocalBounds().reduced(5);
        lfoRow1.items.clear();
        lfoRow2.items.clear();
        lfoCol.items.clear();

        const auto margin = juce::FlexItem::Margin(10.f);
        lfoRow1.items.add(juce::FlexItem(lfo1Controls).withFlex(1.0f).withMargin(margin));
        lfoRow1.items.add(juce::FlexItem(lfo2Controls).withFlex(1.0f).withMargin(margin));
        lfoRow2.items.add(juce::FlexItem(lfo3Controls).withFlex(1.0f).withMargin(margin));
        lfoRow2.items.add(juce::FlexItem(lfo4Controls).withFlex(1.0f).withMargin(margin));

        lfoCol.items.add(juce::FlexItem(lfoRow1).withFlex(1.0f));
        lfoCol.items.add(juce::FlexItem(lfoRow2).withFlex(1.0f));

        lfoCol.performLayout(bounds);
    }

private:
    MapSynthAudioProcessor& audioProcessor;
    LFOControlComponent lfo1Controls, lfo2Controls, lfo3Controls, lfo4Controls;
    juce::FlexBox lfoRow1, lfoRow2, lfoCol;
};

class ADSRsComponent : public juce::Component
{
public:
    ADSRsComponent(MapSynthAudioProcessor& p)
        : audioProcessor(p),
          attackKnob (p.apvts, "Attack", "Attack", ADSRCONTROLCOLOUR),
          decayKnob (p.apvts, "Decay", "Decay", ADSRCONTROLCOLOUR),
          sustainKnob (p.apvts, "Sustain", "Sustain", ADSRCONTROLCOLOUR),
          releaseKnob (p.apvts, "Release", "Release", ADSRCONTROLCOLOUR),
          attack2Knob (p.apvts, "Attack2", "Attack2", ADSRCONTROLCOLOUR),
          decay2Knob (p.apvts, "Decay2", "Decay2", ADSRCONTROLCOLOUR),
          sustain2Knob (p.apvts, "Sustain2", "Sustain2", ADSRCONTROLCOLOUR),
          release2Knob (p.apvts, "Release2", "Release2", ADSRCONTROLCOLOUR),
          attack3Knob (p.apvts, "Attack3", "Attack3", ADSRCONTROLCOLOUR),
          decay3Knob (p.apvts, "Decay3", "Decay3", ADSRCONTROLCOLOUR),
          sustain3Knob (p.apvts, "Sustain3", "Sustain3", ADSRCONTROLCOLOUR),
          release3Knob (p.apvts, "Release3", "Release3", ADSRCONTROLCOLOUR)
    {
        mainAdsrContainer.flexDirection = juce::FlexBox::Direction::column;
        adsrBox.flexDirection = juce::FlexBox::Direction::row;
        adsr2Box.flexDirection = juce::FlexBox::Direction::row;
        adsr3Box.flexDirection = juce::FlexBox::Direction::row;

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
        setupKnobAndLabel(attack3Knob);
        setupKnobAndLabel(decay3Knob);
        setupKnobAndLabel(sustain3Knob);
        setupKnobAndLabel(release3Knob);
    }

    void resized() override
    {
        auto bounds = getLocalBounds().reduced(5);
        mainAdsrContainer.items.clear();
        adsrBox.items.clear();
        adsr2Box.items.clear();
        adsr3Box.items.clear();

        adsrBox.items.add (juce::FlexItem (attackKnob).withFlex (1.0));
        adsrBox.items.add (juce::FlexItem (decayKnob).withFlex (1.0));
        adsrBox.items.add (juce::FlexItem (sustainKnob).withFlex (1.0));
        adsrBox.items.add (juce::FlexItem (releaseKnob).withFlex (1.0));

        adsr2Box.items.add (juce::FlexItem (attack2Knob).withFlex (1.0));
        adsr2Box.items.add (juce::FlexItem (decay2Knob).withFlex (1.0));
        adsr2Box.items.add (juce::FlexItem (sustain2Knob).withFlex (1.0));
        adsr2Box.items.add (juce::FlexItem (release2Knob).withFlex (1.0));

        adsr3Box.items.add (juce::FlexItem (attack3Knob).withFlex (1.0));
        adsr3Box.items.add (juce::FlexItem (decay3Knob).withFlex (1.0));
        adsr3Box.items.add (juce::FlexItem (sustain3Knob).withFlex (1.0));
        adsr3Box.items.add (juce::FlexItem (release3Knob).withFlex (1.0));

        mainAdsrContainer.items.add(juce::FlexItem(adsrBox).withFlex(1.0).withMargin(juce::FlexItem::Margin(5.f, 0, 0, 0)));
        mainAdsrContainer.items.add(juce::FlexItem(adsr2Box).withFlex(1.0));
        mainAdsrContainer.items.add(juce::FlexItem(adsr3Box).withFlex(1.0));

        mainAdsrContainer.performLayout(bounds);

        adsrBox.performLayout(mainAdsrContainer.items[0].currentBounds.toNearestInt());
        adsr2Box.performLayout(mainAdsrContainer.items[1].currentBounds.toNearestInt());
        adsr3Box.performLayout(mainAdsrContainer.items[2].currentBounds.toNearestInt());
    }

private:
    MapSynthAudioProcessor& audioProcessor;
    fxme::FxmeLookAndFeel fxmeLookAndFeel;

    fxme::FxmeKnob attackKnob, decayKnob, sustainKnob, releaseKnob;
    fxme::FxmeKnob attack2Knob, decay2Knob, sustain2Knob, release2Knob;
    fxme::FxmeKnob attack3Knob, decay3Knob, sustain3Knob, release3Knob;

    juce::FlexBox mainAdsrContainer;
    juce::FlexBox adsrBox;
    juce::FlexBox adsr2Box;
    juce::FlexBox adsr3Box;
};

//==============================================================================
MapSynthAudioProcessorEditor::MapSynthAudioProcessorEditor (MapSynthAudioProcessor& p)
    : AudioProcessorEditor (&p), 
      audioProcessor (p),
      ellipseReaderComponent1(p, 1),
      ellipseReaderComponent2(p, 2),
      ellipseReaderComponent3(p, 3)
{
    lfosComponent = std::make_unique<LFOsComponent>(p);
    adsrsComponent = std::make_unique<ADSRsComponent>(p);

    mapDisplayComponentCPU = std::make_unique<MapDisplayComponent>(p);
    mapDisplayComponentCPU->setEditor(this);
    addAndMakeVisible(mapDisplayComponentCPU.get());
    mapDisplayComponentCPU->addButtons(togglePanelButton, fullscreenButton);
    // Give buttons IDs so they can be found by their new parents
    togglePanelButton.setComponentID("togglePanelButton");
    fullscreenButton.setComponentID("fullscreenButton");

    addAndMakeVisible(factoryImageLabel);
    factoryImageLabel.setText("Image:", juce::dontSendNotification);
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

    addAndMakeVisible(importStateButton);
    importStateButton.setButtonText("Import");
    importStateButton.onClick = [this]
    {
        fileChooser = std::make_unique<juce::FileChooser> ("Select an XML state file to load...",
                                                           juce::File{},
                                                           "*.xml");

        auto chooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

        fileChooser->launchAsync (chooserFlags, [this] (const juce::FileChooser& fc)
        {
            auto file = fc.getResult();
            if (file == juce::File{})
                return;

            std::unique_ptr<juce::XmlElement> xmlState (juce::XmlDocument::parse(file));

            if (xmlState != nullptr && xmlState->hasTagName(audioProcessor.apvts.state.getType()))
            {
                // Use replaceState for loading parameters from an XML file.
                audioProcessor.apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
            }
            else
            {
                juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon, "Load Error", "Selected file is not a valid preset for this plugin.");
            }
        });
    };

    addAndMakeVisible(exportStateButton);
    exportStateButton.setButtonText("Export");
    exportStateButton.onClick = [this]
    {
        fileChooser = std::make_unique<juce::FileChooser> ("Save plugin state as XML...",
                                                           juce::File{},
                                                           "*.xml");

        auto chooserFlags = juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles;

        fileChooser->launchAsync (chooserFlags, [this] (const juce::FileChooser& fc)
        {
            auto file = fc.getResult();
            if (file == juce::File{})
                return;

            // Get the state directly from the APVTS and convert to XML.
            auto state = audioProcessor.apvts.copyState();
            std::unique_ptr<juce::XmlElement> xml (state.createXml());
            file.replaceWithText (xml->toString());
        });
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
    readerTabs.addTab("Reader 1", juce::Colours::transparentBlack, &ellipseReaderComponent1, false);
    readerTabs.addTab("Reader 2", juce::Colours::transparentBlack, &ellipseReaderComponent2, false);
    readerTabs.addTab("Reader 3", juce::Colours::transparentBlack, &ellipseReaderComponent3, false);
    readerTabs.addTab("LFOs", juce::Colours::transparentBlack, lfosComponent.get(), false);
    readerTabs.addTab("ADSRs", juce::Colours::transparentBlack, adsrsComponent.get(), false);

    togglePanelButton.setButtonText("<");
    togglePanelButton.onClick = [this]
    {
        if (auto* param = audioProcessor.apvts.getParameter("ShowPanel"))
        {
            // Toggling the parameter will automatically trigger parameterChanged, which calls resized().
            param->setValueNotifyingHost(param->getValue() > 0.5f ? 0.0f : 1.0f);
        }
    };

    fullscreenButton.onClick = [this] {
        if (fullscreenComponent == nullptr)
        {
            // Enter fullscreen
            fullscreenComponent = new FullscreenComponent (this);
            fullscreenComponent->setBounds (juce::Desktop::getInstance().getDisplays()
                                                .getPrimaryDisplay()->userArea);
            fullscreenComponent->setVisible (true);
            fullscreenButton.setToggleState (true, juce::dontSendNotification);
        }
        else
        {
            // Exit fullscreen
            // The FullscreenComponent will delete itself on mouse-click,
            // but we also delete it here if the button is clicked again.
            delete fullscreenComponent;
            fullscreenComponent = nullptr;
            fullscreenButton.setToggleState (false, juce::dontSendNotification);
        }
    };


    addAndMakeVisible(meterL);
    addAndMakeVisible(meterR);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setResizable(true, true);
    setSize (1024, 512);

    audioProcessor.apvts.addParameterListener("ShowPanel", this);

}

MapSynthAudioProcessorEditor::~MapSynthAudioProcessorEditor()
{
    audioProcessor.apvts.removeParameterListener("ShowPanel", this);
}

//==============================================================================
void MapSynthAudioProcessorEditor::parameterChanged(const juce::String& parameterID, float /*newValue*/)
{
    if (parameterID == "ShowPanel")
    {
        // The parameter has changed, so we need to update the layout.
        resized();
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
    auto totalBounds = getLocalBounds();
    juce::Rectangle<int> leftPanelArea;
    juce::Rectangle<int> rightPanel;

    bool isPanelVisible = audioProcessor.apvts.getRawParameterValue("ShowPanel")->load() > 0.5f;

    if (isPanelVisible)
    {
        leftPanelArea = totalBounds.removeFromLeft(juce::jmin(500, getWidth() / 2));
        rightPanel = totalBounds;

        meterL.setVisible(true);
        meterR.setVisible(true);
        masterVolumeSlider.setVisible(true);
        masterVolumeLabel.setVisible(true);
        factoryImageLabel.setVisible(true);
        factoryImageSelector.setVisible(true); 
        loadImageButton.setVisible(true);        
        importStateButton.setVisible(true);
        exportStateButton.setVisible(true);
        readerTabs.setVisible(true);

        auto leftPanelPadded = leftPanelArea.reduced(5);
        auto buttonArea = leftPanelPadded.removeFromTop(30);
        auto fadeArea = leftPanelPadded.removeFromTop(20);
        auto meterArea = leftPanelPadded.removeFromBottom(15);
        meterL.setBounds(meterArea);
        meterArea = leftPanelPadded.removeFromBottom(15);
        meterR.setBounds(meterArea);
        auto volumeArea = leftPanelPadded.removeFromBottom(30);

        readerTabs.setBounds(leftPanelPadded);
        masterVolumeSlider.setBounds(volumeArea);

        factoryImageSelector.setBounds(buttonArea.getX() + 60, buttonArea.getY() + 3, 120, 24);
        loadImageButton.setBounds(factoryImageSelector.getRight() + 10, buttonArea.getY() + 3, 80, 24);
        
        importStateButton.setBounds(loadImageButton.getRight() + 10, buttonArea.getY() + 3, 60, 24);
        exportStateButton.setBounds(importStateButton.getRight() + 5, buttonArea.getY() + 3, 60, 24);

        togglePanelButton.setButtonText("<");    

    }
    else
    {
        rightPanel = totalBounds;
        meterL.setVisible(false);
        meterR.setVisible(false);
        masterVolumeSlider.setVisible(false);
        masterVolumeLabel.setVisible(false);
        factoryImageLabel.setVisible(false);
        factoryImageSelector.setVisible(false); 
        loadImageButton.setVisible(false);        
        importStateButton.setVisible(false);
        exportStateButton.setVisible(false);
        readerTabs.setVisible(false);
        togglePanelButton.setButtonText(">");
    }

    auto mapArea = rightPanel.reduced(5);
    mapDisplayComponentCPU->setBounds(mapArea);
}
