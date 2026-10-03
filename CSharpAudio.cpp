#include"CSharpAudio.h"

bool CSA::CSharpAudio::validHandle(CSAHandle handle)
{
	if (sourceVoices == nullptr)
	{
		return false;
	}
	if (handle < sourceVoices->Count && handle >= 0 &&
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
		if (coInitialized)
		{
			CoUninitialize();
			coInitialized = false;
		}
		return CSAResult::FAILED_CREATE_XAUDIO2;
	}
	xaudio2->SetPointer(xaudio2_tmp);

	IXAudio2MasteringVoice* masterVoice_tmp = nullptr;
	result = xaudio2->Get()->CreateMasteringVoice(&masterVoice_tmp);
	if (FAILED(result))
	{
		if (xaudio2 != nullptr)
		{
			if (xaudio2->IsValid())
			{
				xaudio2->Release();
			}
		}
		if (coInitialized)
		{
			CoUninitialize();
			coInitialized = false;
		}
		return CSAResult::FAILED_CREATE_MASTERINGVOICE;
	}
	masterVoice->SetPointer(masterVoice_tmp);

	return CSAResult::SUCCEEDED;//成功！
}


void CSA::CSharpAudio::printLog(String^ txt)
{
#ifdef CSA_PRINT_LOG
	System::Console::WriteLine(txt);
#endif
}

CSA::CSharpAudio::CSharpAudio()
{
	xaudio2 = gcnew XAudio2();
	masterVoice = gcnew XAudio2MV();
	sourceVoices = gcnew List<XAudio2SV^>(0);
	submitDataPtrs = gcnew List<IntPtr>(0);
	//sourceVoiceInfos = gcnew List<CSAInfo>(0);
	//submitDataSizes = gcnew List<uint64_t>(0);
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
		for(int i = 0; i < sourceVoices->Count; i++)
		{
			if (sourceVoices[i] != nullptr)
			{
				if (sourceVoices[i]->IsValid())
				{
					sourceVoices[i]->Get()->Stop();
					sourceVoices[i]->Get()->FlushSourceBuffers();
					sourceVoices[i]->DestroyVoice();
				}
				delete sourceVoices[i];
//System::Console::WriteLine("DELETE XAudio2SV[" + i + "]");
				printLog("DELETE XAudio2SV[" + i + "]");
			}
		}
		sourceVoices->Clear();
		delete sourceVoices;
		sourceVoices = nullptr;
//System::Console::WriteLine("DELETE XAudio2SV List");
		printLog("DELETE XAudio2SV List");
	}

	if (submitDataPtrs != nullptr)
	{
		for(int i = 0; i < submitDataPtrs->Count; i++)
		{
			if (submitDataPtrs[i].ToPointer() != nullptr && submitDataPtrs[i] != IntPtr::Zero)
			{
				CoTaskMemFree(submitDataPtrs[i].ToPointer());
			}
			submitDataPtrs[i] = IntPtr::Zero;
//System::Console::WriteLine("DELETE SubmitDataPtrs[" + i + "]");
			printLog("DELETE IntPtr[" + i + "]");
		}
		submitDataPtrs->Clear();
		delete submitDataPtrs;
		submitDataPtrs = nullptr;
//System::Console::WriteLine("DELETE SubmitDataPtrs");
		printLog("DELETE IntPtr List");
	}

	if (masterVoice != nullptr)
	{
		if(masterVoice->IsValid())
		{
			masterVoice->DestroyVoice();
//System::Console::WriteLine("DELETE MasteringVoice");
			printLog("DELETE MasteringVoice");
		}
		delete masterVoice;
		masterVoice = nullptr;
	}

	if (xaudio2 != nullptr)
	{
		if (xaudio2->IsValid())
		{
			xaudio2->Release();
//System::Console::WriteLine("DELETE XAudio2");
			printLog("DELETE XAudio2");
		}
		delete xaudio2;
		xaudio2 = nullptr;
	}

	// CoInitializeを行ったスレッドと同じになるようデストラクタで行う
	if (coInitialized)
	{
		CoUninitialize();
		coInitialized = false;
//System::Console::WriteLine("Uninitialize COM");
		printLog("Uninitialize COM");
	}
}

CSAHandle CSA::CSharpAudio::Submit(BaseFormat^ main_data)
{
	//ループしない
	return Submit(main_data, 0);
}

CSAHandle CSA::CSharpAudio::Submit(BaseFormat^ main_data, unsigned int loopCount)
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
		sourceVoice->DestroyVoice();
		delete sourceVoice;
		sourceVoice = nullptr;

		lastError = CSAResult::EMPTY_BUFFER;
		return Types::INVALID_HANDLE;
	}

	uint64_t submitDataSize = (uint64_t)main_data->GetBuffer()->Length * sizeof(main_data->GetBuffer()[0]);
	void* submitDataPtr = CoTaskMemAlloc(submitDataSize);
	if (submitDataPtr == nullptr)
	{
		sourceVoice->DestroyVoice();
		delete sourceVoice;
		sourceVoice = nullptr;

		lastError = CSAResult::FAILED_CO_TASK_MEM_ALLOC;
		return Types::INVALID_HANDLE;
	}

	pin_ptr<short> sourceData = &main_data->GetBuffer()[0];
	memcpy(submitDataPtr, sourceData, submitDataSize);

	xaudio2Buffer.pAudioData = static_cast<const BYTE*>(submitDataPtr);
	xaudio2Buffer.Flags = XAUDIO2_END_OF_STREAM;
	xaudio2Buffer.AudioBytes = submitDataSize;
	xaudio2Buffer.LoopCount = loopCount;

	result = sourceVoice->Get()->SubmitSourceBuffer(&xaudio2Buffer);
	if (FAILED(result))
	{
		CoTaskMemFree(submitDataPtr);

		sourceVoice->DestroyVoice();
		delete sourceVoice;
		sourceVoice = nullptr;

		lastError = CSAResult::FAILED_SUBMIT_XAUDIO2_BUFFER;
		return Types::INVALID_HANDLE;
	}

	if (sourceVoices->Count != submitDataPtrs->Count)
	{
		lastError = CSAResult::INCONSISTENT_HANDLE;
		return Types::INVALID_HANDLE;
	}

	sourceVoices->Add(sourceVoice);
	submitDataPtrs->Add(IntPtr(submitDataPtr));

	lastError = CSAResult::UNUSED;
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
		HRESULT result = sourceVoices[handle]->Get()->Start();
		CSharpAudioExceptionFuncs::ThrowXAudio2Exception(result);
	}
}

void CSA::CSharpAudio::Stop(CSAHandle handle)
{
	if (validHandle(handle))
	{
		HRESULT result = sourceVoices[handle]->Get()->Stop();
		CSharpAudioExceptionFuncs::ThrowXAudio2Exception(result);
	}
}

void CSA::CSharpAudio::SetVolume(CSAHandle handle, float volume)
{
	if (validHandle(handle))
	{
		HRESULT result = sourceVoices[handle]->Get()->SetVolume(volume);
		CSharpAudioExceptionFuncs::ThrowXAudio2Exception(result);
	}
}