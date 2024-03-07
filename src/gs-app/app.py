import dash
from dash import dcc, html
from dash.dependencies import Input, Output
from communication import serial_thread

# Dashアプリケーションのセットアップ
app = dash.Dash(__name__)

# レイアウトの設定
app.layout = html.Div([
    html.H4("Grand Station: 100kinSAT"),
    html.Div(id='live-update-text'),
    html.Div(
        dcc.Graph(id='live-update-motor'),
        style={'display': 'inline-block', 'width': '30%'},
    ),
    html.Div(
        dcc.Graph(id='my-map'),
        style={'display': 'inline-block', 'width': '30%'},
    ),
    html.Div(
        dcc.Graph(id='live-update-altitude'),
        style={'display': 'inline-block', 'width': '30%'},
    ),
    dcc.Textarea(
        id='live-update-console',
        value='',
        style={'width': '90%', 'height': '500px'},
        readOnly=True,
    ),
    dcc.Interval(
        id='interval-component',
        interval=1000,  # ミリ秒単位で1秒ごとに更新
        n_intervals=0
    ),
])

# シリアル通信用のスレッドを開始
serial_thread.start_thread(app)

# コールバック関数の定義
@app.callback(Output('live-update-text', 'children'),
              Input('interval-component', 'n_intervals'))
def update_text_info(n):
    return serial_thread.update_text_info(n)

# コールバック関数の定義
@app.callback(Output('live-update-motor', 'figure'),
              Input('interval-component', 'n_intervals'))
def update_motor(n):
    return serial_thread.update_graph_motor(n)

# コールバック関数の定義
@app.callback(Output('my-map', 'figure'),
              Input('interval-component', 'n_intervals'))
def update_map(n):
    return serial_thread.update_map(n)

# コールバック関数の定義
@app.callback(Output('live-update-altitude', 'figure'),
              Input('interval-component', 'n_intervals'))
def update_map(n):
    return serial_thread.update_graph_altitude(n)

# コールバック関数の定義
@app.callback(Output('live-update-console', 'value'),
              Input('interval-component', 'n_intervals'))
def update_map(n):
    return serial_thread.update_serial_console(n)

if __name__ == '__main__':
    # NOTE: debug=TrueだとCOMポート開けない
    # https://stackoverflow.com/questions/50390506/how-to-make-serial-communication-from-python-dash-server
    app.run_server(debug=False)