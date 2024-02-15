                       CaptCE V0.07 --画面キャプチャ

                  Copyright (C) 1997-2024, Eiichiro Ito
		  mailto: eiichiro.ito@gmail.com

* Introduction

     　これは、WindowsCEの画面内容をBMPファイル(16色/256色)や2BPファイ
     ル(4色)に格納するためのプログラムです。

     CaptCEの主な特徴は

        * H/PCの画面内容をBMPファイルや2BPファイルとして保存できる。
        * 英語版・日本語版を問わずH/PC上で動作可能。
        * 480x240だけでなく、640x240の画面サイズにも対応。
        * 自動的にファイル名を生成し連続してキャプチャできる。
        * ShiftキーやControlキーの押下でキャプチャの開始を指示できる
          。(V0.05)
        * カラーH/PCの画面もキャプチャできる。(V0.06)

     詳しくは、UsageやOptionsを参照してください。

* Installation

     　CaptCE.EXEを、ハンドヘルドPCエクスプローラなどを用いて、H/PC上
     の適当なディレクトリにコピーしてください。

* Usage

     　CaptCE.EXEを起動すると、画面右下の通知エリアにカメラのアイコン
     が現れます。キャプチャしたい画面内容になるようアプリケーションを
     操作し、このアイコンをクリックすればデフォルトでルートディレクト
     リに、「CAPTxxxx.BMP」というファイルが作成されます。

          xxxxというのは0000から始まる連番です。同名のファイルが
          あった場合は、その次の番号になります。

     なお、ShiftキーやControlキーを押しながらアイコンをクリックすると
     、ビープ音が鳴ります。ここで、キャプチャしたいアプリケーションの
     画面表示にして、再度ShiftキーやControlキーを押すと(クリックは必要
     ありません)、押した時点での画面がキャプチャされます。

          この操作は、最初にクリックしてから1分以内に行ってくださ
          い。何もせずに50秒が経過するとアラームがなり警告します
          。

          キャプチャする時には、クリックと同時に押したキーを押し
          てください。ShiftキーとControlキーを同時に押しても構い
          ませんが、その場合はキャプチャする時に、Shiftキー
          とControlキーを同時に押してください。

* Options

     　コマンドラインにはオプションを指定することもできます。オプショ
     ン指定する場合の形式は、

     CAPTCE /オプション名 ...

     となります。使用可能なオプションは、

        * /DIALOG

          連番でのファイル保存ではなく、ファイル保存ダイアログを
          開いて直接格納ファイル名を指定します。一度に沢山のキャ
          プチャを行うのではなく、ちょっとした画面保存などに向い
          ています。

        * /2BP

          デスクトップPCで参照できるBMPファイルではなく
          、WindowsCEに独特な2BPファイルで保存します。キャプチャ
          画面をH/PCで利用したい場合に向いています。

        * /NOSOUND

          キャプチャ時に鳴る音を抑制します。音を鳴らすことでキャ
          プチャに失敗するような場合、このオプションを指定してく
          ださい。

     例) 2BPファイルで保存し、サウンドを使用しない。

          CaptCE.EXE /2BP /NOSOUND

     例) 2BPファイルで保存し、ファイル保存ダイアログを用いてキャプチャ
     結果を保存する

          CaptCE.EXE /dialog /2bp

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

* History

      Date     Ver. Brief comment
      97/04/29 0.01 作成開始
      97/04/29 0.02 ベータ公開
      97/10/26 0.04 2BPファイル保存、キャプチャ時サウンド、ベータ公開
      98/01/22 0.05 タイマー付きキャプチャ機能
      98/02/18 0.06 カラーH/PCの場合はカラーBMPを生成するようにした
      98/02/19 0.07 カラー版でデフォルトパレットを持つようにした
      2024/02/15 ---- MIT License に変更した

CaptCE -- Copyright (C) 1997-2024, Eiichiro Ito
