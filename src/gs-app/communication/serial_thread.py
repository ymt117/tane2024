import threading
import serial
from views import text_info, motor, map, altitude

serial_port = '/dev/ttyUSB0'  # あなたのシリアルポートに合わせて変更してください
baud_rate = 115200

tlm_serial_data = None
tlm_state = None
tlm_count = []
tlm_altitude = []
tlm_motor = None
tlm_lat = []
tlm_lng = []

data_to_display = ""

# 保持するカウンタの数
max_data_count = 20
# 保持する高度データの数
max_data_altitude = 20

# 各テレメトリがカンマ区切りの何番目に位置するか定義
STATE     = 0
COUNT     = 1
MOTOR_A   = 2
MOTOR_B   = 3
ALTITUDE  = 4
LATITUDE  = 5
LONGITUDE = 6

def _serial_thread():
    global tlm_serial_data
    global tlm_state
    global tlm_count
    global tlm_altitude
    global tlm_motor
    global tlm_lat
    global tlm_lng
    with serial.Serial(serial_port, baud_rate, timeout=1) as ser:
        while True:
            try:
                tlm_serial_data = ser.readline().decode('utf-8').strip()
                values = [float(value) for value in tlm_serial_data.split(',')]
                # テレメトリから取得した値を各変数に代入する
                tlm_state = int(values[STATE])
                tlm_count.append(values[COUNT])
                if len(tlm_count) > max_data_count:
                    tlm_count.pop(0) # 古いデータを削除する
                tlm_altitude.append(values[ALTITUDE])
                if len(tlm_altitude) > max_data_altitude:
                    tlm_altitude.pop(0) # 古いデータを削除する
                tlm_motor = [values[MOTOR_A], values[MOTOR_B]]
                tlm_lat = [values[LATITUDE]]
                tlm_lng = [values[LONGITUDE]]
            except (ValueError, IndexError):
                pass  # 数値以外のデータや不正なデータは無視

def start_thread(app_instance):
    thread = threading.Thread(target=_serial_thread)
    thread.daemon = True
    thread.start()

    # アプリケーションインスタンスをスレッド内で使用する場合
    # 以下のようにしてグローバル変数として格納することも可能
    # global app
    # app = app_instance

def update_text_info(n):
    return text_info.update_text_info(tlm_state, tlm_lat, tlm_lng)

def update_graph_motor(n):
    return motor.update_graph_motor(tlm_motor)

def update_map(n):
    return map.update_map(tlm_lat, tlm_lng)

def update_graph_altitude(n):
    return altitude.update_graph_altitude(tlm_altitude, tlm_count)

def update_serial_console(n):
    global tlm_serial_data
    global data_to_display
    if tlm_serial_data is not None:
        data_to_display += tlm_serial_data + "\n"
        tlm_serial_data = "" # 受信データをクリア
        return data_to_display
    else:
        return ""
