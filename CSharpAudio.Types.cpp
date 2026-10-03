#include"CSharpAudio.Types.h"

#pragma warning(push)
#pragma warning(disable: 4793)
#pragma warning(disable: 6262)

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
		Format::BaseFormat::BaseFormat()
		{
			isRead = false;
			info = CSAInfo();
			format = CSAFormat::None;
			buffer = gcnew array<short>(0);
		}
		Format::BaseFormat::~BaseFormat()
		{
			this->!BaseFormat();
		}
		Format::BaseFormat::!BaseFormat()
		{
#ifdef CSA_PRINT_LOG
System::Console::WriteLine("[INTERNAL] DELETE fmt:" + format.ToString());
#endif
			if (buffer != nullptr)
			{
				delete buffer;
			}
		}
		bool Format::BaseFormat::IsValid()
		{
			return isRead;
		}
		CSAFormat Format::BaseFormat::GetFormat()
		{
			return format;
		}
		CSAInfo Format::BaseFormat::GetInfo()
		{
			return info;
		}
		array<short>^ Format::BaseFormat::GetBuffer()
		{
			return buffer;
		}
		array<short>^ Format::BaseFormat::vectorToCLIArray(const std::vector<short>& vec)
		{
			array<short>^ outArr = gcnew array<short>(vec.size());
			for (size_t i = 0; i < vec.size(); i++)
			{
				outArr[i] = vec[i];
			}
			return outArr;
		}

		Format::MP3::MP3(String^ path)
			: BaseFormat()
		{
			format = CSAFormat::MP3;
			pin_ptr<const wchar_t> nativePath = PtrToStringChars(path);

			drmp3* mp3 = new drmp3;
			isRead = drmp3_init_file_w(mp3, nativePath, nullptr);
			if (!isRead)
			{
				delete mp3;
				isRead = false;
				return;
			}

			info.SampleRate = mp3->sampleRate;
			info.Channels = mp3->channels;
			info.BitsPerSample = DEFAULT_BITS_PER_SAMPLE;
			info.BlockAlign = static_cast<unsigned short>(
				info.Channels * info.BitsPerSample / 8
			);
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
					//読み込み終了
					break;
				}

				drmp3_uint64 sample = readLength * (drmp3_uint64)info.Channels;
				main_buffer.insert(main_buffer.end(), cycle_buffer.begin(), cycle_buffer.begin() + (size_t)sample);
			}

			buffer = vectorToCLIArray(main_buffer);

			drmp3_uninit(mp3);
			delete mp3;
		}

		Format::Wave::Wave(String^ path)
			: BaseFormat()
		{
			format = CSAFormat::Wave;
			pin_ptr<const wchar_t> nativePath = PtrToStringChars(path);

			drwav* wave = new drwav{ 0 };
			isRead = drwav_init_file_w(wave, nativePath, nullptr);
			if (!isRead)
			{
				delete wave;
				isRead = false;
				return;
			}

			info.FormatTag = WAVE_FORMAT_PCM;
			info.Channels = static_cast<unsigned short>(wave->channels);
			info.SampleRate = wave->sampleRate;
			info.BitsPerSample = DEFAULT_BITS_PER_SAMPLE;
			info.BlockAlign = static_cast<unsigned short>(
				info.Channels * info.BitsPerSample / 8
			);
			info.AvgBytesPerSec = info.SampleRate * info.BlockAlign;

			uint64_t allFrameCount = static_cast<uint64_t>(info.Channels * wave->totalPCMFrameCount);

			std::vector<short> main_buffer(allFrameCount);

			//drwav_read_raw(wave, wave->totalPCMFrameCount, main_buffer.data());
			uint64_t readSize = drwav_read_pcm_frames_s16(wave, wave->totalPCMFrameCount, main_buffer.data());
			if (readSize != wave->totalPCMFrameCount)
			{
				drwav_uninit(wave);
				delete wave;
				isRead = false;
				return;
			}

			buffer = vectorToCLIArray(main_buffer);

			drwav_uninit(wave);
			delete wave;
		}

		Format::Flac::Flac(String^ path)
			: BaseFormat()
		{
			format = CSAFormat::Flac;
			pin_ptr<const wchar_t> nativePath = PtrToStringChars(path);

			drflac* flac = drflac_open_file_w(nativePath, nullptr);
			if (flac == nullptr)
			{
				isRead = false;
				return;
			}

			info.FormatTag = WAVE_FORMAT_PCM;
			info.Channels = static_cast<unsigned short>(flac->channels);
			info.SampleRate = flac->sampleRate;
			info.BitsPerSample = DEFAULT_BITS_PER_SAMPLE;
			info.BlockAlign = static_cast<unsigned short>(
				info.Channels * info.BitsPerSample / 8
			);
			info.AvgBytesPerSec = info.SampleRate * info.BlockAlign;

			uint64_t allFrameCount = info.Channels * flac->totalPCMFrameCount;

			std::vector<short> main_buffer(allFrameCount);
			uint64_t readSize = drflac_read_pcm_frames_s16(flac, flac->totalPCMFrameCount, main_buffer.data());
			if (readSize != flac->totalPCMFrameCount)
			{
				drflac_close(flac);
				isRead = false;
				return;
			}

			isRead = true;
			buffer = vectorToCLIArray(main_buffer);
			drflac_close(flac);
		}


		template<typename T>
		T* Manage::getPtr(IntPtr ptr)
		{
			return static_cast<T*>(ptr.ToPointer());
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
			Release();
			xaudio2 = IntPtr(ptr);
		}
		void Manage::XAudio2::Release()
		{
			if (xaudio2 != IntPtr::Zero)
			{
#ifdef CSA_PRINT_LOG
System::Console::WriteLine("[INTERNAL] DELETE XAudio2");
#endif
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
			DestroyVoice();
			masterVoice = IntPtr(ptr);
		}
		void Manage::XAudio2MV::DestroyVoice()
		{
			if (masterVoice != IntPtr::Zero)
			{
#ifdef CSA_PRINT_LOG
System::Console::WriteLine("[INTERNAL] DELETE XAudio2MV");
#endif
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
			DestroyVoice();
			sourceVoice = IntPtr(ptr);
		}
		void Manage::XAudio2SV::DestroyVoice()
		{
			if (sourceVoice != IntPtr::Zero)
			{
#ifdef CSA_PRINT_LOG
				System::Console::WriteLine("[INTERNAL] DELETE XAudio2SV");
#endif
				getPtr<IXAudio2SourceVoice>(sourceVoice)->DestroyVoice();
				sourceVoice = IntPtr::Zero;
			}
		}

		void Exception::CSharpAudioExceptionFuncs::ThrowXAudio2Exception(HRESULT result)
		{
			if (SUCCEEDED(result))
			{
				//何も無し！
				return;
			}

			switch (result)
			{
			case XAUDIO2_E_INVALID_CALL:
				throw gcnew CSharpAudioException(CSAResult::INVALID_CALL.ToString());
				break;

			case XAUDIO2_E_XMA_DECODER_ERROR:
				throw gcnew CSharpAudioException(
					"Xbox 360 XMA ハードウェアで回復不能なエラーが発生しました: " + CSAResult::XMA_DECODER_ERROR.ToString()
				);
				break;

			case XAUDIO2_E_XAPO_CREATION_FAILED:
				throw gcnew CSharpAudioException(
					"XAPOのインスタンス化に失敗しました: " + CSAResult::XAPO_CREATION_FAILED.ToString()
				);
				break;

			case XAUDIO2_E_DEVICE_INVALIDATED:
				throw gcnew CSharpAudioException(
					"オーディオデバイスが取り外されたり、他のイベントが発生したため使用出来なくなりました: " + CSAResult::DEVICE_INVALIDATED.ToString()
				);
				break;

			default:
				throw gcnew CSharpAudioException(
					"Win32APIの中で何かしらのエラーが発生しました: " + result
				);
				break;
			}
		}
	}
}
