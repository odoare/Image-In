/*
  ==============================================================================

    VoiceInfo.h
    Created: 18 Sep 2025 10:00:00am
    Author:  Olivier Doaré

  ==============================================================================
*/

#pragma once

struct VoiceInfo
{
    bool isActive = false;
    int midiNote = 0;
    float adsrLevel = 0.0f;
};