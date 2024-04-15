#pragma once

#include <sndcore2/core.h>
#include <sndcore2/device.h>
#include <sndcore2/voice.h>

#include <algorithm>
#include <queue>

namespace love
{
    class AudioBuffer;

    class SoundChannel
    {
      public:
        enum ChannelState
        {
            STATE_PLAYING,
            STATE_PAUSED,
            STATE_STOPPED,
            STATE_DONE
        };

        SoundChannel();

        /*
        ** @brief Resets the channel to defaults.
        */
        void reset();

        /**
        ** @brief Sets the interpolation type (stereo or mono) of the channel.
        ** @param channels The number of channels for the interpolation type.
        */
        void setInterpType(int channels);

        /*
        ** @brief Gets the interpolation type of the channel.
        ** @return The interpolation channel count of the channel.
        */
        int getInterpType() const;

        /*
        ** @brief Sets the sample rate of the channel.
        ** @param sampleRate The sample rate to set.
        */
        void setSampleRate(float sampleRate);

        /*
        ** @brief  Gets the sample rate of the channel.
        ** @return The sample rate of the channel.
        */
        float getSampleRate() const;

        /*
        ** @brief Sets the format of the channel.
        ** @param format The format to set.
        */
        void setFormat(AXVoiceFormat format);

        /**
        ** @brief Gets the format of the channel.
        ** @return The format of the channel.
        */
        AXVoiceFormat getFormat() const;

        /*
        ** @brief Sets the volume of the channel.
        ** @param volume The volume to set.
        */
        void setVolume(float volume);

        /*
        ** @brief Gets the volume of the channel.
        ** @return The volume of the channel.
        */
        float getVolume() const;

        /*
        ** @brief Gets the sample offset of the buffer at the front of the queue.
        ** @return The sample offset of the buffer.
        */
        size_t getSamplePosition() const;

        /*
        ** @brief Plays the buffer at the front of the queue.
        ** @return True if the buffer was played successfully, false otherwise.
        */
        bool addWaveBuffer(AudioBuffer* buffer);

        /*
        ** @brief Updates the channel.
        */
        void update();

        /*
        ** @brief Checks if the channel is playing.
        ** @return True if the channel is playing, false otherwise.
        */
        bool isPlaying() const;

        /*
        ** @brief Plays the buffer at the front of the queue.
        */
        void play();

        /*
        ** @brief Stops the buffer at the front of the queue.
        ** This also pops the buffers from the queue.
        */
        void stop();

        /*
        ** @brief Pauses the buffer at the front of the queue.
        ** @param  paused The paused state to set.
        */
        void setPaused(bool paused);

        /*
        ** @brief Checks if the channel is paused.
        ** @return True if the channel is paused, false otherwise.
        */
        bool isPaused() const;

      private:
        std::queue<AudioBuffer*> buffers;
        ChannelState state;

        float ratio;
        float volume;
        AXVoiceFormat format;
        int channels;
        size_t samplePosition;

        static AXVoiceState getState(bool stopped);
    };
} // namespace love
