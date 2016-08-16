# UEFI_FreeType_MSVC

UEFI アプリケーションで、英日中韓台のアウトライン・フォント表示を行うサンプル・プログラムです。

![screen shot](https://raw.githubusercontent.com/tenpoku1000/UEFI_FreeType_MSVC/master/images/README.PNG)

## 開発環境

* Visual Studio Community 2015 + Windows SDK for Windows 10
https://developer.microsoft.com/ja-jp/windows/hardware/windows-driver-kit
* 64 ビット版 Windows 10
* 64 ビット UEFI 対応 PC
* USB メモリ

## 実行方法

* USB メモリを PC に差し込みます。
* USB メモリを FAT32 形式でフォーマットします。
* efi フォルダを USB メモリにコピーします。
* Windows を再起動します。
* UEFI BIOS 画面を表示させ、USB メモリを Boot 順序最優先に設定します。  
  セキュアブート対応 PC ではセキュアブートを無効に設定します（無効化出来ない PC では動作不可）。
* UEFI BIOS の設定を保存して UEFI BIOS 画面から抜けると UEFI アプリケーションが実行されます。
* USB メモリを PC から抜いて任意のキーを押すと、PC がリセットされて Windows が起動します。

PC の機種によっては、Boot 順序設定変更を恒久的な設定変更ではなく、  
次回起動時のみ有効の設定とした方が好ましい場合があります。恒久的な  
設定変更で、  PC リセット後の Windows 起動がスムーズに行えないか、  
起動しない場合があるためです。ご注意ください。  

## ビルド方法

* UEFI_FreeType_MSVC.sln をダブルクリックします。
* Visual Studio のセキュリティ警告を回避してプロジェクトを開きます。  
![warning](https://raw.githubusercontent.com/tenpoku1000/UEFI_FreeType_MSVC/master/images/MSVC.PNG)
* 「ビルド」→「構成マネージャ」→「アクティブ ソリューション構成」で「Release」を選択
* F7 キーを押下します。

Release ビルドでは efi フォルダ以下に UEFI アプリケーションを生成します。  
Debug ビルドでは bin フォルダ以下に Windows Desktop アプリケーションを生成します。  

## 謝辞

本ソフトウェアは、以下の外部プロジェクトの成果物を利用しています。感謝いたします。

### The FreeType License
* The FreeType Project http://www.freetype.org/

### MIT License
* musl libc http://www.musl-libc.org/

### BSD License
* gnu-efi http://sourceforge.net/projects/gnu-efi/
* UDK2014 http://www.tianocore.org/udk2014/

### Apache License 2.0(Source Han Sans v1.001 or earlier)
* adobe-fonts/source-han-sans https://github.com/adobe-fonts/source-han-sans

## ライセンス

[MIT license](https://raw.githubusercontent.com/tenpoku1000/UEFI_FreeType_MSVC/master/LICENSE)

## 作者

市川 真一 <suigun1000@gmail.com>

## 参考資料

* Play with UEFI http://www.slideshare.net/syuu1228/play-with-uefi
* UEFI - PhoenixWiki http://wiki.phoenix.com/wiki/index.php/UEFI
* FreeType使用記録 http://hp.vector.co.jp/authors/VA028002/freetype.html

