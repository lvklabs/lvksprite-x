//
//  LvkRepeatAction.cpp
//  LvkSpriteProject
//
//  Created by Andres Pagliano
//  Copyright (c) 2012 LVK. All rights reserved.
//
#include "LvkRepeatAction.h"


LvkRepeatAction* LvkRepeatAction::create(CCFiniteTimeAction *pAction, unsigned int times)
{
    LvkRepeatAction *action = new LvkRepeatAction();
    action->initWithAction(pAction, times);
    action->autorelease();
    return action;
}

bool LvkRepeatAction::initWithAction(CCFiniteTimeAction *pAction, unsigned int times)
{
    return CCRepeat::initWithAction(pAction, times);
}

