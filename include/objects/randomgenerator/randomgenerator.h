// This implementation was taken from https://bitbucket.org/rude/love/
// and was modified to work with Löve Potion. 

#pragma once

class RandomGenerator : public Object
{
    public:
        union Seed
        {
            u64 b64;
            struct
            {
                u32 low;
                u32 high;
            } b32;
        };

        RandomGenerator();
        RandomGenerator(Seed s);
        ~RandomGenerator();

        u64 rand();

        double random();                        // result: [0.0, 1.0]
        double random(double max);              // result: [0.0, max]
        double random(double min, double max);  // result: [min, max]

        double randomNormal(double d);

        void setSeed(Seed seed);
        Seed getSeed() const;

        void setState(const std::string &statestr);
        std::string getState() const;
    
    private:
        Seed seed;
        Seed rng_state;
        double last_randomnormal;
};
