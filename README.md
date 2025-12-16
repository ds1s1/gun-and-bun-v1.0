# Gun and Bun v1.0.0

A standalone face detection application with audio feedback. I wrapped the original C++ code into a simple executable to avoid dealing with manual library installations or terminal scripts.


I created this version to make the project portable and easier to run. Instead of setting up a development environment on every machine, this build handles its own dependencies.

### Features

* **Custom Launcher:** I wrote a launcher using Raylib that automatically checks for missing files and downloads the required AI models. You don't need to install OpenCV manually.
* **Audio Feedback:** The system speaks instead of just printing text logs.
    * Press **T** for the current time.
    * Press **W** for a simulated weather report.
    * Press **S** for a security status update on detected targets.
* **Interactive Targeting:** No need to hardcode IDs. You can click on any detected face in the video feed to toggle their status between "Friend" (Green) and "Hostile" (Red).

### How to Run

1. Download the latest release (ZIP file).
2. Extract the folder to a location of your choice.
3. Run `launcher.exe`.
4. Wait for the initialization bar to finish. The main application will start automatically.

### Requirements

* Windows 10 or 11 (64-bit).
* A webcam.
* No programming environment required.

### Tech Stack

* C++
* OpenCV 4.x (Computer Vision)
* Raylib (Launcher UI)

---
*Note: This is v1.0.0. If the application does not start immediately, ensure you have extracted all files from the ZIP archive.*