//
//  LvkAnimate.m
//  LvkSpriteProject
//
//  Created by Andres Pagliano
//  Copyright (c) 2012 LVK. All rights reserved.
//

#include "LvkAnimate.h"


bool LvkAnimate::initWithAnimation(CCAnimation *pAnimation)
{
    return CCAnimate::initWithAnimation(pAnimation);
}

LvkAnimate * LvkAnimate::create(CCAnimation *pAnimation)
{
    LvkAnimate *ani = new LvkAnimate();
    ani->initWithAnimation(pAnimation);
    ani->autorelease();
    return ani;
}

void LvkAnimate::stop()
{
    // Bypass CCAnimate::stop because produces flickering. CCAnimate bug?
    //[cci2ccx] [[[self superclass] superclass] instanceMethodForSelector:@selector(stop)];
    CCActionInterval::stop();
}
