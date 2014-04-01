#ifndef _LVKSPRITE_H_
#define _LVKSPRITE_H_
//
//  LvkSprite.h
//  LvkSpriteProject
//
//  Created by Andres Pagliano
//  Copyright 2012 LVK. All rights reserved.
//

#include "cocos2d.h"
#include "common.h"

typedef enum {
    LkobStandar
#ifdef LVKSPRITE_PVR_ENABLED
    ,LkobPVRTC // Experimental
#endif
} LkobFormat;

using namespace cocos2d;

namespace cocos2d
{
    class CCData;
}

/// This class loads and plays sprites created with the
/// Lvk Sprite Animation Tool. The Lvk Sprite format 
/// consists of two files: the binary file (*.lkob) and 
/// the text file (*.lkot)
/// The first one contains images of each frame, and  
/// the second one contains how to play the frames
class LvkSprite : public CCSprite
{
public:
    ///
    bool init();

    /// Initializes an instance of the class using a Lvk Sprite
    /// @param bin: the lvk sprite binary file (usually *.lkob)
    /// @param format: the lvk sprite binary format (Standar or PVRTC)
    /// @param info: the lvk sprite information file (usually *.lkot)
    /// @returns an initialized instance of LvkSprite
    bool initWithBinary(CCString *binFile, LkobFormat format, CCString *infoFile);

    // Idem but only loads the given animation ids
    bool initWithBinary(CCString *binFile, LkobFormat format, CCString *infoFile, CCArray *ids);

    /// Creates aun autoreleased instance of the class using a Lvk Sprite
    /// @param bin: the lvk sprite binary file (usually *.lkob)
    /// @param format: the lvk sprite binary format (Standar or PVRTC)
    /// @param info: the lvk sprite information file (usually *.lkot)
    /// @returns an initialized instance of LvkSprite
    static LvkSprite* create(CCString *binFile, LkobFormat format, CCString *infoFile);

    // Idem but only loads the given animation ids
    static LvkSprite* create(CCString *binFile, LkobFormat format, CCString *infoFile,
                             CCArray *ids);

    /// Destroys the object
    ~LvkSprite();

    /// Loads a Lvk Sprite
    /// @param bin: the lvk sprite binary file (usually *.lkob)
    /// @param format: the lvk sprite binary format (Standar or PVRTC)
    /// @param info: the lvk sprite information file (usually *.lkot)
    /// @returns TRUE if it loads and parses the files successfully,
    ///          FALSE otherwise
    bool loadBinary(CCString *binFile, LkobFormat format, CCString *infoFile);

    // Idem but only loads the given animation ids
    bool loadBinary(CCString *binFile, LkobFormat format, CCString *infoFile, CCArray *ids);

    /// Plays n times the given animation at the given position
    /// @param name: the animation name
    /// @param x: the x position of the animation
    /// @param y: the y position of the animation
    /// @param n: repeat n times the animation, -1 repeats forever
    void playAnimation(CCString *name, float x, float y, int n);

    /// Plays n times the given animation list
    /// @param names: the list of animation names
    /// @param n: repeat n times the animation, -1 repeats forever
    void playAnimation(CCString *name, int n);

    /// Plays forever the given animation
    /// @param name: the animation name
    void playAnimation(CCString *name);

    /// Plays n times the given animation list at the given position
    /// @param names: the list of animation names
    /// @param ns: the list of repetitions for each animation. If nil it plays once.
    /// @param x: the x position of the animation
    /// @param y: the y position of the animation
    void playAnimations(CCArray *names, CCArray *ns, float x, float y);

    /// Plays n times the given animation list
    /// @param names: the list of animation names
    /// @param ns: the list of repetitions for each animation. If nil it plays once.
    void playAnimations(CCArray *names, CCArray *ns);

    /// Stops the current animation
    void stopAnimation();

    /// Returns if the animation has ended
    /// @returns TRUE if the animation has ended, FALSE otherwise.
    bool animationHasEnded();

    /// Returns if the given animation name exists in the sprite
    /// @returns TRUE if the animation exists, FALSE otherwise.
    bool hasAnimation(CCString *name);

    /// Changes the position of the sprite in the x axis relative to the current position
    /// @param dx: the x offset
    void setDx(float dx);

    /// Changes the position of the sprite in the y axis relative to the current position
    /// @param dy: the y offset
    void setDy(float dy);

    /// Changes the position of the sprite in the x and y axis relative to the current
    /// position
    /// @param dx: the x offset
    /// @param dy: the y offset
    void setDxDy(float dx, float dy);

    /// Moves the sprite at the given velocity until reaches the given x position
    /// @param x: the final x position
    /// @param vel: the velocity (in pixels/frame)
    ///
    /// TODO: implement
    void moveX(float x, float vel);

    /// Moves the sprite at the given velocity until reaches the given y position
    /// @param y: the final y position
    /// @param vel: the velocity (in pixels/frame)
    ///
    /// TODO: implement
    void moveY(float y, float vel);

    /// Moves the sprite at the given velocity until reaches the given (x, y) position
    /// @param x: the final x position
    /// @param y: the final y position
    /// @param vel: the velocity (in pixels/frame)
    ///
    /// TODO: implement
    void moveXY(float x, float y, float vel);

    void moveDx(float dx, float vel);

    void moveDy(float dy, float vel);

    void moveDxDy(float dx, float dy, float vel);

    bool moveHasEnded();

    bool collidesWithSprite(LvkSprite *spr);

    bool collidesWithPoint(const CCPoint &point);

    bool collidesWithRect(const CCRect &rect);

    void draw();

    void setX(float x);

    float x();

    void setY(float y);

    float y();

    CCDictionary* animations();

    CCRect rect();

    CCRect collisionRect();

    float collisionThreshold();

private:
    // does the binary file contain PVRTC images
    LkobFormat _lkobFormat;
    // a dictionary mapping animationName --> cocosAnimation
    CCDictionary *_animations;
    //
    CCDictionary *_stickyAnimations;
    // current animation name
    CCString *_animation;
    // pointer to the x position
    float *_px;
    // pointer to the y position
    float *_py;
    // pointer to the sprite width
    float *_pw;
    // pointer to the sprite height
    float *_ph;
    // collision threshold
    float _collisionThreshold;


    /* [cci2ccx]

    @interface LvkSprite ()

    @property (readwrite, retain) NSMutableDictionary* animationsInternal;
    @property (readwrite, retain) NSEnumerator* linesIterator;
    @property (readwrite, retain) CCActionInterval* aniAction;
    @property (readwrite, retain) NSString* keyPrefix;

    - (NSString*) buildKeyWithFrameId:(NSString*)frameId;
    - (BOOL) parseFpixmaps:(NSMutableDictionary*)frames withBinData:(NSData *)binData;
    - (BOOL) parseAnimations:(NSMutableDictionary*)frames ids:(NSArray *)ids;
    - (BOOL) parseAframes:(NSMutableDictionary*)frames animName:(NSString*)animName animId:(NSString *)animId;

    @end
    */

    std::stringstream *_ss;    // Replacement of NSEnumerator* linesIterator;

    CC_SYNTHESIZE_RETAIN_PRIVATE(CCFiniteTimeAction*, _aniAction, AniAction)
    CC_SYNTHESIZE_RETAIN_PRIVATE(CCString*, _keyPrefix, keyPrefix)

    std::string buildKeyWithFrameId(const std::string &frameId);
    CCString* nextLine();
    bool parseFpixmaps(CCDictionary* frames, CCData *binData);
    bool parseAnimations(CCDictionary* frames, CCArray *ids);
    bool parseAframes(CCDictionary* frames, CCString* animName, CCString * animId);
    void addAframeWithKey(CCAnimation *anim, CCDictionary* frames,
                          const std::string frameKey, const CCPoint &offset);

    CCArray *getActionsFromAnimationList(CCArray *names, CCArray *ns);
    void _playAction(CCFiniteTimeAction *action, int n);
    void _playAnimations(CCArray *names, CCArray *ns, float x, float y);
    void _playAnimation(CCString *name, float x, float y, int n);
};


#endif //_LVKSPRITE_H_

