/*
  ==============================================================================

    ParameterStructs.h
    Created: 18 Sep 2025 11:00:00am
    Author:  Olivier Doaré

  ==============================================================================
*/

#pragma once

#include "LineReader.h"
#include "CircleReader.h"
#include "ADSR.h"

struct GlobalParameters
{
    LineReaderParameters line;
    CircleReaderParameters circle;
    ADSRParameters adsr;
};