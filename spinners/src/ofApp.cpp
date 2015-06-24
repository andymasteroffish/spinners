#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
#ifdef DO_IT_LIVE
    useSound = false;
#else
    useSound = true;
#endif

    prevFrameTime = ofGetElapsedTimef();
    
    ofBackground(0,0,0);
    
    wheelStartingScale = 30;
    scaleToKillWheel = 0.02;
    
    scaleToSpawnNextWheel = 0;
    
    //settup fft
    fftLive.setMirrorData(false);
    fftLive.setup();
    
    if (useSound){
        soundPlayer.loadSound("getlucky.mp3");
        soundPlayer.setLoop(true);
    }
    
    int bufferSize = 64;
    fftLive.setBufferSize(bufferSize);
    numBands = bufferSize/4;
    
    for (int i=0; i<numBands; i++){
        beatVals.push_back(1);
    }
    
    showFFT = false;
    showPanel = false;
    
    setupPanel();
    
    //fill it up with some dummy values!
    reset();
    
}

void ofApp::reset(){
    deltaTime = 0.1;
    for (int i=0; i<2000; i++){
        updateWheels();
    }
    
    startingNoiseOffset = ofRandom(99999);
}

void ofApp::setupPanel(){
    panel.setup("settings", ofGetWidth()-310, 0, 300, 1000);
    
    panel.addPanel("stuff", 1, false);
    panel.setWhichPanel("stuff");
    //panel.panels[panel.currentPanel]
    panel.setWhichColumn(0);
    
    panel.addLabel("Basic");
    panel.addSlider("sensitivity", "SENSITIVITY", 1, 0, 3, false);
    panel.addSlider("base time scale", "TIME_SCALE", 1, 0, 5, false);
    
    panel.addLabel("Wheels");
    panel.addSlider("wheel spawn rate base", "WHEEL_SPAWN_RATE_BASE", 0.5, 5, 0, false);
    panel.addSlider("wheel spawn rate range", "WHEEL_SPAWN_RATE_RANGE", 2.13, 8, 0, false);
    
    panel.addSlider("wheel spin speed adjust", "WHEEL_SPIN_SPEED_ADJUST", 1, 0, 5, false);
    panel.addSlider("wheel spin time adjust", "WHEEL_SPIN_TIME_ADJUST", 1, 0, 5, false);
    
    panel.addLabel("Camera");
    panel.addSlider("base zoom", "BASE_ZOOM", 0.6, 0, 1.5, false);
    panel.addSlider("zoom range", "ZOOM_RANGE", 0.55, 0, 1, false);
    panel.addSlider("zoom speed", "ZOOM_SPEED", 0.08, 0, 0.2, false);
    
    panel.addSlider("cam angle range", "CAM_ANGLE_RANGE", 60, 0, 180, false);
    panel.addSlider("cam angle speed", "CAM_ANGLE_SPEED", 0.03, 0, 0.5, false);
    
    panel.addSlider("cam move range", "CAM_MOVE_RANGE", 700, 0, 1000, false);
    panel.addSlider("cam move speed", "CAM_MOVE_SPEED", 0.02, 0, 0.15, false);
    
    panel.addLabel("FFT");
    panel.addSlider("peak decay speed", "PEAK_DECAY", 0.99, 0.9, 1, false);
    panel.addSlider("beat decay", "BEAT_DECAY", 0.9, 0, 1, false);
    panel.addSlider("min jump for beat", "MIN_BEAT_JUMP", 0.2, 0, 1, false);
    panel.addSlider("beat jump to count as max", "BEAT_JUMP_COUNT_MAX", 0.7, 0, 1, false);
    panel.addSlider("min beat val", "MIN_BEAT_VAL", 0.65, 0, 1, false);
    panel.addSlider("bands to use", "BANDS_TO_USE", 14, 1, numBands, true);
    
    panel.addLabel("Buttons");
    panel.addToggle("[R] - reset", "RESET", false);
    panel.addToggle("[D] - show FFT", "SHOW_FFT", false);
    panel.addToggle("[F] - toggle full screen", "FULLSCREEN", false);
    panel.addLabel("[S] to hide settings");
    
    
    
    //panel.loadSettings("cvSettings.xml");
}

//--------------------------------------------------------------
void ofApp::update(){
    
    float timeScale = panel.getValueF("TIME_SCALE");
    deltaTime = (ofGetElapsedTimef() - prevFrameTime) * timeScale;
    prevFrameTime = ofGetElapsedTimef();
    
    //update the panel
    panel.update();
    
    //get some values
    fftLive.setPeakDecay( panel.getValueF("PEAK_DECAY") );
    curSensitivity = panel.getValueF("SENSITIVITY");
    
    beatDecay = panel.getValueF("BEAT_DECAY");
    minJumpForBeat = panel.getValueF("MIN_BEAT_JUMP");
    beatJumpValToCountAsMax = panel.getValueF("BEAT_JUMP_COUNT_MAX");
    minBeatVal = panel.getValueF("MIN_BEAT_VAL");
    
    maxBandToUseForWheels = panel.getValueI("BANDS_TO_USE");
    
    wheelSpinSpeedAdjust = panel.getValueF("WHEEL_SPIN_SPEED_ADJUST");
    wheelSpinTimeAdjust = panel.getValueF("WHEEL_SPIN_TIME_ADJUST");
    
    if (panel.getValueB("RESET")){
        panel.setValueB("RESET", false);
        reset();
    }
    if (panel.getValueB("FULLSCREEN")){
        panel.setValueB("FULLSCREEN", false);
        ofToggleFullscreen();
    }
    
    if (showFFT != panel.getValueB("SHOW_FFT")){
        showFFT = panel.getValueB("SHOW_FFT");
    }
    
    
    
    //fft
    fftLive.update();
    
    prevFfftData.clear();
    for (int i=0; i<fftData.size(); i++){
        prevFfftData.push_back( fftData[i] );
    }
    fftData = fftLive.getFftPeakData();
    
    //check for "beats"
    if (prevFfftData.size() >= beatVals.size()){
        for (int i=0; i<beatVals.size(); i++){
            beatVals[i] *= beatDecay;
            if (fftData[i] > prevFfftData[i]) {
                float fftDif = fftData[i] - prevFfftData[i];
                if (fftDif >= minJumpForBeat){
                    beatVals[i] = ofMap(fftDif, minJumpForBeat, beatJumpValToCountAsMax, minBeatVal, 1, true);
                }
                
            }
        }
    }
   
    
    //move the wheels
    updateWheels();

}


//--------------------------------------------------------------
void ofApp::updateWheels(){
    
    if (wheels.size() > 1){
        float thisSize = wheels[ wheels.size()-1 ].scale;
        if (thisSize < scaleToSpawnNextWheel){
            spawnWheel();
        }
    }else{
        spawnWheel();
    }
    
    
    for (int i=0; i<wheels.size(); i++){
        float fftVal = 0;
        if(fftData.size() > wheels[i].listeningBand){
            fftVal = fftData[ wheels[i].listeningBand ] * curSensitivity;
        }
        
        float beatVal = 0;
        if (beatVals.size() > wheels[i].listeningBand){
            beatVal = beatVals[ wheels[i].listeningBand ];
        }

        wheels[i].update(deltaTime, fftVal, beatVal, wheelSpinSpeedAdjust, wheelSpinTimeAdjust);
    }
    
    if (wheels[0].scale < scaleToKillWheel){
        wheels.erase( wheels.begin() );
    }
}


//--------------------------------------------------------------
void ofApp::draw(){
    
    //test values
    float xMousePrc = (float)ofGetMouseX() / (float)ofGetScreenWidth();
    float yMousePrc = (float)ofGetMouseY() / (float)ofGetScreenHeight();
    
    ofPushMatrix();
    
    ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
    
    //zoom in and out a bit
    float zoomBaseline = panel.getValueF("BASE_ZOOM");
    float zoomRange = panel.getValueF("ZOOM_RANGE");
    if (zoomRange > zoomBaseline){  //no letting us move into negatives
        zoomRange = zoomBaseline;
        panel.setValueF("ZOOM_RANGE", zoomBaseline);
    }
    float zoomNoiseSpeed = panel.getValueF("ZOOM_SPEED");
    
    float zoomPrc = ofNoise(ofGetElapsedTimef() * zoomNoiseSpeed, 0, startingNoiseOffset);
    float zoomVal = zoomPrc * (zoomBaseline - zoomRange) + (1-zoomPrc) * (zoomBaseline + zoomRange);
    
    ofScale(zoomVal, zoomVal);
    
    
    //spin it around
    float angleRange = panel.getValueF("CAM_ANGLE_RANGE");
    
    float xSpeed = panel.getValueF("CAM_ANGLE_SPEED");
    float xPrc = ofNoise(ofGetElapsedTimef() * xSpeed, 100, startingNoiseOffset);
    float xAngle = xPrc * -angleRange + (1-xPrc) * angleRange;
    
    float ySpeed = panel.getValueF("CAM_ANGLE_SPEED") + 0.002;
    float yPrc = ofNoise(ofGetElapsedTimef() * ySpeed, 1000, startingNoiseOffset);
    float yAngle = yPrc * -angleRange + (1-yPrc) * angleRange;
    
    
    ofRotateX(xAngle);
    ofRotateY(yAngle);
    
    //move it aorund
    float moveRange = panel.getValueF("CAM_MOVE_RANGE");
    float moveSpeed = panel.getValueF("CAM_MOVE_SPEED");
    
    float moveXPrc = ofNoise(ofGetElapsedTimef() * moveSpeed, 200, startingNoiseOffset);
    float xMove = moveXPrc * -moveRange + (1-moveXPrc) * moveRange;
    
    float moveYPrc = ofNoise(ofGetElapsedTimef() * moveSpeed, 2000, startingNoiseOffset);
    float yMove = moveYPrc * -moveRange + (1-moveYPrc) * moveRange;
    
    
    ofTranslate(xMove, yMove);
    
    for (int i=wheels.size()-1; i>=0; i--){
        wheels[i].draw();
    }
    
    
    ofPopMatrix();
    
    if (showFFT){
        drawFFT();
    }
    
    if (showPanel){
        panel.setPosition( ofGetWidth()-310, 0 );
        panel.draw();
    }

}

//--------------------------------------------------------------
void ofApp::drawFFT(){
    
    //drawing info
    int totalWidth = 512;
    int totalHeight = 256;
    int barWidth = 512 / numBands;
    
    ofPushMatrix();
    
    ofTranslate(10, ofGetHeight() - 10);
    
    ofNoFill();
    ofSetColor(0);
    ofRect(0, 0, totalWidth, -totalHeight);
    
    
    for(int i=0; i<numBands; i++) {
        ofFill();
        ofSetColor(100,100,210);
        ofRect(i * barWidth, 0, barWidth/2, -fftData[i] * totalHeight);
        ofNoFill();
        ofSetColor(0);
        ofRect(i * barWidth, 0, barWidth/2, -fftData[i] * totalHeight);
        
        float adjustVal = fftData[i] * curSensitivity;
        ofFill();
        ofSetColor(30,30,100);
        ofRect(i * barWidth + barWidth/2, 0, barWidth/2, -adjustVal * totalHeight);
        ofNoFill();
        ofSetColor(0);
        ofRect(i * barWidth + barWidth/2, 0, barWidth/2, -adjustVal * totalHeight);
        
        ofFill();
        ofSetColor(210,100,100);
        ofRect(i * barWidth + barWidth/4, 0, barWidth/2, -beatVals[i] * totalHeight);
        ofNoFill();
        ofSetColor(0);
        ofRect(i * barWidth + barWidth/4, 0, barWidth/2, -beatVals[i] * totalHeight);
        
    }
    
    ofSetColor(30);
    ofFill();
    ofRect(maxBandToUseForWheels * barWidth, 0, barWidth/4, -totalHeight);
    
    ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    if (key == 'f' || key == 'F'){
        ofToggleFullscreen();
    }
    
    if (key == 'd' || key == 'D'){
        showFFT = !showFFT;
        panel.setValueB("SHOW_FFT", showFFT);
    }
    
    if (key == 's' || key == 'S'){
        showPanel = !showPanel;
        if (!showPanel){
            panel.mouseReleased();
        }
    }
    
    if (key == 'p' && useSound){
        soundPlayer.play();
    }
    
    
    if (key == 'r' || key == 'R'){
        reset();
    }
    

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    if (showPanel){
        panel.mouseDragged(x,y,button);
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    if (showPanel){
        panel.mousePressed(x,y,button);
    }
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    if (showPanel){
        panel.mouseReleased();
    }
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}



//--------------------------------------------------------------
void ofApp::spawnWheel(){
    
    Wheel newWheel;
    newWheel.setup(wheelStartingScale, maxBandToUseForWheels);
    wheels.push_back(newWheel);
    
    float rageAdjust = ofRandom(0, panel.getValueF("WHEEL_SPAWN_RATE_RANGE"));
    scaleToSpawnNextWheel = wheelStartingScale -  panel.getValueF("WHEEL_SPAWN_RATE_BASE") - rageAdjust;
    
}