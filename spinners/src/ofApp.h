#pragma once

#include "ofMain.h"

#include "Wheel.h"

#include "ofxFFTLive.h"
#include "ofxFFTFile.h"

#include "ofxControlPanel.h"

class ofApp : public ofBaseApp{
public:
    void setup();
    void reset();
    void setupPanel();
    void update();
    void updateWheels();
    void draw();
    void drawFFT();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

    void spawnWheel();
    
    
    float deltaTime, prevFrameTime;
    
    vector<Wheel> wheels;
    int maxBandToUseForWheels;
    float scaleToKillWheel;
    
    float wheelStartingScale;
    float scaleToSpawnNextWheel;
    
    float wheelSpinSpeedAdjust, wheelSpinTimeAdjust;
    
    float startingNoiseOffset;
    
    //fft stuff
#define DO_IT_LIVE
    
#ifdef DO_IT_LIVE
    ofxFFTLive fftLive;
#else
    ofxFFTFile fftLive;
#endif
    
    bool useSound;
    ofSoundPlayer soundPlayer;
    
    int numBands;
    vector<float> fftData, prevFfftData;
    float curSensitivity;
    
    vector<float> beatVals;
    float beatDecay;
    float minJumpForBeat;
    
    float beatJumpValToCountAsMax;
    float minBeatVal;
    
    
    //control panel
    ofxControlPanel	panel;
    
    //debug/demo
    bool showFFT, showPanel;
};






/*
 * ORIGINAL VALUES
 *
 
 panel.addPanel("wheels", 1, false);
 panel.setWhichPanel("wheels");
 panel.setWhichColumn(0);
 
 panel.addSlider("wheel spawn rate base", "WHEEL_SPAWN_RATE_BASE", 1, 8, 0, false);
 panel.addSlider("wheel spawn rate range", "WHEEL_SPAWN_RATE_RANGE", 3, 8, 0, false);
 
 panel.addSlider("wheel spin speed adjust", "WHEEL_SPIN_SPEED_ADJUST", 1, 0, 5, false);
 panel.addSlider("wheel spin time adjust", "WHEEL_SPIN_TIME_ADJUST", 1, 0, 5, false);
 
 panel.addSlider("time scale", "TIME_SCALE", 1, 0, 5, false);
 
 
 panel.addPanel("fft", 1, false);
 panel.setWhichPanel("fft");
 panel.setWhichColumn(0);
 
 panel.addSlider("sensitivity", "SENSITIVITY", 1, 0, 3, false);
 panel.addSlider("peak decay speed", "PEAK_DECAY", 0.99, 0.9, 1, false);
 
 panel.addSlider("beat decay", "BEAT_DECAY", 0.9, 0, 1, false);
 panel.addSlider("min jump for beat", "MIN_BEAT_JUMP", 0.25, 0, 1, false);
 panel.addSlider("beat jump to count as max", "BEAT_JUMP_COUNT_MAX", 0.7, 0, 1, false);
 panel.addSlider("min beat val", "MIN_BEAT_VAL", 0.5, 0, 1, false);
 
 panel.addSlider("bands to use", "BANDS_TO_USE", 14, 1, numBands, true);
 
 panel.addPanel("camera", 1, false);
 panel.setWhichPanel("camera");
 panel.setWhichColumn(0);
 
 panel.addSlider("base zoom", "BASE_ZOOM", 0.8, 0, 1.5, false);
 panel.addSlider("zoom range", "ZOOM_RANGE", 0.55, 0, 1, false);
 panel.addSlider("zoom speed", "ZOOM_SPEED", 0.06, 0, 0.2, false);
 
 panel.addSlider("cam angle range", "CAM_ANGLE_RANGE", 60, 0, 180, false);
 panel.addSlider("cam angle speed", "CAM_ANGLE_SPEED", 0.02, 0, 0.5, false);
 
 panel.addSlider("cam move range", "CAM_MOVE_RANGE", 400, 0, 1000, false);
 panel.addSlider("cam move speed", "CAM_MOVE_SPEED", 0.01, 0, 0.15, false);
 */
