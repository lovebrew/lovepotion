#include "common/Console.hpp"
#include "common/Exception.hpp"
#include "common/math.hpp"

#include "modules/graphics/Graphics.hpp"
#include "modules/graphics/ParticleSystem.hpp"
#include "modules/math/RandomGenerator.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>

namespace love
{
    namespace
    {
        RandomGenerator rng;

        float calculate_variation(float inner, float outer, float variable)
        {
            float low  = inner - (outer / 2.0f) * variable;
            float high = inner + (outer / 2.0f) * variable;
            float r    = (float)rng.random();
            return low * (1 - r) + high * r;
        }
    } // namespace

    Type ParticleSystem::type("ParticleSystem", &Drawable::type);

    ParticleSystem::ParticleSystem(TextureBase* texture, uint32_t size) :
        pMemory(nullptr),
        pFree(nullptr),
        pHead(nullptr),
        pTail(nullptr),
        texture(texture),
        active(true),
        insertMode(INSERT_MODE_TOP),
        maxParticles(0),
        activeParticles(0),
        emissionRate(0),
        emitCounter(0),
        emissionAreaDistribution(DISTRIBUTION_NONE),
        emissionAreaAngle(0),
        directionRelativeToEmissionCenter(false),
        lifetime(-1.0f),
        life(0.0f),
        particleLifeMin(0.0f),
        particleLifeMax(0.0f),
        direction(0.0f),
        spread(0.0f),
        speedMin(0.0f),
        speedMax(0.0f),
        linearAccelerationMin(0, 0),
        linearAccelerationMax(0, 0),
        radialAccelerationMin(0.0f),
        radialAccelerationMax(0.0f),
        tangentialAccelerationMin(0.0f),
        tangentialAccelerationMax(0.0f),
        linearDampingMin(0.0f),
        linearDampingMax(0.0f),
        sizeVariation(0.0f),
        rotationMin(0.0f),
        rotationMax(0.0f),
        spinStart(0.0f),
        spinEnd(0.0f),
        spinVariation(0.0f),
        offset(float(texture->getWidth()) * 0.5f, float(texture->getHeight()) * 0.5f),
        defaultOffset(true),
        relativeRotation(false),
        buffer {}
    {
        if (size == 0 || size > MAX_PARTICLES)
            throw love::Exception("Invalid ParticleSystem size.");

        if (texture->getTextureType() != TEXTURE_2D)
            throw love::Exception("Only 2D textures can be used with ParticleSystems.");

        this->sizes.push_back(1.0f);
        this->colors.push_back(Color(1.0f, 1.0f, 1.0f, 1.0f));
        this->setBufferSize(size);
    }

    ParticleSystem::ParticleSystem(const ParticleSystem& p) :
        pMemory(nullptr),
        pFree(nullptr),
        pHead(nullptr),
        pTail(nullptr),
        texture(p.texture),
        active(p.active),
        insertMode(p.insertMode),
        maxParticles(p.maxParticles),
        activeParticles(0),
        emissionRate(p.emissionRate),
        emitCounter(0.0f),
        position(p.position),
        previousPosition(p.previousPosition),
        emissionAreaDistribution(p.emissionAreaDistribution),
        emissionArea(p.emissionArea),
        emissionAreaAngle(p.emissionAreaAngle),
        directionRelativeToEmissionCenter(p.directionRelativeToEmissionCenter),
        lifetime(p.lifetime),
        life(p.lifetime),
        particleLifeMin(p.particleLifeMin),
        particleLifeMax(p.particleLifeMax),
        direction(p.direction),
        spread(p.spread),
        speedMin(p.speedMin),
        speedMax(p.speedMax),
        linearAccelerationMin(p.linearAccelerationMin),
        linearAccelerationMax(p.linearAccelerationMax),
        radialAccelerationMin(p.radialAccelerationMin),
        radialAccelerationMax(p.radialAccelerationMax),
        tangentialAccelerationMin(p.tangentialAccelerationMin),
        tangentialAccelerationMax(p.tangentialAccelerationMax),
        linearDampingMin(p.linearDampingMin),
        linearDampingMax(p.linearDampingMax),
        sizes(p.sizes),
        sizeVariation(p.sizeVariation),
        rotationMin(p.rotationMin),
        rotationMax(p.rotationMax),
        spinStart(p.spinStart),
        spinEnd(p.spinEnd),
        spinVariation(p.spinVariation),
        offset(p.offset),
        defaultOffset(p.defaultOffset),
        colors(p.colors),
        quads(p.quads),
        relativeRotation(p.relativeRotation),
        buffer {}
    {
        setBufferSize(maxParticles);
    }

    ParticleSystem::~ParticleSystem()
    {
        this->deleteBuffers();
    }

    ParticleSystem* ParticleSystem::clone()
    {
        return new ParticleSystem(*this);
    }

    void ParticleSystem::resetOffset()
    {
        if (this->quads.empty())
            offset = Vector2(float(this->texture->getWidth()) * 0.5f, float(texture->getHeight()) * 0.5f);
        else
        {
            auto viewport = this->quads[0]->getViewport();
            offset        = Vector2(viewport.w * 0.5f, viewport.h * 0.5f);
        }
    }

    void ParticleSystem::createBuffers(size_t size)
    {
        this->pFree = this->pMemory = new Particle[size];
        this->maxParticles          = (uint32_t)size;
        this->buffer.resize(size);
    }

    void ParticleSystem::deleteBuffers()
    {
        delete[] this->pMemory;
        this->buffer.clear();

        this->pMemory         = nullptr;
        this->maxParticles    = 0;
        this->activeParticles = 0;
    }

    void ParticleSystem::setBufferSize(uint32_t size)
    {
        if (size == 0 || size > MAX_PARTICLES)
            throw love::Exception("Invalid buffer size.");

        this->deleteBuffers();
        this->createBuffers(size);
        this->reset();
    }

    uint32_t ParticleSystem::getBufferSize() const
    {
        return this->maxParticles;
    }

    void ParticleSystem::addParticle(float t)
    {
        if (this->isFull())
            return;

        auto* particle = this->pFree++;
        this->initParticle(particle, t);

        switch (this->insertMode)
        {
            default:
            case INSERT_MODE_TOP:
            {
                this->insertTop(particle);
                break;
            }
            case INSERT_MODE_BOTTOM:
            {
                this->insertBottom(particle);
                break;
            }
            case INSERT_MODE_RANDOM:
            {
                this->insertRandom(particle);
                break;
            }
        }

        this->activeParticles++;
    }

    void ParticleSystem::initParticle(Particle* particle, float t)
    {
        float min, max;
        Vector2 position = this->previousPosition + (this->position - this->previousPosition) * t;

        min = this->particleLifeMin;
        max = this->particleLifeMax;

        if (min == max)
            particle->life = min;
        else
            particle->life = (float)rng.random(min, max);

        particle->lifetime = particle->life;
        particle->position = position;

        min       = this->direction - this->spread / 2.0f;
        max       = this->direction + this->spread / 2.0f;
        float dir = (float)rng.random(min, max);

        float rand_x, rand_y;
        float c, s;

        switch (emissionAreaDistribution)
        {
            case DISTRIBUTION_UNIFORM:
            {
                c      = cosf(emissionAreaAngle);
                s      = sinf(emissionAreaAngle);
                rand_x = (float)rng.random(-emissionArea.x, emissionArea.x);
                rand_y = (float)rng.random(-emissionArea.y, emissionArea.y);
                particle->position.x += c * rand_x - s * rand_y;
                particle->position.y += s * rand_x + c * rand_y;
                break;
            }
            case DISTRIBUTION_NORMAL:
            {
                c      = cosf(emissionAreaAngle);
                s      = sinf(emissionAreaAngle);
                rand_x = (float)rng.randomNormal(emissionArea.x);
                rand_y = (float)rng.randomNormal(emissionArea.y);
                particle->position.x += c * rand_x - s * rand_y;
                particle->position.y += s * rand_x + c * rand_y;
                break;
            }
            case DISTRIBUTION_ELLIPSE:
            {
                c      = cosf(emissionAreaAngle);
                s      = sinf(emissionAreaAngle);
                rand_x = (float)rng.random(-1, 1);
                rand_y = (float)rng.random(-1, 1);
                min    = emissionArea.x * (rand_x * sqrt(1 - 0.5f * pow(rand_y, 2)));
                max    = emissionArea.y * (rand_y * sqrt(1 - 0.5f * pow(rand_x, 2)));
                particle->position.x += c * min - s * max;
                particle->position.y += s * min + c * max;
                break;
            }
            case DISTRIBUTION_BORDER_ELLIPSE:
            {
                c      = cosf(emissionAreaAngle);
                s      = sinf(emissionAreaAngle);
                rand_x = (float)rng.random(0, LOVE_M_PI * 2);
                min    = cosf(rand_x) * emissionArea.x;
                max    = sinf(rand_x) * emissionArea.y;
                particle->position.x += c * min - s * max;
                particle->position.y += s * min + c * max;
                break;
            }
            case DISTRIBUTION_BORDER_RECTANGLE:
            {
                c      = cosf(emissionAreaAngle);
                s      = sinf(emissionAreaAngle);
                rand_x = (float)rng.random((emissionArea.x + emissionArea.y) * -2,
                                           (emissionArea.x + emissionArea.y) * 2);
                rand_y = emissionArea.y * 2;
                if (rand_x < -rand_y)
                {
                    min = rand_x + rand_y + emissionArea.x;
                    particle->position.x += c * min - s * -emissionArea.y;
                    particle->position.y += s * min + c * -emissionArea.y;
                }
                else if (rand_x < 0)
                {
                    max = rand_x + emissionArea.y;
                    particle->position.x += c * -emissionArea.x - s * max;
                    particle->position.y += s * -emissionArea.x + c * max;
                }
                else if (rand_x < rand_y)
                {
                    max = rand_x - emissionArea.y;
                    particle->position.x += c * emissionArea.x - s * max;
                    particle->position.y += s * emissionArea.x + c * max;
                }
                else
                {
                    min = rand_x - rand_y - emissionArea.x;
                    particle->position.x += c * min - s * emissionArea.y;
                    particle->position.y += s * min + c * emissionArea.y;
                }
                break;
            }
            case DISTRIBUTION_NONE:
            default:
                break;
        };

        // Determine if the origin of each particle is the center of the area
        if (directionRelativeToEmissionCenter)
            dir += atan2(particle->position.y - position.y, particle->position.x - position.x);

        particle->origin = position;

        min         = speedMin;
        max         = speedMax;
        float speed = (float)rng.random(min, max);

        particle->velocity = Vector2(cosf(dir), sinf(dir)) * speed;

        particle->linearAcceleration.x = (float)rng.random(linearAccelerationMin.x, linearAccelerationMax.x);
        particle->linearAcceleration.y = (float)rng.random(linearAccelerationMin.y, linearAccelerationMax.y);

        min                          = radialAccelerationMin;
        max                          = radialAccelerationMax;
        particle->radialAcceleration = (float)rng.random(min, max);

        min                              = tangentialAccelerationMin;
        max                              = tangentialAccelerationMax;
        particle->tangentialAcceleration = (float)rng.random(min, max);

        min                     = linearDampingMin;
        max                     = linearDampingMax;
        particle->linearDamping = (float)rng.random(min, max);

        particle->sizeOffset       = (float)rng.random(sizeVariation); // time offset for size change
        particle->sizeIntervalSize = (1.0f - (float)rng.random(sizeVariation)) - particle->sizeOffset;
        particle->size             = sizes[(size_t)(particle->sizeOffset - .5f) * (sizes.size() - 1)];

        min                 = rotationMin;
        max                 = rotationMax;
        particle->spinStart = calculate_variation(spinStart, spinEnd, spinVariation);
        particle->spinEnd   = calculate_variation(spinEnd, spinStart, spinVariation);
        particle->rotation  = (float)rng.random(min, max);

        particle->angle = particle->rotation;
        if (relativeRotation)
            particle->angle += atan2f(particle->velocity.y, particle->velocity.x);

        particle->color = colors[0];

        particle->quadIndex = 0;
    }

    void ParticleSystem::insertTop(Particle* particle)
    {
        if (this->pHead == nullptr)
        {
            this->pHead        = particle;
            particle->previous = nullptr;
        }
        else
        {
            this->pTail->next  = particle;
            particle->previous = this->pTail;
        }
        particle->next = nullptr;
        this->pTail    = particle;
    }

    void ParticleSystem::insertBottom(Particle* particle)
    {
        if (this->pTail == nullptr)
        {
            this->pTail    = particle;
            particle->next = nullptr;
        }
        else
        {
            this->pHead->previous = particle;
            particle->next        = this->pHead;
        }
        particle->previous = nullptr;
        this->pHead        = particle;
    }

    void ParticleSystem::insertRandom(Particle* particle)
    {
        uint64_t position = rng.rand() % ((int64_t)this->activeParticles + 1);

        if (position == this->activeParticles)
        {
            Particle* pA = this->pHead;

            if (pA)
                pA->previous = particle;

            particle->previous = nullptr;
            particle->next     = pA;
            this->pHead        = particle;
            return;
        }

        Particle* pA = this->pMemory + position;
        Particle* pB = pA->next;
        pA->next     = particle;

        if (pB)
            pB->previous = particle;
        else
            this->pTail = particle;

        particle->previous = pA;
        particle->next     = pB;
    }

    ParticleSystem::Particle* ParticleSystem::removeParticle(Particle* particle)
    {
        Particle* pNext = nullptr;

        if (particle->previous)
            particle->previous->next = particle->next;
        else
            this->pHead = particle->next;

        if (particle->next)
        {
            particle->next->previous = particle->previous;
            pNext                    = particle->next;
        }
        else
            this->pTail = particle->previous;

        this->pFree--;

        if (particle != this->pFree)
        {
            *particle = *this->pFree;

            if (pNext == this->pFree)
                pNext = particle;

            if (particle->previous)
                particle->previous->next = particle;
            else
                this->pHead = particle;

            if (particle->next)
                particle->next->previous = particle;
            else
                this->pTail = particle;
        }

        this->activeParticles--;
        return pNext;
    }

    void ParticleSystem::setTexture(TextureBase* texture)
    {
        if (texture->getTextureType() != TEXTURE_2D)
            throw love::Exception("Only 2D textures can be used with ParticleSystems.");

        this->texture.set(texture);

        if (this->defaultOffset)
            this->resetOffset();
    }

    TextureBase* ParticleSystem::getTexture() const
    {
        return this->texture.get();
    }

    void ParticleSystem::setInsertMode(InsertMode mode)
    {
        this->insertMode = mode;
    }

    ParticleSystem::InsertMode ParticleSystem::getInsertMode() const
    {
        return this->insertMode;
    }

    void ParticleSystem::setEmissionRate(float rate)
    {
        if (rate < 0.0f)
            throw love::Exception("Invalid emission rate.");

        this->emissionRate = rate;
        this->emitCounter  = std::min(this->emitCounter, 1.0f / rate);
    }

    float ParticleSystem::getEmissionRate() const
    {
        return this->emissionRate;
    }

    void ParticleSystem::setEmitterLifetime(float life)
    {
        this->life = this->lifetime = life;
    }

    float ParticleSystem::getEmitterLifetime() const
    {
        return this->lifetime;
    }

    void ParticleSystem::setParticleLifetime(float min, float max)
    {
        this->particleLifeMin = min;
        if (max == 0)
            this->particleLifeMax = min;
        else
            this->particleLifeMax = max;
    }

    void ParticleSystem::getParticleLifetime(float& min, float& max) const
    {
        min = this->particleLifeMin;
        max = this->particleLifeMax;
    }

    void ParticleSystem::setPosition(float x, float y)
    {
        this->position         = Vector2(x, y);
        this->previousPosition = this->position;
    }

    const Vector2& ParticleSystem::getPosition() const
    {
        return this->position;
    }

    void ParticleSystem::moveTo(float x, float y)
    {
        this->position = Vector2(x, y);
    }

    void ParticleSystem::setEmissionArea(AreaSpreadDistribution distribution, float x, float y, float angle,
                                         bool directionRelativeToCenter)
    {
        this->emissionArea                      = Vector2(x, y);
        this->emissionAreaDistribution          = distribution;
        this->emissionAreaAngle                 = angle;
        this->directionRelativeToEmissionCenter = directionRelativeToCenter;
    }

    ParticleSystem::AreaSpreadDistribution ParticleSystem::getEmissionArea(
        Vector2& params, float& angle, bool& directionRelativeToCenter) const
    {
        params                    = this->emissionArea;
        angle                     = this->emissionAreaAngle;
        directionRelativeToCenter = this->directionRelativeToEmissionCenter;
        return this->emissionAreaDistribution;
    }

    void ParticleSystem::setDirection(float direction)
    {
        this->direction = direction;
    }

    float ParticleSystem::getDirection() const
    {
        return this->direction;
    }

    void ParticleSystem::setSpread(float spread)
    {
        this->spread = spread;
    }

    float ParticleSystem::getSpread() const
    {
        return this->spread;
    }

    void ParticleSystem::setSpeed(float speed)
    {
        this->speedMin = this->speedMax = speed;
    }

    void ParticleSystem::setSpeed(float min, float max)
    {
        this->speedMin = min;
        this->speedMax = max;
    }

    void ParticleSystem::getSpeed(float& min, float& max) const
    {
        min = this->speedMin;
        max = this->speedMax;
    }

    void ParticleSystem::setLinearAcceleration(float x, float y)
    {
        this->linearAccelerationMin.x = this->linearAccelerationMax.x = x;
        this->linearAccelerationMin.y = this->linearAccelerationMax.y = y;
    }

    void ParticleSystem::setLinearAcceleration(float xmin, float ymin, float xmax, float ymax)
    {
        this->linearAccelerationMin = Vector2(xmin, ymin);
        this->linearAccelerationMax = Vector2(xmax, ymax);
    }

    void ParticleSystem::getLinearAcceleration(Vector2& min, Vector2& max) const
    {
        min = this->linearAccelerationMin;
        max = this->linearAccelerationMax;
    }

    void ParticleSystem::setRadialAcceleration(float acceleration)
    {
        this->radialAccelerationMin = this->radialAccelerationMax = acceleration;
    }

    void ParticleSystem::setRadialAcceleration(float min, float max)
    {
        this->radialAccelerationMin = min;
        this->radialAccelerationMax = max;
    }

    void ParticleSystem::getRadialAcceleration(float& min, float& max) const
    {
        min = this->radialAccelerationMin;
        max = this->radialAccelerationMax;
    }

    void ParticleSystem::setTangentialAcceleration(float acceleration)
    {
        this->tangentialAccelerationMin = this->tangentialAccelerationMax = acceleration;
    }

    void ParticleSystem::setTangentialAcceleration(float min, float max)
    {
        this->tangentialAccelerationMin = min;
        this->tangentialAccelerationMax = max;
    }

    void ParticleSystem::getTangentialAcceleration(float& min, float& max) const
    {
        min = this->tangentialAccelerationMin;
        max = this->tangentialAccelerationMax;
    }

    void ParticleSystem::setLinearDamping(float min, float max)
    {
        this->linearDampingMin = min;
        this->linearDampingMax = max;
    }

    void ParticleSystem::getLinearDamping(float& min, float& max) const
    {
        min = this->linearDampingMin;
        max = this->linearDampingMax;
    }

    void ParticleSystem::setSize(float size)
    {
        this->sizes.resize(1);
        this->sizes[0] = size;
    }

    void ParticleSystem::setSizes(const std::vector<float>& sizes)
    {
        this->sizes = sizes;
    }

    const std::vector<float>& ParticleSystem::getSizes() const
    {
        return this->sizes;
    }

    void ParticleSystem::setSizeVariation(float variation)
    {
        this->sizeVariation = variation;
    }

    float ParticleSystem::getSizeVariation() const
    {
        return this->sizeVariation;
    }

    void ParticleSystem::setRotation(float rotation)
    {
        this->rotationMin = this->rotationMax = rotation;
    }

    void ParticleSystem::setRotation(float min, float max)
    {
        this->rotationMin = min;
        this->rotationMax = max;
    }

    void ParticleSystem::getRotation(float& min, float& max) const
    {
        min = this->rotationMin;
        max = this->rotationMax;
    }

    void ParticleSystem::setSpin(float spin)
    {
        this->spinStart = this->spinEnd = spin;
    }

    void ParticleSystem::setSpin(float start, float end)
    {
        this->spinStart = start;
        this->spinEnd   = end;
    }

    void ParticleSystem::getSpin(float& start, float& end) const
    {
        start = this->spinStart;
        end   = this->spinEnd;
    }

    void ParticleSystem::setSpinVariation(float variation)
    {
        this->spinVariation = variation;
    }

    float ParticleSystem::getSpinVariation() const
    {
        return this->spinVariation;
    }

    void ParticleSystem::setOffset(float x, float y)
    {
        this->offset        = Vector2(x, y);
        this->defaultOffset = false;
    }

    Vector2 ParticleSystem::getOffset() const
    {
        return this->offset;
    }

    void ParticleSystem::setColor(const std::vector<Color>& colors)
    {
        this->colors = colors;

        for (auto& color : this->colors)
        {
            color.r = std::clamp(color.r, 0.0f, 1.0f);
            color.g = std::clamp(color.g, 0.0f, 1.0f);
            color.b = std::clamp(color.b, 0.0f, 1.0f);
            color.a = std::clamp(color.a, 0.0f, 1.0f);
        }
    }

    std::vector<Color> ParticleSystem::getColor() const
    {
        return this->colors;
    }

    void ParticleSystem::setQuads(const std::vector<Quad*>& quads)
    {
        std::vector<StrongRef<Quad>> quadList;
        quadList.reserve(quads.size());

        for (auto* quad : quads)
            quadList.push_back(quad);

        this->quads = quadList;

        if (this->defaultOffset)
            this->resetOffset();
    }

    void ParticleSystem::setQuads()
    {
        this->quads.clear();
    }

    std::vector<Quad*> ParticleSystem::getQuads() const
    {
        std::vector<Quad*> quadList;
        quadList.reserve(this->quads.size());

        for (const auto& quad : this->quads)
            quadList.push_back(quad.get());

        return quadList;
    }

    void ParticleSystem::setRelativeRotation(bool enable)
    {
        this->relativeRotation = enable;
    }

    bool ParticleSystem::hasRelativeRoation() const
    {
        return this->relativeRotation;
    }

    uint32_t ParticleSystem::getCount() const
    {
        return this->activeParticles;
    }

    void ParticleSystem::start()
    {
        this->active = true;
    }

    void ParticleSystem::stop()
    {
        this->active      = false;
        this->life        = this->lifetime;
        this->emitCounter = 0;
    }

    void ParticleSystem::pause()
    {
        this->active = false;
    }

    void ParticleSystem::reset()
    {
        if (this->pMemory == nullptr)
            return;

        this->pFree           = this->pMemory;
        this->pHead           = nullptr;
        this->pTail           = nullptr;
        this->activeParticles = 0;
        this->life            = this->lifetime;
        this->emitCounter     = 0;
    }

    void ParticleSystem::emit(uint32_t count)
    {
        if (!this->active)
            return;

        count = std::min(count, this->maxParticles - this->activeParticles);

        while (count--)
            this->addParticle(1.0f);
    }

    bool ParticleSystem::isActive() const
    {
        return this->active;
    }

    bool ParticleSystem::isPaused() const
    {
        return !this->active && this->life < this->lifetime;
    }

    bool ParticleSystem::isStopped() const
    {
        return !this->active && this->life >= this->lifetime;
    }

    bool ParticleSystem::isEmpty() const
    {
        return this->activeParticles == 0;
    }

    bool ParticleSystem::isFull() const
    {
        return this->activeParticles == this->maxParticles;
    }

    void ParticleSystem::update(float dt)
    {
        if (this->pMemory == nullptr || dt == 0.0f)
            return;

        auto* particle = this->pHead;

        while (particle)
        {
            particle->life -= dt;

            if (particle->life <= 0)
                particle = removeParticle(particle);
            else
            {
                Vector2 radial, tangential;
                Vector2 ppos = this->position;

                radial = ppos - particle->origin;
                radial.normalize();
                tangential = radial;

                radial *= particle->radialAcceleration;

                {
                    float a      = tangential.x;
                    tangential.x = -tangential.y;
                    tangential.y = a;
                }

                tangential *= particle->tangentialAcceleration;
                particle->velocity += (radial + tangential + particle->linearAcceleration) * dt;
                particle->velocity *= 1.0f / (1.0f + particle->linearDamping * dt);

                ppos += particle->velocity * dt;
                particle->position = ppos;

                const float t = 1.0f - particle->life / particle->lifetime;
                particle->rotation += (particle->spinStart * (1.0f - t) + particle->spinEnd * t) * dt;
                particle->angle = particle->rotation;

                if (this->relativeRotation)
                    particle->angle += atan2(particle->velocity.y, particle->velocity.x);

                float s = particle->sizeOffset + t * particle->sizeIntervalSize;
                s *= (float)(this->sizes.size() - 1);

                size_t i = (size_t)s;
                size_t k = (i == this->sizes.size() - 1) ? i : i + 1;
                s -= (float)i;

                particle->size = this->sizes[i] * (1.0f - s) + this->sizes[k] * s;

                s = t * (float)(this->colors.size() - 1);
                i = (size_t)s;
                k = (i == this->colors.size() - 1) ? i : i + 1;
                s -= (float)i;

                particle->color = this->colors[i] * (1.0f - s) + this->colors[k] * s;

                k = this->quads.size();

                if (k > 0)
                {
                    s                   = t * (float)k;
                    i                   = (s > 0.0f) ? (size_t)s : 0;
                    particle->quadIndex = (int)((i < k) ? i : k - 1);
                }

                particle = particle->next;
            }
        }

        if (this->active)
        {
            float rate = 1.0f / this->emissionRate;
            this->emitCounter += dt;
            float total = this->emitCounter - rate;

            while (this->emitCounter > rate)
            {
                this->addParticle(1.0f - (this->emitCounter - rate) / total);
                this->emitCounter -= rate;
            }

            this->life -= dt;
            if (this->lifetime != -1 && this->life < 0)
                this->stop();
        }

        this->previousPosition = this->position;
    }

    static constexpr ShaderBase::StandardShader SHADER_TYPE =
        (Console::is(Console::CTR)) ? ShaderBase::STANDARD_DEFAULT : ShaderBase::STANDARD_TEXTURE;

    void ParticleSystem::draw(GraphicsBase* graphics, const Matrix4& matrix)
    {
        uint32_t count = this->getCount();

        if (count == 0 || this->texture.get() == nullptr || this->pMemory == nullptr || this->buffer.empty())
            return;

        graphics->flushBatchedDraws();

        if (ShaderBase::isDefaultActive())
            ShaderBase::attachDefault(ShaderBase::STANDARD_DEFAULT);

        const auto* positions = this->texture->getQuad()->getVertexPositions();
        const auto* texcoords = this->texture->getQuad()->getTextureCoordinates();

        auto* vertices = this->buffer.data();
        auto* p        = this->pHead;

        bool useQuads = !this->quads.empty();

        Matrix3 t {};
        size_t baseIndex = 0;

        while (p)
        {
            if (useQuads)
            {
                positions = this->quads[p->quadIndex]->getVertexPositions();
                texcoords = this->quads[p->quadIndex]->getTextureCoordinates();
            }

            // clang-format off
            t.setTransformation(p->position.x, p->position.y, p->angle, p->size, p->size, this->offset.x, this->offset.y, 0.0f, 0.0f);
            auto* destination  = &this->buffer[baseIndex];
            t.transformXY(destination, positions, 4);
            // clang-format on

            for (int vertex = 0; vertex < 4; vertex++)
            {
                destination[vertex].s     = texcoords[vertex].x;
                destination[vertex].t     = texcoords[vertex].y;
                destination[vertex].color = p->color;
            }

            baseIndex += 4;
            p = p->next;
        }

        GraphicsBase::TempTransform(graphics, matrix);

        BatchedDrawCommand command {};
        command.format        = CommonFormat::XYf_STf_RGBAf;
        command.indexMode     = TRIANGLEINDEX_QUADS;
        command.texture       = this->texture;
        command.vertexCount   = baseIndex;
        command.pushTransform = false;
        command.shaderType    = SHADER_TYPE;

        auto data = graphics->requestBatchedDraw(command);

        Vertex* stream = (Vertex*)data.stream;

        auto& m_transform = graphics->getTransform();
        m_transform.transformXY(stream, this->buffer.data(), command.vertexCount);

        for (int index = 0; index < command.vertexCount; index++)
        {
            stream[index].s     = this->buffer[index].s;
            stream[index].t     = this->buffer[index].t;
            stream[index].color = this->buffer[index].color;
        }
    }
} // namespace love
