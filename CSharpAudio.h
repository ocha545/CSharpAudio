#pragma once
#using<System.dll>
#using<System.Collections.dll>
#include"CSharpAudio.Types.h"
#include"CSharpAudio.Helper.h"
using CSA::Types::Manage::XAudio2;
using CSA::Types::Manage::XAudio2MV;
using CSA::Types::Manage::XAudio2SV;
using CSA::Types::CSAResult;
using CSA::Types::CSAHandle;
using CSA::Types::CSAInfo;
using CSA::Types::Format::BaseFormat;
using System::Collections::Generic::List;

namespace CSA
{
	public ref class CSharpAudio
	{
	private:
		XAudio2^ xaudio2;
		XAudio2MV^ masterVoice;
		List<XAudio2SV^>^ sourceVoices;
		List<CSAInfo>^ sourceVoiceInfos;

		bool coInitialized;

	public:
		/// @brief CSharpAudioを作成し、初期化します
		CSharpAudio();

		~CSharpAudio();
		!CSharpAudio();

		/// @brief 全てのデータを初期化します。
		/// 出来るだけ直接実行せず、コンストラクタを使用してください
		/// @return 
		CSAResult Initialize();

		/// @brief 音声データからハンドルを作成します
		/// @param main_data 
		/// @return 作成したハンドルを返します。
		/// ハンドルを作成できなかった場合、マイナスの値が返されます
		CSAHandle Submit(BaseFormat^ main_data);

		/// @brief 音声データからハンドルを作成します
		/// @param main_data
		/// @param loop ループの回数を指定します
		/// @return 作成したハンドルを返します。
		/// ハンドルを作成できなかった場合、マイナスの値が返されます
		CSAHandle Submit(BaseFormat^ main_data, int loop);

		/// @brief ハンドルの音声が再生されているか判定します
		/// @param handle Submitで作成したハンドルを指定します
		/// @return 
		bool IsStarting(CSAHandle handle);

		/// @brief ハンドルの音声を再生します
		/// @param handle Submitで作成したハンドルを指定します
		void Start(CSAHandle handle);

		/// @brief ハンドルの音声を停止します
		/// @param handle Submitで作成したハンドルを指定します
		void Stop(CSAHandle handle);

		/// @brief ハンドルの音声の音量を設定します
		/// @param handle Submitで作成したハンドルを指定します
		/// @param volume 0 から 1までの少数を指定してください
		void Volume(CSAHandle handle, float volume);
	};
}
