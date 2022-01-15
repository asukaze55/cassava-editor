# Cassava Editor

あすかぜ が 2000 年から開発を続けている CSV 専用エディターです。
編集作業に必要なさまざまな機能を備えています。

## ビルド方法

### 開発環境

開発には Embarcadero C++Builder を使用しています。
個人またはスタートアップ企業の場合 https://www.embarcadero.com/jp/products/cbuilder/starter から C++Builder Community Edition がダウンロードできます。
その他の企業で Cassava Editor をビルドする場合、有償エディションの購入が必要となります。

### Boost Library

正規表現の処理に Boost Library を使用しています。
C++Builder の \[ツール\]-\[GetIt パッケージマネージャ...\] メニューでパッケージマネージャを開き、
「C++ Libraries」内の「Boost 1.70 for the Win64 Toolchain」および「Boost 1.70 for the Win32 Clang-enhanced Toolchain」をインストールしてください。

### プロジェクトのビルドと実行

cassava-editor リポジトリをクローンし、「Cassava**.groupproj」ファイルを C++Builder で開いてください。
\[実行\] メニューをクリックすると Cassava Editor がビルド・実行されます。

### テスト

実行中の Cassava Editor 内で  \[マクロ(M)\]-\[指定したマクロを実行(X)...\] メニューを実行し、
リポジトリ内の「Macro/tests/TestAll.cms」を選択してください。マクロのテストが実行されます。
今のところ、それ以外のテストはありません。変更箇所を手作業で動作確認してください。

## 関連リンク

- 公式ページ: https://www.asukaze.net/soft/cassava/
- 掲示板: https://www.asukaze.net/soft/cassava/bbs/
