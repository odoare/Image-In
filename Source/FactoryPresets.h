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
  <PARAM id="FactoryImage" value="1"/>
  <PARAM id="Ellipse1_CX" value="0.5"/>
  <PARAM id="Ellipse1_CY" value="0.5"/>
  <PARAM id="Ellipse1_R1" value="0.15"/>
  <PARAM id="Ellipse1_R2" value="0.001"/>
  <PARAM id="Ellipse1_Angle" value="0.785398185"/>
  <PARAM id="Ellipse1_Volume" value="1.0"/>
  <PARAM id="Ellipse1_Pan" value="0.0"/>
  <PARAM id="Ellipse2_Volume" value="0.0"/>
  <PARAM id="Ellipse3_Volume" value="0.0"/>
  <PARAM id="LFOFreq" value="2.0" />
  <PARAM id="LFO1Sync" value="0.0" />
  <PARAM id="LFO1Wave" value="0.0" />
  <PARAM id="LFO1Phase" value="0.0" />
  <PARAM id="Attack" value="0.01" />
  <PARAM id="Decay" value="0.3" />
  <PARAM id="Sustain" value="0.8" />
  <PARAM id="Release" value="0.5" />
  <PARAM id="Mod_Ellipse1_R1_Amount" value="0.7"/>
  <PARAM id="Mod_Ellipse1_R1_Select" value="0"/>
  <PARAM id="Mod_Ellipse1_Volume_Amount" value="1.0"/>
  <PARAM id="Mod_Ellipse1_Volume_Select" value="4"/>
</Parameters>
)xml";

    // Preset 2: "Wobbly Circle"
    // A pad-like sound using the circle reader, with its position modulated by two LFOs.
    const char* wobblyCircleXml = R"xml(
<Parameters>
  <PARAM id="FactoryImage" value="2"/>
  <PARAM id="Ellipse1_Volume" value="1.0"/>
  <PARAM id="Ellipse1_CX" value="0.5"/>
  <PARAM id="Ellipse1_CY" value="0.5"/>
  <PARAM id="Ellipse1_R1" value="0.2"/>
  <PARAM id="Ellipse1_R2" value="0.2"/>
  <PARAM id="Ellipse2_Volume" value="0.0"/>
  <PARAM id="Ellipse3_Volume" value="0.0"/>
  <PARAM id="LFO2Freq" value="0.3" />
  <PARAM id="LFO3Freq" value="0.45" />
  <PARAM id="Mod_Ellipse1_CX_Amount" value="0.4"/>
  <PARAM id="Mod_Ellipse1_CX_Select" value="1"/>
  <PARAM id="Mod_Ellipse1_CY_Amount" value="0.4"/>
  <PARAM id="Mod_Ellipse1_CY_Select" value="2"/>
  <PARAM id="Attack" value="0.2" />
  <PARAM id="Sustain" value="1.0" />
  <PARAM id="Release" value="0.8" />
</Parameters>
)xml";

    // Preset 3: "Rotating Ellipse"
    // An ellipse reader with its angle modulated by an LFO.
    const char* rotatingEllipseXml = R"xml(
<Parameters>
  <PARAM id="FactoryImage" value="3"/>
  <PARAM id="Ellipse1_Volume" value="1.0"/>
  <PARAM id="Ellipse1_CX" value="0.5"/>
  <PARAM id="Ellipse1_CY" value="0.5"/>
  <PARAM id="Ellipse1_R1" value="0.4"/>
  <PARAM id="Ellipse1_R2" value="0.15"/>
  <PARAM id="Ellipse1_Angle" value="0.0"/>
  <PARAM id="Ellipse2_Volume" value="0.0"/>
  <PARAM id="Ellipse3_Volume" value="0.0"/>
  <PARAM id="LFO1Freq" value="0.2" />
  <PARAM id="Mod_Ellipse1_Angle_Amount" value="1.0"/>
  <PARAM id="Mod_Ellipse1_Angle_Select" value="0"/>
  <PARAM id="Attack" value="0.1" />
  <PARAM id="Sustain" value="1.0" />
  <PARAM id="Release" value="0.6" />
</Parameters>
)xml";

    // Array of all factory presets
    const Preset factoryPresets[] =
    {
        { "Pulsing Line",  pulsingLineXml },
        { "Wobbly Circle", wobblyCircleXml },
        { "Rotating Ellipse", rotatingEllipseXml }
    };

    const int numFactoryPresets = sizeof(factoryPresets) / sizeof(Preset);

} // namespace FactoryPresets