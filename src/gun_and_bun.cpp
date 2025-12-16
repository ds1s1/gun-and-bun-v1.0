#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <thread>
#include <cstdlib>
#include <sstream>
#include <opencv2/opencv.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;
using namespace std;

class VoiceAssistant {
public:
    void speak(string text) {
        thread t([text]() {
            #ifdef _WIN32
                string cmd = "mshta vbscript:Execute(\"CreateObject(\"\"SAPI.SpVoice\"\").Speak(\"\"" + text + "\"\")(window.close)\")";
                system(cmd.c_str());
            #else
                string cmd = "espeak \"" + text + "\"";
                system(cmd.c_str());
            #endif
        });
        t.detach();
    }

    void tellTime() {
        time_t now = time(0);
        tm *ltm = localtime(&now);
        stringstream ss;
        ss << "The time is " << ltm->tm_hour << " " << ltm->tm_min;
        speak(ss.str());
    }

    void tellWeather() {
        int temp = 20 + (rand() % 16); 
        speak("Current temperature is " + to_string(temp) + " degrees celsius");
    }

    void reportStatus(int threats) {
        if (threats == 0) speak("System is secure. No threats detected.");
        else speak("Warning. " + to_string(threats) + " hostile targets detected.");
    }
};

Point mouseClickPoint(-1, -1);
bool mouseClicked = false;

void onMouse(int event, int x, int y, int flags, void* userdata) {
    if (event == EVENT_LBUTTONDOWN) {
        mouseClickPoint = Point(x, y);
        mouseClicked = true;
    }
}

class VisualEffects {
private:
    int scanLineY;
public:
    VisualEffects() : scanLineY(0) {}

    void drawScanner(Mat& frame) {
        scanLineY += 5;
        if (scanLineY > frame.rows) scanLineY = 0;
        line(frame, Point(0, scanLineY), Point(frame.cols, scanLineY), Scalar(0, 255, 0), 2);
    }

    void drawReticle(Mat& frame, Rect r, Scalar color, string label) {
        rectangle(frame, r, color, 1);
        int len = 20;
        line(frame, r.tl(), Point(r.x + len, r.y), color, 3);
        line(frame, r.tl(), Point(r.x, r.y + len), color, 3);
        line(frame, r.br(), Point(r.x + r.width - len, r.y + r.height), color, 3);
        line(frame, r.br(), Point(r.x + r.width, r.y + r.height - len), color, 3);
        putText(frame, label, Point(r.x, r.y - 10), FONT_HERSHEY_SIMPLEX, 0.6, color, 2);
    }

    void drawUI(Mat& frame, int threatCount) {
        int h = frame.rows;
        rectangle(frame, Rect(0, h-40, frame.cols, 40), Scalar(0,0,0), FILLED);
        string msg = "[T] TIME  |  [W] WEATHER  |  [S] STATUS REPORT";
        putText(frame, msg, Point(20, h-15), FONT_HERSHEY_PLAIN, 1.0, Scalar(255, 255, 255), 1);

        if (threatCount > 0) {
            putText(frame, "THREATS: " + to_string(threatCount), Point(frame.cols - 150, h-15), FONT_HERSHEY_BOLD, 1.0, Scalar(0, 0, 255), 2);
        } else {
             putText(frame, "ALL CLEAR", Point(frame.cols - 120, h-15), FONT_HERSHEY_BOLD, 1.0, Scalar(0, 255, 0), 2);
        }
    }
};

int main() {
    VideoCapture cap(0);
    if (!cap.isOpened()) return -1;

    CascadeClassifier faceDetector;
    if (!faceDetector.load("models/haarcascade_frontalface_default.xml")) {
        cerr << "Error: Model not found.\n";
        return -1;
    }

    VisualEffects vfx;
    VoiceAssistant jarvis;
    Mat frame;
    
    namedWindow("Gun and Bun v5.0", WINDOW_AUTOSIZE);
    setMouseCallback("Gun and Bun v5.0", onMouse, NULL);

    vector<int> friendlyIDs;
    jarvis.speak("System Online.");

    while (true) {
        cap >> frame;
        if (frame.empty()) break;
        flip(frame, frame, 1); 

        vector<Rect> faces;
        Mat gray;
        cvtColor(frame, gray, COLOR_BGR2GRAY);
        faceDetector.detectMultiScale(gray, faces, 1.1, 5, 0, Size(30, 30));

        vfx.drawScanner(frame);

        int currentThreats = 0;
        
        for (size_t i = 0; i < faces.size(); i++) {
            Rect r = faces[i];
            int approxID = (r.x / 10) + (r.y / 10); 
            bool isFriendly = false;

            for (int id : friendlyIDs) {
                if (abs(id - approxID) < 5) isFriendly = true; 
            }

            if (mouseClicked && r.contains(mouseClickPoint)) {
                if (isFriendly) {
                     for (auto it = friendlyIDs.begin(); it != friendlyIDs.end(); ) {
                        if (abs(*it - approxID) < 5) it = friendlyIDs.erase(it);
                        else ++it;
                    }
                    isFriendly = false;
                    jarvis.speak("Target Hostile.");
                } else {
                    friendlyIDs.push_back(approxID);
                    isFriendly = true;
                    jarvis.speak("Target Authorized.");
                }
                mouseClicked = false; 
            }

            Scalar color = isFriendly ? Scalar(0, 255, 0) : Scalar(0, 0, 255);
            string label = isFriendly ? "FRIEND" : "HOSTILE";
            if (!isFriendly) currentThreats++;

            vfx.drawReticle(frame, r, color, label);
        }
        mouseClicked = false; 

        vfx.drawUI(frame, currentThreats);

        imshow("Gun and Bun v5.0", frame);
        
        char key = (char)waitKey(30);
        if (key == 27) break; 
        if (key == 't' || key == 'T') jarvis.tellTime();
        if (key == 'w' || key == 'W') jarvis.tellWeather();
        if (key == 's' || key == 'S') jarvis.reportStatus(currentThreats);
    }
    
    jarvis.speak("System shutting down.");
    return 0;
}