#include"CSharpAudio.h"

CSA::CSharpAudio::CSharpAudio()
{
	xaudio2 = gcnew XAudio2();
	masterVoice = gcnew XAudio2MV();
	sourceVoices = gcnew List<XAudio2SV^>(0);
	sourceVoiceInfos = gcnew List<CSAInfo>(0);
	coInitialized = false;

	switch (Initialize())
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
	this->!CSharpAudio();
}

CSA::CSharpAudio::!CSharpAudio()
{
	for (int i = 0; i < sourceVoices->Count; i++)
	{
		if (sourceVoices[i]->IsValid())
		{
			sourceVoices[i]->DestroyVoice();
		}
	}
	if (masterVoice->IsValid())
	{
		masterVoice->DestroyVoice();
	}
	if (xaudio2->IsValid())
	{
		xaudio2->Release();
	}
	if (coInitialized)
	{
		CoUninitialize();
		coInitialized = false;
	}
}

CSA::CSharpAudio::CSAResult CSA::CSharpAudio::Initialize()
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

CSAHandle CSA::CSharpAudio::Submit(BaseFormat^ main_data)
{
	IXAudio2SourceVoice* sourceVoice_tmp = nullptr;
	WAVEFORMATEX fmt = main_data->GetInfo().GetNativeData();

	HRESULT result = xaudio2->Get()->CreateSourceVoice(&sourceVoice_tmp, &fmt, XAUDIO2_VOICE_USEFILTER);
	if (FAILED(result))
	{
		return -1;
	}
	XAudio2SV^ sourceVoice = gcnew XAudio2SV();
	sourceVoice->SetPointer(sourceVoice_tmp);

	XAUDIO2_BUFFER xaudio2Buffer{};
	if (main_data->GetBuffer()->Length == 0)
	{
		return -1;
	}
	pin_ptr<short> nativeBuf = &main_data->GetBuffer()[0];
	//	GCHandle gcHandle = GCHandle::Alloc(main_data->GetBuffer(), GCHandleType::Pinned);
	xaudio2Buffer.pAudioData = (BYTE*)nativeBuf;
	xaudio2Buffer.Flags = XAUDIO2_END_OF_STREAM;
	xaudio2Buffer.AudioBytes = (size_t)main_data->GetBuffer()->Length * sizeof(main_data->GetBuffer()[0]);
	xaudio2Buffer.LoopCount = 0;

	result = sourceVoice->Get()->SubmitSourceBuffer(&xaudio2Buffer);
	if (FAILED(result))
	{
		System::Console::WriteLine("オーディオバッファの送信に失敗しました");
		return -1;
	}

	sourceVoices->Add(sourceVoice);
	sourceVoiceInfos->Add(main_data->GetInfo());

	return (sourceVoices->Count - 1);
}


CSAHandle CSA::CSharpAudio::Submit(BaseFormat^ main_data, int loop)
{
	IXAudio2SourceVoice* sourceVoice_tmp = nullptr;
	WAVEFORMATEX fmt = main_data->GetInfo().GetNativeData();

	HRESULT result = xaudio2->Get()->CreateSourceVoice(&sourceVoice_tmp, &fmt, XAUDIO2_VOICE_USEFILTER);
	if (FAILED(result))
	{
		return -1;
	}
	XAudio2SV^ sourceVoice = gcnew XAudio2SV();
	sourceVoice->SetPointer(sourceVoice_tmp);

	XAUDIO2_BUFFER xaudio2Buffer{};
	if (main_data->GetBuffer()->Length == 0)
	{
		return -1;
	}

	pin_ptr<short> nativeBuf = &main_data->GetBuffer()[0];
	xaudio2Buffer.pAudioData = (BYTE*)nativeBuf;
	xaudio2Buffer.Flags = XAUDIO2_END_OF_STREAM;
	xaudio2Buffer.AudioBytes = (size_t)main_data->GetBuffer()->Length * sizeof(main_data->GetBuffer()[0]);
	xaudio2Buffer.LoopCount = loop;

	result = sourceVoice->Get()->SubmitSourceBuffer(&xaudio2Buffer);
	if (FAILED(result))
	{
		System::Console::WriteLine("オーディオバッファの送信に失敗しました");
		return -1;
	}

	sourceVoices->Add(sourceVoice);
	sourceVoiceInfos->Add(main_data->GetInfo());

	return (sourceVoices->Count - 1);
}

/*
CSAHandle CSA::CSharpAudio::OldSubmit(array<short>^ buffer, CSAInfo info, int loop)
{
	IXAudio2SourceVoice* sourceVoice_tmp = nullptr;
	WAVEFORMATEX fmt = info.GetNativeData();

	HRESULT result = xaudio2->Get()->CreateSourceVoice(&sourceVoice_tmp, &fmt, XAUDIO2_VOICE_USEFILTER);
	if (FAILED(result))
	{
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
*/
bool CSA::CSharpAudio::IsStarting(CSAHandle handle)
{
	if (sourceVoices[handle]->IsValid())
	{
		XAUDIO2_VOICE_STATE state{};
		sourceVoices[handle]->Get()->GetState(&state);
		return state.BuffersQueued != 0;
	}
	return false;
}

void CSA::CSharpAudio::Start(CSAHandle handle)
{
	if (sourceVoices[handle]->IsValid())
	{
		sourceVoices[handle]->Get()->Start();
	}
}

void CSA::CSharpAudio::Stop(CSAHandle handle)
{
	if (sourceVoices[handle]->IsValid())
	{
		sourceVoices[handle]->Get()->Stop();
	}
}

void CSA::CSharpAudio::Volume(CSAHandle handle, float volume)
{
	if (sourceVoices[handle]->IsValid())
	{
		sourceVoices[handle]->Get()->SetVolume(volume);
	}
}