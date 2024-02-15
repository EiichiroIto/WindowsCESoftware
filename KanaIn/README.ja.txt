KanaIn.EXE V0.02 --- 半角かな入力プログラム

    Copyright (C) 2024, Eiichiro Ito
    mailto: eiichiro.ito@gmail.com

MIT License

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

これは、ローマ字を半角かなに変換するプログラムです。

このプログラムは、以下の手順で半角かな入力を行います。
  1. 入力したローマ字を半角かなに変換する
  2. 変換結果をクリップボードに格納する
  3. 直前のアプリにタスクスイッチする
  4. ペースト(Ctrl-V)する
  5. KanaIn に戻る

V0.01 との違い
  * 他アプリでのペースト処理の自動化
  * '.' '[' ']' ',' '/' をそれぞれ '｡' '｢' '｣' '､' '･' に変換
  * 大文字で入力できないバグを修正
  * 画面レイアウトの変更
  * wo を 'ヲ' に、di を 'ヂ' に変換するようにした。

準備
  1. まず、Windows95 などからシステムフォントを HPC の /Windows/Fonts ディレ
     クトリにコピーして下さい。
  2. Pocket Word など、文章編集可能なプログラムを起動し、1. でコピーしたフォ
     ントを選択して半角かなが表示できることを確認してください。
  3. KanaIn.EXE を、HPC の適当なディレクトリにコピーしてください。
  4. KanaIn.EXE を起動してください。

操作
  KanaIn.EXE のダイアログにローマ字を入力してEnter を押すと、KanaIn の画面が
隠れ、元々開いていたアプリが開かれます。その後アプリ内にかな文字が挿入され、
再度 KanaIn の画面に戻ります。
  終了させたい場合は、ダイアログが表示されている状態で ESC を押してください。

KanaIn.EXE              Copyright (C) 2024, Eiichiroh Itoh
