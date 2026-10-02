#pragma once
#using<System.dll>
#include<vcclr.h>
using System::IntPtr;
using System::String;

#include<xaudio2.h>
#include<vector>

namespace CSA
{
	namespace Types
	{
		constexpr unsigned short DEFAULT_BITS_PER_SAMPLE = 16;
		constexpr int INVALID_HANDLE = -1;

		typedef int CSAHandle;

		public enum class CSAResult
		{
			// Unused Result
			// 何も情報が無い場合Unusedにする
			UNUSED,

			// Initialize Result
			FAILED_INITIALIZE_COM,
			FAILED_CREATE_XAUDIO2,
			FAILED_CREATE_MASTERINGVOICE,
			SUCCEEDED,

			// Submit Result
			INVALID_DATA,
			FAILED_CREATE_SOURCEVOICE,
			EMPTY_BUFFER,
			FAILED_SUBMIT_XAUDIO2_BUFFER,


			// XAudio2 Specific Result
			INVALID_CALL = XAUDIO2_E_INVALID_CALL,
			XMA_DECODER_ERROR = XAUDIO2_E_XMA_DECODER_ERROR,
			XAPO_CREATION_FAILED = XAUDIO2_E_XAPO_CREATION_FAILED,
			DEVICE_INVALIDATED = XAUDIO2_E_DEVICE_INVALIDATED
		};

		public enum class CSAFormat
		{
			None,
			MP3,
			Wave,
			Flac,
		};

		/// @brief 音声データの情報を扱う構造体です
		public value class CSAInfo
		{
		public:
			unsigned short FormatTag;
			unsigned short Channels;
			unsigned long SampleRate;
			unsigned long AvgBytesPerSec;
			unsigned short BlockAlign;
			unsigned short BitsPerSample;

			WAVEFORMATEX GetNativeData()
			{
				return WAVEFORMATEX {
					FormatTag,
					Channels,
					SampleRate,
					AvgBytesPerSec,
					BlockAlign,
					BitsPerSample,
					0
				};
			}
		};

		/// @brief 音声データのフォーマットを扱うクラスを格納しています
		namespace Format
		{
			public ref class BaseFormat
			{
			protected:
				CSAInfo info;
				CSAFormat format;
				bool isRead;
				array<short>^ buffer;
				array<short>^ vectorToCLIArray(const std::vector<short>& vec);

			public:
				BaseFormat();
				~BaseFormat();
				!BaseFormat();

				bool IsValid();
				CSAFormat GetFormat();
				CSAInfo GetInfo();
				array<short>^% GetBuffer();
			};

			/// @brief mp3ファイルのデータを抽出します。
			/// Submit関数に渡すと、ハンドルを作成できます
			public ref class MP3 : public BaseFormat
			{
			public:
				MP3(String^ path);
			};

			/// @brief wavファイルのデータを抽出します。
			/// Submit関数に渡すと、ハンドルを作成できます
			public ref class Wave : public BaseFormat
			{
			public:
				Wave(String^ path);
			};

			/// @brief flacファイルのデータを抽出します。
			/// Submit関数に渡すと、ハンドルを作成できます
			public ref class Flac : public BaseFormat
			{
			public:
				Flac(String^ path);
			};
		}

		/// @brief IXAudio2のラッパークラスを格納しています
		namespace Manage
		{
			template<typename T>
			T* getPtr(IntPtr ptr);

			public ref class XAudio2
			{
			private:
				IntPtr xaudio2;

			public:
				XAudio2();
				~XAudio2();
				!XAudio2();
				bool IsValid();

				IXAudio2* Get();

				void SetPointer(IXAudio2* ptr);

				void Release();
			};

			public ref class XAudio2MV
			{
			private:
				IntPtr masterVoice;

			public:
				XAudio2MV();
				~XAudio2MV();
				!XAudio2MV();
				bool IsValid();

				IXAudio2MasteringVoice* Get();

				void SetPointer(IXAudio2MasteringVoice* ptr);

				void DestroyVoice();
			};

			public ref class XAudio2SV
			{
			private:
				IntPtr sourceVoice;

			public:
				XAudio2SV();
				~XAudio2SV();
				!XAudio2SV();
				bool IsValid();

				IXAudio2SourceVoice* Get();

				void SetPointer(IXAudio2SourceVoice* ptr);

				void DestroyVoice();
			};
		}

		/// @brief 基本的な例外クラスを格納しています
		namespace Exception
		{
			public ref class CSharpAudioException : public System::Exception
			{
			public:
				CSharpAudioException(String^ message)
					: System::Exception(message)
				{
				}
			};
		}
	}
}
