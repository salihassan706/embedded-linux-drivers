from flask import Flask, render_template_string
import sqlite3

app = Flask(__name__)
DB_PATH = "/home/pi/sensor_data.db"

HTML = '''
<!DOCTYPE html>
<html>
<head>
    <title>MPU6050 Sensor Dashboard</title>
    <meta http-equiv="refresh" content="2">
    <style>
        body { font-family: Arial; background: #1a1a2e; color: #eee; padding: 20px; }
        h1   { color: #00d4ff; text-align: center; }
        .stats { display: flex; justify-content: center; gap: 20px; margin: 20px 0; }
        .card  { background: #16213e; padding: 20px; border-radius: 10px; text-align: center; min-width: 150px; }
        .card h3 { color: #00d4ff; margin: 0 0 10px 0; }
        .card p  { font-size: 24px; margin: 0; color: #00ff88; }
        table { width: 100%; border-collapse: collapse; margin-top: 20px; }
        th { background: #16213e; color: #00d4ff; padding: 10px; }
        td { padding: 8px; border-bottom: 1px solid #333; text-align: center; }
        tr:hover { background: #16213e; }
    </style>
</head>
<body>
    <h1>MPU6050 Live Sensor Dashboard</h1>
    <p style="text-align:center; color:#888">Auto-refreshes every 2 seconds | Total readings: {{ total }}</p>

    <div class="stats">
        <div class="card">
            <h3>Accel X</h3>
            <p>{{ "%.3f"|format(latest.accel_x) }}</p>
        </div>
        <div class="card">
            <h3>Accel Y</h3>
            <p>{{ "%.3f"|format(latest.accel_y) }}</p>
        </div>
        <div class="card">
            <h3>Accel Z</h3>
            <p>{{ "%.3f"|format(latest.accel_z) }}</p>
        </div>
        <div class="card">
            <h3>Gyro X</h3>
            <p>{{ "%.3f"|format(latest.gyro_x) }}</p>
        </div>
        <div class="card">
            <h3>Gyro Y</h3>
            <p>{{ "%.3f"|format(latest.gyro_y) }}</p>
        </div>
        <div class="card">
            <h3>Gyro Z</h3>
            <p>{{ "%.3f"|format(latest.gyro_z) }}</p>
        </div>
    </div>

    <table>
        <tr>
            <th>Timestamp</th>
            <th>Accel X</th>
            <th>Accel Y</th>
            <th>Accel Z</th>
            <th>Gyro X</th>
            <th>Gyro Y</th>
            <th>Gyro Z</th>
        </tr>
        {% for row in rows %}
        <tr>
            <td>{{ row.timestamp[11:19] }}</td>
            <td>{{ "%.3f"|format(row.accel_x) }}</td>
            <td>{{ "%.3f"|format(row.accel_y) }}</td>
            <td>{{ "%.3f"|format(row.accel_z) }}</td>
            <td>{{ "%.3f"|format(row.gyro_x) }}</td>
            <td>{{ "%.3f"|format(row.gyro_y) }}</td>
            <td>{{ "%.3f"|format(row.gyro_z) }}</td>
        </tr>
        {% endfor %}
    </table>
</body>
</html>
'''

@app.route('/')
def dashboard():
    conn = sqlite3.connect(DB_PATH)
    conn.row_factory = sqlite3.Row
    rows = conn.execute(
        'SELECT * FROM sensor_readings ORDER BY id DESC LIMIT 50'
    ).fetchall()
    total = conn.execute(
        'SELECT COUNT(*) FROM sensor_readings'
    ).fetchone()[0]
    conn.close()
    latest = rows[0] if rows else None
    return render_template_string(HTML, rows=rows, latest=latest, total=total)

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=False)
