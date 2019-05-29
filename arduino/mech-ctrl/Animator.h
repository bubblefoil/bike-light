#ifndef Animator_h
#define Animator_h

class Animator
{
private:
    unsigned long anim_start;

public:
    Animator(int interval);
    ~Animator();
};

Animator::Animator(int interval)
{
}

Animator::~Animator()
{
}

#endif