#include "ofxGrabMask.h"

using namespace ofxCv;
using namespace cv;
using namespace std;

void ofxGrabMask::setup() {
    ofSetWindowTitle("ofxGrabMask");

    // Initialize default values
    ww = 512;
    hh = 512;
    
    ofSetWindowShape(ww, hh);
    ofSetBackgroundColor(90);
    
    // Initialize mouse interaction
    startX = 0;
    startY = 0;
    endX = 0;
    endY = 0;
    dragging = false;
    
    // Initialize overlay
    overlay.allocate(ww, hh, OF_IMAGE_COLOR_ALPHA);
    
    // Initialize state flags
    maskCreated = false;
    showPopup = false;
    hasContourInfo = false;
    
    // Initialize menu items
    menuItems = {
        "Hu Moments Features",
        "Fourier Descriptor Features",
        "Shape Context Features",
        "Inner Distance",
        "Curvature based Features",
        "Skelton based Features"
    };
    
    // Initialize status message
    statusMessage = "";
    statusMessageTimer = 0;
    
    ofSetWindowTitle("ofxGrabMask - Right click to extract features");
}

void ofxGrabMask::update() {
    if(statusMessageTimer > 0) {
        statusMessageTimer -= ofGetLastFrameTime();
        if(statusMessageTimer <= 0) {
            statusMessage = "";
        }
    }
}

void ofxGrabMask::draw() {
    // Check if image is loaded
    if(!image.isAllocated()) {
        ofSetColor(255);
        string text = "Drop an Image to Here";
        ofBitmapFont bf;
        float textWidth = bf.getBoundingBox(text, 0, 0).width;
        ofPushMatrix();
        float scale = 2.0;
        ofTranslate((ww - textWidth * scale) / 2, hh / 2);
        ofScale(scale, scale);
        ofDrawBitmapString(text, 0, 0);
        ofPopMatrix();
        return;
    }
    
    ofSetColor(255);
    image.draw(0, 0, ww, hh);
    
    ofSetColor(255, 255, 255, 128);
    overlay.draw(0, 0, ww, hh);
    
    ofSetColor(255, 0, 0);
    ofNoFill();
    ofDrawRectangle(startX, startY, endX - startX, endY - startY);
    
    // Display contour information
    if(hasContourInfo && maskCreated) {
        int x = 10;
        int y = 20;
        int lineHeight = 20;
        int padding = 10;
        
        vector<string> infoTexts;
        string featureTitle;
        
        switch(currentFeatureType) {
            case 0: { // Hu Moments
                featureTitle = "Hu Moments Features:";
                for(int i = 0; i < huMoments.size(); i++) {
                    infoTexts.push_back("h" + ofToString(i+1) + ": " +
                                      ofToString(huMoments[i], 4));
                }
                break;
            }
            case 1: { // Fourier Descriptors
                featureTitle = "Fourier Descriptors:";
                for(int i = 0; i < fourierDescriptors.size(); i++) {
                    infoTexts.push_back("FD" + ofToString(i+1) + ": " +
                                      ofToString(fourierDescriptors[i], 4));
                }
                break;
            }
            case 2: { // Shape Context
                featureTitle = "Shape Context Features:";
                for(int i = 0; i < shapeContextFeatures.size(); i++) {
                    infoTexts.push_back("SC" + ofToString(i+1) + ": " +
                                      ofToString(shapeContextFeatures[i], 4));
                }
                break;
            }
            case 3: { // Inner Distance
                featureTitle = "Inner Distance Features:";
                for(int i = 0; i < innerDistanceFeatures.size(); i++) {
                    infoTexts.push_back("ID" + ofToString(i+1) + ": " +
                                      ofToString(innerDistanceFeatures[i], 4));
                }
                break;
            }
            case 4: { // Curvature
                featureTitle = "Curvature Features:";
                for(int i = 0; i < curvatureFeatures.size(); i++) {
                    infoTexts.push_back("Curv" + ofToString(i+1) + ": " +
                                      ofToString(curvatureFeatures[i], 4));
                }
                break;
            }
            case 5: { // Skeleton
                featureTitle = "Skeleton Features:";
                for(int i = 0; i < skeletonFeatures.size(); i++) {
                    infoTexts.push_back("Skel" + ofToString(i+1) + ": " +
                                      ofToString(skeletonFeatures[i], 4));
                }
                break;
            }
            default: {
                featureTitle = "Basic Shape Features:";
                infoTexts = {
                    "Area: " + ofToString(contourArea, 1) + " px^2",
                    "Perimeter: " + ofToString(contourPerimeter, 1) + " px"
                };
            }
        }
        
        int boxWidth = 250;
        int boxHeight = (lineHeight * (infoTexts.size() + 1)) + (padding * 2);
        
        ofSetColor(0, 0, 0, 100);
        ofFill();
        ofDrawRectangle(x - padding, y - padding - 15, boxWidth, boxHeight);
        
        ofSetColor(255);
        ofDrawBitmapString(featureTitle, x, y);
        for(int i = 0; i < infoTexts.size(); i++) {
            ofDrawBitmapString(infoTexts[i], x, y + ((i+1) * lineHeight));
        }
    }
    
    // Draw popup menu
    if(showPopup && maskCreated) {
        ofSetColor(0, 0, 0, 200);
        
        // Set menu item position and size
        int menuWidth = 200;
        int menuItemHeight = 30;
        int totalHeight = menuItemHeight * menuItems.size();
        
        // Adjust popup to stay within screen
        int adjustedX = popupX;
        int adjustedY = popupY;
        if (adjustedX + menuWidth > ww) adjustedX = ww - menuWidth;
        if (adjustedY + totalHeight > hh) adjustedY = hh - totalHeight;
        
        // Draw menu background
        ofFill();
        ofDrawRectangle(adjustedX, adjustedY, menuWidth, totalHeight);
        
        // Draw menu items
        menuItemRects.clear();
        ofSetColor(255);
        for(int i = 0; i < menuItems.size(); i++) {
            ofRectangle itemRect(adjustedX, adjustedY + (i * menuItemHeight),
                               menuWidth, menuItemHeight);
            menuItemRects.push_back(itemRect);
            
            // Highlight if mouse is over item
            if(itemRect.inside(ofGetMouseX(), ofGetMouseY())) {
                ofSetColor(100, 100, 255, 100);
                ofDrawRectangle(itemRect);
                ofSetColor(255);
            }
            
            ofDrawBitmapString(menuItems[i],
                              adjustedX + 10,
                              adjustedY + (i * menuItemHeight) + 20);
        }
    }
    
    // Display status message
    if(!statusMessage.empty()) {
        ofSetColor(0, 0, 0, 180);
        ofDrawRectangle(10, hh - 40, 300, 30);
        ofSetColor(255);
        ofDrawBitmapString(statusMessage, 20, hh - 20);
    }
}

void ofxGrabMask::mouseMoved(int x, int y) {
    #ifdef TARGET_WIN32
        // Windows cursor
        if(showPopup && maskCreated) {
            for(const auto& rect : menuItemRects) {
                if(rect.inside(x, y)) {
                    SetCursor(LoadCursor(NULL, IDC_HAND));
                    return;
                }
            }
        }
        SetCursor(LoadCursor(NULL, IDC_ARROW));
    #endif
}

void ofxGrabMask::mouseDragged(int x, int y, int button) {
    // When the mouse is dragged, update the end point of the bounding box
    if (dragging) {
        endX = x;
        endY = y;
    }
}

void ofxGrabMask::mousePressed(int x, int y, int button) {
    // When the mouse is pressed, set the start point of the bounding box
    startX = x;
    startY = y;
    dragging = true;
}