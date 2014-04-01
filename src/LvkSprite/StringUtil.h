//
//  StringUtil.h
//  igirl-cpp
//
//  Created by Andres Pagliano
//  Copyright 2013 LVK. All rights reserved.
//

#ifndef STRINGUTIL_H
#define STRINGUTIL_H

#include <string>
#include <vector>
#include <sstream>

class StringUtil
{
public:
    static std::string ltrim(const std::string &s);

    static std::string rtrim(const std::string &s);

    static std::string trim(const std::string &s);

    static bool hasPrefix(const std::string &s, const std::string &prefix);

    static bool hasSuffix(const std::string &s, const std::string &sufix);

    static std::vector<std::string> split(const std::string &s, char delim);

    static std::vector<std::string> split(const std::string &s, std::string delim);

    template <typename T>
    static T value(const std::string &s)
    {
        T t;
        std::istringstream(s) >> t;
        return t;
    }

    static bool matches(const std::string &str, const std::string &regex);

    static std::string replaceAll(const std::string &str, const std::string &regex,
                                  const std::string &replacement);

};

#endif // STRINGUTIL_H
