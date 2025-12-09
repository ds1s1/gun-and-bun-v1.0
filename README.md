Project: Gun and Bun
=============================================================================

### What is this?

Basically, I wrote a C++ program that turns my computer into a security turret simulation. It watches the webcam video feed, looks for people (the "Bun"), locks onto them, and if it decides they look suspicious, it pretends to engage them (the "Gun").

It is a simulation project for my Computer Science portfolio. It does not control real weapons; it just prints "BANG" and status updates to the terminal.

### How it works (The Technical Stuff)

I used OpenCV for the heavy lifting, but the logic is all C++ state management. Here is exactly what is happening under the hood:

1. The Vision System ("The Bun")
   Most basic tutorials just use face detection. I wanted this to be smarter, so I implemented two layers of detection:
   
   - Full Body Detection:
     I am using HOG Descriptors (Histogram of Oriented Gradients) combined with a pre-trained SVM (Support Vector Machine). This scans the image for the specific gradients and shapes that look like a standing human body. It is computationally heavy (CPU intensive), but it allows the system to see a "person" even if their face is turned away.

   - Face Detection:
     On top of the body detection, I run a Haar Cascade Classifier using the 'haarcascade_frontalface_default.xml' model. This is faster and gives me the precise location of the head for targeting.

   - Coordinate Tracking:
     Once a target is found, the code calculates the center centroid (X and Y coordinates) of the bounding box. It compares this to the center of the screen to determine how far the "turret" needs to move to lock on.

2. The Weapon System ("The Gun")
   This class simulates the hardware control.
   
   - Locking: It takes the coordinate error from the vision system and simulates a servo motor moving to correct the aim until the target is centered.
   - Firing Logic: I wrote a simple "Friend or Foe" logic. Right now, it acts as a placeholder that simulates checking biometric data (like gait analysis or voice prints). If the logic returns "Hostile," the system logs a firing sequence to the console.
   - Safety: If the person leaves the frame or the logic decides they are friendly, the system automatically stands down and stops tracking.

### Project Structure

You need to keep the files organized exactly like this, or the code will crash because it won't find the model file.

Gun-and-Bun/
|
|-- src/
|   |-- gun_and_bun.cpp                 (The main C++ source code)
|
|-- models/
|   |-- haarcascade_frontalface_default.xml   (The brain for face detection. Don't move this.)
|
|-- README.txt                          (This file)

### Prerequisites

To run this, you need a few things installed on your machine:
1. A C++ Compiler (GCC or G++)
2. OpenCV 4 (The computer vision library)

If you are on Linux (Ubuntu/Debian), you can install OpenCV with this command:
$ sudo apt-get install libopencv-dev

### How to Compile and Run

Since C++ needs to be linked to OpenCV, the compilation command is a bit long. I recommend just copying and pasting this line into your terminal while inside the project folder:

1. Compile:
   g++ "src/gun and bun.cpp" -o gun_and_bun `pkg-config --cflags --libs opencv4`

2. Run:
   ./gun_and_bun

### Using the System

- Once it is running, a window will pop up showing what your camera sees.
- Blue Boxes mean it sees a human body.
- Red Boxes mean it has locked onto a face.
- Check your terminal window. You will see it logging "Locking on Target" and "Firing Sequence" when it spots you.
- To quit the program, just press the 'Esc' key.

### Troubleshooting

- Error "Face XML not found":
  This happens 99% of the time because you are running the program from the wrong folder. Make sure the 'models' folder is sitting right next to your executable file.

- The video is lagging/slow:
  This is normal. The HOG body detection algorithm is very heavy on the CPU because it analyzes every pixel for gradients. If it is too slow, you can try compiling with optimization flags like -O3, or go into the code and disable the 'scanFrame' body detection part and just use the face detection.

### Disclaimer

This is strictly a software simulation for educational purposes. It demonstrates computer vision state machines and C++ logic. Do not use this code to control actual dangerous hardware.