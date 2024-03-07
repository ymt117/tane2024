'''
テキスト情報の描画
'''
from dash import html

state_list = [
    '待機状態',
    '打ち上げ中',
    '放出',
]

def update_text_info(tlm_state, tlm_lat, tlm_lng):
    style = {'padding': '5px', 'fontsize': '16px'}
    if tlm_state is not None:
        return [
            html.Span('ステータス: {}'.format(state_list[tlm_state]), style=style),
            html.Span('緯度: {}'.format(tlm_lat), style=style),
            html.Span('経度: {}'.format(tlm_lng), style=style),
        ]
    else:
        return [
            html.Span('ステータス: None', style=style),
        ]