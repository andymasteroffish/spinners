//
//  Wheel.h
//  emptyExample
//
//  Created by Andy Wallace on 6/10/15.
//
//

#ifndef __emptyExample__Wheel__
#define __emptyExample__Wheel__

#include "ofMain.h"

class Wheel{
public:
    
    void setup(float _startingScale, int maxBand);
    void update(float _deltaTime, float _fftVal, float _beatVal, float spinSpeedAdjust, float spinTimeAdjust);
    void draw();
    void drawForeground();
    void drawBackground();
    
    void drawCircleOutline(float x, float y, float radius, float width, int resolution);
    
    
    float deltaTime;
    
    float startingScale;
    float scale;
    
    float scaleDecreasePerSecond;
    
    float radiusSize;
    
    float outerAngle;
    float needleWidth, needleHeight;
    
    float spinTimer, spinTime;
    bool isSpinning;
    float spinTargetAngle;
    
    float minTimeBetweenSpins, maxTimeBetweenSpins;
    float spinRangeMin, spinRangeMax;
    float chanceOfPossitiveSpinDir;
    
    int spinDir;
    float spinVel;
    float spinAcceleration;
    
    ofColor fillCol, fadeCol;
    ofColor outlineCol;
    
    
    //fft stuff
    int listeningBand;
    float fftVal;
    
    float beatVal;
    
    
    
};

#endif /* defined(__emptyExample__Wheel__) */
