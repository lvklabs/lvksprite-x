/****************************************************************************
Copyright (c) 2010 cocos2d-x.org

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/


#include "CCData.h"
#include "platform/CCFileUtils.h"

#include <stdio.h>

using namespace std;

NS_CC_BEGIN

CCData::CCData()
    : m_pData(NULL), m_nSize(0)
{
}

CCData::~CCData()
{
	CC_SAFE_DELETE_ARRAY(m_pData);
}

CCData* CCData::createWithContentsOfFile(const string &strPath)
{
    unsigned long nSize = 0;
    unsigned char* pBuffer = CCFileUtils::sharedFileUtils()->getFileData(strPath.c_str(), "rb", &nSize);

    if (!pBuffer) {
        return NULL;
    }

	CCData *pRet = new CCData();
    pRet->m_nSize = nSize;
    pRet->m_pData = pBuffer;
    pRet->autorelease();

	return pRet;
}

unsigned char * CCData::bytes()
{
	return m_pData;
}

unsigned long cocos2d::CCData::length()
{
    return m_nSize;
}

CCData * CCData::subdataWithRange(unsigned long offset, unsigned long length)
{
    if (!m_pData) {
        return NULL;
    }

    CCData *pRet = new CCData();
    pRet->m_nSize = length;
    pRet->m_pData = new unsigned char[length];
    memcpy(pRet->m_pData, m_pData + offset, length);

    pRet->autorelease();

    return pRet;
}

NS_CC_END

