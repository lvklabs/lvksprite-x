//
//  LvkSpawn.m
//  LvkSpriteProject
//
//  Copyright 2011, 2012 LVK. All rights reserved.
//

#include "LvkSpawn.h"
#include "common.h"

#include <algorithm>


bool LvkSpawn::initOneTwo(CCFiniteTimeAction *one, CCFiniteTimeAction *two)
{
    float d1 = one->getDuration();
    float d2 = two->getDuration();

    bool initOk = CCActionInterval::initWithDuration(std::max(d1, d2));

    if (initOk) {
        _childAdded = false;
        _nested = false;

        _child = new CCSprite();
        _child->init();

        if (d1 > d2) {
            _one = one;
            _two = CCSequence::createWithTwoActions(two, CCDelayTime::create(d1-d2));
        } else {
            _one = CCSequence::createWithTwoActions(one, CCDelayTime::create(d2-d1));
            _two = two;
        }

        _one->retain();
        _two->retain();
    }

    return initOk;
}

LvkSpawn * LvkSpawn::create(CCFiniteTimeAction *one, CCFiniteTimeAction *two)
{
    LvkSpawn * spawn = new LvkSpawn();
    spawn->initOneTwo(one, two);
    spawn->autorelease();
    return spawn;
}

LvkSpawn::~LvkSpawn()
{
    if (_childAdded) {
        m_pTarget->removeChild(_child, true);
    }
    _one->release();
    _two->release();
    _child->release();
}

void LvkSpawn::startWithTarget(CCNode *target)
{
    if (!_childAdded) {
        _child->setAnchorPoint(CCPoint(0, 1));
        _child->setContentSize(target->getContentSize());

        if (!_nested) {
            target->addChild(_child, 1);
            _child->setPosition(CCPoint(0, target->getContentSize().height));
        } else {
            target->addChild(_child, -1);
        }
        _childAdded = true;
    }

    CCActionInterval::startWithTarget(target);

    if (!_nested) {
        _one->startWithTarget(_child);
        _two->startWithTarget(m_pTarget);
    } else {
        _one->startWithTarget(m_pTarget);
        _two->startWithTarget(_child);
    }
}

void LvkSpawn::stop()
{
    if (_childAdded) {
        m_pTarget->removeChild(_child, true);
        _childAdded = false;
    }

    _one->stop();
    _two->stop();
    CCActionInterval::stop();
}

void LvkSpawn::update(float t)
{
    if (!_nested) {
        _child->setPosition(CCPoint(0, m_pTarget->getContentSize().height));
    }
    _one->update(t);
    _two->update(t);
}
