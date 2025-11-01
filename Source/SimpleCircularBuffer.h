/*
  ==============================================================================

    SimpleCircularBuffer.h
    Created: 4 Oct 2025 10:00:00am
    Author:  Olivier Doaré

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

/** A simple, lock-free, single-producer, single-consumer circular buffer for audio samples. */
class SimpleCircularBuffer
{
public:
    SimpleCircularBuffer (int capacity)
        : buffer (capacity), size (capacity)
    {
        juce::zeromem (buffer.get(), (size_t) size * sizeof (float));
    }

    void writeSample (float sample)
    {
        // Use relaxed memory order as we only need atomicity, not synchronization with other variables.
        const int index = writeIndex.load (std::memory_order_relaxed);
        buffer[index] = sample;
        writeIndex.store ((index + 1) % size, std::memory_order_relaxed);
    }

    /** Clears the buffer to zeros and resets the write position. */
    void clear()
    {
        // This is safe to be called from the audio thread.
        // It will atomically reset the write index first.
        writeIndex.store(0);
        juce::zeromem (buffer.get(), (size_t) size * sizeof (float));
    }

    // This is not a true SPSC fifo, but for visualization it's good enough.
    // The GUI thread will grab a snapshot of the buffer.
    void readFullBuffer (juce::AudioBuffer<float>& destBuffer)
    {
        const int localWriteIndex = writeIndex.load();
        const int localSize = size;
        destBuffer.setSize (1, localSize, false, false, true);
        auto* writer = destBuffer.getWritePointer (0);

        for (int i = 0; i < localSize; ++i)
        {
            writer[i] = buffer[(localWriteIndex + i) % localSize];
        }
    }

    int getCapacity() const
    {
        return size;
    }

private:
    juce::HeapBlock<float> buffer;
    const int size;
    std::atomic<int> writeIndex { 0 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleCircularBuffer)
};