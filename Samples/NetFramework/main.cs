using System;
using System.Threading;
using CSA;
using CSA.Types;
using CSA.Types.Format;

class Program
{
	const int INVALID_HANDLE = -1;
	public static void Main()
	{
		using CSharpAudio csa = new CSharpAudio();

		// シャイニングスター : 魔王魂(MaouDamashii)
		MP3 mp3 = new MP3("../maou_14_shining_star.mp3");
		int handle = csa.Submit(mp3);
		if(handle == INVALID_HANDLE)
		{
			Console.WriteLine("音声ファイルが読み込めませんでした");
			Console.WriteLine("Error: " + csa.GetLastError().ToString());
			return;
		}

		Console.WriteLine("再生中...");
		csa.Start(handle);
		while(csa.IsStarting(handle))
		{
			Thread.Sleep(1000);
		}
		Console.WriteLine("再生終了");
	}
}
