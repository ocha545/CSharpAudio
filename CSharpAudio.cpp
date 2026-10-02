#include"CSharpAudio.h"

bool CSA::CSharpAudio::validHandle(CSAHandle handle)
{
	if (handle < sourceVoices->Count && handle != Types::INVALID_HANDLE &&
		sourceVoices[handle] != nullptr)
	{
		return sourceVoices[handle]->IsValid();
	}
	return false;
}

CSAResult CSA::CSharpAudio::initialize()
{
	HRESULT result = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if (FAILED(result))
	{
		return CSAResult::FAILED_INITIALIZE_COM;
	}
	coInitialized = true;

	IXAudio2* xaudio2_tmp = nullptr;
	result = XAudio2Create(&xaudio2_tmp);
	if (FAILED(result))
	{
		if (coInitialized) CoUninitialize();
		return CSAResult::FAILED_CREATE_XAUDIO2;
	}
	xaudio2->SetPointer(xaudio2_tmp);

	IXAudio2MasteringVoice* masterVoice_tmp = nullptr;
	result = xaudio2->Get()->CreateMasteringVoice(&masterVoice_tmp);
	if (FAILED(result))
	{
		if (coInitialized) CoUninitialize();
		if (xaudio2) xaudio2->Release();
		return CSAResult::FAILED_CREATE_MASTERINGVOICE;
	}
	masterVoice->SetPointer(masterVoice_tmp);

	return CSAResult::SUCCEEDED;//成功！
}

CSA::CSharpAudio::CSharpAudio()
{
	xaudio2 = gcnew XAudio2();
	masterVoice = gcnew XAudio2MV();
	sourceVoices = gcnew List<XAudio2SV^>(0);
	//sourceVoiceInfos = gcnew List<CSAInfo>(0);
	coInitialized = false;
	lastError = CSAResult::UNUSED;

	switch (initialize())
	{
	case CSAResult::FAILED_INITIALIZE_COM:
		throw gcnew CSA::Types::Exception::CSharpAudioException("COMの初期化に失敗しました");
		break;

	case CSAResult::FAILED_CREATE_XAUDIO2:
		throw gcnew CSA::Types::Exception::CSharpAudioException("XAudio2の作成に失敗しました");
		break;

	case CSAResult::FAILED_CREATE_MASTERINGVOICE:
		throw gcnew CSA::Types::Exception::CSharpAudioException("XAudio2 MasteringVoiceの作成に失敗しました");
		break;

	case CSAResult::SUCCEEDED:
		break;
	}
}

CSA::CSharpAudio::~CSharpAudio()
{
	if (sourceVoices != nullptr)
	{
		for each(XAudio2SV ^ sv in sourceVoices)
		{
			if (sv != nullptr)
			{
				sv->DestroyVoice();
				delete sv;
			}
		}
	}

	if (masterVoice != nullptr)
	{
		masterVoice->DestroyVoice();
		delete masterVoice;
	}

	if (xaudio2 != nullptr)
	{
		xaudio2->Release();
		delete xaudio2;
	}

	// CoInitializeを行ったスレッドと同じになるようデストラクタで行う
	if (coInitialized)
	{
		CoUninitialize();
		coInitialized = false;
	}
}

CSAHandle CSA::CSharpAudio::Submit(BaseFormat^ main_data)
{
	if (main_data == nullptr)
	{
		lastError = CSAResult::NULL_DATA;
		return Types::INVALID_HANDLE;
	}
	if (!main_data->IsValid())
	{
		lastError = CSAResult::INVALID_DATA;
		return Types::INVALID_HANDLE;
	}

	IXAudio2SourceVoice* sourceVoice_tmp = nullptr;
	WAVEFORMATEX fmt = main_data->GetInfo().GetNativeData();

	HRESULT result = xaudio2->Get()->CreateSourceVoice(&sourceVoice_tmp, &fmt, XAUDIO2_VOICE_USEFILTER);
	if (FAILED(result))
	{
		lastError = CSAResult::FAILED_CREATE_SOURCEVOICE;
		return Types::INVALID_HANDLE;
	}
	XAudio2SV^ sourceVoice = gcnew XAudio2SV();
	sourceVoice->SetPointer(sourceVoice_tmp);

	XAUDIO2_BUFFER xaudio2Buffer{};
	if (main_data->GetBuffer()->Length == 0)
	{
		lastError = CSAResult::EMPTY_BUFFER;
		return Types::INVALID_HANDLE;
	}

	pin_ptr<short> nativeBuf = &main_data->GetBuffer()[0];
	xaudio2Buffer.pAudioData = (BYTE*)nativeBuf;
	xaudio2Buffer.Flags = XAUDIO2_END_OF_STREAM;
	xaudio2Buffer.AudioBytes = (size_t)main_data->GetBuffer()->Length * sizeof(main_data->GetBuffer()[0]);
	xaudio2Buffer.LoopCount = 0;

	result = sourceVoice->Get()->SubmitSourceBuffer(&xaudio2Buffer);
	if (FAILED(result))
	{
		lastError = CSAResult::FAILED_SUBMIT_XAUDIO2_BUFFER;
		return Types::INVALID_HANDLE;
	}

	sourceVoices->Add(sourceVoice);
	//sourceVoiceInfos->Add(main_data->GetInfo());

	return (sourceVoices->Count - 1);
}

CSAHandle CSA::CSharpAudio::Submit(BaseFormat^ main_data, int loopCount)
{
	if (!main_data->IsValid())
	{
		lastError = CSAResult::INVALID_DATA;
		return Types::INVALID_HANDLE;
	}

	IXAudio2SourceVoice* sourceVoice_tmp = nullptr;
	WAVEFORMATEX fmt = main_data->GetInfo().GetNativeData();

	HRESULT result = xaudio2->Get()->CreateSourceVoice(&sourceVoice_tmp, &fmt, XAUDIO2_VOICE_USEFILTER);
	if (FAILED(result))
	{
		lastError = CSAResult::FAILED_CREATE_SOURCEVOICE;
		return Types::INVALID_HANDLE;
	}
	XAudio2SV^ sourceVoice = gcnew XAudio2SV();
	sourceVoice->SetPointer(sourceVoice_tmp);

	XAUDIO2_BUFFER xaudio2Buffer{};
	if (main_data->GetBuffer()->Length == 0)
	{
		lastError = CSAResult::EMPTY_BUFFER;
		return Types::INVALID_HANDLE;
	}

	pin_ptr<short> nativeBuf = &main_data->GetBuffer()[0];
	xaudio2Buffer.pAudioData = (BYTE*)nativeBuf;
	xaudio2Buffer.Flags = XAUDIO2_END_OF_STREAM;
	xaudio2Buffer.AudioBytes = (size_t)main_data->GetBuffer()->Length * sizeof(main_data->GetBuffer()[0]);
	xaudio2Buffer.LoopCount = loopCount;

	result = sourceVoice->Get()->SubmitSourceBuffer(&xaudio2Buffer);
	if (FAILED(result))
	{
		lastError = CSAResult::FAILED_SUBMIT_XAUDIO2_BUFFER;
		return Types::INVALID_HANDLE;
	}

	sourceVoices->Add(sourceVoice);
	//sourceVoiceInfos->Add(main_data->GetInfo());

	return (sourceVoices->Count - 1);
}

CSAResult CSA::CSharpAudio::GetLastError()
{
	return lastError;
}

bool CSA::CSharpAudio::IsStarting(CSAHandle handle)
{
	if (validHandle(handle))
	{
		XAUDIO2_VOICE_STATE state{};
		sourceVoices[handle]->Get()->GetState(&state);
		return state.BuffersQueued != 0;
	}
	return false;
}

void CSA::CSharpAudio::Start(CSAHandle handle)
{
	if (validHandle(handle))
	{
		//HRESULT result = sourceVoices->ToArray()[handle]->Get()->Start();
		HRESULT result = sourceVoices[handle]->Get()->Start();
		if (FAILED(result))
		{
			switch (static_cast<CSAResult>(result))
			{
			case CSAResult::INVALID_CALL:
				throw gcnew CSharpAudioException(CSAResult::INVALID_CALL.ToString());
				break;

			case CSAResult::XMA_DECODER_ERROR:
				throw gcnew CSharpAudioException(
					"Xbox 360 XMA ハードウェアで回復不能なエラーが発生しました: " + CSAResult::XMA_DECODER_ERROR.ToString()
				);
				break;

			case CSAResult::XAPO_CREATION_FAILED:
				throw gcnew CSharpAudioException(
					"XAPOのインスタンス化に失敗しました: " + CSAResult::XAPO_CREATION_FAILED.ToString()
				);
				break;

			case CSAResult::DEVICE_INVALIDATED:
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

void CSA::CSharpAudio::Stop(CSAHandle handle)
{
	if (validHandle(handle))
	{
		HRESULT result = sourceVoices[handle]->Get()->Stop();
		if (FAILED(result))
		{
			switch (static_cast<CSAResult>(result))
			{
			case CSAResult::INVALID_CALL:
				throw gcnew CSharpAudioException(CSAResult::INVALID_CALL.ToString());
				break;

			case CSAResult::XMA_DECODER_ERROR:
				throw gcnew CSharpAudioException(
					"Xbox 360 XMA ハードウェアで回復不能なエラーが発生しました: " + CSAResult::XMA_DECODER_ERROR.ToString()
				);
				break;

			case CSAResult::XAPO_CREATION_FAILED:
				throw gcnew CSharpAudioException(
					"XAPOのインスタンス化に失敗しました: " + CSAResult::XAPO_CREATION_FAILED.ToString()
				);
				break;

			case CSAResult::DEVICE_INVALIDATED:
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

void CSA::CSharpAudio::SetVolume(CSAHandle handle, float volume)
{
	if (validHandle(handle))
	{
		HRESULT result = sourceVoices[handle]->Get()->SetVolume(volume);
		if (FAILED(result))
		{
			switch (static_cast<CSAResult>(result))
			{
			case CSAResult::INVALID_CALL:
				throw gcnew CSharpAudioException(CSAResult::INVALID_CALL.ToString());
				break;

			case CSAResult::XMA_DECODER_ERROR:
				throw gcnew CSharpAudioException(
					"Xbox 360 XMA ハードウェアで回復不能なエラーが発生しました: " + CSAResult::XMA_DECODER_ERROR.ToString()
				);
				break;

			case CSAResult::XAPO_CREATION_FAILED:
				throw gcnew CSharpAudioException(
					"XAPOのインスタンス化に失敗しました: " + CSAResult::XAPO_CREATION_FAILED.ToString()
				);
				break;

			case CSAResult::DEVICE_INVALIDATED:
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