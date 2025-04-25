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

void ofxGrabMask::mouseReleased(int x, int y, int button) {
   if(button == OF_MOUSE_BUTTON_LEFT) {
       if(showPopup) {
           // Check popup menu click
           for(int i = 0; i < menuItemRects.size(); i++) {
               if(menuItemRects[i].inside(x, y)) {
                   handleFeatureExtraction(i);
                   currentFeatureType = i;
                   showPopup = false;
                   return;
               }
           }
           showPopup = false;
       } else {
           // Validate bounding box
           int boxWidth = abs(endX-startX);
           int boxHeight = abs(endY-startY);
           
           // Check minimum size (20x20 pixels)
           if(boxWidth < 20 || boxHeight < 20) {
               showStatusMessage("Box too small! Draw a larger box.");
               dragging = false;
               return;
           }
           
           // Check image range
           int validStartX = std::max(0, std::min(startX, endX));
           int validStartY = std::max(0, std::min(startY, endY));
           int validEndX = std::min((int)image.getWidth(), std::max(startX, endX));
           int validEndY = std::min((int)image.getHeight(), std::max(startY, endY));
           
           boxWidth = validEndX - validStartX;
           boxHeight = validEndY - validStartY;
           
           // Check for valid area
           if(boxWidth <= 0 || boxHeight <= 0) {
               showStatusMessage("Invalid selection! Try again.");
               dragging = false;
               return;
           }
           
           cv::Rect bbox(validStartX, validStartY, boxWidth, boxHeight);
           bboxRect = bbox;
           ofLog() << "bbox = " << bbox;
           
           try {
               cv::Mat src = toCv(image);
               cv::Mat mask = cv::Mat::zeros(src.rows, src.cols, CV_8UC1);
               cv::Mat bgModel, fgModel;
               
               unsigned int iteration = 9;
               
               // Validate GrabCut input
               if(src.empty() || !src.data) {
                   showStatusMessage("Invalid image data!");
                   dragging = false;
                   return;
               }
               
               if(bbox.x < 0 || bbox.y < 0 ||
                  bbox.x + bbox.width > src.cols ||
                  bbox.y + bbox.height > src.rows) {
                   showStatusMessage("Selection out of bounds!");
                   dragging = false;
                   return;
               }
               
               cv::grabCut(src, mask, bbox, bgModel, fgModel, iteration, GC_INIT_WITH_RECT);
               
               // Validate result mask
               cv::Mat mask2 = (mask == 1) + (mask == 3);
               if(cv::countNonZero(mask2) == 0) {
                   showStatusMessage("No foreground detected! Try again.");
                   dragging = false;
                   return;
               }
               
               // Save binary mask
               mask2.convertTo(binaryMask, CV_8UC1, 255);
               
               // Save segment mask
               cv::Mat dest;
               src.copyTo(dest, mask2);
               segmentMask = dest.clone();
               
               // Create and save edge image
               cv::Canny(binaryMask, edgeImage, 100, 200);
               
               // Find contours and calculate features
               vector<vector<cv::Point>> contours;
               vector<cv::Vec4i> hierarchy;
               cv::findContours(binaryMask, contours, hierarchy,
                              cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
               
               if(!contours.empty()) {
                   // Find largest contour
                   int largest_contour_idx = 0;
                   double largest_area = 0;
                   
                   for(int i = 0; i < contours.size(); i++) {
                       double area = cv::contourArea(contours[i]);
                       if(area > largest_area) {
                           largest_area = area;
                           largest_contour_idx = i;
                       }
                   }
                   
                   // Calculate contour properties
                   vector<cv::Point>& contour = contours[largest_contour_idx];
                   
                   // Area
                   contourArea = cv::contourArea(contour);
                   
                   // Perimeter
                   contourPerimeter = cv::arcLength(contour, true);
                   
                   // Center of mass
                   cv::Moments m = cv::moments(contour);
                   centerOfMass = cv::Point2f(m.m10/m.m00, m.m01/m.m00);
                   
                   // Ellipse fitting
                   if(contour.size() >= 5) { // Minimum 5 points required
                       cv::RotatedRect rotatedRect = cv::fitEllipse(contour);
                       contourAngle = rotatedRect.angle;
                       contourDirection = rotatedRect.angle;
                       // Ovality = major axis/minor axis
                       contourOvality = std::max(rotatedRect.size.width, rotatedRect.size.height) /
                                      std::min(rotatedRect.size.width, rotatedRect.size.height);
                       
                       // Best-fit rectangle info
                       cv::RotatedRect minRect = cv::minAreaRect(contour);
                       rectFitWidth = minRect.size.width;
                       rectFitHeight = minRect.size.height;
                   }
                   
                   // Best fit circle radius
                   cv::Point2f center;
                   float radius;
                   cv::minEnclosingCircle(contour, center, radius);
                   bestFitRadius = radius;
                   
                   hasContourInfo = true;
                   
                   // Draw contour
                   cv::Mat contourImage = cv::Mat::zeros(src.size(), CV_8UC4);
                   cv::drawContours(contourImage, contours, largest_contour_idx,
                                  cv::Scalar(0, 255, 0, 255), 2);
                   
                   toOf(contourImage, overlay);
                   overlay.update();
               }
               
               maskCreated = true;
               showStatusMessage("Mask created successfully!");
               
           } catch(cv::Exception& e) {
               ofLog(OF_LOG_ERROR) << "OpenCV error: " << e.what();
               showStatusMessage("Processing error! Try again.");
               dragging = false;
               return;
           }
           
           dragging = false;
       }
   }
   else if(button == OF_MOUSE_BUTTON_RIGHT && maskCreated) {
       // Show popup menu on right click
       popupX = x;
       popupY = y;
       showPopup = true;
   }
}

void ofxGrabMask::windowResized(int w, int h) {
    ww = w;
    hh = h;
    
    if (overlay.isAllocated()) {
        // Resize overlay to match window
        ofPixels pixels;
        pixels.allocate(ww, hh, OF_IMAGE_COLOR_ALPHA);
        pixels.set(0);
        overlay.setFromPixels(pixels);
        overlay.update();
    }
}

void ofxGrabMask::dragEvent(ofDragInfo dragInfo) {
    if (dragInfo.files.size() > 0) {
        image.load(dragInfo.files[0]);
        image.setImageType(OF_IMAGE_COLOR);
        
        // Extract filename from path (without extension)
        string fullPath = dragInfo.files[0];
        size_t lastSlash = fullPath.find_last_of("/\\");
        size_t lastDot = fullPath.find_last_of(".");
        
        if (lastSlash == string::npos) lastSlash = -1;
        if (lastDot == string::npos) lastDot = fullPath.length();
        
        currentImageFileName = fullPath.substr(lastSlash + 1, lastDot - lastSlash - 1);
    }
    
    ww = image.getWidth();
    hh = image.getHeight();
    ofSetWindowShape(ww, hh);
    
    // Reset the overlay
    overlay.allocate(ww, hh, OF_IMAGE_COLOR_ALPHA);
    overlay.clear();
    
    // Set the initial values for the bounding box
    startX = 0;
    startY = 0;
    endX = 0;
    endY = 0;
    dragging = false;
    
    // Reset state
    maskCreated = false;
    hasContourInfo = false;
    showPopup = false;
}

// Helper function to display status message
void ofxGrabMask::showStatusMessage(const string& message) {
    statusMessage = message;
    statusMessageTimer = 3.0; // Display message for 3 seconds
}

// Handle feature extraction
void ofxGrabMask::handleFeatureExtraction(int featureType) {
    if (!maskCreated || binaryMask.empty()) return;
    
    vector<vector<cv::Point>> contours;
    vector<cv::Vec4i> hierarchy;
    cv::findContours(binaryMask, contours, hierarchy,
                    cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    
    if (contours.empty()) return;
    
    // Find largest contour
    size_t maxContourIdx = 0;
    double maxArea = 0;
    for (size_t i = 0; i < contours.size(); i++) {
        double area = cv::contourArea(contours[i]);
        if (area > maxArea) {
            maxArea = area;
            maxContourIdx = i;
        }
    }
    
    vector<cv::Point>& contour = contours[maxContourIdx];
    
    switch(featureType) {
        case 0: { // Hu Moments
            cv::Moments moments = cv::moments(contour);
            double huMomentsArray[7];
            cv::HuMoments(moments, huMomentsArray);
            
            this->huMoments.clear();
            for(int i = 0; i < 7; i++) {
                this->huMoments.push_back(-1 * copysign(1.0, huMomentsArray[i]) *
                                      log10(abs(huMomentsArray[i])));
            }
            showStatusMessage("Hu Moments calculated");
            break;
        }
        case 1: { // Fourier Descriptors
            vector<cv::Point2f> contourF(contour.begin(), contour.end());
            fourierDescriptors = calculateFourierDescriptors(contourF, 10);
            showStatusMessage("Fourier Descriptors calculated");
            break;
        }
        case 2: { // Shape Context
            shapeContextFeatures = calculateShapeContext(contour);
            showStatusMessage("Shape Context Features calculated");
            break;
        }
        case 3: { // Inner Distance
            innerDistanceFeatures = calculateInnerDistance(contour);
            showStatusMessage("Inner Distance Features calculated");
            break;
        }
        case 4: { // Curvature
            curvatureFeatures = calculateCurvature(contour);
            showStatusMessage("Curvature Features calculated");
            break;
        }
        case 5: { // Skeleton
            skeletonFeatures = calculateSkeletonFeatures(binaryMask);
            showStatusMessage("Skeleton Features calculated");
            break;
        }
    }
}

// Fourier Descriptors 계산
vector<double> ofxGrabMask::calculateFourierDescriptors(const vector<cv::Point2f>& contour, int numDescriptors) {
    vector<double> descriptors;
    
    // 컨투어 점들을 복소수로 변환
    vector<complex<double>> complexContour;
    for(const auto& point : contour) {
        complexContour.push_back(complex<double>(point.x, point.y));
    }
    
    // FFT 계산
    vector<complex<double>> fftResult = complexContour;
    int n = complexContour.size();
    for(int k = 0; k < n; k++) {
        complex<double> sum(0, 0);
        for(int t = 0; t < n; t++) {
            double angle = -2 * M_PI * k * t / n;
            sum += complexContour[t] * complex<double>(cos(angle), sin(angle));
        }
        fftResult[k] = sum;
    }
    
    // 크기 정규화
    double dc = abs(fftResult[0]);
    for(int i = 1; i <= numDescriptors && i < fftResult.size(); i++) {
        descriptors.push_back(abs(fftResult[i]) / dc);
    }
    
    return descriptors;
}

// Shape Context Features 계산
vector<double> ofxGrabMask::calculateShapeContext(const vector<cv::Point>& contour) {
    vector<double> features;
    const int anglesBins = 12;  // 각도 구간 수
    const int radiusBins = 5;   // 반지름 구간 수
    
    // 중심점 계산
    cv::Point2f center(0, 0);
    for(const auto& p : contour) {
        center.x += p.x;
        center.y += p.y;
    }
    center.x /= contour.size();
    center.y /= contour.size();
    
    // 히스토그램 초기화
    vector<vector<int>> histogram(radiusBins, vector<int>(anglesBins, 0));
    
    // 최대 반지름 찾기
    double maxRadius = 0;
    for(const auto& p : contour) {
        double dx = p.x - center.x;
        double dy = p.y - center.y;
        maxRadius = std::max(maxRadius, sqrt(dx*dx + dy*dy));
    }
    
    // 히스토그램 계산
    for(const auto& p : contour) {
        double dx = p.x - center.x;
        double dy = p.y - center.y;
        double radius = sqrt(dx*dx + dy*dy);
        double angle = atan2(dy, dx);
        if(angle < 0) angle += 2*M_PI;
        
        int rBin = min(radiusBins-1, (int)(radius * radiusBins / maxRadius));
        int aBin = min(anglesBins-1, (int)(angle * anglesBins / (2*M_PI)));
        
        histogram[rBin][aBin]++;
    }
    
    // 히스토그램을 1차원 벡터로 변환
    for(const auto& row : histogram) {
        features.insert(features.end(), row.begin(), row.end());
    }
    
    return features;
}

// Inner Distance Features 계산
vector<double> ofxGrabMask::calculateInnerDistance(const vector<cv::Point>& contour) {
    vector<double> features;
    const int numSamples = 20;  // 샘플링 포인트 수
    
    // 컨투어 길이에 따른 균등 샘플링
    vector<cv::Point> sampledPoints;
    double arcLength = cv::arcLength(contour, true);
    double stepSize = arcLength / numSamples;
    
    double accLength = 0;
    int nextIdx = 0;
    sampledPoints.push_back(contour[0]);
    
    for(int i = 1; i < contour.size(); i++) {
        double segLength = norm(contour[i] - contour[i-1]);
        accLength += segLength;
        
        while(accLength >= stepSize && sampledPoints.size() < numSamples) {
            double t = (stepSize - (accLength - segLength)) / segLength;
            cv::Point interpolated;
            interpolated.x = contour[i-1].x + t * (contour[i].x - contour[i-1].x);
            interpolated.y = contour[i-1].y + t * (contour[i].y - contour[i-1].y);
            sampledPoints.push_back(interpolated);
            accLength -= stepSize;
        }
    }
    
    // 모든 점쌍 간의 거리 계산
    for(int i = 0; i < sampledPoints.size(); i++) {
        for(int j = i+1; j < sampledPoints.size(); j++) {
            double dist = norm(sampledPoints[i] - sampledPoints[j]);
            features.push_back(dist);
        }
    }
    
    return features;
}

// Curvature Features 계산
vector<double> ofxGrabMask::calculateCurvature(const vector<cv::Point>& contour) {
    vector<double> features;
    const int numSamples = 50;  // 곡률 샘플링 포인트 수
    
    // 컨투어 스무딩
    vector<cv::Point2f> smoothContour;
    for(const auto& p : contour) {
        smoothContour.push_back(cv::Point2f(p.x, p.y));
    }
    
    // 균등 간격으로 곡률 계산
    for(int i = 0; i < numSamples; i++) {
        int idx = (i * contour.size()) / numSamples;
        int prev = (idx + contour.size() - 1) % contour.size();
        int next = (idx + 1) % contour.size();
        
        // 중심차분법으로 곡률 계산
        float dx = (smoothContour[next].x - smoothContour[prev].x) / 2.0f;
        float dy = (smoothContour[next].y - smoothContour[prev].y) / 2.0f;
        float ddx = smoothContour[next].x - 2*smoothContour[idx].x + smoothContour[prev].x;
        float ddy = smoothContour[next].y - 2*smoothContour[idx].y + smoothContour[prev].y;
        
        float curvature = (dx*ddy - dy*ddx) / pow(dx*dx + dy*dy, 1.5);
        features.push_back(curvature);
    }
    
    return features;
}

// Skeleton Features 계산
vector<double> ofxGrabMask::calculateSkeletonFeatures(const cv::Mat& binaryMask) {
    vector<double> features;
    
    // Distance Transform 계산
    cv::Mat dist;
    cv::distanceTransform(binaryMask, dist, cv::DIST_L2, 3);
    
    // 스켈레톤 추출
    cv::Mat skeleton = cv::Mat::zeros(binaryMask.size(), CV_8UC1);
    cv::Mat temp = binaryMask.clone();
    cv::Mat eroded;
    cv::Mat opened;
    
    do {
        cv::erode(temp, eroded, cv::Mat());
        cv::dilate(eroded, opened, cv::Mat());
        cv::subtract(temp, opened, opened);
        cv::bitwise_or(skeleton, opened, skeleton);
        eroded.copyTo(temp);
    } while(cv::countNonZero(temp) > 0);
    
    // 스켈레톤 특징 추출
    vector<cv::Point> skelPoints;
    cv::findNonZero(skeleton, skelPoints);
    
    if(!skelPoints.empty()) {
        // 분기점 수 계산
        int branchPoints = 0;
        cv::Mat kernel = cv::Mat::ones(3, 3, CV_8UC1);
        for(const auto& p : skelPoints) {
            cv::Mat roi = skeleton(cv::Rect(p.x-1, p.y-1, 3, 3));
            if(cv::countNonZero(roi) > 3) {
                branchPoints++;
            }
        }
        features.push_back(branchPoints);
        
        // 평균 거리 변환값
        double meanDist = 0;
        for(const auto& p : skelPoints) {
            meanDist += dist.at<float>(p.y, p.x);
        }
        features.push_back(meanDist / skelPoints.size());
        
        // 스켈레톤 길이
        features.push_back(skelPoints.size());
    }
    
    return features;
}