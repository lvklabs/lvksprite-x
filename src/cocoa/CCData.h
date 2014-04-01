//
//  CCData.h
//
//  Created by Andres Pagliano
//  Copyright (c) 2012 LVK. All rights reserved.
//
#ifndef _CCDATA_H_
#define _CCDATA_H_

#include <cocoa/CCObject.h>

#include "cocos2d.h"

#include <string>

NS_CC_BEGIN

class CCData : public CCObject
{
public:
    CCData();

    ~CCData();
	
    static CCData* createWithContentsOfFile(const std::string &strPath);

    unsigned char *bytes();

    unsigned long length();

    CCData* subdataWithRange(unsigned long offset, unsigned long length);

private:
    unsigned char *m_pData;
    unsigned long  m_nSize;
};

NS_CC_END

#endif //_CCDATA_H_
