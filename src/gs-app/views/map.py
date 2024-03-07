'''
マップの描画
'''
import plotly.graph_objects as go

goal_lat = 35.714949
goal_lng = 139.669047

def update_map(tlm_lat, tlm_lng):
    if tlm_lat != [] and tlm_lng != []:
        # 地図の作成
        fig = go.Figure()

        # ゴールのポイント
        fig.add_trace(go.Scattergeo(
            lat=[goal_lat],
            lon=[goal_lng],
            mode='markers',
            marker=dict(size=14, color='red'),
            text=['goal'],
            name='goal',
        ))

        # ローバーのポイント
        fig.add_trace(go.Scattergeo(
            lat=tlm_lat,
            lon=tlm_lng,
            mode='markers',
            marker=dict(size=14, color='blue'),
            text=['rover'],
            name='rover',
        ))

        # 地図のスタイル設定
        fig.update_geos(
            center=dict(lat=goal_lat, lon=goal_lng), # 地図の中心を指定
            projection_type='natural earth', # 地図の投影タイプを指定
            lataxis_range=[goal_lat - 0.0002, goal_lat + 0.0002], # 緯度の範囲を指定
            lonaxis_range=[goal_lng - 0.0002, goal_lng + 0.0002], # 経度の範囲を指定
        )

        # レイアウトの設定
        fig.update_layout(
            width=400, # 幅をピクセルで指定
            height=400, # 高さをピクセルで指定
            margin=dict(l=50, r=50, t=50, b=50), # 上下左右のマージン
        )

        return fig
    else:
        return None