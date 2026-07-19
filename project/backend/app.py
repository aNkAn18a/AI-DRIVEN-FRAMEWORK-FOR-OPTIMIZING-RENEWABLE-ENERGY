import os
import csv
import io
import math
import random
import joblib
import numpy as np
from datetime import datetime, timedelta
from flask import Flask, render_template, jsonify, Response, request

BACKEND_DIR = os.path.dirname(os.path.abspath(__file__))
FRONTEND_DIR = os.path.join(os.path.dirname(BACKEND_DIR), 'frontend')

app = Flask(__name__, template_folder=FRONTEND_DIR)
MODEL_PATH = os.path.join(BACKEND_DIR, 'electricity_model.pkl')

try:
    AI_MODEL = joblib.load(MODEL_PATH)
    print("\n🚀 SUCCESS: 11-Feature AI Model Loaded Cleanly!")
except Exception as e:
    AI_MODEL = None
    print(f"\n⚠️ Model fallback active: {e}")

# Global storage for the latest ESP32 real-time sensor data
LIVE_SENSOR_DATA = {
    "voltage": 5.02,
    "current": 0.41,
    "power": 2.06,
    "energy_today": 6.35,
    "prediction_tomorrow": 7.10
}

@app.route('/api/update-sensor', methods=['POST'])
def update_sensor_data():
    global LIVE_SENSOR_DATA
    try:
        data = request.get_json()
        if data:
            LIVE_SENSOR_DATA["voltage"] = round(float(data.get("voltage", LIVE_SENSOR_DATA["voltage"])), 2)
            LIVE_SENSOR_DATA["current"] = round(float(data.get("current", LIVE_SENSOR_DATA["current"])), 2)
            LIVE_SENSOR_DATA["power"] = round(float(data.get("power", LIVE_SENSOR_DATA["power"])), 2)
            LIVE_SENSOR_DATA["energy_today"] = round(float(data.get("energy_today", LIVE_SENSOR_DATA["energy_today"])), 2)
            LIVE_SENSOR_DATA["prediction_tomorrow"] = round(float(data.get("prediction_tomorrow", LIVE_SENSOR_DATA["prediction_tomorrow"])), 2)
            return jsonify({"status": "success", "message": "Sensor data updated"}), 200
    except Exception as e:
        return jsonify({"status": "error", "message": str(e)}), 400
    return jsonify({"status": "error", "message": "Invalid data"}), 400

def get_ai_predictions():
    data_matrix = []
    now = datetime.now()
    
    for i in range(24):
        forecast_time = now + timedelta(hours=i)
        
        forecast_hour = forecast_time.hour
        forecast_day = forecast_time.day
        forecast_month = forecast_time.month
        
        # Sun=0, Mon=1, Tue=2, Wed=3, Thu=4, Fri=5, Sat=6
        forecast_weekday = (forecast_time.weekday() + 1) % 7
        is_weekend = 1 if forecast_weekday in [0, 6] else 0
        
        hour_str = f"{str(forecast_hour).zfill(2)}:00"
        
        # All weather features are dynamically modeled per hour
        temp = round(26.0 + math.sin((forecast_hour - 8) / 24 * math.pi * 2) * 4 + random.uniform(-0.5, 0.5), 1)
        humidity = round(75.0 - math.sin((forecast_hour - 8) / 24 * math.pi * 2) * 15 + random.uniform(-2, 2), 1)
        wind_speed = round(2.5 + random.uniform(-0.8, 1.2), 1)
        pressure = round(1005.0 + random.uniform(-3, 3), 1)
        rainfall = round(random.choice([0.0, 0.0, 0.0, 0.1, 0.0]), 1)
        solar_rad = round(max(0, math.sin((forecast_hour - 6) / 12 * math.pi) * 650 + random.uniform(-20, 20)), 1) if 6 <= forecast_hour <= 18 else 0.0

        if AI_MODEL:
            try:
                input_features = np.array([[
                    temp, humidity, wind_speed, pressure, rainfall, solar_rad,
                    forecast_hour, forecast_day, forecast_month, forecast_weekday, is_weekend
                ]], dtype=np.float32)
                
                prediction = AI_MODEL.predict(input_features)
                demand = round(float(prediction[0]), 2)
            except Exception:
                demand = round(3200 + (solar_rad * 0.5) + (temp * 15) + random.randint(-100, 100), 2)
        else:
            demand = round(3800 + math.sin((forecast_hour - 9) / 24 * math.pi * 2) * 1100 + random.randint(-150, 150), 2)

        solar_gen = round(solar_rad * 1.8) if 6 <= forecast_hour <= 18 else 0
        wind_gen = round(680 + (forecast_hour * 8) + random.randint(-40, 40))
        green_total = solar_gen + wind_gen
        
        data_matrix.append({
            'hour_label': hour_str,
            'hour': forecast_hour,
            'day': forecast_day,
            'month': forecast_month,
            'weekday': forecast_weekday,
            'is_weekend': is_weekend,
            'temp': temp,
            'humidity': humidity,
            'wind_speed': wind_speed,
            'pressure': pressure,
            'rainfall': rainfall,
            'solar_rad': solar_rad,
            'solar': max(0, solar_gen),
            'wind': max(0, wind_gen),
            'greenTotal': max(0, green_total),
            'demand': max(0, demand)
        })
    return data_matrix

@app.route('/')
def dashboard_home():
    return render_template('dashboard.html')

@app.route('/api/analytics/')
def grid_analytics_api():
    data = get_ai_predictions()
    demands = [row['demand'] for row in data]
    
    response = jsonify({
        'metrics': {
            'peakDemand': max(demands) if demands else 0,
            'minDemand': min(demands) if demands else 0,
        },
        'live_sensors': LIVE_SENSOR_DATA,
        'dataset': data
    })
    
    response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
    response.headers["Pragma"] = "no-cache"
    response.headers["Expires"] = "0"
    return response

@app.route('/api/export-csv/')
def export_grid_csv():
    output = io.StringIO()
    writer = csv.writer(output)
    
    writer.writerow([
        'Hour', 'Day', 'Month', 'Weekday', 'Weekend', 
        'Temperature (°C)', 'Humidity (%)', 'Wind Speed (m/s)', 
        'Pressure (hPa)', 'Rainfall (mm)', 'Solar Radiation', 'Demand Prediction (W)'
    ])
    
    for row in get_ai_predictions():
        scaled_demand = round(row['demand'] / 200, 2)
        
        writer.writerow([
            row['hour_label'], row['day'], row['month'], row['weekday'], row['is_weekend'], 
            row['temp'], row['humidity'], row['wind_speed'], 
            row['pressure'], row['rainfall'], row['solar_rad'], scaled_demand
        ])
        
    output.seek(0)
    return Response(output.getvalue(), mimetype="text/csv", headers={"Content-Disposition": "attachment; filename=AI_Grid_Optimization_Dataset.csv"})

if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0', port=8000)