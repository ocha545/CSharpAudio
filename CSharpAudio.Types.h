#pragma once
#using<System.dll>
#include<msclr/marshal.h>
using System::IntPtr;
using System::String;
using msclr::interop::marshal_context;

#include<xaudio2.h>
#include<vector>
//#include"third_party/dr_mp3.h"
//#include"third_party/dr_wav.h"
//#include"third_party/dr_flac.h"

namespace CSA
{
	namespace Types
	{
		typedef int CSAHandle;

		public enum class CSAFormat
		{
			Base,
			MP3,
			Wave,
			Flac,
		};

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

		namespace Format
		{
			public interface class IFormat
			{
			public:
				virtual bool IsValid();
				virtual CSAFormat GetFormat();
				virtual CSAInfo GetInfo();
				virtual array<short>^ GetBuffer();
			};
			public ref class FormatData
			{
			protected:
				bool isRead;
				CSAInfo info;
				array<short>^ buffer;

			public:
				FormatData();

			protected:
				array<short>^ VectorToCLIArray(const std::vector<short>& vec);
			};

			public ref class MP3 : public IFormat, public FormatData
			{
			public:
				MP3(String^ path);
				~MP3();
				!MP3();

				virtual bool IsValid();

				virtual CSAFormat GetFormat();

				virtual CSAInfo GetInfo();

				virtual array<short>^ GetBuffer();
			};

			public ref class Wave : public IFormat, public FormatData
			{
			public:
				Wave(String^ path);
				~Wave();
				!Wave();
				virtual bool IsValid();

				virtual CSAFormat GetFormat();

				virtual CSAInfo GetInfo();

				virtual array<short>^ GetBuffer();
			};

			public ref class Flac : public IFormat, public FormatData
			{
			public:
				Flac(String^ path);
				~Flac();
				!Flac();
				virtual bool IsValid();

				virtual CSAFormat GetFormat();

				virtual CSAInfo GetInfo();

				virtual array<short>^ GetBuffer();
			};
		}

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
