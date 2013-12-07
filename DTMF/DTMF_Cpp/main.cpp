#include "Dtmf.h"

int main()
{
	Dtmf dtmf;
	dtmf.InitResultDictionary();

	dtmf.SetDigit('6');
	dtmf.Init();
	double* test = dtmf.GetData();
	for (int i = 0; i < dtmf.GetLength(); i++)
	{
		std::cout << dtmf.Run(test[i]) << " ";
	}
	std::cout << std::endl;

	delete[] test;
	return 0;
}