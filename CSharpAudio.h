#pragma once

// x86対応が出来ていない為、エラーにします
#ifndef _WIN64
#error Unsupported Target
#endif

#using<System.dll>
#using<System.Collections.dll>

//#define CSA_PRINT_LOG

#include"CSharpAudio.Types.h"
#include"CSharpAudio.Helper.h"
using CSA::Types::Manage::XAudio2;
using CSA::Types::Manage::XAudio2MV;
using CSA::Types::Manage::XAudio2SV;
using CSA::Types::CSAResult;
using CSA::Types::CSAHandle;
using CSA::Types::CSAInfo;
using CSA::Types::Format::BaseFormat;
using CSA::Types::Exception::CSharpAudioException;
using CSA::Types::Exception::CSharpAudioExceptionFuncs;
using System::Collections::Generic::List;

namespace CSA
{
	public ref class CSharpAudio
	{
	private:
		XAudio2^ xaudio2;
		XAudio2MV^ masterVoice;
		List<XAudio2SV^>^ sourceVoices;
		List<IntPtr>^ submitDataPtrs;
		//List<uint64_t>^ submitDataSizes;
		//List<CSAInfo>^ sourceVoiceInfos;
		CSAResult lastError;

		bool coInitialized;

		bool validHandle(CSAHandle handle);

		CSAResult initialize();

		void printLog(String^ txt);

	public:
		/// @brief CSharpAudioを作成し、初期化します
		CSharpAudio();

		/// @brief COMオブジェクトを解放する
		~CSharpAudio();

		/// @brief C++のリソースを解放する
		//!CSharpAudio();

		/// @brief 音声データからハンドルを作成します
		/// @param main_data 
		/// @return 作成したハンドルを返します。
		/// INVALID_HANDLEの場合、GetLastErrorで詳細なエラーを取得できます
		CSAHandle Submit(BaseFormat^ main_data);

		/// @brief 音声データからハンドルを作成します
		/// @param main_data
		/// @param loop ループの回数を指定します
		/// @return 作成したハンドルを返します。
		/// INVALID_HANDLEの場合、GetLastErrorで詳細なエラーを取得できます
		CSAHandle Submit(BaseFormat^ main_data, unsigned int loop);

		/// @brief 最後に発生したエラーを返します
		/// @return 
		CSAResult GetLastError();

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
		void SetVolume(CSAHandle handle, float volume);
	};
}
