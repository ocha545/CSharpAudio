#include"CSharpAudio.Types.h"

#pragma warning(push)
#pragma warning(disable: 4793)

#define DR_MP3_IMPLEMENTATION
#include"third_party/dr_mp3.h"

#define DR_WAV_IMPLEMENTATION
#include"third_party/dr_wav.h"

#define DR_FLAC_IMPLEMENTATION
#include"third_party/dr_flac.h"

#pragma warning(pop)

namespace CSA
{
	namespace Types
	{
		Format::FormatData::FormatData()
		{
			isRead = false;
			info = CSAInfo();
			buffer = gcnew array<short>(0);
		}
		array<short>^ Format::FormatData::VectorToCLIArray(const std::vector<short>& vec)
		{
			array<short>^ tmp = gcnew array<short>((int)vec.size());

			for (size_t i = 0; i < vec.size(); i++)
			{
				tmp[i] = vec[i];
			}

			return tmp;
		}

		Format::MP3::MP3(String^ path)
			: FormatData()
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

				drmp3_uint64 sample = readLength * (drmp3_uint64)info.Channels;
				main_buffer.insert(main_buffer.end(), cycle_buffer.begin(), cycle_buffer.begin() + (size_t)sample);
			}

			buffer = VectorToCLIArray(main_buffer);

			drmp3_uninit(mp3);
			delete mp3;
		}
		Format::MP3::~MP3()
		{
			this->!MP3();
		}
		Format::MP3::!MP3()
		{
			this->info = CSAInfo();
			this->isRead = false;
		}
		bool Format::MP3::IsValid()
		{
			return isRead;
		}
		CSAFormat Format::MP3::GetFormat()
		{
			return CSAFormat::MP3;
		}
		CSAInfo Format::MP3::GetInfo()
		{
			return info;
		}
		array<short>^ Format::MP3::GetBuffer()
		{
			return buffer;
		}

		Format::Wave::Wave(String^ path) : FormatData()
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

			std::vector<short> main_buffer((size_t)allFrameCount);
			if (info.BitsPerSample == 8)
			{
				drwav_read_raw(wave, (size_t)wave->totalPCMFrameCount, main_buffer.data());
			}
			else
			{
				drwav_read_pcm_frames_s16(wave, wave->totalPCMFrameCount, main_buffer.data());
			}
			buffer = VectorToCLIArray(main_buffer);

			drwav_uninit(wave);
			delete wave;
		}
		Format::Wave::~Wave()
		{
			this->!Wave();
		}
		Format::Wave::!Wave()
		{
			this->info = CSAInfo();
			this->isRead = false;
		}
		bool Format::Wave::IsValid()
		{
			return isRead;
		}
		CSAFormat Format::Wave::GetFormat()
		{
			return CSAFormat::Wave;
		}
		CSAInfo Format::Wave::GetInfo()
		{
			return info;
		}
		array<short>^ Format::Wave::GetBuffer()
		{
			return buffer;
		}

		Format::Flac::Flac(String^ path) : FormatData()
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
			if (allFrameCount > UINT64_MAX)
			{
				System::Console::WriteLine("drflacで扱える値の数を超えました");
			}

			std::vector<short> main_buffer((size_t)allFrameCount);
			drflac_read_pcm_frames_s16(flac, flac->totalPCMFrameCount, main_buffer.data());

			buffer = VectorToCLIArray(main_buffer);
			drflac_close(flac);
		}
		Format::Flac::~Flac()
		{
			this->!Flac();
		}
		Format::Flac::!Flac()
		{
			this->info = CSAInfo();
			this->isRead = false;
		}
		bool Format::Flac::IsValid()
		{
			return isRead;
		}
		CSAFormat Format::Flac::GetFormat()
		{
			return CSAFormat::Flac;
		}
		CSAInfo Format::Flac::GetInfo()
		{
			return info;
		}
		array<short>^ Format::Flac::GetBuffer()
		{
			return buffer;
		}


		template<typename T>
		T* Manage::getPtr(IntPtr ptr)
		{
			return (T*)(ptr.ToPointer());
		}

		Manage::XAudio2::XAudio2()
		{
			xaudio2 = IntPtr::Zero;
		}
		Manage::XAudio2::~XAudio2()
		{
			this->!XAudio2();
		}
		Manage::XAudio2::!XAudio2()
		{
			Release();
		}
		bool Manage::XAudio2::IsValid()
		{
			return (xaudio2 != IntPtr::Zero);
		}
		IXAudio2* Manage::XAudio2::Get()
		{
			return getPtr<IXAudio2>(xaudio2);
		}
		void Manage::XAudio2::SetPointer(IXAudio2* ptr)
		{
			xaudio2 = IntPtr(ptr);
		}
		void Manage::XAudio2::Release()
		{
			if (xaudio2 != IntPtr::Zero)
			{
				getPtr<IXAudio2>(xaudio2)->Release();
				xaudio2 = IntPtr::Zero;
			}
		}

		Manage::XAudio2MV::XAudio2MV()
		{
			masterVoice = IntPtr::Zero;
		}
		Manage::XAudio2MV::~XAudio2MV()
		{
			this->!XAudio2MV();
		}
		Manage::XAudio2MV::!XAudio2MV()
		{
			DestroyVoice();
		}
		bool Manage::XAudio2MV::IsValid()
		{
			return (masterVoice != IntPtr::Zero);
		}
		IXAudio2MasteringVoice* Manage::XAudio2MV::Get()
		{
			return getPtr<IXAudio2MasteringVoice>(masterVoice);
		}
		void Manage::XAudio2MV::SetPointer(IXAudio2MasteringVoice* ptr)
		{
			masterVoice = IntPtr(ptr);
		}
		void Manage::XAudio2MV::DestroyVoice()
		{
			if (masterVoice != IntPtr::Zero)
			{
				getPtr<IXAudio2MasteringVoice>(masterVoice)->DestroyVoice();
				masterVoice = IntPtr::Zero;
			}
		}

		Manage::XAudio2SV::XAudio2SV()
		{
			sourceVoice = IntPtr::Zero;
		}
		Manage::XAudio2SV::~XAudio2SV()
		{
			this->!XAudio2SV();
		}
		Manage::XAudio2SV::!XAudio2SV()
		{
			DestroyVoice();
		}
		bool Manage::XAudio2SV::IsValid()
		{
			return (sourceVoice != IntPtr::Zero);
		}
		IXAudio2SourceVoice* Manage::XAudio2SV::Get()
		{
			return getPtr<IXAudio2SourceVoice>(sourceVoice);
		}
		void Manage::XAudio2SV::SetPointer(IXAudio2SourceVoice* ptr)
		{
			sourceVoice = IntPtr(ptr);
		}
		void Manage::XAudio2SV::DestroyVoice()
		{
			if (sourceVoice != IntPtr::Zero)
			{
				getPtr<IXAudio2SourceVoice>(sourceVoice)->DestroyVoice();
				sourceVoice = IntPtr::Zero;
			}
		}
	}
}
