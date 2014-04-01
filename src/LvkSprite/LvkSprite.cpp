//
//  LvkSprite.cpp
//  LvkSpriteProject
//
//  Created by Andres Pagliano
//  Copyright 2012 LVK. All rights reserved.
//

#include "LvkSprite.h"
#include "LvkRepeatAction.h"
#include "LvkSpawn.h"
#include "LvkAnimate.h"
#include "common.h"
#include "StringUtil.h"

// Included in this repo:
#include "cocoa/CCNumber.h"
#include "cocoa/CCData.h"

#include <string>
#include <sstream>
#include <algorithm>

//#define USE_LVK_CUSTOM_CC     // Use Cocos2d with LVK extensions

const float LVK_SPRITE_FPS = 1.0/24.0;

//--------------------------------------------------------------------------------------------------
// LvkSprite
//--------------------------------------------------------------------------------------------------

LvkSprite* LvkSprite::create(CCString *binFile, LkobFormat format, CCString *infoFile)
{
    LvkSprite *spr = new LvkSprite();
    spr->initWithBinary(binFile, format, infoFile);
    spr->autorelease();
    return spr;
}

LvkSprite* LvkSprite::create(CCString *binFile, LkobFormat format, CCString *infoFile, CCArray *ids)
{
    LvkSprite *spr = new LvkSprite();
    spr->initWithBinary(binFile, format, infoFile, ids);
    spr->autorelease();
    return spr;
}

bool LvkSprite::init()
{
    return initWithBinary(NULL, LkobStandar, NULL);
}

bool LvkSprite::initWithBinary(CCString *binFile, LkobFormat format, CCString *infoFile)
{
    return initWithBinary(binFile, format, infoFile, NULL);
}

bool LvkSprite::initWithBinary(CCString *binFile, LkobFormat format, CCString *infoFile,
                               CCArray *ids)
{
    bool initOk = CCSprite::init();

    if (initOk) {
        _lkobFormat = format;

        _animations = CCDictionary::create();
        _animations->retain();

        loadBinary(binFile, format, infoFile, ids);

        _animation = NULL;
        _aniAction = NULL;
        _px = &(m_obPosition.x);
        _py = &(m_obPosition.y);
        _pw = &(m_obContentSize.width);
        _ph = &(m_obContentSize.height);
        _collisionThreshold = 0;

        _ss = NULL;
        _keyPrefix = NULL;

        setAnchorPoint(CCPoint(0, 1));

        //_linesIterator = NULL;
    }

    return initOk;
}


LvkSprite::~LvkSprite()
{
    CC_SAFE_RELEASE(_animation);
    CC_SAFE_RELEASE(_animations);
    delete _ss; //CC_SAFE_RELEASE(_linesIterator);
    CC_SAFE_RELEASE(_aniAction);
}


void LvkSprite::draw()
{
    CCSprite::draw();
}

// Returns the next line of data in file that we are currently parsing
CCString * LvkSprite::nextLine()
{
    CCString *line = NULL;

    std::string tmp;
    do {
        if (!std::getline(*_ss, tmp, '\n')) {
            line = NULL;
            break;
        }

        line = CCString::create(StringUtil::trim(tmp));

    } while (line->length() == 0 || StringUtil::StringUtil::hasPrefix(line->m_sString, "#"));

    return line;
}

bool LvkSprite::loadBinary(CCString *binFile, LkobFormat format, CCString *infoFile)
{
    if (binFile == NULL || infoFile == NULL) {
        return false;
    }

    _animations->removeAllObjects();

    return loadBinary(binFile, format, infoFile, NULL);
}

bool LvkSprite::loadBinary(CCString *binFile, LkobFormat format, CCString *infoFile, CCArray *ids)
{
    if (binFile == NULL || infoFile == NULL) {
        return false;
    }

    setkeyPrefix(infoFile);
    _lkobFormat = format;

    // load bin file -------------------------------

    CCData *binData = CCData::createWithContentsOfFile(binFile->getCString());
    if (!binData) {
        LKLOG("LvkSprite - ERROR Error opening binary file: %s", binFile->getCString());
        return false;
    }

    // Load info file ------------------------------

    CCString * infoData = CCString::createWithContentsOfFile(infoFile->getCString());
    if (!infoData) {
        LKLOG("LvkSprite - ERROR Error opening info file: %s", infoFile->getCString());
        return false;
    }

    _ss = new std::stringstream(infoData->getCString());

    // Parse file ----------------------------------

    CCDictionary *frames = CCDictionary::create();

    CCString *line = NULL;

    while ( (line = nextLine()) ) {
        CCAutoreleasePool *pool = new CCAutoreleasePool();

        bool ok = true;

        // parsing frames
        if (StringUtil::hasPrefix(line->m_sString, "fpixmaps(")) {
            if (!parseFpixmaps(frames, binData)) {
                LKLOG("LvkSprite - ERROR cannot parse fpixmaps section.");
                ok = false;
            }
        } else if (StringUtil::hasPrefix(line->m_sString, "animations(")) {
            if (!parseAnimations(frames, ids)) {
                LKLOG("LvkSprite - ERROR cannot parse animations section.");
                ok = false;
            }
// FIXME implement this:
//		} else {
//			LKLOG("LvkSprite - ERROR cannot parse line '%@'", line);
//			ok = false;
        }

        delete pool;

        if (!ok) {
            return false;
        }
    }

    // Create a Null animation --------------------

    CCAnimation *nullAnim = CCAnimation::create();
    nullAnim->setDelayPerUnit(LVK_SPRITE_FPS);
    _animations->setObject(nullAnim, "NullAnimation");

#ifdef LVKSPRITELOG
    LKLOG(" LvkSprite - Load binary success!");
#endif

    return true;
}

std::string LvkSprite::buildKeyWithFrameId(const std::string &frameId)
{
    std::string prefix = _keyPrefix != NULL ? _keyPrefix->m_sString : std::string("Null");

    return prefix + "_" + frameId;
}

bool LvkSprite::parseFpixmaps(CCDictionary* frames, CCData *binData)
{
    CCString *line = NULL;

    for (line = nextLine(); line != NULL && !StringUtil::hasPrefix(line->m_sString, ")"); line = nextLine()) {

        // line format "frameId,offset,length"
        std::vector<std::string> lineInfo = StringUtil::split(line->m_sString, ',');

        if (lineInfo.size() < 3) {
            LKLOG("LvkSprite - Error: parsing line %s", line->getCString());
            continue;
        }

        const std::string & frameId = lineInfo[0];
        uint offset = StringUtil::value<uint>(lineInfo[1]); // No CCUInteger type, using uint
        uint length = StringUtil::value<uint>(lineInfo[2]); // No CCUInteger type, using uint

#ifdef LVKSPRITELOG
        LKLOG(" LvkSprite - Parsing frame: %s,%i,%i", frameId.c_str(), offset, length);
#endif

        CCData *data = binData->subdataWithRange(offset, length);
        std::string key = buildKeyWithFrameId(frameId);

        frames->setObject(data, key);
    }

    return line != NULL;
}

bool LvkSprite::parseAnimations(CCDictionary* frames, CCArray *ids)
{
    CCString *line = NULL;

    for (line = nextLine(); line != NULL && !StringUtil::hasPrefix(line->m_sString, ")"); line = nextLine()) {

        // line format "animId,animName"
        //             "aframes("
        //             "    aframeId,frameId,delay,ox,oy"
        //             ")"

        std::vector<std::string> lineInfo = StringUtil::split(line->m_sString, ',');

        if (lineInfo.size() < 2) {
            LKLOG(" LvkSprite - Error: Ill-formed sprite file: expected animId,animName but"
                  " got '%s'", line->getCString());
            continue;
        }

        CCString *animationId = CCString::create(lineInfo[0]);
        CCString *animationName = CCString::create(lineInfo[1]);

        // if ids if not nil, we have an array with the list of animation ids to parse,
        // otherwise we parse all animations
        if (ids != NULL) {
            bool found = false;
            CCNumber *currentId = CCNumber::create(StringUtil::value<int>(animationId->m_sString));
            for (unsigned i = 0; i < ids->count(); ++i) {
                CCNumber *id_ = dynamic_cast<CCNumber *>(ids->objectAtIndex(i));
                if (id_ && id_->isEqualToNumber(currentId)) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                // If not found, ommit the whole section aframes and continue with next animation
                for (line = nextLine(); line != NULL && !StringUtil::hasPrefix(line->m_sString, ")");
                     line = nextLine());
                continue;
            }
        }

#ifdef LVKSPRITELOG
        LKLOG(" LvkSprite - Parsing animation: %s %s", animationId->getCString(),
              animationName->getCString());
#endif

        line = nextLine();
        if (!StringUtil::hasPrefix(line->m_sString, "aframes(")) {
            LKLOG(" LvkSprite - Error: Ill-formed sprite file: aframes(...) section expected but"
                  " got '%s'", line->getCString());
            return false;
        }

        parseAframes(frames, animationName, animationId);
    }

    return line != NULL;
}

bool LvkSprite::parseAframes(CCDictionary* frames, CCString* animName, CCString * animId)
{
    // TODO refactor this class!

    // Frames can be "Common" or "Sticky". Sticky frames are frames that are displayed always.
    // To implement sticky frames we have one animation per sticky that only renders that frame.
    // Finally we merge all sticky animations with the main animation to achieve the desired effect.
    CCAnimation *anim = new CCAnimation();  // main animation
    anim->init();
    anim->setDelayPerUnit(LVK_SPRITE_FPS);

    CCArray *stickyAnims = new CCArray();   // array of sticky animations
    stickyAnims->init();

    int frameCount = 1;
    float timeCount = 0;

    CCString *line = NULL;
 
   // aframes parsing
    for (line = nextLine(); line != NULL && !StringUtil::hasPrefix(line->m_sString, ")"); line = nextLine()) {

        CCAutoreleasePool * subpool = new CCAutoreleasePool();

        // line format "<aframeId,frameId,delay>[,ox,oy][,sticky]"

        std::vector<std::string> lineInfo = StringUtil::split(line->m_sString, ',');

        if (lineInfo.size() < 3) {
            LKLOG("LvkSprite - Error: Ill-formed sprite file: expected aframe but got '%s'."
                  " Line omitted", line->getCString());
            continue;
        }


        std::string frameId = lineInfo[1];
        float duration = StringUtil::value<float>(lineInfo[2]);
        int ox = 0;
        int oy = 0;
        bool isSticky = false;

        if (lineInfo.size() > 4) { // Since LvkSprite format 0.2
            ox = StringUtil::value<int>(lineInfo[3]);
            oy = StringUtil::value<int>(lineInfo[4]);
            oy *= -1; // because the Lvk Sprite Tool has the origin at top-left corner
        }
        if (lineInfo.size() > 5) { // Since LvkSprite format 0.4
            isSticky = StringUtil::value<int>(lineInfo[5]);
        }

        std::string frameKey = buildKeyWithFrameId(frameId);

        if (!isSticky) {
            timeCount += duration*0.001;

            while (frameCount*LVK_SPRITE_FPS < timeCount) {
                if (_lkobFormat == LkobStandar) {
                    addAframeWithKey(anim, frames, frameKey, CCPoint(ox, oy));
                } else {
                    // Not supported
                }
                frameCount++;
            }
        } else {
            // Create an animation that contains only one frame. This animation will be merged
            // with the main animation.
            CCAnimation *stickyAnim = CCAnimation::create();

            addAframeWithKey(stickyAnim, frames, frameKey, CCPoint(ox, oy));

            stickyAnims->addObject(stickyAnim);
        }

        delete subpool;
    }

    // main animation
    CCActionInterval *animAction = LvkAnimate::create(anim);

    // If there is a sticky frame, merge (or "spawn" using cocos jargon) sticky animations with
    // the animation

    for (uint i = 0; i < stickyAnims->count(); ++i) {
        CCAnimation *stickyAnim = (CCAnimation *)stickyAnims->objectAtIndex(stickyAnims->count() - i - 1);
        stickyAnim->setDelayPerUnit(animAction->getDuration());
        CCActionInterval *stickyAnimAction = LvkAnimate::create(stickyAnim);

        animAction = LvkSpawn::create(animAction, stickyAnimAction);

        // FIXME!!!!!!!!!! Nasty workaround to avoid flickering
        if (i == 0) {
            ((LvkSpawn *)animAction)->setnested(false);
        }
    }

    _animations->setObject(animAction, animName->m_sString);

    stickyAnims->release();
    anim->release();

    return line != NULL;
}


void LvkSprite::addAframeWithKey(CCAnimation *anim, CCDictionary* frames,
                                 const std::string frameKey, const CCPoint &offset)
{
#ifdef USE_LVK_CUSTOM_CC
    CCData *data = dynamic_cast<CCData *>(frames->objectForKey(frameKey));

    if (data) {
        anim->addFrameContentWithKey(data->bytes(), data->length(), frameKey.c_str(),
                                     CCPoint(ox, oy));
    } else {
        LKLOG(" LvkSprite - Error: no frame data for key '%s'", frameKey.c_str());
    }

#else //USE_LVK_CUSTOM_CC
    CCTexture2D *tex = CCTextureCache::sharedTextureCache()->textureForKey(frameKey.c_str());

    if (!tex) {
        CCData *data = dynamic_cast<CCData *>(frames->objectForKey(frameKey));
        if (data) {
            CCImage *img = new CCImage();
            img->initWithImageData(data->bytes(), data->length(), CCImage::kFmtPng);
            img->autorelease();
            tex = CCTextureCache::sharedTextureCache()->addUIImage(img, frameKey.c_str());
        }
    }

    if (tex) {
        CCRect rect = CCRectZero;
        rect.size = tex->getContentSize();
        CCRect rectInPixels = CC_RECT_POINTS_TO_PIXELS(rect);
        CCSpriteFrame *frame = CCSpriteFrame::createWithTexture(tex, rect, false, offset, rectInPixels.size);
        anim->addSpriteFrame(frame);
    } else {
        LKLOG(" LvkSprite - Error: no frame data for key '%s'", frameKey.c_str());
    }
#endif //USE_LVK_CUSTOM_CC
}

// Helper function to build CCActions from the list of animation names
CCArray *LvkSprite::getActionsFromAnimationList(CCArray *names, CCArray *ns)
{
    CCArray *actions = CCArray::createWithCapacity(names->count());

    for (unsigned i = 0; i < names->count(); ++i) {
        CCString *name = dynamic_cast<CCString *>(names->objectAtIndex(i));

        if (!name) {
            LKLOG(" LvkSprite - Skipping null animation name at index %u", i);
            continue;
        }

        int n = 1; // By default repeat 1

        if (ns && ns->count() > i) {
            if (CCNumber * numb = dynamic_cast<CCNumber*>(ns->objectAtIndex(i))) {
                n = numb->intValue();
            } else {
                LKLOG(" LvkSprite - Error wrong CCNumber at index %u", i);
            }
        }

        CCActionInterval *anim =
                dynamic_cast<CCActionInterval *>(_animations->objectForKey(name->getCString()));

        if (anim) {
            CCAction *action = NULL;

            if (n == 1) {
                action = anim;
            } else if (n > 1) {
                action = LvkRepeatAction::create(anim, n);
            } else if (n == -1) {
                // Cannot use CCRepeatForever because this array of CCActions is used by CCSequence.
                // CCSequence requires all CCActions to be CCFiniteAction and CCRepeatForever is
                // not.
                //action = [CCRepeatForever actionWithAction:[LvkAnimate actionWithAnimation:anim]];
                // NSUIntegerMax/10 should be enough for most purposes. Not using NSUIntegerMax
                // because I guess there is a risk of overflow at some point.
                action = LvkRepeatAction::create(anim, UINT_MAX/100);
            }

            if (action) {
                actions->addObject(action);
            } else {
                LKLOG(" LvkSprite - Could not create action from anim '%s'", name->getCString());
            }
        } else {
            LKLOG(" LvkSprite - animation '%s' does not exist", name->getCString());
        }
    }

    return actions;
}

void LvkSprite::_playAction(CCFiniteTimeAction *action, int n)
{
    stopAnimation();
    setAniAction(NULL);

    if (n == 1) {
        setAniAction(action);
    } else if (n > 1) {
        setAniAction(LvkRepeatAction::create(action, n));
    } else if (n == -1) {
        // Warning! this cast can be dangerous!
        setAniAction(CCRepeatForever::create(reinterpret_cast<CCActionInterval*>(action)));
    }

    if (_aniAction) {
        runAction(getAniAction());
    }
}

void LvkSprite::_playAnimations(CCArray *names, CCArray *ns, float x, float y)
{
    setPosition(CCPoint(x, y));

    CCFiniteTimeAction *seq = CCSequence::create(getActionsFromAnimationList(names, ns));

    _playAction(seq, 1);
}

void LvkSprite::_playAnimation(CCString *name, float x, float y, int n)
{
    setPosition(CCPoint(x, y));

    CCActionInterval *anim =
            dynamic_cast<CCActionInterval *>(_animations->objectForKey(name->getCString()));

    if (anim) {
        _playAction(anim, n);
    } else {
        LKLOG(" LvkSprite - animation '%s' does not exist", name->getCString());
    }
}

void LvkSprite::playAnimation(CCString *name, float x, float y, int n)
{
    _playAnimation(name, x, y, n);
}

void LvkSprite::playAnimation(CCString *name, int n)
{
    _playAnimation(name, getPosition().x, getPosition().y, n);
}

void LvkSprite::playAnimation(CCString *name)
{
    _playAnimation(name, getPosition().x, getPosition().y, -1);
}

void LvkSprite::playAnimations(CCArray *names, CCArray *ns, float x, float y)

    _playAnimations(names, ns, x, y);
}

void LvkSprite::playAnimations(CCArray *names, CCArray *ns)
{
    _playAnimations(names, ns, getPosition().x, getPosition().y);
}

void LvkSprite::stopAnimation()
{
    if (getAniAction() && !getAniAction()->isDone()) {
        stopAction(getAniAction());
        setAniAction(NULL);
    }
}

bool LvkSprite::animationHasEnded()
{
   return !getAniAction() || getAniAction()->isDone();
}

bool LvkSprite::hasAnimation(CCString *name)
{
    return _animations->objectForKey(name->getCString()) != NULL;
}

void LvkSprite::setX(float x)
{
    m_bDirty = true;
    *_px = x;
}

float LvkSprite::x()
{
    return *_px;
}

void LvkSprite::setY(float y)
{
    m_bDirty = true;
    *_py = y;
}

float LvkSprite::y()
{
    return *_py;
}

void LvkSprite::setDx(float dx)
{
    m_bDirty = true;
    *_px += dx;
}

void LvkSprite::setDy(float dy)
{
    m_bDirty = true;
    *_py += dy;
}

void LvkSprite::setDxDy(float dx, float dy)
{
    m_bDirty = true;
    *_px += dx;
    *_py += dy;
}

void LvkSprite::moveX(float /*x*/, float /*vel*/)
{
/* [cci2ccx]

	//TODO
*/
}

void LvkSprite::moveY(float /*y*/, float /*vel*/)
{
/* [cci2ccx]

	//TODO
*/
}

void LvkSprite::moveXY(float /*x*/, float /*y*/, float /*vel*/)
{
/* [cci2ccx]

	//TODO
*/
}

void LvkSprite::moveDx(float /*dx*/, float /*vel*/)
{
/* [cci2ccx]

	//TODO
*/
}

void LvkSprite::moveDy(float /*dy*/, float /*vel*/)
{
/* [cci2ccx]

	//TODO
*/
}

void LvkSprite::moveDxDy(float /*dx*/, float /*dy*/, float /*vel*/)
{
/* [cci2ccx]

	//TODO
*/
}

bool LvkSprite::moveHasEnded()
{
    return false;
}

CCRect LvkSprite::rect()
{
    return CCRect(*_px, *_py, *_pw, *_ph);
}

CCRect LvkSprite::collisionRect()
{
    return CCRect(*_px - _collisionThreshold,
                  *_py - _collisionThreshold,
                  *_pw + _collisionThreshold,
                  *_ph + _collisionThreshold);
}

bool LvkSprite::collidesWithSprite(LvkSprite *spr)
{
    return collisionRect().intersectsRect(spr->collisionRect());
}

bool LvkSprite::collidesWithPoint(const CCPoint &point)
{
    return collisionRect().containsPoint(point);
}

bool LvkSprite::collidesWithRect(const CCRect &rect)
{
    return collisionRect().intersectsRect(rect);
}


