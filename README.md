
# AI-Driven Framework for Optimizing Renewable Energy

An IoT-enabled smart solar tracking and energy prediction system. This project combines dual-axis sunlight tracking, live IoT monitoring, battery storage tracking, and a machine learning model to maximize energy generation and forecast electricity demand.

## 🚀 Features

* **Automatic Solar Tracking:** Orients the solar panel toward maximum sunlight using LDR sensors and a servo motor.
* **IoT Dashboard:** Real-time monitoring of generated power and battery status.
* **AI Energy Prediction:** Forecasts electricity consumption using a Random Forest Regression model.
* **Hardware Integrated:** Built using an ESP32 controller, sensors, and OLED display.

## 🛠️ System Architecture & Workflow

1. **Hardware Layer:** Dual LDR sensors detect sunlight intensity $\rightarrow$ ESP32 processes data $\rightarrow$ SG90 Servo motor rotates the panel.
2. **Data Layer:** The ACS712 current sensor measures generated power, which is stored in a 18650 Li-ion battery via a TP4056 module.
3. **IoT Layer:** ESP32 uploads live data over Wi-Fi to the dashboard.
4. **ML Layer:** Web application backend utilizes a Random Forest model to predict electricity consumption based on environmental factors.

---

## 📦 Repository Structure

```bash
├── templates/
│   └── dashboard.html   # Frontend web interface for analytics and tracking
├── app.py               # Flask backend handling ML inference and web routes
├── README.md            # Project documentation
└── .gitignore           # Ignores large model files

```

> **⚠️ Note on Machine Learning Model (`.pkl`):**
> The trained Random Forest model file (`model.pkl`) is omitted from this repository due to GitHub's file size limitations. To run the backend successfully, you must train your model locally or download the file from [Insert your external Google Drive / Dropbox / OneDrive Link here] and place it in the root directory.

---

## 📊 Machine Learning Model Details

* **Algorithm Used:** Random Forest Regression (Ensemble of Decision Trees)
* **Model Performance:** Achieved ~97.7% prediction accuracy ($R^2$ Score)
* **Input Parameters:** Temperature, Humidity, Wind Speed, Pressure, Rainfall, Solar Radiation, Hour, Day, Month, Weekend.
* **Output:** Electricity Consumption Prediction (Watts)

---

## 💻 Prerequisites & Setup

### 1. Clone the repository

```bash
git clone https://github.com/YOUR_USERNAME/YOUR_REPO_NAME.git
cd YOUR_REPO_NAME

```

### 2. Install Dependencies

Ensure you have Python installed, then install the required libraries:

```bash
pip install flask scikit-learn pandas numpy

```

### 3. Add the Model File

Place your trained `model.pkl` file directly into the root directory of this project.

### 4. Run the Application

Start the Flask development server:

```bash
python app.py

```

Open your browser and navigate to `[http://127.0.0.1:5000/](http://127.0.0.1:5000/)` to view the dashboard.

---

## 🔧 Hardware Components Used

* ESP32 Development Board
* Mini Solar Panel
* SG90 Servo Motor
* Dual LDR Sensors
* ACS712 Current Sensor
* OLED Display
* TP4056 Charging Module & 18650 Li-ion Battery

---

## 👥 Team Members

* Soham Chatterjee
* Anirban Das
* Ankan Hazra
* Soumyadeep Dey

**Institution**: St. Thomas' College of Engineering & Technology

**Department**: Department of Computer Science & Engineering
