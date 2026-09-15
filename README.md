- このMarkdownは現在整備途中です。分かりづらい点が多くあると思いますが、ご了承ください。
 
 # CSharpAudio Version 0.1
- C++/CLIで作成したXAudio2のライブラリです
- C#でも手軽に音楽を再生するために作成されました。

# Requirement
- VisualStudio2022以上

# Build
> [!NOTE]
> CSharpAudio.cpp内でE2242エラーが発生していますが、IntelliSenseの不具合だと思われます。\
> ビルド自体は通るので、明確な解決方法が見つかるまでそのままにしておく予定です。\
> 問題のコードは100~130行目に含まれています。

1. 任意のディレクトリにCSharpAudioのリポジトリをクローンする\
	``git clone https://github.com/ocha545/CSharpAudio.git AnyDirectory``
2. slnファイルをVisualStudioで開き、C#向けのDLLを生成する
	1. .NET8以上のバージョン向けでは構成を**Debug-netcore**に設定してビルドして下さい
	2. .NET Framework向けでは構成を**Debug-netframework**に設定してビルドして下さい

# Samples
- Samplesフォルダには音楽を再生するコードが記載されています。\
どちらもCSharpAudioをビルドして、DLLを生成しないと使うことが出来ません。
- このサンプルで使用している音声ファイルは魔王魂からダウンロードした物です。
- **シャイニングスター : 魔王魂(MaouDamashii)**
- **https://maou.audio/14_shining_star/**

## NetCore
- ``dotnet run``コマンドを使用してサンプルを実行してください。
- csprojファイル内の.NETのバージョンは適宜変更してください
## NetFramework
- DLLファイルをmain.csのあるフォルダにコピーして使います。
- ``csc main.cs /r:CSharpAudio.dll``コマンドを使用してサンプルを作成し、実行してください。

# How To Use!
- C#プロジェクトのコード例
```C#
後で書きます
```
# 今後の機能
果たしていくつチェックを入れることができるかな？
- [ ] 音声再生デバイスの操作
- [ ] 音声再生中のシーク
- [ ] X3DAudioの操作
- [ ] OggやOpusなどのフォーマットの対応
- [ ] PCMデータを任意のフォーマットにエンコード
