//
//  StringUtil.cpp
//  igirl-cpp
//
//  Created by Andres Pagliano
//  Copyright 2013 LVK. All rights reserved.
//
#include "Util/StringUtil.h"
#include "cocos2d.h" // CCLog
#include "common.h"  // LKLog

bool __replaceAll(const std::string &str, const std::string &regex,
                      const std::string &replacement, std::string &result);

bool __matches(const std::string &str, const std::string &regex, bool &result);


//--------------------------------------------------------------------------------------------------
// StringUtil
//--------------------------------------------------------------------------------------------------

std::string StringUtil::ltrim(const std::string &s)
{
    unsigned i = 0;
    for (; i < s.size() && std::isspace(s[i]); ++i);
    return s.substr(i);
}

std::string StringUtil::rtrim(const std::string &s)
{
    unsigned i = s.size();
    for (; i > 0 && std::isspace(s[i-1]); --i);
    return s.substr(0, i);
}

std::string StringUtil::trim(const std::string &s)
{
    return rtrim(ltrim(s));
}

bool StringUtil::hasPrefix(const std::string &s, const std::string &prefix)
{
    if (s.size() < prefix.size()) {
        return false;
    }
    unsigned i = 0;
    for (; i < prefix.size(); ++i) {
        if (prefix[i] != s[i]) {
            break;
        }
    }
    return i == prefix.size();
}

bool StringUtil::hasSuffix(const std::string &s, const std::string &sufix)
{
    if (s.size() < sufix.size()) {
        return false;
    }
    unsigned i = 0;
    for (; i < sufix.size(); ++i) {
        if (sufix[i] != s[s.size() - sufix.size() + i]) {
            break;
        }
    }
    return i == sufix.size();
}

std::vector<std::string> StringUtil::split(const std::string &s, char delim)
{
    return split(s, std::string(1, delim));

    // Old impl:
    //    std::vector<std::string> tokens;
    //    std::istringstream iss(s);
    //    std::string token;
    //    while (std::getline(iss, token, delim)) {
    //        tokens.push_back(token);
    //    }
    //    return tokens;
}

std::vector<std::string> StringUtil::split(const std::string &s, std::string delim)
{
    std::vector<std::string> comps;
    size_t offset = 0;
    size_t pos;

    while ( (pos = s.find(delim, offset)) != std::string::npos )  {
        comps.push_back(s.substr(offset, pos - offset));
        offset = pos + delim.size();
    }
    comps.push_back(s.substr(offset));

    return comps;
}

bool StringUtil::matches(const std::string &str, const std::string &regex)
{
    bool result = false;

    //LKLOG("StringUtil: %s m? %s", str.c_str(), regex.c_str());
    if (__matches(str, regex, result)) {
        //LKLOG("StringUtil: result %d", result);
    } else {
        LKLOG("StringUtil: ERROR __matches failed!");
    }
    return result;
}

std::string StringUtil::replaceAll(const std::string &str, const std::string &regex,
                                   const std::string &replacement)
{
    std::string result;

    //LKLOG("StringUtil: %s: s/%s/%s/", str.c_str(), regex.c_str(), replacement.c_str());
    if (__replaceAll(str, regex, replacement, result)) {
        //LKLOG("StringUtil: result: %s", result.c_str());
    } else {
        LKLOG("StringUtil: ERROR __replaceAll failed!");
    }
    return result;
}


//--------------------------------------------------------------------------------------------------
// Platform dependant helpers
//--------------------------------------------------------------------------------------------------

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID

#include <platform/android/jni/JniHelper.h>
#include <jni.h>

using namespace cocos2d;

bool __replaceAll(const std::string &str, const std::string &regex,
                      const std::string &replacement, std::string &result)
{
    JniMethodInfo mi;
    if (! JniHelper::getMethodInfo(mi, "java/lang/String", "replaceAll",
        "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;"))
    {
        LKLOG("%s %d: error to get String.replaceAll() methodInfo", __FILE__, __LINE__);
        return false;
    }

    jstring jsStr = mi.env->NewStringUTF(str.c_str());
    jstring jsRegex = mi.env->NewStringUTF(regex.c_str());
    jstring jsRepl = mi.env->NewStringUTF(replacement.c_str());

    jstring jsResult = (jstring)(mi.env->CallObjectMethod(jsStr, mi.methodID, jsRegex, jsRepl));

    if (jsResult) {
        const char* chars = mi.env->GetStringUTFChars(jsResult, NULL);
        result = std::string(chars);
        mi.env->ReleaseStringUTFChars(jsResult, chars);
    }

    mi.env->DeleteLocalRef(jsResult);
    mi.env->DeleteLocalRef(jsRepl);
    mi.env->DeleteLocalRef(jsRegex);
    mi.env->DeleteLocalRef(jsStr);
    mi.env->DeleteLocalRef(mi.classID);

    return true;
}

bool __matches(const std::string &str, const std::string &regex, bool &result)
{
    JniMethodInfo mi;
    if (! JniHelper::getStaticMethodInfo(mi, "java/util/regex/Pattern", "matches",
        "(Ljava/lang/String;Ljava/lang/CharSequence;)Z"))
    {
        LKLOG("%s %d: error to get Pattern.matches() methodInfo", __FILE__, __LINE__);
        return false;
    }

    jstring jstrStr = mi.env->NewStringUTF(str.c_str());
    jstring jstrRegex = mi.env->NewStringUTF(regex.c_str());

    jboolean jresult = mi.env->CallStaticBooleanMethod(mi.classID, mi.methodID, jstrStr, jstrRegex);

    mi.env->DeleteLocalRef(jstrStr);
    mi.env->DeleteLocalRef(jstrRegex);
    mi.env->DeleteLocalRef(mi.classID);

    result = !jresult; //(jresult != JNI_FALSE); // CHECK!

    return true;
}
/*
#elif CC_TARGET_PLATFORM == CC_PLATFORM_EMSCRIPTEN

#include <regex>

bool __replaceAll(const std::string &str, const std::string &pattern,
                  const std::string &replacement, std::string &result)
{
    LKLOG("regex_replace");

    result = std::regex_replace(str, std::regex(pattern), replacement);

    return true;
}

bool __matches(const std::string &str, const std::string &pattern, bool &result)
{
    LKLOG("regex_match");

    result = std::regex_match(str, std::regex(pattern));

    return true;
}
*/
#else

bool __replaceAll(const std::string &str, const std::string &regex,
                  const std::string &replacement, std::string &result)
{

    LKLOG("ERROR: __replaceAll not implemented for plaform %i", CC_TARGET_PLATFORM);

    return false;
}

bool __matches(const std::string &str, const std::string &regex, bool &result)
{
    LKLOG("ERROR: __matches not implemented for plaform %i", CC_TARGET_PLATFORM);

    return false;
}

#endif
