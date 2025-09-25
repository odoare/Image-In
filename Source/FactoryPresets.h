/*
  ==============================================================================

    FactoryPresets.h - Manages factory presets for the MapSynth plugin.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

namespace FactoryPresets
{
    // Helper to hold preset information. Using juce::String for the name
    // makes it more flexible than a raw char pointer.
    struct Preset
    {
        juce::String name;
        const char* data;
    };

    // This function now dynamically builds the preset list from BinaryData
    static juce::Array<Preset> getAvailablePresets()
    {
        juce::Array<Preset> presets;

        for (int i = 0; i < BinaryData::namedResourceListSize; ++i)
        {
            juce::String filename(BinaryData::originalFilenames[i]);
            if (filename.endsWithIgnoreCase(".xml"))
            {
                // Create a user-friendly name from the filename (e.g., "DeepDive.xml" -> "Deep Dive")
                auto friendlyName = filename.upToLastOccurrenceOf(".", false, false)
                                            .replaceCharacter('_', ' ');

                int dataSize = 0; // Dummy variable to satisfy the function signature
                const char* resourceData = BinaryData::getNamedResource(BinaryData::namedResourceList[i], dataSize);

                presets.add (Preset { friendlyName, resourceData });
            }
        }
        return presets;
    }

} // namespace FactoryPresets
