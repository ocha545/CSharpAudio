#pragma once
#using<System.dll>
#using<System.Collections.dll>
#include"CSharpAudio.Types.h"
#include"CSharpAudio.Helper.h"
using CSA::Types::Manage::XAudio2;
using CSA::Types::Manage::XAudio2MV;
using CSA::Types::Manage::XAudio2SV;
using CSA::Types::CSAHandle;
using CSA::Types::CSAInfo;
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

		enum class CSAResult
		{
			FAILED_INITIALIZE_COM,
			FAILED_CREATE_XAUDIO2,
			FAILED_CREATE_MASTERINGVOICE,
			SUCCEEDED
		};

	public:
		CSharpAudio();

		CSAResult Initialize();

		CSAHandle Submit(array<short>^ buffer, CSAInfo info, int loop);

		bool IsStarting(CSAHandle handle);

		void Start(CSAHandle handle);

		void Stop(CSAHandle handle);

		void Volume(CSAHandle handle, float volume);
	};
}

/*

#pragma once
#using<System.dll>
#using<System.Collections.dll>
#include"SharpAudio.Types.h"
#include"SharpAudio.Helper.h"
using SA::Types::Manage::XAudio2;
using SA::Types::Manage::XAudio2MV;
using SA::Types::Manage::XAudio2SV;
using SA::Types::SAHandle;
using SA::Types::SAInfo;
using System::Collections::Generic::List;

namespace SA
{
	public ref class SharpAudio
	{
	private:
		XAudio2^ xaudio2;
		XAudio2MV^ masterVoice;
		List<XAudio2SV^>^ sourceVoices;
		List<SAInfo>^ sourceVoiceInfos;

		bool coInitialized;

		enum class SAResult
		{
			FAILED_INITIALIZE_COM,
			FAILED_CREATE_XAUDIO2,
			FAILED_CREATE_MASTERINGVOICE,
			SUCCEEDED
		};

	public:
		SharpAudio()
		{
			xaudio2 = gcnew XAudio2();
			masterVoice = gcnew XAudio2MV();
			sourceVoices = gcnew List<XAudio2SV^>(0);
			sourceVoiceInfos = gcnew List<SAInfo>(0);
			coInitialized = false;

			switch (Initialize())
			{
			case SAResult::FAILED_INITIALIZE_COM:
				throw gcnew SA::Types::Exception::SharpAudioException("COMの初期化に失敗しました");
				break;

			case SAResult::FAILED_CREATE_XAUDIO2:
				throw gcnew SA::Types::Exception::SharpAudioException("XAudio2の作成に失敗しました");
				break;

			case SAResult::FAILED_CREATE_MASTERINGVOICE:
				throw gcnew SA::Types::Exception::SharpAudioException("XAudio2 MasteringVoiceの作成に失敗しました");
				break;

			case SAResult::SUCCEEDED:
				break;
			}
		}

		SAResult Initialize()
		{
			HRESULT result = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
			if (FAILED(result))
			{
				System::Console::WriteLine("COMの初期化に失敗しました");
				return SAResult::FAILED_INITIALIZE_COM;
			}
			coInitialized = true;

			IXAudio2* xaudio2_tmp = nullptr;
			result = XAudio2Create(&xaudio2_tmp);
			if (FAILED(result))
			{
				if (coInitialized) CoUninitialize();
				System::Console::WriteLine("XAudio2の作成に失敗しました");
				return SAResult::FAILED_CREATE_XAUDIO2;
			}
			xaudio2->SetPointer(xaudio2_tmp);

			IXAudio2MasteringVoice* masterVoice_tmp = nullptr;
			result = xaudio2->Get()->CreateMasteringVoice(&masterVoice_tmp);
			if (FAILED(result))
			{
				if (coInitialized) CoUninitialize();
				if (xaudio2) xaudio2->Release();
				System::Console::WriteLine("MasteringVoiceの作成に失敗しました");
				return SAResult::FAILED_CREATE_MASTERINGVOICE;
			}
			masterVoice->SetPointer(masterVoice_tmp);

			return SAResult::SUCCEEDED;//成功！
		}

		SAHandle Submit(array<short>^ buffer, SAInfo info, int loop)
		{
			IXAudio2SourceVoice* sourceVoice_tmp = nullptr;
			WAVEFORMATEX fmt = info.GetNativeData();

			HRESULT result = xaudio2->Get()->CreateSourceVoice(&sourceVoice_tmp, &fmt, XAUDIO2_VOICE_USEFILTER);
			if (FAILED(result))
			{
				System::Console::WriteLine("SourceVoiceの作成に失敗しました");
				return -1;
			}
			XAudio2SV^ sourceVoice = gcnew XAudio2SV();
			sourceVoice->SetPointer(sourceVoice_tmp);

			XAUDIO2_BUFFER xaudio2Buffer{};
			if (buffer->Length == 0)
			{
				return -1;
			}
			pin_ptr<short> nativeBuf = &buffer[0];
			xaudio2Buffer.pAudioData = (BYTE*)nativeBuf;
			xaudio2Buffer.Flags = XAUDIO2_END_OF_STREAM;
			xaudio2Buffer.AudioBytes = (size_t)buffer->Length * sizeof(buffer[0]);
			xaudio2Buffer.LoopCount = loop;

			result = sourceVoice->Get()->SubmitSourceBuffer(&xaudio2Buffer);
			if (FAILED(result))
			{
				System::Console::WriteLine("オーディオバッファの送信に失敗しました");
				return -1;
			}

			sourceVoices->Add(sourceVoice);
			sourceVoiceInfos->Add(info);

			return (sourceVoices->Count - 1);
		}

		bool IsStarting(SAHandle handle)
		{
			if (sourceVoices[handle]->IsValid())
			{
				XAUDIO2_VOICE_STATE state{};
				sourceVoices[handle]->Get()->GetState(&state);
				return state.BuffersQueued;
			}
			return false;
		}

		void Start(SAHandle handle)
		{
			if (sourceVoices[handle]->IsValid())
			{
				sourceVoices[handle]->Get()->Start();
			}
		}

		void Stop(SAHandle handle)
		{
			if (sourceVoices[handle]->IsValid())
			{
				sourceVoices[handle]->Get()->Stop();
			}
		}

		void Volume(SAHandle handle, float volume)
		{
			if (sourceVoices[handle]->IsValid())
			{
				sourceVoices[handle]->Get()->SetVolume(volume);
			}
		}
	};
}

*/