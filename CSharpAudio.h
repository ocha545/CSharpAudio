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
