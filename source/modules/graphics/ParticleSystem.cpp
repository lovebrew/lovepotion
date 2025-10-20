#include "common/Console.hpp"
#include "common/Exception.hpp"
#include "common/math.hpp"

#include "modules/graphics/Graphics.hpp"
#include "modules/graphics/ParticleSystem.hpp"
#include "modules/math/RandomGenerator.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>

#include "common/debug.hpp"

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
        try
        {
            this->pFree = this->pMemory = new Particle[size];
            this->maxParticles          = (uint32_t)size;
            this->buffer.resize(size * 4);
        }
        catch (std::bad_alloc&)
        {
            this->deleteBuffers();
            throw love::Exception(E_OUT_OF_MEMORY);
        }
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

        Particle* p = this->pFree++;
        this->initParticle(p, t);

        switch (insertMode)
        {
            default:
            case INSERT_MODE_TOP:
                insertTop(p);
                break;
            case INSERT_MODE_BOTTOM:
                insertBottom(p);
                break;
            case INSERT_MODE_RANDOM:
                insertRandom(p);
                break;
        }

        this->activeParticles++;
    }

    void ParticleSystem::initParticle(Particle* p, float t)
    {
        float min, max;

        // Linearly interpolate between the previous and current emitter position.
        love::Vector2 pos = previousPosition + (position - previousPosition) * t;

        min = particleLifeMin;
        max = particleLifeMax;
        if (min == max)
            p->life = min;
        else
            p->life = (float)rng.random(min, max);
        p->lifetime = p->life;

        p->position = pos;

        min       = direction - spread / 2.0f;
        max       = direction + spread / 2.0f;
        float dir = (float)rng.random(min, max);

        // In this switch statement, variables 'rand_y', 'min', and 'max'
        // are sometimes reused as data stores for performance reasons
        float rand_x, rand_y;
        float c, s;
        switch (emissionAreaDistribution)
        {
            case DISTRIBUTION_UNIFORM:
                c      = cosf(emissionAreaAngle);
                s      = sinf(emissionAreaAngle);
                rand_x = (float)rng.random(-emissionArea.x, emissionArea.x);
                rand_y = (float)rng.random(-emissionArea.y, emissionArea.y);
                p->position.x += c * rand_x - s * rand_y;
                p->position.y += s * rand_x + c * rand_y;
                break;
            case DISTRIBUTION_NORMAL:
                c      = cosf(emissionAreaAngle);
                s      = sinf(emissionAreaAngle);
                rand_x = (float)rng.randomNormal(emissionArea.x);
                rand_y = (float)rng.randomNormal(emissionArea.y);
                p->position.x += c * rand_x - s * rand_y;
                p->position.y += s * rand_x + c * rand_y;
                break;
            case DISTRIBUTION_ELLIPSE:
                c      = cosf(emissionAreaAngle);
                s      = sinf(emissionAreaAngle);
                rand_x = (float)rng.random(-1, 1);
                rand_y = (float)rng.random(-1, 1);
                min    = emissionArea.x * (rand_x * sqrt(1 - 0.5f * pow(rand_y, 2)));
                max    = emissionArea.y * (rand_y * sqrt(1 - 0.5f * pow(rand_x, 2)));
                p->position.x += c * min - s * max;
                p->position.y += s * min + c * max;
                break;
            case DISTRIBUTION_BORDER_ELLIPSE:
                c      = cosf(emissionAreaAngle);
                s      = sinf(emissionAreaAngle);
                rand_x = (float)rng.random(0, LOVE_M_PI * 2);
                min    = cosf(rand_x) * emissionArea.x;
                max    = sinf(rand_x) * emissionArea.y;
                p->position.x += c * min - s * max;
                p->position.y += s * min + c * max;
                break;
            case DISTRIBUTION_BORDER_RECTANGLE:
                c      = cosf(emissionAreaAngle);
                s      = sinf(emissionAreaAngle);
                rand_x = (float)rng.random((emissionArea.x + emissionArea.y) * -2,
                                           (emissionArea.x + emissionArea.y) * 2);
                rand_y = emissionArea.y * 2;
                if (rand_x < -rand_y)
                {
                    min = rand_x + rand_y + emissionArea.x;
                    p->position.x += c * min - s * -emissionArea.y;
                    p->position.y += s * min + c * -emissionArea.y;
                }
                else if (rand_x < 0)
                {
                    max = rand_x + emissionArea.y;
                    p->position.x += c * -emissionArea.x - s * max;
                    p->position.y += s * -emissionArea.x + c * max;
                }
                else if (rand_x < rand_y)
                {
                    max = rand_x - emissionArea.y;
                    p->position.x += c * emissionArea.x - s * max;
                    p->position.y += s * emissionArea.x + c * max;
                }
                else
                {
                    min = rand_x - rand_y - emissionArea.x;
                    p->position.x += c * min - s * emissionArea.y;
                    p->position.y += s * min + c * emissionArea.y;
                }
                break;
            case DISTRIBUTION_NONE:
            default:
                break;
        }

        // Determine if the origin of each particle is the center of the area
        if (directionRelativeToEmissionCenter)
            dir += atan2(p->position.y - pos.y, p->position.x - pos.x);

        p->origin = pos;

        min         = speedMin;
        max         = speedMax;
        float speed = (float)rng.random(min, max);

        p->velocity = love::Vector2(cosf(dir), sinf(dir)) * speed;

        p->linearAcceleration.x = (float)rng.random(linearAccelerationMin.x, linearAccelerationMax.x);
        p->linearAcceleration.y = (float)rng.random(linearAccelerationMin.y, linearAccelerationMax.y);

        min                   = radialAccelerationMin;
        max                   = radialAccelerationMax;
        p->radialAcceleration = (float)rng.random(min, max);

        min                       = tangentialAccelerationMin;
        max                       = tangentialAccelerationMax;
        p->tangentialAcceleration = (float)rng.random(min, max);

        min              = linearDampingMin;
        max              = linearDampingMax;
        p->linearDamping = (float)rng.random(min, max);

        p->sizeOffset       = (float)rng.random(sizeVariation); // time offset for size change
        p->sizeIntervalSize = (1.0f - (float)rng.random(sizeVariation)) - p->sizeOffset;
        p->size             = sizes[(size_t)(p->sizeOffset - .5f) * (sizes.size() - 1)];

        min          = rotationMin;
        max          = rotationMax;
        p->spinStart = calculate_variation(spinStart, spinEnd, spinVariation);
        p->spinEnd   = calculate_variation(spinEnd, spinStart, spinVariation);
        p->rotation  = (float)rng.random(min, max);

        p->angle = p->rotation;
        if (relativeRotation)
            p->angle += atan2f(p->velocity.y, p->velocity.x);

        p->color = colors[0];

        p->quadIndex = 0;
    }

    void ParticleSystem::insertTop(Particle* p)
    {
        if (pHead == nullptr)
        {
            pHead   = p;
            p->prev = nullptr;
        }
        else
        {
            pTail->next = p;
            p->prev     = pTail;
        }
        p->next = nullptr;
        pTail   = p;
    }

    void ParticleSystem::insertBottom(Particle* p)
    {
        if (pTail == nullptr)
        {
            pTail   = p;
            p->next = nullptr;
        }
        else
        {
            pHead->prev = p;
            p->next     = pHead;
        }
        p->prev = nullptr;
        pHead   = p;
    }

    void ParticleSystem::insertRandom(Particle* p)
    {
        // Nonuniform, but 64-bit is so large nobody will notice. Hopefully.
        uint64_t pos = rng.rand() % ((int64_t)activeParticles + 1);

        // Special case where the particle gets inserted before the head.
        if (pos == activeParticles)
        {
            Particle* pA = pHead;
            if (pA)
                pA->prev = p;
            p->prev = nullptr;
            p->next = pA;
            pHead   = p;
            return;
        }

        // Inserts the particle after the randomly selected particle.
        Particle* pA = pMemory + pos;
        Particle* pB = pA->next;
        pA->next     = p;
        if (pB)
            pB->prev = p;
        else
            pTail = p;
        p->prev = pA;
        p->next = pB;
    }

    ParticleSystem::Particle* ParticleSystem::removeParticle(Particle* p)
    {
        Particle* pNext = nullptr;

        // Removes the particle from the linked list.
        if (p->prev)
            p->prev->next = p->next;
        else
            pHead = p->next;

        if (p->next)
        {
            p->next->prev = p->prev;
            pNext         = p->next;
        }
        else
            pTail = p->prev;

        // The (in memory) last particle can now be moved into the free slot.
        // It will skip the moving if it happens to be the removed particle.
        pFree--;
        if (p != pFree)
        {
            *p = *pFree;
            if (pNext == pFree)
                pNext = p;

            if (p->prev)
                p->prev->next = p;
            else
                pHead = p;

            if (p->next)
                p->next->prev = p;
            else
                pTail = p;
        }

        activeParticles--;
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
        if (pMemory == nullptr || dt == 0.0f)
            return;

        // Traverse all particles and update.
        Particle* p = pHead;

        while (p)
        {
            // Decrease lifespan.
            p->life -= dt;

            if (p->life <= 0)
                p = removeParticle(p);
            else
            {
                // Temp variables.
                love::Vector2 radial, tangential;
                love::Vector2 ppos = p->position;

                // Get vector from particle center to particle.
                radial = ppos - p->origin;
                radial.normalize();
                tangential = radial;

                // Resize radial acceleration.
                radial *= p->radialAcceleration;

                // Calculate tangential acceleration.
                {
                    float a      = tangential.x;
                    tangential.x = -tangential.y;
                    tangential.y = a;
                }

                // Resize tangential.
                tangential *= p->tangentialAcceleration;

                // Update velocity.
                p->velocity += (radial + tangential + p->linearAcceleration) * dt;

                // Apply damping.
                p->velocity *= 1.0f / (1.0f + p->linearDamping * dt);

                // Modify position.
                ppos += p->velocity * dt;

                p->position = ppos;

                const float t = 1.0f - p->life / p->lifetime;

                // Rotate.
                p->rotation += (p->spinStart * (1.0f - t) + p->spinEnd * t) * dt;

                p->angle = p->rotation;

                if (relativeRotation)
                    p->angle += atan2f(p->velocity.y, p->velocity.x);

                // Change size according to given intervals:
                // i = 0       1       2      3          n-1
                //     |-------|-------|------|--- ... ---|
                // t = 0    1/(n-1)        3/(n-1)        1
                //
                // `s' is the interpolation variable scaled to the current
                // interval width, e.g. if n = 5 and t = 0.3, then the current
                // indices are 1,2 and s = 0.3 - 0.25 = 0.05
                float s = p->sizeOffset + t * p->sizeIntervalSize; // size variation
                s *= (float)(sizes.size() - 1);                    // 0 <= s < sizes.size()
                size_t i = (size_t)s;
                size_t k =
                    (i == sizes.size() - 1) ? i : i + 1; // boundary check (prevents failing on t = 1.0f)
                s -= (float)i; // transpose s to be in interval [0:1]: i <= s < i + 1 ~> 0 <= s < 1
                p->size = sizes[i] * (1.0f - s) + sizes[k] * s;

                // Update color according to given intervals (as above)
                s = t * (float)(colors.size() - 1);
                i = (size_t)s;
                k = (i == colors.size() - 1) ? i : i + 1;
                s -= (float)i; // 0 <= s <= 1
                p->color = colors[i] * (1.0f - s) + colors[k] * s;

                // Update the quad index.
                k = quads.size();
                if (k > 0)
                {
                    s            = t * (float)k; // [0:numquads-1] (clamped below)
                    i            = (s > 0.0f) ? (size_t)s : 0;
                    p->quadIndex = (int)((i < k) ? i : k - 1);
                }

                // Next particle.
                p = p->next;
            }
        }

        // Make some more particles.
        if (active)
        {
            float rate = 1.0f / emissionRate; // the amount of time between each particle emit
            emitCounter += dt;
            float total = emitCounter - rate;
            while (emitCounter > rate)
            {
                addParticle(1.0f - (emitCounter - rate) / total);
                emitCounter -= rate;
            }

            life -= dt;
            if (lifetime != -1 && life < 0)
                stop();
        }

        this->previousPosition = position;
    }

    static constexpr ShaderBase::StandardShader SHADER_TYPE =
        (Console::is(Console::CTR)) ? ShaderBase::STANDARD_DEFAULT : ShaderBase::STANDARD_TEXTURE;

    void ParticleSystem::draw(GraphicsBase* graphics, const Matrix4& matrix)
    {
        uint32_t count = this->getCount();

        if (count == 0 || this->texture.get() == nullptr || this->pMemory == nullptr || this->buffer.empty())
            return;

        graphics->flushBatchedDraws();

        const auto* positions = this->texture->getQuad()->getVertexPositions();
        const auto* texcoords = this->texture->getQuad()->getTextureCoordinates();

        Particle* p = this->pHead;

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

        // GraphicsBase::TempTransform(graphics, matrix);

        BatchedDrawCommand command {};
        command.format        = CommonFormat::XYf_STf_RGBAf;
        command.indexMode     = TRIANGLEINDEX_QUADS;
        command.texture       = this->texture;
        command.vertexCount   = count * 4;
        command.pushTransform = false;
        command.shaderType    = SHADER_TYPE;

        auto data = graphics->requestBatchedDraw(command);

        Vertex* stream = (Vertex*)data.stream;
        std::memcpy(stream, this->buffer.data(), command.vertexCount * sizeof(Vertex));

        auto& m_transform = graphics->getTransform();
        Matrix4 translated(m_transform, matrix);

        translated.transformXY(stream, this->buffer.data(), command.vertexCount);
    }
} // namespace love
