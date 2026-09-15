// cp ../../x64/Debug-netframework/CSharpAudio.dll

using System;
using CSA;
using CSA.Types;
using CSA.Types.Format;


class Program
{
	public static void Main()
	{
		CSharpAudio csa = new CSharpAudio();

		// シャイニングスター : 魔王魂(MaouDamashii)
		MP3 mp3 = new MP3("../maou_14_shining_star.mp3");

		int handle = csa.Submit(mp3);
		if(handle == -1)
		{
			Console.WriteLine("音声ファイルが読み込めませんでした");
			return;
		}
		

		Console.WriteLine("再生中...");
		csa.Start(handle);
		while(csa.IsStarting(handle))
		{
		}
		Console.WriteLine("再生終了");
	}
}
