## 動作環境

- Python 3.11

## 実行前の準備

Pythonのパッケージをインストールする。

```sh
cd path/to/gs-app
pip install -r requirement.txt
```

`/gs-app/communication/serial_thread.py`の 5行目を自身のCanSatのポート番号に修正する。

Windowsの場合`COM6`など。

```python
serial_port = '/dev/ttyUSB0'  # あなたのシリアルポートに合わせて変更してください
```

## 実行

```sh
cd path/to/gs-app
python app.py
```

ブラウザで`127.0.0.1:8050`にアクセスする。
