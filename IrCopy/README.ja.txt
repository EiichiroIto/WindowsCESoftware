                IrCopy V0.10 -- H/PCのIRを使ってファイル転送

                     Copyright (C) 2024, Eiichiro Ito
		     mailto: eiichiro.ito@gmail.com

* Introduction

     　これは、WindowsCEの赤外線ポートを使って、ファイルの転送を行うた
     めのプログラムです。

     IrCopyの主な特徴は

        * 予め転送するファイルを複数選んでおき、接続時、一度に転送を行
          うことができる。
        * 転送先のディレクトリ／ファイル名も転送元で指定できる。
        * 互いに転送するファイルを指定してあれば、双方向に転送を行う。
        * 転送パターンをファイルに保存しておくことができる。
        * 英語版H/PCと日本語版H/PCでファイルの交換が可能である。
        * EXEファイルも転送可能である。
        * Microsoft InfraRed V3.0モジュールを組み込んだWindows95マシン
          とファイルのやりとりができる(一部制限あり。Bugsを参照のこと)

     赤外線を用いたファイルの転送は、H/PCエクスプローラにも標準であり
     ますが、英語版H/PCと日本語版H/PCで通信できなかったり、一度に1つの
     ファイルしか転送できなかったりと制限が大きいので、このプログラム
     を作成しました。

     V0.08からの変更点は、

        * Windows95版の作成と、それに対応させるためのプロトコルの修正
        * ファイル選択ダイアログボックスの変更

     です。

     ※なお、通信プロトコルの変更のため、V0.08と互換性はありません。

* Installation

     　IrCopy.EXEを、ハンドヘルドPCエクスプローラなどを用いて、H/PC上
     の適当なディレクトリにコピーしてください。

* Usage

     　IrCopy.EXEを起動すると、一覧画面があらわれます。この画面の左側
     には、ローカルの転送予定ファイル名が表示され、右側には転送先のフ
     ァイル名が表示されていきます。

     {File}-{Add}を選ぶと、転送するファイル名の入力ダイアログが現れま
     す。ここに指定したファイルが、赤外線で接続された別のH/PCに転送さ
     れます。ファイルを転送先のH/PC上の別ディレクトリ／ファイルに転送
     したい場合は、下の段に転送先ファイル名を記述してください。デフォ
     ルトでは、転送元と同じディレクトリ／ファイルに転送されます。{OK}
     で確定すると転送ファイル一覧に表示されます。

     転送したいファイルを指定しおわったら、転送先のH/PCでもIrCopyを起
     動し、{Transfer}-{Receive}を選択して転送ダイアログを表示しておき
     ます。そして、転送元と転送先のH/PCの赤外線ポートを10cm程度離して
     向かい合わせにしておき、転送元のH/PCで、{Transfer}-{Send}を選択し
     ます。すると、しばらくして接続音がなり、転送が行われます。

     なお、転送先でも同様に転送したいファイルを指定しておけば、転送先
     の方からもファイルを取得することができます。つまり、転送元と転送
     先の役割は相対的なもので、役割を交代させても動作は同じです。

     {Transfer}-{Execute}では、最初に送信を試み、転送先が見つからなけ
     れば、受信側に回って転送元の接続を待ちます。また、この場合、一度
     接続が成功すれば転送後にダイアログが自動的に閉じます。

     転送が成功すると、転送一覧は削除されます。もし、転送パターンを保
     存しておく必要がある場合は、転送に先立って{File}-{Export}で転送パ
     ターンを保存することができます。逆に、{File}-{Import}で保存してお
     いた転送パターンを取り込むこともできます。

     なお、未転送の転送一覧は、プログラムの終了時にファイルに保存され
     るので、IrCopyを終了しても次回起動時に転送一覧の内容は復帰します
     。

* Microsoft InfraRed V3.0

     Windows95版を動作させる場合、上記バージョンの赤外線ドライバが必要
     です。このドライバの日本語版は、97/11/09現在作成されていません。
     現時点で、英語版ドライバが

     http://www.microsoft.com/windows95/info/irda.htm

     から入手することができます。なお、このドライバが正しく動作する保
     証はないので、利用には十分注意してください。

* Bugs

     現在、以下のような不具合／問題点があります。

        * 転送中に接続を中断すると、次回から{Receive}でエラーが発生す
          る場合があります。このような時は、一旦IrCopyを終了して、再度
          起動しなおしてください。
        * 転送に関しては特にエラーチェック等を行っていません。重要なフ
          ァイルの転送等については、信頼をおかないで下さい。
        * 日本語のファイル名の転送ができない。
        * Win95版ではReceiveモードが実行できない(原因不明)。受け取るフ
          ァイルだけしかない場合でも必ずSendで通信すること。
        * Win95版ではカレントドライブの内容しか表示しません。おまけソ
          フトと思ってください。

* Copyright

     This program is a copyrighted freeware. Its copyright is owned by
     its author. You may use it freely, but you are not permitted to
     use it for any commercial purpose. If you want to post this
     program to other BBS, ftp site or network, or if you want to
     bundle it to other program or publication, you should receive
     prior permission from its author. The author is in no way liable
     to any damage, which may be caused by the use of or inability to
     use this software.

     このプログラムはフリーソフトウェアです。著作権は伊藤　栄一郎が保
     持しています。このプログラムをBBSやftpサイトなどで配布する場合は
     あらかじめ著作権者まで許可を取ってください。

     また，このプログラムは無保証です。このプログラムを利用して被った
     被害等に関して著作権者は責任を負わないものとします。全て利用者の
     自己責任においてこのプログラムを利用してください。

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

* Thanks

     Windows95版を作成するにあたって、水越康博さんから情報を頂きました
     。ありがとうございました。

* History

      Date     Ver. Brief comment
      97/11/03 0.01 作成開始
      97/11/06 0.08 ベータ公開
      97/11/09 0.10 95版作成、ダイアログ変更
      2024/02/15 ---- MIT License に変更した

IrCopy -- Copyright (C) 2024, Eiichiro Ito
