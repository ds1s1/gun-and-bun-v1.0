#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <opencv2/opencv.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

class WeaponSystem {
public:
    void lockTarget(int x, int y) {
        std::cout << "\033[1;33m[GUN]: Locking on Target at (" << x << ", " << y << ")...\033[0m\n";
    }

    void fire() {
        std::cout << "\033[1;31m[GUN]: !!! FIRING SEQUENCE INITIATED !!! \033[0m\n";
        std::cout << "\033[1;31m[GUN]: BANG! BANG! BANG! \033[0m\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(500)); 
    }
    
    void standDown() {
        std::cout << "\033[1;32m[GUN]: Target Safe. Holding Fire.\033[0m\n";
    }
};

class BiometricScanner {
private:
    cv::HOGDescriptor bodyDetector;
    cv::CascadeClassifier faceDetector;

public:
    BiometricScanner() {
        bodyDetector.setSVMDetector(cv::HOGDescriptor::getDefaultPeopleDetector());
        if (!faceDetector.load("models/haarcascade_frontalface_default.xml")) {
            std::cerr << "[ERROR]: Face XML not found.\n";
        }
    }

    bool analyzeGait(const cv::Rect& body) {
        return true; 
    }

    bool analyzeVoiceSignature() {
        return false; 
    }

    bool isStranger(const cv::Mat& faceROI) {
        return true; 
    }

    void scanFrame(cv::Mat& frame, WeaponSystem& weapon) {
        std::vector<cv::Rect> foundBodies;
        std::vector<cv::Rect> foundFaces;
        
        bodyDetector.detectMultiScale(frame, foundBodies, 0, cv::Size(8, 8), cv::Size(32, 32), 1.05, 2);

        cv::Mat gray;
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
        faceDetector.detectMultiScale(gray, foundFaces, 1.1, 5);

        bool threatDetected = false;
        int targetX = 0, targetY = 0;

        for (const auto& body : foundBodies) {
            cv::rectangle(frame, body, cv::Scalar(255, 0, 0), 2);
            if (analyzeGait(body)) {
                cv::putText(frame, "[BUN]: Suspicious Movement", cv::Point(body.x, body.y - 10), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255), 2);
            }
        }

        for (const auto& face : foundFaces) {
            targetX = face.x + face.width / 2;
            targetY = face.y + face.height / 2;
            
            cv::rectangle(frame, face, cv::Scalar(0, 0, 255), 2);

            bool isFaceStranger = isStranger(frame(face));
            bool isVoiceStranger = !analyzeVoiceSignature();

            if (isFaceStranger && isVoiceStranger) {
                threatDetected = true;
                cv::putText(frame, "TARGET: HOSTILE", cv::Point(face.x, face.y - 30), cv::FONT_HERSHEY_BOLD, 0.8, cv::Scalar(0, 0, 255), 2);
            } else {
                cv::putText(frame, "TARGET: FRIENDLY", cv::Point(face.x, face.y - 30), cv::FONT_HERSHEY_BOLD, 0.8, cv::Scalar(0, 255, 0), 2);
            }
        }

        if (threatDetected) {
            weapon.lockTarget(targetX, targetY);
            weapon.fire();
            cv::putText(frame, "!!! GUN ACTIVATED !!!", cv::Point(50, 50), cv::FONT_HERSHEY_DUPLEX, 1.5, cv::Scalar(0, 0, 255), 3);
        } else if (!foundFaces.empty()) {
            weapon.standDown();
        }
    }
};

int main() {
    std::cout << "=================================\n";
    std::cout << "   GUN AND BUN: SYSTEM ONLINE    \n";
    std::cout << "=================================\n";
    
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "Error: Cannot open camera.\n";
        return -1;
    }

    WeaponSystem turret;
    BiometricScanner scanner;
    cv::Mat frame;

    while (true) {
        cap >> frame;
        if (frame.empty()) break;

        scanner.scanFrame(frame, turret);
        cv::imshow("Gun and Bun UI", frame);

        if (cv::waitKey(30) == 27) break;
    }

    return 0;
}