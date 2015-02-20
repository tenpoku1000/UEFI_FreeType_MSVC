# UEFI_FreeType_MSVC

UEFI アプリケーションで、英日中韓台のアウトライン・フォント表示を行うサンプル・プログラムです。

![screen shot](https://github.com/tenpoku1000/UEFI_FreeType_MSVC/images/README.png)

## 開発環境

* Visual Studio Community 2013  
http://www.microsoft.com/ja-jp/dev/products/community.aspx
* Windows 8.1 Update 64 ビット版
* 64 ビット UEFI 対応 PC
* USB メモリ

## 実行方法

* USB メモリを PC に差し込みます。
* USB メモリを FAT32 形式でフォーマットします。
* efi フォルダを USB メモリにコピーします。
* Windows を再起動します。
* UEFI BIOS 画面を表示させ、USB メモリを Boot 順序最優先に設定します。
* UEFI BIOS の設定を保存して UEFI BIOS 画面から抜けると UEFI アプリケーションが実行されます。
* USB メモリを PC から抜いて任意のキーを押すと、PC がリセットされて Windows が起動します。

## ビルド方法

* UEFI_FreeType_MSVC.sln をダブルクリックします。
* Visual Studio の起動後に F7 キーを押下します。

Release ビルドでは efi フォルダ以下に UEFI アプリケーションを生成します。  
Debug ビルドでは bin フォルダ以下に Windows Desktop アプリケーションを生成します。  

## 謝辞

本ソフトウェアは、以下の外部プロジェクトの成果物を利用しています。感謝いたします。

* The FreeType Project http://www.freetype.org/
* musl libc http://www.musl-libc.org/
* gnu-efi http://sourceforge.net/projects/gnu-efi/
* UDK2014 http://www.tianocore.org/udk2014/
* adobe-fonts/source-han-sans https://github.com/adobe-fonts/source-han-sans

## ライセンス

[MIT license](https://github.com/tenpoku1000/UEFI_FreeType_MSVC/LICENCE)

## 作者

市川 真一 <suigun1000@gmail.com>

## 参考資料

* UEFI - PhoenixWiki http://wiki.phoenix.com/wiki/index.php/UEFI
* FreeType使用記録 http://hp.vector.co.jp/authors/VA028002/freetype.html
