//
//  LvkRepeatAction.h
//  LvkSpriteProject
//
//  Created by Andres Pagliano
//  Copyright (c) 2012 LVK. All rights reserved.
//
#ifndef _LVKREPEATACTION_H_
#define _LVKREPEATACTION_H_

#include "cocos2d.h"

using namespace cocos2d;

class LvkRepeatAction : public CCRepeat
{
public:
    virtual bool initWithAction(CCFiniteTimeAction *pAction, unsigned int times);

    static LvkRepeatAction* create(CCFiniteTimeAction *pAction, unsigned int times);
};

#endif //_LVKREPEATACTION_H_

