# Opener_sproject
文系の子とプロジェクトを制作してみる


機能1
ノートをアップロードしたらAIが概要・内容・結論をまとめて出力
機能2
授業のコマを入力したら、単位数を最大化した時間割案が表示される
(条件として、空きコマを避けたり、1限は空けるなど指定できる)


技術スタック
・機能2の時短を図るためc++
・フレームワークはcrow
・ライブラリはnlohmann/json(JSONの読み書き),cpp-httplib(AIを使うため)


main.exeを実行すれば、サーバーを建てることができる。
例:PS C:\Users\user\Documents\univer_support\Opener_sproject> .\src\main.exe

ファイル
main.cpp:サーバーを建てるためのファイル。(これを実行したらサイトから検索できる)


Docker手順:Docker Desktopのインストール,vscode(Dev Containers)のインストールをする。次に><ボタンからReopenを押す。そんでmain.cppを実行する。