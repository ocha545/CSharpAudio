#pragma once
#using<System.dll>
#include<msclr/marshal.h>
using System::IntPtr;
using System::String;
using msclr::interop::marshal_context;

#include<xaudio2.h>
#include<vector>

namespace CSA
{
	namespace Types
	{
		typedef int CSAHandle;

		public enum class CSAResult
		{
			// Initialize Result
			FAILED_INITIALIZE_COM = -1,
			FAILED_CREATE_XAUDIO2 = -2,
			FAILED_CREATE_MASTERINGVOICE = -3,
			SUCCEEDED = -4,

			// Submit Result
			FAILED_CREATE_SOURCEVOICE = -5,
			EMPTY_BUFFER = -6,
			FAILED_SUBMIT_XAUDIO2_BUFFER = -7,
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
				array<short>^ GetBuffer();
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
