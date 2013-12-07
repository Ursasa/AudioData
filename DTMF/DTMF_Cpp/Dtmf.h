#pragma once
#ifndef _DTMF_H
#define _DTMF_H

#include <stdio.h>
#include <iostream>
#include <map>
#include <string>
#include <math.h>

#define M_PI 3.14159265358979323846

class Dtmf
{
private:
	double SampleRate;
	double* m_pTone;
	int m_iPacketLength;
	double* goertzel_freq;
	int goertzel_freq_length;
	std::map<double, double> s_prev, s_prev2, totalpower, N, coeff;
	std::map<std::pair<double, double>, std::string> Result;
	char m_digit;
	int iMilliSeconds;

public:
	Dtmf(void);
	Dtmf(char);
	~Dtmf(void);
	void SetDigit(char);
	void Init();
	void InitResultDictionary();
	void GenerateSample();
	double* GetData();
	int GetLength();
	std::string Run(double);
	std::string GetNumber(std::map<double, double>);
};

#endif


