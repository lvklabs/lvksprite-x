//
//  LvkAnimate.h
//  LvkSpriteProject
//
//  Created by Andres Pagliano
//  Copyright (c) 2012 LVK. All rights reserved.
//
#ifndef _LVKANIMATE_H_
#define _LVKANIMATE_H_

#include "cocos2d.h"

using namespace cocos2d;

class LvkAnimate : public CCAnimate
{
public:
    bool virtual initWithAnimation(CCAnimation *pAnimation);

    static LvkAnimate* create(CCAnimation *pAnimation);

    void stop();
};


#endif //_LVKANIMATE_H_

