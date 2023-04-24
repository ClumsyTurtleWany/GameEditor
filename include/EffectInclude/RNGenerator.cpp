#include "RNGenerator.h"

namespace EFFECTUTIL
{
	RNGenerator::RNGenerator() : gen(rd())
	{
		setI_Range(0, 1);
		setLL_Range(0, 1);
		setF_Range(0.0f, 1.0f);
		setD_Range(0.0f, 1.0f);
	}

	RNGenerator::~RNGenerator()
	{
	}

	void RNGenerator::setI_Range(int min, int max)
	{
		if (min > max)
		{
			auto temp = min;
			min = max;
			max = temp;
		}

		iParam._Init(min, max);
		iDist.param(iParam);
	}

	void RNGenerator::setLL_Range(long long min, long long max)
	{
		if (min > max)
		{
			auto temp = min;
			min = max;
			max = temp;
		}

		llParam._Init(min, max);
		llDist.param(llParam);
	}

	void RNGenerator::setF_Range(float min, float max)
	{
		if (min > max)
		{
			auto temp = min;
			min = max;
			max = temp;
		}

		fParam._Init(min, max);
		fDist.param(fParam);
	}

	void RNGenerator::setD_Range(double min, double max)
	{
		if (min > max)
		{
			auto temp = min;
			min = max;
			max = temp;
		}

		dParam._Init(min, max);
		dDist.param(dParam);
	}

	std::pair<int, int> RNGenerator::getI_Range()
	{
		return std::pair<int, int>(iParam._Min, iParam._Max);
	}

	std::pair<long long, long long> RNGenerator::getLL_Range()
	{
		return std::pair<long long, long long>(llParam._Min, llParam._Max);
	}

	std::pair<float, float> RNGenerator::getF_Range()
	{
		return std::pair<float, float>(fParam._Min, fParam._Max);
	}

	std::pair<double, double> RNGenerator::getD_Range()
	{
		return std::pair<double, double>(dParam._Min, dParam._Max);
	}

	int RNGenerator::getI()
	{
		return iDist(gen);
	}

	int RNGenerator::getI(int min, int max)
	{
		if (min > max)
		{
			auto temp = min;
			min = max;
			max = temp;
		}

		std::uniform_int<int>::param_type temp(min, max);
		iTDist.param(temp);
		return iTDist(gen);
	}

	long long RNGenerator::getLL()
	{
		return llDist(gen);
	}

	long long RNGenerator::getLL(long long min, long long max)
	{
		if (min > max)
		{
			auto temp = min;
			min = max;
			max = temp;
		}

		std::uniform_int<long long>::param_type temp(min, max);
		llTDist.param(temp);
		return llTDist(gen);
	}

	float RNGenerator::getF()
	{
		return fDist(gen);
	}

	float RNGenerator::getF(float min, float max)
	{
		if (min > max)
		{
			auto temp = min;
			min = max;
			max = temp;
		}

		std::uniform_real<float>::param_type temp(min, max);
		fTDist.param(temp);
		return fTDist(gen);
	}

	double RNGenerator::getD()
	{
		return dDist(gen);
	}

	double RNGenerator::getD(double min, double max)
	{
		if (min > max)
		{
			auto temp = min;
			min = max;
			max = temp;
		}

		std::uniform_real<double>::param_type temp(min, max);
		dTDist.param(temp);
		return dTDist(gen);
	}
}