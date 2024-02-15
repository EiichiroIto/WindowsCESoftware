MukaMuka

    Copyright (C) 2024, Eiichiro Ito
    mailto: eiichiro.ito@gmail.com

* MIT License

    Copyright (c) 2024 Eiichiro Ito

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.

* Introduction
      　MukaMuka は，動き回る物体を避けながら白い道をスタイラスでたどっ
      ていく単純なゲームです。

    左上のスタート領域でスタイラスをタップし，ゲートが開いたらタップし続
    けたまま，右下のゴールへ向かってください。

------------------------------------------------------------------------

MukaMuka の画面は自由に設計することができます。MukaMuka の面データはビッ
トマップファイルと，物体を記述したMUKファイルから成り立っています。

ビットマップファイル(????????.2BP)のサイズは 480x192 ドットが適当でしょう。

MUKファイル(????????.MUK)は1行の長さが固定長のテキストファイルです。
1行で1物体を定義しています。各桁の意味は以下の通りです。サンプルも参考に
してください。

カラム位置 	　 	　
0 	領域/物体の種類(type) 	s = スタート領域(x,y,w,hのみ有効)
g = ゴール領域(x,y,w,hのみ有効)

B = 上下に行き来する[箱]
S = 上→下，下→上にスクロールする[すきま]
O = [転がる玉]
P = 左右に伸びる棒

01-03 	物体領域の左上X座標(x) 	物体が通過する全領域を指定します。
物体そのもののサイズはオプションで指定します。

[転がる玉]では高さを無視します。

04-06 	物体領域の左上Y座標(y)
07-09 	物体領域の幅(w)
10-12 	物体領域の高さ(h)
13 	ダミー 	通常はカンマです
14-16 	Tick Count(tick) 	Tick Countは物体を描画するタイミングを指定します。

例えば，TickCount=1のときは毎回描画，
2のときは2回に1回描画します。

17-19 	State数(state) 	物体の状態数です。大きいほどきめの細かい動作に
なりますが，1クール終えるまでに時間がかかるように
なります。

ただし，「転がる玉」で10の倍数を指定して下さい。

20-22 	開始State番号(cur_state) 	どの状態から開始するかを指定します。

値により最初に表示される位置が変わります。

23 	ダミー 	通常はカンマです
24-26 	オプション1(w2) 	[箱]
　オプション1 = 箱の幅
　オプション2 = 箱の高さ

[すきま]
　オプション1 = 方向(0=上→下, 1=下→上)
　オプション2 = すきまの幅

[転がる玉]
　オプション1 = 玉の直径

[棒]
　オプション1 = 方向(0=左→右, 1=右→左)

27-29 	オプション2(h2)
30-31 	ダミー 	通常は CR+LF です
