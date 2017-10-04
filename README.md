kumaWorkspace
====

# ETロボコン開発用リポジトリ

# ファイル構成
  - README.md
    - このファイルです。
  - bond
    - リリース用のフォルダです。
  - model
    - モデリング作業用のフォルダです。
  - parts
    - 各種モジュール開発用のフォルダです。
  - schedule
    - スケジュール表の置き場所です。

# 開発手順
  - make app=$filename TGT=$appname
    - 対象ファイルをコンパイルします。

# 開発環境
  - 使用言語 : C++
  - コンパイラ : gcc

# 依存ライブラリ
  - ev3rt
