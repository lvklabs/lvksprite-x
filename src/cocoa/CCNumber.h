//
//  CCNumber.h
//
//  Created by Andres Pagliano
//  Copyright (c) 2012 LVK. All rights reserved.
//
#ifndef _CCNUMBER_H_
#define _CCNUMBER_H_

#include "cocos2d.h"

#include "Util/LvkSerializable.h"
#include "Util/LvkStream.h"

NS_CC_BEGIN

class CCNumber : public CCObject, public LvkSerializable
{
public:

    CCNumber(): _value(0) {}

    CCNumber(bool v): _value(v) {}

    CCNumber(int v): _value(v) {}

    //CCNumber(double v): _value(v) {}

    static CCNumber* create(int v = 0)
    {
        CCNumber* pRet = new CCNumber(v);
        pRet->autorelease();
        return pRet;
    }

    bool boolValue() { return _value; }

    int intValue() { return _value; }

    //double doubleValue() { return _value; }

    bool isEqualToNumber(const CCNumber *other)
    {
        return _value == other->_value;
    }

    virtual LvkByteArray serialize() const
    {
        LvkOutputStream os;
        os << _value;
        return os.byteArray();
    }

    virtual bool deserialize(const LvkByteArray &data)
    {
        LvkInputStream is(data);
        is >> _value;
        return is.isGood();
    }

private:
    //double _value;
    int _value;
};

NS_CC_END

#endif //_CCNUMBER_H_

