#pragma once

#include "ofMain.h"
#include "ofxCv.h"

class ofxGrabMask {
public:
    void setup();
    void update();
    void draw();

    // Event handlers
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    
    // Feature extraction
    void handleFeatureExtraction(int featureType);
    
    // Status message
    void showStatusMessage(const string& message);

    // Getters for extracted features
    const vector<double>& getHuMoments() const { return huMoments; }
    const vector<double>& getFourierDescriptors() const { return fourierDescriptors; }
    const vector<double>& getShapeContextFeatures() const { return shapeContextFeatures; }
    const vector<double>& getInnerDistanceFeatures() const { return innerDistanceFeatures; }
    const vector<double>& getCurvatureFeatures() const { return curvatureFeatures; }
    const vector<double>& getSkeletonFeatures() const { return skeletonFeatures; }
    
    // Basic shape features
    double getArea() const { return contourArea; }
    double getPerimeter() const { return contourPerimeter; }
    cv::Point2f getCenterOfMass() const { return centerOfMass; }
    double getAngle() const { return contourAngle; }
    double getOvality() const { return contourOvality; }
    
    // Access to generated masks
    ofImage getOverlayImage() const { return overlay; }
    cv::Mat getBinaryMask() const { return binaryMask; }
    cv::Mat getSegmentMask() const { return segmentMask; }
    cv::Mat getEdgeImage() const { return edgeImage; }
    
private:
    // Feature extraction methods
    vector<double> calculateFourierDescriptors(const vector<cv::Point2f>& contour, int numDescriptors);
    vector<double> calculateShapeContext(const vector<cv::Point>& contour);
    vector<double> calculateInnerDistance(const vector<cv::Point>& contour);
    vector<double> calculateCurvature(const vector<cv::Point>& contour);
    vector<double> calculateSkeletonFeatures(const cv::Mat& binaryMask);

    // Variables
    ofImage image;            // Main image
    ofImage overlay;          // Overlay for contours
    cv::Mat binaryMask;       // Binary mask result
    cv::Mat segmentMask;      // Segmented mask result
    cv::Mat edgeImage;        // Edge detection result
    cv::Rect bboxRect;        // Bounding box rectangle

    // Window dimensions
    int ww, hh;

    // Mouse interaction
    int startX, startY;       // Starting point of selection
    int endX, endY;           // Ending point of selection
    bool dragging;            // Is currently dragging

    // State flags
    bool maskCreated;         // Has mask been created
    bool showPopup;           // Show export popup menu
    bool hasContourInfo;      // Has contour information

    // Contour properties
    double contourArea;
    double contourPerimeter;
    cv::Point2f centerOfMass;
    double contourAngle;
    double contourOvality;
    double contourDirection;
    double bestFitRadius;
    double rectFitWidth;      // Best-fit rectangle width
    double rectFitHeight;     // Best-fit rectangle height

    // Menu related
    vector<string> menuItems;
    vector<ofRectangle> menuItemRects;
    int popupX, popupY;

    // Status message
    string statusMessage;
    float statusMessageTimer;

    // Current image file name
    string currentImageFileName;
    
    // Current feature type
    int currentFeatureType = -1;
    
    // Extracted features
    vector<double> huMoments;
    vector<double> fourierDescriptors;
    vector<double> shapeContextFeatures;
    vector<double> innerDistanceFeatures;
    vector<double> curvatureFeatures;
    vector<double> skeletonFeatures;
};