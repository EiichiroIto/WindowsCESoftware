                TRAIN for WindowsCE Ver.0.43(Beta版)　README

         Copyright(C) 1996-2024 Eiichiroh Itoh All rights reserved.
	 mailto: eiichiro.ito@gmail.com

        * １．はじめに
        * ２．おことわり
        * ３．概要
        * ４．インストールと使い方
        * ５．謝辞
        * ６．参考文献

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

１．はじめに

     　TRAINをダウンロード頂きありがとうございます。

     　私は生まれつきの記憶力の弱さにより、首都圏在住２０数年にもなる
     のに未だに東京の交通網を理解しておりません。目的地に行くのにどこ
     でどう乗り換えたらいいのか見当もつかず、とんでもない遠回りをする
     ことはしょっちゅうです。仕事でサブノートを持ち歩いていた時期があ
     り、その中で「＊すぱあと」を使おうかと思ったこともありましたが、
     さすがに電車の中でサブノートを取り出すのも度胸がいります。まして
     立った状態ではとてもとても...

     　そこで、1994年にHP社の200LXというパームトップコンピュータ上に
     、TRAINというソフトを作成しました。おかげさまで好評を得、いろいろ
     な方々のご協力によってたくさんのデータも揃うようになりました。

     　そして1996年末にカシオから新しいハンドヘルドコンピュータ(以
     下HPC)「カシオペア」が発売されました。このHPCはＯＳにWindowsCEと
     いう新しいＯＳを搭載しており、ペンインターフェイスを備えています
     。この全く新しいマシンの上でも TRAINを動かそうと思って作成したの
     が、この「TRAIN for WindowsCE」です。

     　そうこうしているうちに，国内でも日本語版H/PCが発売されるように
     なりました。元々英語版向けに開発したTRAINですが，日本語版向けにバ
     グをとったバージョンを作成するのに加えて，より操作性とスピードを
     向上すべく修正を加えました。自分としては「まだまだ」の出来ですが
     ，より一層努力を重ねて使いやすいものにしていきたいと思っています
     。

     　感想などをメールやFWINCEUの7番会議室に頂けるととっても嬉しいで
     す。

     　なお、私のホームページで TRAINの紹介などもおこなっています。い
     ろんなプラットフォームへの移植情報なども載せていくつもりです。興
     味がありましたら一度ご覧ください。

          http://www.oohito.com/

２．おことわり

        * このパッケージ内のプログラムの著作権は作者(伊藤栄一郎)にあります。
        * このパッケージ内のプログラムを使用した、または、使用しなかっ
          たことによる全ての損害について作者はその責を負いませんし、ま
          た不具合等に対処する義務も一切負わない事をここで明らかにして
          おきます。
        * データファイル(*.dat, *.tri)への変更は構いません。但し、他人
          の作成した物を変更し配布する場合には、変更箇所の著作権表示に
          変更者と変更日時が分かるように明記して下さい。また全般的な再
          構成をされた方はその旨を明記しデーターを配布下さい。
          最後に、makedatでコンパイルされたデーターのみを配布されると
          きにも少なくとも元データーの著作権表示の部分を「grep」したフ
          ァイルは付けるマナーは守るようにしましょう。

３．概要

     　TRAINは、入力された２駅間を最短時間で結ぶ経路を探索するプログラ
     ムです。（もちろん最短時間といってもあくまでデータ上のことですが
     ．．．）つまり、列車でどこかの駅に行きたい時に、どのような乗り換
     えをすればいいかその目安を知らせてくれるわけです。

     　こんな機能を提供するプログラムは多々ありますが、この TRAINは以
     下のような特長を持っています。

        * WindowsCEを搭載したハンドヘルドコンピュータで動作する。
        * プログラムサイズが小さいのでフラッシュメモリ等の空き領域が少
          なくても大丈夫。
        * （機能が限られているので）動作スピードが早い。
        * （上と同じ理由により）操作も単純である。
        * データファイルの形式が公開されているので、自分でデータをつく
          れる。

     　もちろん、他の市販／シェアウェア／フリーソフトに比べて、劣る部
     分もたくさんあります。（かっこ内はいい訳です (^^;)）

        * 料金の計算もできないし、安い経路を探索することもできない。
          （料金での探索は難しいし...データいれるのも面倒...）
        * 全国にまたがったデータがない。
          （96/04/28現在、東京周辺、大阪周辺、名古屋周辺、四国、広島周
          辺、九州、ソウル地下鉄などのデータがあります。作成して下さっ
          た皆様ありがとうございました）
        * 最短経路を１通りしか知ることができない。
          （早いアルゴリズムを選んだので仕方ないのです。おそくてよけれ
          ば作りますが「＊すぱあと」にはかなわないような気が...）

４．インストールと使い方

     　パッケージ付属の INSTALL.TXTをご覧ください。Ｑ＆Ａ形式で記述さ
     れています。

５．謝辞

     　HP-200LX上のTRAINを登録して以来、数え切れない程のたくさんの方々
     からアドバイスやバグの指摘、励ましのメールを頂いております。ここ
     に、全てのみなさまへ感謝の意を表したいと思います。ありがとうござ
     いました。

     　また、TRAINホームパーティのメンバーの皆様からいつも適切な助言を
     頂き、大変感謝しております。主宰の植村　徹(KFC01106)様を始め常連
     の皆様、ありがとうございました。

     　最後に海の向こうからいつも激励のメールを下さる、TRAINデータの神
     様、ぼおぜ(PXR00066)様にお礼申し上げます。

６．参考文献

        * Stroustrup, Bjarne. 1991. The C++ Programming Language
          Second Edition,
          　Addison-Wesley Publishing Company, ISBN 0-201-53992-6
        * Kernighan, B.W. and Ritcie, D.M. 1981,プログラミング言語Ｃ,
          　共立出版株式会社,石田晴久訳, ISBN 4-320-02145-2
        * Sedgewick, Robert. 1992. Algorithms in C++, Addison-Wesley
          Publishing
          　Company, ISBN 0-201-51059-6
        * Moret, B.M.E. and Shapiro, H.D. 1991. Algorithms from P to
          NP,　The Benjamin/Cummings Publishing Company, INC. ISBN
          0-8053-8008-6
        * MATT関東圏私鉄・ＪＲ時刻表（１月号１９９５）,八峰出版株式会
          社
        * Schildt, Herbert. 1993. WindowsNTプログラミング入門
          　クオリティ株式会社,長谷川　章弘訳, ISBN4-7952-1478-6
        * Brain, Marshall. 1996. Win32システムサービスプログラミング,
          　プレンティスホール出版,郡司　芳昭訳, ISBN4-88735-030-9
        * Richter, Jeffrey. 1996.「WindowsCE(Pegasus):Microsoftの次期
          ポケットコンピュータについて知っておかなければならないこと」
          　Microsoft Systems Journal(P.42〜P.51),株式会社ASCII,
          ISBN4-7561-1972-7

（README終わり）
