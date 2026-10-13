#pragma once

#include "common/Object.hpp"
#include "common/StrongRef.hpp"
#include "common/types.hpp"
#include "modules/audio/Source.hpp"

#include <cstdint>
#include <mutex>
#include <thread>

namespace love
{
    class VideoStream : public Object
    {
      public:
        static Type type;

        virtual ~VideoStream()
        {}

        virtual void fillBackBuffer()
        {}

        virtual const void* getFrontBuffer() const = 0;

        virtual size_t getSize() const = 0;

        virtual bool swapBuffers() = 0;

        virtual int getWidth() const = 0;

        virtual int getHeight() const = 0;

        virtual const std::string& getFilename() const = 0;

        virtual void play();

        virtual void pause();

        virtual void seek(double offset);

        virtual double tell() const;

        virtual bool isPlaying() const;

        class FrameSync;
        class DeltaSync;

        virtual void setSync(FrameSync* frame);

        virtual FrameSync* getSync() const;

        struct Frame
        {
            Frame();
            ~Frame();

            int yw, yh;
            uint8_t* yplane;

            int cw, ch;
            uint8_t* cbplane;
            uint8_t* crplane;
        };

        class FrameSync : public Object
        {
          public:
            virtual double getPosition() const = 0;

            virtual void update(double /* dt */)
            {}

            virtual ~FrameSync()
            {}

            void copyState(const FrameSync* other);

            virtual void play()              = 0;
            virtual void pause()             = 0;
            virtual void seek(double offset) = 0;
            virtual double tell() const;
            virtual bool isPlaying() const = 0;
        };

        class DeltaSync : public FrameSync
        {
          public:
            DeltaSync();
            ~DeltaSync();

            virtual double getPosition() const override;

            virtual void update(double dt) override;

            virtual void play() override;
            virtual void pause() override;
            virtual void seek(double time) override;
            virtual bool isPlaying() const override;

          private:
            bool playing;
            double position;
            double speed;
            std::mutex mutex;
        };

        class SourceSync : public FrameSync
        {
          public:
            SourceSync(SourceBase* source);

            virtual double getPosition() const override;
            virtual void play() override;
            virtual void pause() override;
            virtual void seek(double time) override;
            virtual bool isPlaying() const override;

          private:
            StrongRef<SourceBase> source;
        };

      protected:
        StrongRef<FrameSync> sync;
    };
} // namespace love
