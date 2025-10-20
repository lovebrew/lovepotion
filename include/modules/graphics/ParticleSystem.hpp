#pragma once

#include "common/Color.hpp"
#include "common/Map.hpp"
#include "common/Vector.hpp"

#include "modules/graphics/Drawable.hpp"
#include "modules/graphics/Quad.hpp"
#include "modules/graphics/Texture.tcc"

#include <vector>

namespace love
{
    class GraphicsBase;

    class ParticleSystem final : public Drawable
    {
      public:
        static Type type;

        enum AreaSpreadDistribution
        {
            DISTRIBUTION_NONE,
            DISTRIBUTION_UNIFORM,
            DISTRIBUTION_NORMAL,
            DISTRIBUTION_ELLIPSE,
            DISTRIBUTION_BORDER_ELLIPSE,
            DISTRIBUTION_BORDER_RECTANGLE,
            DISTRIBUTION_MAX_ENUM
        };

        enum InsertMode
        {
            INSERT_MODE_TOP,
            INSERT_MODE_BOTTOM,
            INSERT_MODE_RANDOM,
            INSERT_MODE_MAX_ENUM
        };

        static constexpr uint32_t MAX_PARTICLES = LOVE_UINT32_MAX / 4;

        ParticleSystem(TextureBase* texture, uint32_t bufferSize);

        ParticleSystem(const ParticleSystem& other);

        virtual ~ParticleSystem();

        ParticleSystem* clone();

        void setTexture(TextureBase* texture);

        TextureBase* getTexture() const;

        void setBufferSize(uint32_t size);

        uint32_t getBufferSize() const;

        void setInsertMode(InsertMode mode);

        InsertMode getInsertMode() const;

        void setEmissionRate(float rate);

        float getEmissionRate() const;

        void setEmitterLifetime(float life);

        float getEmitterLifetime() const;

        void setParticleLifetime(float min, float max = 0);

        void getParticleLifetime(float& min, float& max) const;

        void setPosition(float x, float y);

        const Vector2& getPosition() const;

        void moveTo(float x, float y);

        void setEmissionArea(AreaSpreadDistribution distribution, float x, float y, float angle,
                             bool directionRelativeToCenter);

        AreaSpreadDistribution getEmissionArea(Vector2& params, float& angle,
                                               bool& directionRelativeToCenter) const;

        void setDirection(float direction);

        float getDirection() const;

        void setSpread(float spread);

        float getSpread() const;

        void setSpeed(float speed);

        void setSpeed(float min, float max);

        void getSpeed(float& min, float& max) const;

        void setLinearAcceleration(float x, float y);

        void setLinearAcceleration(float xmin, float ymin, float xmax, float ymax);

        void getLinearAcceleration(Vector2& min, Vector2& max) const;

        void setRadialAcceleration(float acceleration);

        void setRadialAcceleration(float min, float max);

        void getRadialAcceleration(float& min, float& max) const;

        void setTangentialAcceleration(float acceleration);

        void setTangentialAcceleration(float min, float max);

        void getTangentialAcceleration(float& min, float& max) const;

        void setLinearDamping(float min, float max);

        void getLinearDamping(float& min, float& max) const;

        void setSize(float size);

        void setSizes(const std::vector<float>& sizes);

        const std::vector<float>& getSizes() const;

        void setSizeVariation(float variation);

        float getSizeVariation() const;

        void setRotation(float rotation);

        void setRotation(float min, float max);

        void getRotation(float& min, float& max) const;

        void setSpin(float spin);

        void setSpin(float start, float end);

        void getSpin(float& start, float& end) const;

        void setSpinVariation(float variation);

        float getSpinVariation() const;

        void setOffset(float x, float y);

        Vector2 getOffset() const;

        void setColor(const std::vector<Color>& colors);

        std::vector<Color> getColor() const;

        void setQuads(const std::vector<Quad*>& quads);

        void setQuads();

        std::vector<Quad*> getQuads() const;

        void setRelativeRotation(bool enable);

        bool hasRelativeRoation() const;

        uint32_t getCount() const;

        void start();

        void stop();

        void pause();

        void reset();

        void emit(uint32_t count);

        bool isActive() const;

        bool isPaused() const;

        bool isStopped() const;

        bool isEmpty() const;

        bool isFull() const;

        void update(float dt);

        void draw(GraphicsBase* graphics, const Matrix4& matrix) override;

        // clang-format off
        STRINGMAP_DECLARE(AreaSpreadDistributions, AreaSpreadDistribution,
            { "none",            DISTRIBUTION_NONE             },
            { "uniform",         DISTRIBUTION_UNIFORM          },
            { "normal",          DISTRIBUTION_NORMAL           },
            { "ellipse",         DISTRIBUTION_ELLIPSE          },
            { "borderellipse",   DISTRIBUTION_BORDER_ELLIPSE   },
            { "borderrectangle", DISTRIBUTION_BORDER_RECTANGLE }
        );

        STRINGMAP_DECLARE(InsertModes, InsertMode,
            { "top",    INSERT_MODE_TOP    },
            { "bottom", INSERT_MODE_BOTTOM },
            { "random", INSERT_MODE_RANDOM }
        );
        // clang-format on

      private:
        struct Particle
        {
            Particle* prev;
            Particle* next;

            float lifetime;
            float life;

            Vector2 position;
            Vector2 origin;

            Vector2 velocity;
            Vector2 linearAcceleration;

            float radialAcceleration;
            float tangentialAcceleration;

            float linearDamping;

            float size;
            float sizeOffset;
            float sizeIntervalSize;

            float rotation;
            float angle;
            float spinStart;
            float spinEnd;

            Color color;
            int quadIndex;
        };

        void resetOffset();

        void createBuffers(size_t size);

        void deleteBuffers();

        void addParticle(float t);

        Particle* removeParticle(Particle* particle);

        void initParticle(Particle* particle, float t);

        void insertTop(Particle* particle);

        void insertBottom(Particle* particle);

        void insertRandom(Particle* particle);

        Particle* pMemory;
        Particle* pFree;
        Particle* pHead;
        Particle* pTail;

        StrongRef<TextureBase> texture;
        bool active;

        InsertMode insertMode;

        uint32_t maxParticles;
        uint32_t activeParticles;

        float emissionRate;
        float emitCounter;

        Vector2 position;
        Vector2 previousPosition;

        AreaSpreadDistribution emissionAreaDistribution;
        Vector2 emissionArea;
        float emissionAreaAngle;
        bool directionRelativeToEmissionCenter;

        float lifetime;
        float life;

        float particleLifeMin;
        float particleLifeMax;

        float direction;
        float spread;

        float speedMin;
        float speedMax;

        Vector2 linearAccelerationMin;
        Vector2 linearAccelerationMax;

        float radialAccelerationMin;
        float radialAccelerationMax;

        float tangentialAccelerationMin;
        float tangentialAccelerationMax;

        float linearDampingMin;
        float linearDampingMax;

        std::vector<float> sizes;
        float sizeVariation;

        float rotationMin;
        float rotationMax;

        float spinStart;
        float spinEnd;
        float spinVariation;

        Vector2 offset;
        bool defaultOffset;

        std::vector<Color> colors;
        std::vector<StrongRef<Quad>> quads;
        bool relativeRotation;

        std::vector<Vertex> buffer;
    };
} // namespace love
