#include "ofApp.h"

void ofApp::setup() {
    ofSetFrameRate(60);
    grabMask.setup();
    
    // Load font for instructions
    font.load(OF_TTF_SANS, 12);
}

void ofApp::update() {
    grabMask.update();
}

void ofApp::draw() {
    grabMask.draw();
    
    // Draw instructions
    ofSetColor(255, 255, 255, 200);
    ofFill();
    ofDrawRectangle(10, 10, 350, 60);
    
    ofSetColor(0);
    font.drawString("Instructions:", 20, 30);
    font.drawString("1. Drag an image file to this window", 20, 50);
    font.drawString("2. Draw a box around object to extract", 20, 70);
    font.drawString("3. Right-click to extract shape features", 20, 90);
}

void ofApp::keyPressed(int key) {
}

void ofApp::keyReleased(int key) {
}

void ofApp::mouseMoved(int x, int y) {
    grabMask.mouseMoved(x, y);
}

void ofApp::mouseDragged(int x, int y, int button) {
    grabMask.mouseDragged(x, y, button);
}

void ofApp::mousePressed(int x, int y, int button) {
    grabMask.mousePressed(x, y, button);
}

void ofApp::mouseReleased(int x, int y, int button) {
    grabMask.mouseReleased(x, y, button);
}

void ofApp::mouseEntered(int x, int y) {
}

void ofApp::mouseExited(int x, int y) {
}

void ofApp::windowResized(int w, int h) {
    grabMask.windowResized(w, h);
}

void ofApp::gotMessage(ofMessage msg) {
}

void ofApp::dragEvent(ofDragInfo dragInfo) {
    grabMask.dragEvent(dragInfo);
}