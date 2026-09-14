# CSharpAudio v0.1!
- C++/CLIで作成したXAudio2のライブラリです
- C#でも手軽に音楽の再生をするために作成されました。

# Requirement
- VisualStudio2022以上

# Build
1. 任意のディレクトリにCSharpAudioのリポジトリをクローンする
  ``git clone https://github.com/ocha545/CSharpAudio.git AnyDirectory``
2. slnファイルをVisualStudioで開き、C#向けのDLLを生成する
2-1. .NET8以上のバージョン向けでは構成を**Debug-netcore**に設定してビルドして下さい
2-2. .NET Framework向けでは構成を**Debug-netframework**に設定してビルドして下さい
3. C#プロジェクトで生成したDLLと``Ijhost.dll``を参照に追加して使用してください