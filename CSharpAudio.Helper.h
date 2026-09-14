#pragma once
#using<System.dll>
#include<vector>
#include<cmath>
using System::String;


namespace CSA
{
	namespace Helper
	{
		void SampleToTime(double sample, int& h, int& m, double& s);

		String^ FormatTwoDigids(int num);

		String^ FormatTwoDigids(double num);

		String^ ConvertTimeString(uint64_t sample, unsigned long sampleRate, unsigned short channels);
	}
}