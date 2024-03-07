'''
モータ制御量のグラフ
'''
import plotly.graph_objs as go

yaxis_max = 255
yaxis_min = -255

def update_graph_motor(tlm_motor):
    if tlm_motor is not None:
        trace = go.Bar(
            x=[f'モータ {i+1}' for i in range(len(tlm_motor))],
            y=tlm_motor,
            name='Value',
            width=0.5,
        )

        layout = go.Layout(
            yaxis=dict(title='PWM', range=[yaxis_min, yaxis_max]),
            width=400
        )

        return {'data': [trace], 'layout': layout}
    else:
        return {'data': [], 'layout': go.Layout(title='モータの制御量')}