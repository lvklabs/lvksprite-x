#ifndef _LVKSPAWN_H_
#define _LVKSPAWN_H_
//
//  LvkSpawn.h
//  LvkSpriteProject
//
//  Created by Andres Pagliano
//  Copyright 2011, 2012 LVK. All rights reserved.
//

#include "cocos2d.h"
#include "ccMacros.h"

using namespace cocos2d;

// CCSpawn does not support running two CCAnimations at the same time
// This class does a trick to do it.

class LvkSpawn : public CCActionInterval
{
public:
    bool initOneTwo(CCFiniteTimeAction *one, CCFiniteTimeAction *two);

    static LvkSpawn* create(CCFiniteTimeAction *one, CCFiniteTimeAction *two);

    ~LvkSpawn();

    void startWithTarget(CCNode *target);

    void stop();

    void update(float time);

    CC_SYNTHESIZE(bool, _nested, nested)

private:
    bool _childAdded;
    CCSprite *_child;
    CCFiniteTimeAction *_one;
    CCFiniteTimeAction *_two;
};


#endif //_LVKSPAWN_H_

