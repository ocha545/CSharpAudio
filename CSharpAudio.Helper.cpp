#include"CSharpAudio.Helper.h"


void CSA::Helper::SampleToTime(double sample, int& h, int& m, double& s)
{
	if (sample <= 0.0) sample = 0.0;

	h = (int)(sample / 3600);
	sample = std::fmod(sample, 3600);
	m = (int)(sample / 60);
	sample = std::fmod(sample, 60);
	s = sample;
}

String^ CSA::Helper::FormatTwoDigids(int num)
{
	return (num < 10) ? ("0" + num.ToString()) : num.ToString();
}


String^ CSA::Helper::FormatTwoDigids(double num)
{
	return "00";
	//return (num < 10) ? num.ToString()->Substring(2, 2) : num.ToString()->Substring(3, 2);
}

String^ CSA::Helper::ConvertTimeString(uint64_t sample, unsigned long sampleRate, unsigned short channels)
{
	if (sampleRate <= 0 || channels <= 0) return "";

	double totalSample = ((double)sample / (double)channels) / (double)sampleRate;

	int h = 0;
	int m = 0;
	double s = 0.0;
	SampleToTime(totalSample, h, m, s);

	return FormatTwoDigids(h) + ":" + FormatTwoDigids(m) + ":" + FormatTwoDigids((int)s) + "." + FormatTwoDigids(s);
}
