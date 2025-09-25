/*
  ==============================================================================

    FactoryPresets.h

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

    // Preset 1: "Deep dive"
    const char* deepDiveXml = R"xml(
<?xml version="1.0" encoding="UTF-8"?>
<Parameters>
  <PARAM id="FactoryImage" value="11.0"/>
  <PARAM id="Ellipse1_CX" value="0.5"/>
  <PARAM id="Ellipse1_CY" value="0.5"/>
  <PARAM id="Ellipse1_R1" value="0.05999999865889549"/>
  <PARAM id="Ellipse1_R2" value="0.3299999833106995"/>
  <PARAM id="Ellipse1_Angle" value="4.039999961853027"/>
  <PARAM id="Ellipse1_Volume" value="0.7999999523162842"/>
  <PARAM id="Ellipse1_Pan" value="0.25"/>
  <PARAM id="Ellipse2_Volume" value="0.8100000023841858"/>
  <PARAM id="Ellipse3_Volume" value="0.7899999618530273"/>
  <PARAM id="LFOFreq" value="0.1400000005960464"/>
  <PARAM id="LFO1Sync" value="0.0"/>
  <PARAM id="LFO1Wave" value="0.0"/>
  <PARAM id="LFO1Phase" value="0.0"/>
  <PARAM id="Attack" value="1.209999918937683"/>
  <PARAM id="Decay" value="0.3"/>
  <PARAM id="Sustain" value="0.8"/>
  <PARAM id="Release" value="2.399999856948853"/>
  <PARAM id="Mod_Ellipse1_R1_Amount" value="1.0"/>
  <PARAM id="Mod_Ellipse1_R1_Select" value="0.0"/>
  <PARAM id="Mod_Ellipse1_Volume_Amount" value="1.0"/>
  <PARAM id="Mod_Ellipse1_Volume_Select" value="4"/>
  <PARAM id="Attack2" value="2.059999942779541"/>
  <PARAM id="Attack3" value="0.8899999856948853"/>
  <PARAM id="Decay2"/>
  <PARAM id="Decay3"/>
  <PARAM id="Ellipse1_FilterFreq" value="590.0"/>
  <PARAM id="Ellipse1_FilterQuality"/>
  <PARAM id="Ellipse1_FilterType"/>
  <PARAM id="Ellipse2_Angle"/>
  <PARAM id="Ellipse2_CX" value="0.5"/>
  <PARAM id="Ellipse2_CY" value="0.2599999904632568"/>
  <PARAM id="Ellipse2_FilterFreq" value="842.0"/>
  <PARAM id="Ellipse2_FilterQuality"/>
  <PARAM id="Ellipse2_FilterType"/>
  <PARAM id="Ellipse2_Pan" value="-0.2200000286102295"/>
  <PARAM id="Ellipse2_R1"/>
  <PARAM id="Ellipse2_R2"/>
  <PARAM id="Ellipse3_Angle" value="5.730000019073486"/>
  <PARAM id="Ellipse3_CX" value="0.7699999809265137"/>
  <PARAM id="Ellipse3_CY" value="0.6499999761581421"/>
  <PARAM id="Ellipse3_FilterFreq" value="3824.0"/>
  <PARAM id="Ellipse3_FilterQuality"/>
  <PARAM id="Ellipse3_FilterType"/> currentProgram="3" useOpenGL="0"
  <PARAM id="Ellipse3_Pan" value="0.0"/>
  <PARAM id="Ellipse3_R1" value="0.239999994635582"/>
  <PARAM id="Ellipse3_R2"/>
  <PARAM id="LFO1Rate"/>
  <PARAM id="LFO2Freq"/>
  <PARAM id="LFO2Phase"/>
  <PARAM id="LFO2Rate" value="5.0"/>
  <PARAM id="LFO2Sync" value="1.0"/>
  <PARAM id="LFO2Wave" value="4.0"/>
  <PARAM id="LFO3Freq" value="6.610000133514404"/>
  <PARAM id="LFO3Phase"/>
  <PARAM id="LFO3Rate"/>
  <PARAM id="LFO3Sync"/>
  <PARAM id="LFO3Wave"/>
  <PARAM id="LFO4Freq"/>
  <PARAM id="LFO4Phase"/>
  <PARAM id="LFO4Rate"/>
  <PARAM id="LFO4Sync"/>
  <PARAM id="LFO4Wave"/>
  <PARAM id="Level" value="-4.970001220703125"/>
  <PARAM id="Mod_Ellipse1_Angle_Amount"/>
  <PARAM id="Mod_Ellipse1_Angle_Select"/>
  <PARAM id="Mod_Ellipse1_CX_Amount"/>
  <PARAM id="Mod_Ellipse1_CX_Select"/>
  <PARAM id="Mod_Ellipse1_CY_Amount"/>
  <PARAM id="Mod_Ellipse1_CY_Select"/>
  <PARAM id="Mod_Ellipse1_FilterFreq_Amount"/>
  <PARAM id="Mod_Ellipse1_FilterFreq_Select"/>
  <PARAM id="Mod_Ellipse1_FilterQuality_Amount"/>
  <PARAM id="Mod_Ellipse1_FilterQuality_Select"/>
  <PARAM id="Mod_Ellipse1_Freq_Amount"/>
  <PARAM id="Mod_Ellipse1_Freq_Select"/>
  <PARAM id="Mod_Ellipse1_Pan_Amount"/>
  <PARAM id="Mod_Ellipse1_Pan_Select"/>
  <PARAM id="Mod_Ellipse1_R2_Amount" value="-0.6200000047683716"/>
  <PARAM id="Mod_Ellipse1_R2_Select" value="1.0"/>
  <PARAM id="Mod_Ellipse2_Angle_Amount"/>
  <PARAM id="Mod_Ellipse2_Angle_Select"/>
  <PARAM id="Mod_Ellipse2_CX_Amount"/>
  <PARAM id="Mod_Ellipse2_CX_Select"/>
  <PARAM id="Mod_Ellipse2_CY_Amount"/>
  <PARAM id="Mod_Ellipse2_CY_Select"/>
  <PARAM id="Mod_Ellipse2_FilterFreq_Amount"/>
  <PARAM id="Mod_Ellipse2_FilterFreq_Select"/>
  <PARAM id="Mod_Ellipse2_FilterQuality_Amount"/>
  <PARAM id="Mod_Ellipse2_FilterQuality_Select"/>
  <PARAM id="Mod_Ellipse2_Freq_Amount" value="0.02999997138977051"/>
  <PARAM id="Mod_Ellipse2_Freq_Select" value="1.0"/>
  <PARAM id="Mod_Ellipse2_Pan_Amount"/>
  <PARAM id="Mod_Ellipse2_Pan_Select"/>
  <PARAM id="Mod_Ellipse2_R1_Amount" value="0.309999942779541"/>
  <PARAM id="Mod_Ellipse2_R1_Select"/>
  <PARAM id="Mod_Ellipse2_R2_Amount" value="0.2699999809265137"/>
  <PARAM id="Mod_Ellipse2_R2_Select"/>
  <PARAM id="Mod_Ellipse2_Volume_Amount"/>
  <PARAM id="Mod_Ellipse2_Volume_Select"/>
  <PARAM id="Mod_Ellipse3_Angle_Amount"/>
  <PARAM id="Mod_Ellipse3_Angle_Select"/>
  <PARAM id="Mod_Ellipse3_CX_Amount"/>
  <PARAM id="Mod_Ellipse3_CX_Select"/>
  <PARAM id="Mod_Ellipse3_CY_Amount"/>
  <PARAM id="Mod_Ellipse3_CY_Select"/>
  <PARAM id="Mod_Ellipse3_FilterFreq_Amount"/>
  <PARAM id="Mod_Ellipse3_FilterFreq_Select"/>
  <PARAM id="Mod_Ellipse3_FilterQuality_Amount"/>
  <PARAM id="Mod_Ellipse3_FilterQuality_Select"/>
  <PARAM id="Mod_Ellipse3_Freq_Amount" value="0.009999990463256836"/>
  <PARAM id="Mod_Ellipse3_Freq_Select" value="14.0"/>
  <PARAM id="Mod_Ellipse3_Pan_Amount" value="0.5"/>
  <PARAM id="Mod_Ellipse3_Pan_Select"/>
  <PARAM id="Mod_Ellipse3_R1_Amount"/>
  <PARAM id="Mod_Ellipse3_R1_Select"/>
  <PARAM id="Mod_Ellipse3_R2_Amount"/>
  <PARAM id="Mod_Ellipse3_R2_Select"/>
  <PARAM id="Mod_Ellipse3_Volume_Amount"/>
  <PARAM id="Mod_Ellipse3_Volume_Select"/>
  <PARAM id="Release2" value="2.609999895095825"/>
  <PARAM id="Release3"/>
  <PARAM id="Sustain2"/>
  <PARAM id="Sustain3"/>
</Parameters>
)xml";

    // Preset 2: "Hesitant"
    const char* hesitantXml = R"xml(
<Parameters>
  <PARAM id="FactoryImage" value="11.0"/>
  <PARAM id="Ellipse1_CX" value="0.5"/>
  <PARAM id="Ellipse1_CY" value="0.5"/>
  <PARAM id="Ellipse1_R1" value="0.05999999865889549"/>
  <PARAM id="Ellipse1_R2" value="0.3299999833106995"/>
  <PARAM id="Ellipse1_Angle" value="4.039999961853027"/>
  <PARAM id="Ellipse1_Volume" value="0.0"/>
  <PARAM id="Ellipse1_Pan" value="0.25"/>
  <PARAM id="Ellipse2_Volume" value="0.0"/>
  <PARAM id="Ellipse3_Volume" value="0.7899999618530273"/>
  <PARAM id="LFOFreq" value="0.1400000005960464"/>
  <PARAM id="LFO1Sync" value="0.0"/>
  <PARAM id="LFO1Wave" value="0.0"/>
  <PARAM id="LFO1Phase" value="0.0"/>
  <PARAM id="Attack" value="1.209999918937683"/>
  <PARAM id="Decay" value="0.3"/>
  <PARAM id="Sustain" value="0.8"/>
  <PARAM id="Release" value="2.399999856948853"/>
  <PARAM id="Mod_Ellipse1_R1_Amount" value="1.0"/>
  <PARAM id="Mod_Ellipse1_R1_Select" value="0.0"/>
  <PARAM id="Mod_Ellipse1_Volume_Amount" value="1.0"/>
  <PARAM id="Mod_Ellipse1_Volume_Select" value="4"/>
  <PARAM id="Attack2" value="2.059999942779541"/>
  <PARAM id="Attack3" value="0.8899999856948853"/>
  <PARAM id="Decay2"/>
  <PARAM id="Decay3"/>
  <PARAM id="Ellipse1_FilterFreq" value="2711.0"/>
  <PARAM id="Ellipse1_FilterQuality"/>
  <PARAM id="Ellipse1_FilterType"/>
  <PARAM id="Ellipse2_Angle"/>
  <PARAM id="Ellipse2_CX" value="0.5"/>
  <PARAM id="Ellipse2_CY" value="0.2599999904632568"/>
  <PARAM id="Ellipse2_FilterFreq" value="1128.0"/>
  <PARAM id="Ellipse2_FilterQuality"/>
  <PARAM id="Ellipse2_FilterType"/>
  <PARAM id="Ellipse2_Pan" value="-0.2200000286102295"/>
  <PARAM id="Ellipse2_R1"/>
  <PARAM id="Ellipse2_R2"/>
  <PARAM id="Ellipse3_Angle" value="5.730000019073486"/>
  <PARAM id="Ellipse3_CX" value="0.5"/>
  <PARAM id="Ellipse3_CY" value="0.5"/>
  <PARAM id="Ellipse3_FilterFreq" value="20000.0"/>
  <PARAM id="Ellipse3_FilterQuality"/>
  <PARAM id="Ellipse3_FilterType"/>
  <PARAM id="Ellipse3_Pan" value="0.0"/>
  <PARAM id="Ellipse3_R1" value="0.199999988079071"/>
  <PARAM id="Ellipse3_R2" value="0.199999988079071"/>
  <PARAM id="LFO1Rate"/>
  <PARAM id="LFO2Freq"/>
  <PARAM id="LFO2Phase"/>
  <PARAM id="LFO2Rate" value="5.0"/>
  <PARAM id="LFO2Sync" value="0.0"/>
  <PARAM id="LFO2Wave" value="0.0"/>
  <PARAM id="LFO3Freq" value="6.610000133514404"/>
  <PARAM id="LFO3Phase"/>
  <PARAM id="LFO3Rate"/>
  <PARAM id="LFO3Sync"/>
  <PARAM id="LFO3Wave"/>
  <PARAM id="LFO4Freq"/>
  <PARAM id="LFO4Phase"/>
  <PARAM id="LFO4Rate"/>
  <PARAM id="LFO4Sync"/>
  <PARAM id="LFO4Wave"/>
  <PARAM id="Level" value="0.0"/>
  <PARAM id="Mod_Ellipse1_Angle_Amount"/>
  <PARAM id="Mod_Ellipse1_Angle_Select"/>
  <PARAM id="Mod_Ellipse1_CX_Amount"/>
  <PARAM id="Mod_Ellipse1_CX_Select"/>
  <PARAM id="Mod_Ellipse1_CY_Amount"/>
  <PARAM id="Mod_Ellipse1_CY_Select"/>
  <PARAM id="Mod_Ellipse1_FilterFreq_Amount"/>
  <PARAM id="Mod_Ellipse1_FilterFreq_Select"/>
  <PARAM id="Mod_Ellipse1_FilterQuality_Amount"/>
  <PARAM id="Mod_Ellipse1_FilterQuality_Select"/>
  <PARAM id="Mod_Ellipse1_Freq_Amount"/>
  <PARAM id="Mod_Ellipse1_Freq_Select"/>
  <PARAM id="Mod_Ellipse1_Pan_Amount"/>
  <PARAM id="Mod_Ellipse1_Pan_Select"/>
  <PARAM id="Mod_Ellipse1_R2_Amount" value="-0.6200000047683716"/>
  <PARAM id="Mod_Ellipse1_R2_Select" value="1.0"/>
  <PARAM id="Mod_Ellipse2_Angle_Amount"/>
  <PARAM id="Mod_Ellipse2_Angle_Select"/>
  <PARAM id="Mod_Ellipse2_CX_Amount"/>
  <PARAM id="Mod_Ellipse2_CX_Select"/>
  <PARAM id="Mod_Ellipse2_CY_Amount"/>
  <PARAM id="Mod_Ellipse2_CY_Select"/>
  <PARAM id="Mod_Ellipse2_FilterFreq_Amount"/>
  <PARAM id="Mod_Ellipse2_FilterFreq_Select"/>
  <PARAM id="Mod_Ellipse2_FilterQuality_Amount"/>
  <PARAM id="Mod_Ellipse2_FilterQuality_Select"/>
  <PARAM id="Mod_Ellipse2_Freq_Amount" value="0.02999997138977051"/>
  <PARAM id="Mod_Ellipse2_Freq_Select" value="1.0"/>
  <PARAM id="Mod_Ellipse2_Pan_Amount"/>
  <PARAM id="Mod_Ellipse2_Pan_Select"/>
  <PARAM id="Mod_Ellipse2_R1_Amount" value="0.309999942779541"/>
  <PARAM id="Mod_Ellipse2_R1_Select"/>
  <PARAM id="Mod_Ellipse2_R2_Amount" value="0.2699999809265137"/>
  <PARAM id="Mod_Ellipse2_R2_Select"/>
  <PARAM id="Mod_Ellipse2_Volume_Amount"/>
  <PARAM id="Mod_Ellipse2_Volume_Select"/>
  <PARAM id="Mod_Ellipse3_Angle_Amount"/>
  <PARAM id="Mod_Ellipse3_Angle_Select"/>
  <PARAM id="Mod_Ellipse3_CX_Amount" value="0.02999997138977051"/>
  <PARAM id="Mod_Ellipse3_CX_Select" value="1.0"/>
  <PARAM id="Mod_Ellipse3_CY_Amount" value="0.02999997138977051"/>
  <PARAM id="Mod_Ellipse3_CY_Select" value="0.0"/>
  <PARAM id="Mod_Ellipse3_FilterFreq_Amount"/>
  <PARAM id="Mod_Ellipse3_FilterFreq_Select"/>
  <PARAM id="Mod_Ellipse3_FilterQuality_Amount"/>
  <PARAM id="Mod_Ellipse3_FilterQuality_Select"/>
  <PARAM id="Mod_Ellipse3_Freq_Amount" value="0.009999990463256836"/>
  <PARAM id="Mod_Ellipse3_Freq_Select" value="14.0"/>
  <PARAM id="Mod_Ellipse3_Pan_Amount" value="0.5"/>
  <PARAM id="Mod_Ellipse3_Pan_Select"/>
  <PARAM id="Mod_Ellipse3_R1_Amount" value="0.2899999618530273"/>
  <PARAM id="Mod_Ellipse3_R1_Select" value="6.0"/>
  <PARAM id="Mod_Ellipse3_R2_Amount" value="0.2899999618530273"/>
  <PARAM id="Mod_Ellipse3_R2_Select" value="6.0"/>
  <PARAM id="Mod_Ellipse3_Volume_Amount"/>
  <PARAM id="Mod_Ellipse3_Volume_Select"/>
  <PARAM id="Release2" value="2.609999895095825"/>
  <PARAM id="Release3"/>
  <PARAM id="Sustain2"/>
  <PARAM id="Sustain3"/>
</Parameters>
)xml";

    // Preset 3: "Moon riding"
    const char* moonRidingXml = R"xml(
<Parameters>
  <PARAM id="FactoryImage" value="2.0"/>
  <PARAM id="Ellipse1_CX" value="0.5"/>
  <PARAM id="Ellipse1_CY" value="0.2800000011920929"/>
  <PARAM id="Ellipse1_R1" value="0.01999999955296516"/>
  <PARAM id="Ellipse1_R2" value="0.2699999809265137"/>
  <PARAM id="Ellipse1_Angle" value="3.109999895095825"/>
  <PARAM id="Ellipse1_Volume" value="0.5"/>
  <PARAM id="Ellipse1_Pan" value="0.309999942779541"/>
  <PARAM id="Ellipse2_Volume" value="0.9799999594688416"/>
  <PARAM id="Ellipse3_Volume" value="0.4399999976158142"/>
  <PARAM id="LFOFreq" value="0.1400000005960464"/>
  <PARAM id="LFO1Sync" value="1.0"/>
  <PARAM id="LFO1Wave" value="0.0"/>
  <PARAM id="LFO1Phase" value="0.0"/>
  <PARAM id="Attack" value="1.209999918937683"/>
  <PARAM id="Decay" value="0.3"/>
  <PARAM id="Sustain" value="0.8"/>
  <PARAM id="Release" value="2.399999856948853"/>
  <PARAM id="Mod_Ellipse1_R1_Amount" value="1.0"/>
  <PARAM id="Mod_Ellipse1_R1_Select" value="0.0"/>
  <PARAM id="Mod_Ellipse1_Volume_Amount" value="1.0"/>
  <PARAM id="Mod_Ellipse1_Volume_Select" value="4"/>
  <PARAM id="Attack2" value="2.059999942779541"/>
  <PARAM id="Attack3" value="0.8899999856948853"/>
  <PARAM id="Decay2"/>
  <PARAM id="Decay3"/>
  <PARAM id="Ellipse1_FilterFreq" value="590.0"/>
  <PARAM id="Ellipse1_FilterQuality"/>
  <PARAM id="Ellipse1_FilterType"/>
  <PARAM id="Ellipse2_Angle" value="6.269999980926514"/>
  <PARAM id="Ellipse2_CX" value="0.5"/>
  <PARAM id="Ellipse2_CY" value="0.6899999976158142"/>
  <PARAM id="Ellipse2_FilterFreq" value="842.0"/>
  <PARAM id="Ellipse2_FilterQuality"/>
  <PARAM id="Ellipse2_FilterType"/>
  <PARAM id="Ellipse2_Pan" value="-0.2800000309944153"/>
  <PARAM id="Ellipse2_R1"/>
  <PARAM id="Ellipse2_R2" value="0.1099999994039536"/>
  <PARAM id="Ellipse3_Angle" value="4.71999979019165"/>
  <PARAM id="Ellipse3_CX" value="0.7699999809265137"/>
  <PARAM id="Ellipse3_CY" value="0.5299999713897705"/>
  <PARAM id="Ellipse3_FilterFreq" value="3824.0"/>
  <PARAM id="Ellipse3_FilterQuality"/>
  <PARAM id="Ellipse3_FilterType"/>
  <PARAM id="Ellipse3_Pan" value="0.0"/>
  <PARAM id="Ellipse3_R1" value="0.3499999940395355"/>
  <PARAM id="Ellipse3_R2" value="0.01999999955296516"/>
  <PARAM id="LFO1Rate"/>
  <PARAM id="LFO2Freq"/>
  <PARAM id="LFO2Phase"/>
  <PARAM id="LFO2Rate" value="2.0"/>
  <PARAM id="LFO2Sync" value="1.0"/>
  <PARAM id="LFO2Wave" value="4.0"/>
  <PARAM id="LFO3Freq" value="0.07000000029802322"/>
  <PARAM id="LFO3Phase"/>
  <PARAM id="LFO3Rate"/>
  <PARAM id="LFO3Sync"/>
  <PARAM id="LFO3Wave"/>
  <PARAM id="LFO4Freq"/>
  <PARAM id="LFO4Phase"/>
  <PARAM id="LFO4Rate"/>
  <PARAM id="LFO4Sync"/>
  <PARAM id="LFO4Wave"/>
  <PARAM id="Level" value="-3.300003051757812"/>
  <PARAM id="Mod_Ellipse1_Angle_Amount"/>
  <PARAM id="Mod_Ellipse1_Angle_Select"/>
  <PARAM id="Mod_Ellipse1_CX_Amount"/>
  <PARAM id="Mod_Ellipse1_CX_Select"/>
  <PARAM id="Mod_Ellipse1_CY_Amount"/>
  <PARAM id="Mod_Ellipse1_CY_Select"/>
  <PARAM id="Mod_Ellipse1_FilterFreq_Amount" value="0.3799999952316284"/>
  <PARAM id="Mod_Ellipse1_FilterFreq_Select" value="1.0"/>
  <PARAM id="Mod_Ellipse1_FilterQuality_Amount"/>
  <PARAM id="Mod_Ellipse1_FilterQuality_Select"/>
  <PARAM id="Mod_Ellipse1_Freq_Amount"/>
  <PARAM id="Mod_Ellipse1_Freq_Select"/>
  <PARAM id="Mod_Ellipse1_Pan_Amount"/>
  <PARAM id="Mod_Ellipse1_Pan_Select"/>
  <PARAM id="Mod_Ellipse1_R2_Amount" value="-0.6200000047683716"/>
  <PARAM id="Mod_Ellipse1_R2_Select" value="0.0"/>
  <PARAM id="Mod_Ellipse2_Angle_Amount"/>
  <PARAM id="Mod_Ellipse2_Angle_Select"/>
  <PARAM id="Mod_Ellipse2_CX_Amount"/>
  <PARAM id="Mod_Ellipse2_CX_Select"/>
  <PARAM id="Mod_Ellipse2_CY_Amount"/>
  <PARAM id="Mod_Ellipse2_CY_Select"/>
  <PARAM id="Mod_Ellipse2_FilterFreq_Amount"/>
  <PARAM id="Mod_Ellipse2_FilterFreq_Select"/>
  <PARAM id="Mod_Ellipse2_FilterQuality_Amount"/>
  <PARAM id="Mod_Ellipse2_FilterQuality_Select"/>
  <PARAM id="Mod_Ellipse2_Freq_Amount" value="0.02999997138977051"/>
  <PARAM id="Mod_Ellipse2_Freq_Select" value="1.0"/>
  <PARAM id="Mod_Ellipse2_Pan_Amount"/>
  <PARAM id="Mod_Ellipse2_Pan_Select"/>
  <PARAM id="Mod_Ellipse2_R1_Amount" value="0.309999942779541"/>
  <PARAM id="Mod_Ellipse2_R1_Select"/>
  <PARAM id="Mod_Ellipse2_R2_Amount" value="0.2699999809265137"/>
  <PARAM id="Mod_Ellipse2_R2_Select"/>
  <PARAM id="Mod_Ellipse2_Volume_Amount"/>
  <PARAM id="Mod_Ellipse2_Volume_Select"/>
  <PARAM id="Mod_Ellipse3_Angle_Amount"/>
  <PARAM id="Mod_Ellipse3_Angle_Select"/>
  <PARAM id="Mod_Ellipse3_CX_Amount" value="0.1799999475479126"/>
  <PARAM id="Mod_Ellipse3_CX_Select" value="2.0"/>
  <PARAM id="Mod_Ellipse3_CY_Amount" value="0.0"/>
  <PARAM id="Mod_Ellipse3_CY_Select"/>
  <PARAM id="Mod_Ellipse3_FilterFreq_Amount"/>
  <PARAM id="Mod_Ellipse3_FilterFreq_Select"/>
  <PARAM id="Mod_Ellipse3_FilterQuality_Amount"/>
  <PARAM id="Mod_Ellipse3_FilterQuality_Select"/>
  <PARAM id="Mod_Ellipse3_Freq_Amount" value="0.009999990463256836"/>
  <PARAM id="Mod_Ellipse3_Freq_Select" value="14.0"/>
  <PARAM id="Mod_Ellipse3_Pan_Amount" value="0.5"/>
  <PARAM id="Mod_Ellipse3_Pan_Select"/>
  <PARAM id="Mod_Ellipse3_R1_Amount" value="0.0"/>
  <PARAM id="Mod_Ellipse3_R1_Select" value="0.0"/>
  <PARAM id="Mod_Ellipse3_R2_Amount" value="0.0"/>
  <PARAM id="Mod_Ellipse3_R2_Select" value="0.0"/>
  <PARAM id="Mod_Ellipse3_Volume_Amount"/>
  <PARAM id="Mod_Ellipse3_Volume_Select"/>
  <PARAM id="Release2" value="2.609999895095825"/>
  <PARAM id="Release3"/>
  <PARAM id="Sustain2"/>
  <PARAM id="Sustain3"/>
</Parameters>

)xml";


    // Preset 4: "Workshop router"
    const char* workshopRouterXml = R"xml(
<Parameters>
  <PARAM id="FactoryImage" value="6.0"/>
  <PARAM id="Ellipse1_CX" value="0.550000011920929"/>
  <PARAM id="Ellipse1_CY" value="0.5"/>
  <PARAM id="Ellipse1_R1" value="0.05999999865889549"/>
  <PARAM id="Ellipse1_R2" value="0.1899999976158142"/>
  <PARAM id="Ellipse1_Angle" value="3.949999809265137"/>
  <PARAM id="Ellipse1_Volume" value="0.7999999523162842"/>
  <PARAM id="Ellipse1_Pan" value="0.25"/>
  <PARAM id="Ellipse2_Volume" value="0.0"/>
  <PARAM id="Ellipse3_Volume" value="0.0"/>
  <PARAM id="LFOFreq" value="0.1400000005960464"/>
  <PARAM id="LFO1Sync" value="0.0"/>
  <PARAM id="LFO1Wave" value="0.0"/>
  <PARAM id="LFO1Phase" value="0.0"/>
  <PARAM id="Attack" value="0.1700000017881393"/>
  <PARAM id="Decay" value="0.3"/>
  <PARAM id="Sustain" value="1.0"/>
  <PARAM id="Release" value="0.0"/>
  <PARAM id="Mod_Ellipse1_R1_Amount" value="-0.9100000262260437"/>
  <PARAM id="Mod_Ellipse1_R1_Select" value="6.0"/>
  <PARAM id="Mod_Ellipse1_Volume_Amount" value="1.0"/>
  <PARAM id="Mod_Ellipse1_Volume_Select" value="5.0"/>
  <PARAM id="Attack2" value="3.43999981880188"/>
  <PARAM id="Attack3" value="3.289999961853027"/>
  <PARAM id="Decay2"/>
  <PARAM id="Decay3"/>
  <PARAM id="Ellipse1_FilterFreq" value="20000.0"/>
  <PARAM id="Ellipse1_FilterQuality"/>
  <PARAM id="Ellipse1_FilterType"/>
  <PARAM id="Ellipse2_Angle" value="5.650000095367432"/>
  <PARAM id="Ellipse2_CX" value="0.6200000047683716"/>
  <PARAM id="Ellipse2_CY" value="0.3999999761581421"/>
  <PARAM id="Ellipse2_FilterFreq" value="5762.0"/>
  <PARAM id="Ellipse2_FilterQuality"/>
  <PARAM id="Ellipse2_FilterType"/>
  <PARAM id="Ellipse2_Pan" value="-0.2200000286102295"/>
  <PARAM id="Ellipse2_R1" value="0.2899999916553497"/>
  <PARAM id="Ellipse2_R2" value="0.009999999776482582"/>
  <PARAM id="Ellipse3_Angle" value="3.859999895095825"/>
  <PARAM id="Ellipse3_CX" value="0.2800000011920929"/>
  <PARAM id="Ellipse3_CY" value="0.3100000023841858"/>
  <PARAM id="Ellipse3_FilterFreq" value="3994.0"/>
  <PARAM id="Ellipse3_FilterQuality"/>
  <PARAM id="Ellipse3_FilterType"/>
  <PARAM id="Ellipse3_Pan" value="0.0"/>
  <PARAM id="Ellipse3_R1" value="0.2099999934434891"/>
  <PARAM id="Ellipse3_R2" value="0.2599999904632568"/>
  <PARAM id="LFO1Rate"/>
  <PARAM id="LFO2Freq" value="0.3799999952316284"/>
  <PARAM id="LFO2Phase"/>
  <PARAM id="LFO2Rate" value="5.0"/>
  <PARAM id="LFO2Sync" value="0.0"/>
  <PARAM id="LFO2Wave" value="0.0"/>
  <PARAM id="LFO3Freq" value="6.610000133514404"/>
  <PARAM id="LFO3Phase"/>
  <PARAM id="LFO3Rate"/>
  <PARAM id="LFO3Sync"/>
  <PARAM id="LFO3Wave"/>
  <PARAM id="LFO4Freq" value="0.2000000029802322"/>
  <PARAM id="LFO4Phase"/>
  <PARAM id="LFO4Rate"/>
  <PARAM id="LFO4Sync"/>
  <PARAM id="LFO4Wave"/>
  <PARAM id="Level" value="0.0"/>
  <PARAM id="Mod_Ellipse1_Angle_Amount" value="0.0"/>
  <PARAM id="Mod_Ellipse1_Angle_Select"/>
  <PARAM id="Mod_Ellipse1_CX_Amount" value="0.1000000238418579"/>
  <PARAM id="Mod_Ellipse1_CX_Select"/>
  <PARAM id="Mod_Ellipse1_CY_Amount" value="0.1000000238418579"/>
  <PARAM id="Mod_Ellipse1_CY_Select"/>
  <PARAM id="Mod_Ellipse1_FilterFreq_Amount"/>
  <PARAM id="Mod_Ellipse1_FilterFreq_Select"/>
  <PARAM id="Mod_Ellipse1_FilterQuality_Amount"/>
  <PARAM id="Mod_Ellipse1_FilterQuality_Select"/>
  <PARAM id="Mod_Ellipse1_Freq_Amount"/>
  <PARAM id="Mod_Ellipse1_Freq_Select"/>
  <PARAM id="Mod_Ellipse1_Pan_Amount" value="0.8199999332427979"/>
  <PARAM id="Mod_Ellipse1_Pan_Select" value="1.0"/>
  <PARAM id="Mod_Ellipse1_R2_Amount" value="-0.8999999761581421"/>
  <PARAM id="Mod_Ellipse1_R2_Select" value="6.0"/>
  <PARAM id="Mod_Ellipse2_Angle_Amount"/>
  <PARAM id="Mod_Ellipse2_Angle_Select"/>
  <PARAM id="Mod_Ellipse2_CX_Amount"/>
  <PARAM id="Mod_Ellipse2_CX_Select"/>
  <PARAM id="Mod_Ellipse2_CY_Amount"/>
  <PARAM id="Mod_Ellipse2_CY_Select"/>
  <PARAM id="Mod_Ellipse2_FilterFreq_Amount" value="-0.7400000095367432"/>
  <PARAM id="Mod_Ellipse2_FilterFreq_Select" value="6.0"/>
  <PARAM id="Mod_Ellipse2_FilterQuality_Amount"/>
  <PARAM id="Mod_Ellipse2_FilterQuality_Select"/>
  <PARAM id="Mod_Ellipse2_Freq_Amount" value="0.0"/>
  <PARAM id="Mod_Ellipse2_Freq_Select" value="1.0"/>
  <PARAM id="Mod_Ellipse2_Pan_Amount"/>
  <PARAM id="Mod_Ellipse2_Pan_Select"/>
  <PARAM id="Mod_Ellipse2_R1_Amount" value="0.4399999380111694"/>
  <PARAM id="Mod_Ellipse2_R1_Select" value="6.0"/>
  <PARAM id="Mod_Ellipse2_R2_Amount" value="0.1100000143051147"/>
  <PARAM id="Mod_Ellipse2_R2_Select" value="6.0"/>
  <PARAM id="Mod_Ellipse2_Volume_Amount" value="1.0"/>
  <PARAM id="Mod_Ellipse2_Volume_Select"/>
  <PARAM id="Mod_Ellipse3_Angle_Amount"/>
  <PARAM id="Mod_Ellipse3_Angle_Select"/>
  <PARAM id="Mod_Ellipse3_CX_Amount" value="0.0"/>
  <PARAM id="Mod_Ellipse3_CX_Select" value="1.0"/>
  <PARAM id="Mod_Ellipse3_CY_Amount" value="0.0"/>
  <PARAM id="Mod_Ellipse3_CY_Select" value="0.0"/>
  <PARAM id="Mod_Ellipse3_FilterFreq_Amount" value="-1.0"/>
  <PARAM id="Mod_Ellipse3_FilterFreq_Select" value="6.0"/>
  <PARAM id="Mod_Ellipse3_FilterQuality_Amount"/>
  <PARAM id="Mod_Ellipse3_FilterQuality_Select"/>
  <PARAM id="Mod_Ellipse3_Freq_Amount" value="0.0"/>
  <PARAM id="Mod_Ellipse3_Freq_Select" value="14.0"/>
  <PARAM id="Mod_Ellipse3_Pan_Amount" value="0.5"/>
  <PARAM id="Mod_Ellipse3_Pan_Select"/>
  <PARAM id="Mod_Ellipse3_R1_Amount" value="0.2899999618530273"/>
  <PARAM id="Mod_Ellipse3_R1_Select" value="6.0"/>
  <PARAM id="Mod_Ellipse3_R2_Amount" value="0.2899999618530273"/>
  <PARAM id="Mod_Ellipse3_R2_Select" value="6.0"/>
  <PARAM id="Mod_Ellipse3_Volume_Amount"/>
  <PARAM id="Mod_Ellipse3_Volume_Select"/>
  <PARAM id="Release2" value="2.480000019073486"/>
  <PARAM id="Release3" value="5.0"/>
  <PARAM id="Sustain2"/>
  <PARAM id="Sustain3"/>
</Parameters>
    )xml";

    // Array of all factory presets
    const Preset factoryPresets[] =
    {
        { "Deep Dive",  deepDiveXml },
        { "Hesitant", hesitantXml },
        { "Moon Riding", moonRidingXml },
        { "Workshop Router", workshopRouterXml }
    };

    const int numFactoryPresets = sizeof(factoryPresets) / sizeof(Preset);

} // namespace FactoryPresets
