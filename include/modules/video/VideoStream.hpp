#pragma once

#include "common/Object.hpp"
#include "modules/audio/Source.hpp"

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

        virtual void setSync(FrameSync* sync);

        virtual FrameSync* getSync() const;

        struct Frame
        {
            Frame();

            ~Frame();

            uint8_t* data;

            int yw, yh;
            uint8_t* yPlane;

            int cw, ch;
            uint8_t* cbPlane;
            uint8_t* crPlane;
        };

        class FrameSync : public Object
        {
          public:
            virtual ~FrameSync()
            {}

            virtual double getPosition() const = 0;

            virtual void update(double /*dt*/)
            {}

            void copyState(const FrameSync* other);

            virtual void play() = 0;

            virtual void pause() = 0;

            virtual void seek(double offset) = 0;

            virtual double tell() const;

            virtual bool isPlaying() const = 0;
        };

        class DeltaSync : public FrameSync
        {
          public:
            DeltaSync();

            virtual ~DeltaSync();

            double getPosition() const override;

            void update(double dt) override;

            void play() override;

            void pause() override;

            void seek(double offset) override;

            bool isPlaying() const override;

          private:
            bool playing;
            double position;
            double speed;
            std::recursive_mutex mutex;
        };

        class SourceSync : public FrameSync
        {
          public:
            SourceSync(SourceBase* source);

            double getPosition() const override;

            void play() override;

            void pause() override;

            void seek(double offset) override;

            bool isPlaying() const override;

          private:
            StrongRef<SourceBase> source;
        };

      protected:
        StrongRef<FrameSync> frameSync;
    };
} // namespace love
