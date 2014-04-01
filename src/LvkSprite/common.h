/*
 *  common.h
 *  igirl
 *
 *  Created by Mario Tambos on 7/4/10.
 *  Copyright 2010 LavandaInk. All rights reserved.
 *
 */

//[objc2cpp] #import "config.h"
#include "config.h"

#define STR(s) #s

#ifdef LKLOG_ENABLED
#  define LKLOG(...) CCLOG(__VA_ARGS__)
#else
#  define LKLOG(...)
#endif

#ifdef LKTRACE_ENABLED
#  define LKTRACE(...) CCLOG(__VA_ARGS__)
#else
#  define LKTRACE(...)
#endif

#define RESOURCE_PATH(name, ext) (name ext)

#define RAND(min, max)    ((min) == (max) ? (min) : ((int)((min) + rand()/(double)(RAND_MAX - 1)*((max) - (min) + 1))))

#define VAR_USERNAME         "$USERNAME"
#define VAR_USERNAME_REGEX   "\\$USERNAME"



#define CC_SYNTHESIZE_PRIVATE(varType, varName, funName)\
private: varType varName;\
private: virtual varType get##funName(void) const { return varName; }\
private: virtual void set##funName(varType var){ varName = var; }

#define CC_SYNTHESIZE_PASS_BY_REF_PRIVATE(varType, varName, funName)\
private: varType varName;\
private: virtual const varType& get##funName(void) const { return varName; }\
private: virtual void set##funName(const varType& var){ varName = var; }

#define CC_SYNTHESIZE_RETAIN_PRIVATE(varType, varName, funName)    \
private: varType varName; \
private: virtual varType get##funName(void) const { return varName; } \
private: virtual void set##funName(varType var)   \
{ \
    if (varName != var) \
    { \
        CC_SAFE_RETAIN(var); \
        CC_SAFE_RELEASE(varName); \
        varName = var; \
    } \
}

#define ccTime float

#define ABS(x)  ((x) >= 0 ? (x) : -(x))

#define CC_FOREACH_ARRAY(ElemType, ElemName, Container)\
    ElemType ElemName = 0;\
    for (unsigned i = 0; \
         Container && i < (Container)->count() && (ElemName = dynamic_cast<ElemType>((Container)->objectAtIndex(i)));\
         ++i)
