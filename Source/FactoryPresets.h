/*
  ==============================================================================

    FactoryPresets.h
    Created: 20 Sep 2025 10:00:00am
    Author:  Gemini Code Assist

    This file contains factory presets for the MapSynth plugin.
    To generate a new preset:
    1. Run the plugin in a host and set the parameters as you like.
    2. Add a logging statement in your processor to print the state, e.g.:
       DBG(apvts.copyState().createXml()->toString());
    3. Copy the logged XML into a new string constant in this file.
    4. Add a new entry to the `factoryPresets` array.

  ==============================================================================
*/

#pragma once

namespace FactoryPresets
{
    // Helper to create a preset from a name and XML data
    struct Preset
    {
        const char* name;
        const char* data;
    };

    // Preset 1: "Pulsing Line"
    // A simple sound using the line reader, with its length modulated by LFO 1.
    const char* pulsingLineXml = R"xml(
<Parameters>
  <PARAM id="FactoryImage" value="1.0" />
  <PARAM id="LineCX" value="0.5" />
  <PARAM id="LineCY" value="0.5" />
  <PARAM id="LineLength" value="0.3" />
  <PARAM id="LineAngle" value="0.7853981852531433" />
  <PARAM id="LineVolume" value="1.0" />
  <PARAM id="LinePan" value="0.0" />
  <PARAM id="CX" value="0.5" />
  <PARAM id="CY" value="0.5" />
  <PARAM id="R" value="0.25" />
  <PARAM id="CircleVolume" value="0.0" />
  <PARAM id="CirclePan" value="0.0" />
  <PARAM id="LFOFreq" value="2.0" />
  <PARAM id="LFO1Sync" value="0.0" />
  <PARAM id="LFO1Wave" value="0.0" />
  <PARAM id="LFO1Phase" value="0.0" />
  <PARAM id="Attack" value="0.01" />
  <PARAM id="Decay" value="0.3" />
  <PARAM id="Sustain" value="0.8" />
  <PARAM id="Release" value="0.5" />
  <PARAM id="Mod_LineLength_Amount" value="0.7" />
  <PARAM id="Mod_LineLength_Select" value="0.0" />
  <PARAM id="Mod_LineVolume_Amount" value="1.0" />
  <PARAM id="Mod_LineVolume_Select" value="4.0" />
</Parameters>
)xml";

    // Preset 2: "Wobbly Circle"
    // A pad-like sound using the circle reader, with its position modulated by two LFOs.
    const char* wobblyCircleXml = R"xml(
<Parameters>
  <PARAM id="FactoryImage" value="2.0" />
  <PARAM id="LineVolume" value="0.0" />
  <PARAM id="CircleVolume" value="1.0" />
  <PARAM id="CX" value="0.5" />
  <PARAM id="CY" value="0.5" />
  <PARAM id="R" value="0.2" />
  <PARAM id="LFO2Freq" value="0.3" />
  <PARAM id="LFO3Freq" value="0.45" />
  <PARAM id="Mod_CircleCX_Amount" value="0.4" />
  <PARAM id="Mod_CircleCX_Select" value="1.0" />
  <PARAM id="Mod_CircleCY_Amount" value="0.4" />
  <PARAM id="Mod_CircleCY_Select" value="2.0" />
  <PARAM id="Attack" value="0.2" />
  <PARAM id="Sustain" value="1.0" />
  <PARAM id="Release" value="0.8" />
</Parameters>
)xml";

    // Array of all factory presets
    const Preset factoryPresets[] =
    {
        { "Pulsing Line",  pulsingLineXml },
        { "Wobbly Circle", wobblyCircleXml }
    };

    const int numFactoryPresets = sizeof(factoryPresets) / sizeof(Preset);

} // namespace FactoryPresets