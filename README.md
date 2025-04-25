# ofxGrabMask

OpenFrameworks addon for object segmentation and shape feature extraction.

## Features

- Interactive object segmentation using GrabCut algorithm
- Various shape feature extraction methods:
  - Hu Moments
  - Fourier Descriptors
  - Shape Context
  - Inner Distance
  - Curvature Features
  - Skeleton Features
- Visualization of extracted features
- Simple drag & drop interface

## Requirements

- OpenFrameworks 0.11.0+
- ofxCv addon
- OpenCV 4.0+

## Usage

```cpp
#include "ofxGrabMask.h"

ofxGrabMask grabMask;

void ofApp::setup() {
    grabMask.setup();
}

void ofApp::update() {
    grabMask.update();
}

void ofApp::draw() {
    grabMask.draw();
}

// Forward mouse and window events to the grabMask
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

void ofApp::windowResized(int w, int h) {
    grabMask.windowResized(w, h);
}

void ofApp::dragEvent(ofDragInfo dragInfo) {
    grabMask.dragEvent(dragInfo);
}
```

## How it works

1. Drag and drop an image onto the application window
2. Draw a rectangle around the object you want to segment
3. The GrabCut algorithm will extract the object from the background
4. Right-click to access the feature extraction menu
5. Select a feature extraction method to analyze the shape

## Methods

### Setup and Main Loop
- `setup()`: Initialize the addon
- `update()`: Update internal state
- `draw()`: Draw the current state to the screen

### Event Handlers
- `mouseMoved(int x, int y)`: Handle mouse movement
- `mouseDragged(int x, int y, int button)`: Handle mouse dragging
- `mousePressed(int x, int y, int button)`: Handle mouse press
- `mouseReleased(int x, int y, int button)`: Handle mouse release
- `dragEvent(ofDragInfo dragInfo)`: Handle drag and drop events

### Feature Extraction
- `calculateHuMoments()`: Calculate Hu moment invariants
- `calculateFourierDescriptors()`: Calculate Fourier descriptors
- `calculateShapeContext()`: Calculate shape context features
- `calculateInnerDistance()`: Calculate inner distance features
- `calculateCurvature()`: Calculate curvature features
- `calculateSkeletonFeatures()`: Calculate skeleton-based features

## License

MIT License

## Author

@bemoregt