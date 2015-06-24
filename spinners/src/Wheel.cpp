//
//  Wheel.cpp
//  emptyExample
//
//  Created by Andy Wallace on 6/10/15.
//
//

#include "Wheel.h"


void Wheel::setup(float _startingScale, int maxBand){
    
    radiusSize = 200;
    startingScale = _startingScale;
    scale = startingScale;
    
    scaleDecreasePerSecond = 0.45;
    
    outerAngle = ofRandom(TWO_PI);
    
    needleWidth = ofRandom(10, 100);
    needleHeight = ofRandom(50, 100);
    
    spinTimer = 0;
    spinVel = 0;
    isSpinning = false;
    
    minTimeBetweenSpins = 0.5;
    maxTimeBetweenSpins = 3;
    spinRangeMin = PI*0.2;
    spinRangeMax = PI;
    chanceOfPossitiveSpinDir = ofRandomuf();
    spinAcceleration = ofRandom(PI*0.0005, PI*0.002);
    
    spinTime = ofRandom(minTimeBetweenSpins, maxTimeBetweenSpins);
    
    
    fillCol.setHsb(ofRandom(256), 100, 230);
    
    fadeCol.setHsb(ofRandom(256), 100, 230);
    
    outlineCol.set(0,0,0);
    
    listeningBand = ofRandom(maxBand);
    
    
}

void Wheel::update(float _deltaTime, float _fftVa, float _beatVal, float spinSpeedAdjust, float spinTimeAdjust){
    deltaTime = _deltaTime;
    
    fftVal = CLAMP(_fftVa, 0 ,1);
    beatVal = _beatVal;
    
    scale -= scaleDecreasePerSecond * deltaTime;// * scaleDecreaseAdjust;
    
    spinTimer += deltaTime;
    
    //cout<<"spinning: "<<isSpinning<<"  timer: "<<spinTimer<<endl;
    
    if (isSpinning){
        
        spinVel += spinAcceleration * spinDir * spinSpeedAdjust;
        outerAngle += spinVel;
        
        if ( (spinDir > 0 && outerAngle > spinTargetAngle) || (spinDir < 0 && outerAngle < spinTargetAngle)){
            isSpinning = false;
            spinVel = 0;    //reset for next time;
        }
        
    }else{
        
        if (spinTimer >= spinTime){
            isSpinning = true;
            spinTimer = 0;
            
            spinDir = ofRandomuf() < chanceOfPossitiveSpinDir ? 1 : -1;
            
            
            spinTargetAngle = outerAngle + ofRandom(spinRangeMin, spinRangeMax) * spinDir;
            
            spinTime = ofRandom(minTimeBetweenSpins, maxTimeBetweenSpins) * spinTimeAdjust; //reset for next time
            
            //spinTimeToTarget = ofRandom(spinMinTimeToTarget, spinMaxTimeToTarget);
            
        }
        
    }
}

void Wheel::draw(){
    
    
    ofPushMatrix();
    
    //THIS COULD LARGELY BE HANDLED IN THE MAIN CLASS SINCE MOST OF THE MATH IS IDENTICAL FOR ALL CIRCLES
    
    float zTransWaveSpeed = 0.03;
    float zTransWaveCutoff = -0.5;
    float zTransMod = ofMap( cos(ofGetElapsedTimef()*zTransWaveSpeed), zTransWaveCutoff, 1, 0, 1, true);
    
    float maxZTranslate = 90 * zTransMod;
    float zTranslateNoiseSpeed = 0.1;
    float zTranslate = maxZTranslate * ofNoise(scale * 0.5, ofGetElapsedTimef() * zTranslateNoiseSpeed) - maxZTranslate/2;
    
    zTranslate += beatVal * 100;
    
    ofTranslate(0, 0, zTranslate);
    
    
    drawBackground();
    drawForeground();
    
    ofPopMatrix();
}

void Wheel::drawForeground(){
    
    
    ofPushMatrix();
    ofScale(scale, scale);
    
    ofRotate( ofRadToDeg(outerAngle) );
    
    ofFill();
    ofSetColor(outlineCol);
    
    float scaleWhereWeStartCaring = 25;
    
    float outlineWidth = ofMap( scale, scaleWhereWeStartCaring, 1, 0.5, 10, true);
    
    drawCircleOutline(0, 0, radiusSize-5, outlineWidth, 50);
    
    
    float needlePrc = CLAMP( 1 - scale/(scaleWhereWeStartCaring-1), 0, 1 );
    needlePrc = powf(needlePrc, 4);
    float needleHeightNow = needleHeight*needlePrc; //ofMap( scale, 5, 1, 0, needleHeight, true);
    
    ofRect(-needleWidth/2, radiusSize-needleHeightNow - 5, needleWidth, needleHeightNow);
    
    ofPopMatrix();
    
}

void Wheel::drawBackground(){
    
    ofPushMatrix();
    ofScale(scale, scale);
    
    ofFill();
    
    float colPrc = fftVal;
    float colR = fftVal * fillCol.r + (1-fftVal) * fadeCol.r;
    float colG = fftVal * fillCol.g + (1-fftVal) * fadeCol.g;
    float colB = fftVal * fillCol.b + (1-fftVal) * fadeCol.b;
    ofColor thisCol;
    thisCol.set(colR, colG, colB);
    
    ofSetColor(thisCol);
    
    ofCircle(0, 0, radiusSize);
    
    ofPopMatrix();
    
}


void Wheel::drawCircleOutline(float x, float y, float radius, float width, int resolution){
    
    float angleStep = TWO_PI/(float)resolution;
    
    ofBeginShape();
    
    for (int i=0; i<=resolution; i++){
        float angle = (float)i * angleStep;
        float xPos = x + cos(angle) * radius;
        float yPos = y + sin(angle) * radius;
        ofVertex(xPos, yPos);
    }
    
    for (int i=resolution; i>= 0; i--){
        float angle = (float)i * angleStep;
        float xPos = x + cos(angle) * (radius + width);
        float yPos = y + sin(angle) * (radius + width);
        ofVertex(xPos, yPos);
    }
    
    
    ofEndShape();
    
}






























