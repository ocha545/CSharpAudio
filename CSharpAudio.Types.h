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
/*
#pragma once
#using<System.dll>
#include<msclr/marshal.h>
using System::IntPtr;
using System::String;
using msclr::interop::marshal_context;

#include<xaudio2.h>
#include<vector>
#include"third_party/dr_mp3.h"
#include"third_party/dr_wav.h"
#include"third_party/dr_flac.h"

namespace SA
{
	namespace Types
	{
		typedef unsigned int SAHandle;

		public enum class SAFormat
		{
			MP3,
			Wave,
			Flac,
		};

		public value class SAInfo
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
				virtual SAFormat GetFormat();
				virtual SAInfo GetInfo();
				virtual array<short>^ GetBuffer();
			};
			public ref class FormatData
			{
			protected:
				bool isRead;
				SAInfo info;
				array<short>^ buffer;

			public:
				FormatData()
				{
					isRead = false;
					info = SAInfo();
					buffer = gcnew array<short>(0);
				}

			protected:
				array<short>^ VectorToCLIArray(const std::vector<short>& vec)
				{
					array<short>^ tmp = gcnew array<short>(vec.size());

					for (int i = 0; i < vec.size(); i++)
					{
						tmp[i] = vec[i];
					}

					return tmp;
				}
			};

			public ref class MP3 : public IFormat, public FormatData
			{
			public:
				MP3(String^ path) : FormatData()
				{
					marshal_context ctx{};
					const wchar_t* nativePath = ctx.marshal_as<const wchar_t*>(path);

					drmp3* mp3 = new drmp3;
					isRead = drmp3_init_file_w(mp3, nativePath, nullptr);
					if (!isRead)
					{
						drmp3_uninit(mp3);
						delete mp3;
						System::Console::WriteLine("drmp3の初期化に失敗しました");
						return;
					}

					info.SampleRate = mp3->sampleRate;
					info.Channels = mp3->channels;
					info.BitsPerSample = 16;
					info.BlockAlign = static_cast<unsigned short>(info.Channels * 16 / 8);
					info.FormatTag = WAVE_FORMAT_PCM;
					info.AvgBytesPerSec = info.SampleRate * info.BlockAlign;

					std::vector<short> main_buffer{};
					const int buffer_size = 4096;
					std::vector<short> cycle_buffer(buffer_size * info.Channels);

					while (true)
					{
						drmp3_uint64 readLength = drmp3_read_pcm_frames_s16(mp3, buffer_size, cycle_buffer.data());
						if (readLength == 0)
						{
							break;
						}
						if (readLength < 0)
						{
							drmp3_uninit(mp3);
							delete mp3;
						}

						size_t sample = readLength * info.Channels;
						main_buffer.insert(main_buffer.end(), cycle_buffer.begin(), cycle_buffer.begin() + sample);
					}

					buffer = VectorToCLIArray(main_buffer);

					drmp3_uninit(mp3);
					delete mp3;
				}
				~MP3()
				{
					this->!MP3();
				}
				!MP3()
				{
					this->info = SAInfo();
					this->isRead = false;
				}

				virtual bool IsValid()
				{
					return isRead;
				}

				virtual SAFormat GetFormat()
				{
					return SAFormat::MP3;
				}

				virtual SAInfo GetInfo()
				{
					return info;
				}

				virtual array<short>^ GetBuffer()
				{
					return buffer;
				}
			};

			public ref class Wave : public IFormat, public FormatData
			{
			public:
				Wave(String^ path) : FormatData()
				{
					marshal_context ctx{};
					const wchar_t* nativePath = ctx.marshal_as<const wchar_t*>(path);

					drwav* wave = new drwav;
					isRead = drwav_init_file_w(wave, nativePath, nullptr);

					info.FormatTag = WAVE_FORMAT_PCM;
					info.Channels = static_cast<unsigned short>(wave->channels);
					info.SampleRate = wave->sampleRate;
					info.BitsPerSample = wave->bitsPerSample;
					info.BlockAlign = static_cast<unsigned short>(wave->channels * wave->bitsPerSample / 8);
					info.AvgBytesPerSec = info.SampleRate * info.BlockAlign;

					uint64_t allFrameCount = (uint64_t)(info.Channels * wave->totalPCMFrameCount);
					if (allFrameCount > UINT64_MAX)
					{
						drwav_uninit(wave);
						delete wave;
						System::Console::WriteLine("drwavで扱える値の数を超えました");
						return;
					}

					std::vector<short> main_buffer(allFrameCount);
					if (info.BitsPerSample == 8)
					{
						drwav_read_raw(wave, wave->totalPCMFrameCount, main_buffer.data());
					}
					else
					{
						drwav_read_pcm_frames_s16(wave, wave->totalPCMFrameCount, main_buffer.data());
					}
					buffer = VectorToCLIArray(main_buffer);

					drwav_uninit(wave);
					delete wave;
				}

				~Wave()
				{
					this->!Wave();
				}
				!Wave()
				{
					this->info = SAInfo();
					this->isRead = false;
				}

				virtual bool IsValid()
				{
					return isRead;
				}

				virtual SAFormat GetFormat()
				{
					return SAFormat::Wave;
				}

				virtual SAInfo GetInfo()
				{
					return info;
				}

				virtual array<short>^ GetBuffer()
				{
					return buffer;
				}
			};

			public ref class Flac : public IFormat, public FormatData
			{
			public:
				Flac(String^ path)
				{
					marshal_context ctx{};
					const wchar_t* nativePath = ctx.marshal_as<const wchar_t*>(path);

					drflac* flac = drflac_open_file_w(nativePath, nullptr);
					if (flac == nullptr)
					{
						System::Console::WriteLine("Flacファイルを読み込めませんでした");
						return;
					}
					isRead = true;

					info.FormatTag = WAVE_FORMAT_PCM;
					info.Channels = static_cast<unsigned short>(flac->channels);
					info.SampleRate = flac->sampleRate;
					info.BitsPerSample = 16;
					info.BlockAlign = static_cast<unsigned short>(info.Channels * 16 / 8);
					info.AvgBytesPerSec = info.SampleRate * info.BlockAlign;

					uint64_t allFrameCount = info.Channels * flac->totalPCMFrameCount;
					if(allFrameCount > UINT64_MAX)
					{
						System::Console::WriteLine("drflacで扱える値の数を超えました");
					}

					std::vector<short> main_buffer(allFrameCount);
					drflac_read_pcm_frames_s16(flac, flac->totalPCMFrameCount, main_buffer.data());

					buffer = VectorToCLIArray(main_buffer);
					drflac_close(flac);
				}
				~Flac()
				{
					this->!Flac();
				}
				!Flac()
				{
					this->info = SAInfo();
					this->isRead = false;
				}

				virtual bool IsValid()
				{
					return isRead;
				}

				virtual SAFormat GetFormat()
				{
					return SAFormat::Flac;
				}

				virtual SAInfo GetInfo()
				{
					return info;
				}

				virtual array<short>^ GetBuffer()
				{
					return buffer;
				}
			};
		}

		namespace Manage
		{
			template<typename T>
			T* getPtr(IntPtr ptr)
			{
				return (T*)(ptr.ToPointer());
			}

			public ref class XAudio2
			{
			private:
				IntPtr xaudio2;

			public:
				XAudio2()
				{
					xaudio2 = IntPtr::Zero;
				}
				~XAudio2()
				{
					this->!XAudio2();
				}
				!XAudio2()
				{
					Release();
				}
				bool IsValid()
				{
					return (xaudio2 != IntPtr::Zero);
				}

				IXAudio2* Get()
				{
					return getPtr<IXAudio2>(xaudio2);
				}

				void SetPointer(IXAudio2* ptr)
				{
					xaudio2 = IntPtr(ptr);
				}

				void Release()
				{
					if (xaudio2 != IntPtr::Zero)
					{
						getPtr<IXAudio2>(xaudio2)->Release();
						xaudio2 = IntPtr::Zero;
					}
				}
			};

			public ref class XAudio2MV
			{
			private:
				IntPtr masterVoice;

			public:
				XAudio2MV()
				{
					masterVoice = IntPtr::Zero;
				}
				~XAudio2MV()
				{
					this->!XAudio2MV();
				}
				!XAudio2MV()
				{
					DestroyVoice();
				}
				bool IsValid()
				{
					return (masterVoice != IntPtr::Zero);
				}

				IXAudio2MasteringVoice* Get()
				{
					return getPtr<IXAudio2MasteringVoice>(masterVoice);
				}

				void SetPointer(IXAudio2MasteringVoice* ptr)
				{
					masterVoice = IntPtr(ptr);
				}

				void DestroyVoice()
				{
					if (masterVoice != IntPtr::Zero)
					{
						getPtr<IXAudio2MasteringVoice>(masterVoice)->DestroyVoice();
						masterVoice = IntPtr::Zero;
					}
				}
			};

			public ref class XAudio2SV
			{
			private:
				IntPtr sourceVoice;

			public:
				XAudio2SV()
				{
					sourceVoice = IntPtr::Zero;
				}
				~XAudio2SV()
				{
					this->!XAudio2SV();
				}
				!XAudio2SV()
				{
					DestroyVoice();
				}
				bool IsValid()
				{
					return (sourceVoice != IntPtr::Zero);
				}

				IXAudio2SourceVoice* Get()
				{
					return getPtr<IXAudio2SourceVoice>(sourceVoice);
				}

				void SetPointer(IXAudio2SourceVoice* ptr)
				{
					sourceVoice = IntPtr(ptr);
				}

				void DestroyVoice()
				{
					if (sourceVoice != IntPtr::Zero)
					{
						getPtr<IXAudio2SourceVoice>(sourceVoice)->DestroyVoice();
						sourceVoice = IntPtr::Zero;
					}
				}
			};
		}

		namespace Exception
		{
			public ref class SharpAudioException : public System::Exception
			{
			public:
				SharpAudioException(String^ message)
					: System::Exception(message)
				{
				}
			};
		}
	}
}
*/