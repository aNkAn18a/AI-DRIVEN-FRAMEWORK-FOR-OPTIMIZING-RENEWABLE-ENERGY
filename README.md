# AI-Driven Framework for Optimizing Renewable Energy

An IoT-enabled smart solar tracking and energy prediction system. This project combines dual-axis sunlight tracking, live IoT monitoring, battery storage tracking, and a machine learning model to maximize energy generation and forecast electricity demand.

## Features
- **Automatic Solar Tracking:** Orients the solar panel toward maximum sunlight using LDR sensors and a servo motor.
- **IoT Dashboard:** Real-time monitoring of generated power and battery status via Blynk.
- **AI Energy Prediction:** Forecasts electricity consumption using a Random Forest Regression model.
- **Hardware Integrated:** Built using an ESP32 controller, sensors, and OLED display.

## System Architecture and Workflow
1. **Hardware Layer:** Dual LDR sensors detect sunlight intensity, the ESP32 processes the data, and the SG90 Servo motor rotates the panel.
2. **Data Layer:** The ACS712 current sensor measures generated power, which is stored in a 18650 Li-ion battery via a TP4056 module.
3. **IoT Layer:** The ESP32 uploads live data over Wi-Fi to the dashboard.
4. **ML Layer:** The web application backend utilizes a Random Forest model to predict electricity consumption based on environmental factors.

---

## Repository Structure
```text
├── frontend/
│   └── dashboard.html   # Frontend web interface for analytics and tracking
├── firmware/
│   └── tracker.ino      # ESP32 microcontroller firmware (C++ code)
├── backend/
│   └── app.py           # Flask backend handling ML inference and web routes
├── README.md            # Project documentation
└── .gitignore           # Ignores large model files
```

Note on Machine Learning Model (.pkl):
The pre-trained Random Forest model file is omitted from this GitHub repository due to file size limitations. You can download the trained model file directly from https://drive.google.com/file/d/15_kFgZkK4JdlXpHzt2OuL2wffGcV3I7v/view?usp=drive_link. Once downloaded, place the file in the root directory of this project before running the application.

Machine Learning Model Details
Algorithm Used: Random Forest Regression (Ensemble of Decision Trees)

Model Performance: Achieved approximately 97.7% prediction accuracy (R-squared Score)

Input Parameters: Temperature, Humidity, Wind Speed, Pressure, Rainfall, Solar Radiation, Hour, Day, Month, Weekend.

Output: Electricity Consumption Prediction in Watts

Hardware Setup & Code Upload
1. Pin Configuration (ESP32)
LDR Left Sensor: GPIO 34

LDR Right Sensor: GPIO 35

SG90 Servo Motor: GPIO 13

ACS712 Current Sensor: GPIO 32

OLED Display (SH1106): I2C Interface (SDA/SCL pins)

2. Required Arduino IDE Libraries
Install the following libraries using the Arduino Library Manager before compiling:

Blynk (by Volodymyr Shymanskyy)

Adafruit GFX Library

Adafruit SH110X

ESP32Servo

3. Configure and Flash Firmware
Open the .ino firmware file located in the firmware/ folder using Arduino IDE.

Replace the Wi-Fi credentials placeholders with your actual network name and password:

C++
char ssid[] = "YOUR_WIFI_NAME";      
char pass[] = "YOUR_WIFI_PASSWORD";  
(Optional) Update your Blynk Template ID and Auth Token if you are using your own Blynk Cloud project.

Select your ESP32 Dev Module board and the correct COM port, then click Upload.

During startup, the OLED will display "CALIBRATING LDRs...". Shine equal light on both sensors for 3 seconds to auto-calibrate the system offset.

Prerequisites and Setup (Web Application)
1. Clone the repository
Bash
git clone [https://github.com/aNkAn18a/AI-DRIVEN-FRAMEWORK-FOR-OPTIMIZING-RENEWABLE-ENERGY.git](https://github.com/aNkAn18a/AI-DRIVEN-FRAMEWORK-FOR-OPTIMIZING-RENEWABLE-ENERGY.git)
cd final-year-project
2. Install Dependencies
Ensure you have Python installed, then install the required libraries:

Bash
pip install flask scikit-learn pandas numpy
3. Add the Model File
Download the model from the Google Drive link provided in the note above, and place the file directly into the root directory of this project.

4. Run the Application
Start the Flask development server:

Bash
python app.py
Open your browser and navigate to http://127.0.0.1:5000/ to view the dashboard.

Hardware Components Used
ESP32 Development Board

Mini Solar Panel

SG90 Servo Motor

Dual LDR Sensors

ACS712 Current Sensor

OLED Display (SH1106)

TP4056 Charging Module and 18650 Li-ion Battery

Team Members
Soham Chatterjee

Anirban Das

Ankan Hazra

Soumyadeep Dey

Institution: St. Thomas' College of Engineering & Technology

Department: Department of Computer Science & Engineering

