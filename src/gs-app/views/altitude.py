'''
高度のグラフ
'''
import plotly.graph_objects as go

def update_graph_altitude(tlm_altitude, tlm_count):
    if tlm_altitude != []:
        fig = go.Figure()

        fig.add_trace(go.Scatter(
            x=tlm_count,
            y=tlm_altitude,
            mode='lines',
        ))

        # レイアウトの設定
        fig.update_layout(
            yaxis=dict(range=[-10, 100]),
            xaxis_title='経過時間',
            yaxis_title='高度',
        )

        return fig
    else:
        None