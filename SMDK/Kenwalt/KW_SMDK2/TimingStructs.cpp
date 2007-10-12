//Code definitions of timing structures that can be used in Downtime, ScheduledEvents, and Random Failure.
//Initial Implementation only used by Combined Downtime.

#include "stdafx.h"
#define __TIMINGSTRUCTS_CPP

#include "TimingStructs.h"

FailureTiming::FailureTiming()
{
	dAvgDowntime = 24 * 3600;
	dDowntimeStdDev = 24 * 3600;
	dAvgUptime = 30 * 24 * 3600;
	dUptimeStdDev = 10 * 24 * 3600;

	eFailureType = PDFType_Constant;
	eRepairType = PDFType_Constant;

	Reset();
}

void FailureTiming::Reset()
{
	dBackedUpDowntime = 0;
	dNextFailure = 0;
	dRepairsDone = 0;
	lFailureCount = 0;

	Repair();
}

void FailureTiming::Update(double deltaTime)
{
	if (dNextFailure < 0)	//Item is failed...
	{
		dRepairsDone -= deltaTime;
		if (dRepairsDone < 0)
			Repair();
	}
	else
	{
		dNextFailure -= deltaTime;
		if (dNextFailure < 0)
			Fail();
	}
}

void FailureTiming::Fail()
{
	double repairTime = CalculateVariable(dAvgDowntime, dDowntimeStdDev, eRepairType);
	dBackedUpDowntime += repairTime;
	dRepairsDone = dNextFailure + repairTime;
}

void FailureTiming::Repair()
{
	dNextFailure = dRepairsDone + CalculateVariable(dAvgUptime, dUptimeStdDev, eFailureType);
}

bool FailureTiming::ValidateDataFields()
{
	if (eFailureType == PDFType_Exponential)
		dDowntimeStdDev = dAvgDowntime;
	if (eRepairType == PDFType_Exponential)
		dUptimeStdDev = dAvgUptime;

	if (dAvgDowntime != dLastAvgDowntime || dAvgUptime != dLastAvgUptime ||
		dLastDowntimeStdDev != dDowntimeStdDev || dLastUptimeStdDev != dUptimeStdDev) //If you adjust the values, we'll reset.
	{
		Reset();
		dLastAvgDowntime = dAvgDowntime;
		dLastAvgUptime = dAvgUptime;
		dLastDowntimeStdDev = dDowntimeStdDev;
		dLastUptimeStdDev = dUptimeStdDev;
	}
	if (dAvgDowntime < 0)
		dAvgDowntime = 0;
	if (dAvgUptime < 0)
		dAvgUptime = 0;
	if (dDowntimeStdDev < 0)
		dDowntimeStdDev = 0;
	if (dUptimeStdDev < 0)
		dUptimeStdDev = 0;
	return true;
}

void FailureTiming::RevalidateParameters()
{
	if (dDowntimeStdDev > dAvgDowntime)
		dDowntimeStdDev = dAvgDowntime;
	if (dUptimeStdDev > dAvgUptime)
		dUptimeStdDev = dAvgUptime;
}

double FailureTiming::CalculateVariable(double dAverage, double dStdDev, PDFType ePDF)
{
	double rnd = (double) rand() / (double) RAND_MAX;
	double ret;
	if (rnd == 0) rnd = (double) 1 / (double) RAND_MAX;	//To avoid log(0) errors.
	if (rnd == 1) rnd = (double) (RAND_MAX - 1) / (double) RAND_MAX;	//To avoid any errors caused by rnd = 1 (Possible future errors).
	switch (ePDF)
	{
	case PDFType_Constant:
		ret = dAverage + 2 * dStdDev * (rnd - 0.5);
		break;
	case PDFType_Exponential:
		ret = -log(rnd) * dAverage;	//TODO: Check that this is valid (Initial testing indicates that it is.)
		break;
	case PDFType_Normal:			//This is derived from the approximation found on Mathworld for 1/2 * Erf(x) = 0.1x(4.4-x). http://mathworld.wolfram.com/NormalDistributionFunction.html
		double dev;
		if (rnd > 0.5)				//Effectively, we're normalising this so -0.5 <= rnd <= 0.5.
			if (rnd <= 0.98)
				dev = 2.2 - sqrt(4.84-10*(rnd-0.5));
			else
				dev = 2.2 + 40 * (rnd - 0.98);
		else
			if (rnd <= 0.48)
				dev = -2.2 + sqrt(4.84-10*rnd);
			else
				dev = -2.2 - 40 * (rnd - 0.98);
		ret = dAverage + dStdDev * dev;
		break;
	default:
		ret = dAverage;				//If the PDFType is not supported.
	}
	return ret > 0 ? ret : 0;		//Simple failsafe for in case the function results in the event happening before the present.
}

ScheduledTiming::ScheduledTiming()
{
	dDesiredDowntime = 3600;
	dOffset = 0.0;
	dDesiredPeriod = 24 * 3600;
	dDowntime = dDesiredDowntime;
	dPeriod = dDesiredDowntime;
}

void ScheduledTiming::Reset()
{
	dNextShutdown = dOffset;
	dBackedUpDowntime = 0.0;
}

void ScheduledTiming::Update(double deltaTime)
{
	dNextShutdown -= deltaTime;
	if (dNextShutdown < 0)
	{
		dBackedUpDowntime += dDowntime;
		dNextShutdown += dPeriod;
	}
}

bool ScheduledTiming::ValidateDataFields(double deltaTime, bool bForceIntegralPeriod, bool bForceIntegralDowntime)
{
	if (dOffset < 0)
		dOffset = 0.0;

	if (bForceIntegralPeriod)
	{
		dPeriod = ROUNDBY(dDesiredPeriod, deltaTime);
		if (dPeriod == 0 && dDesiredPeriod != 0)
			dPeriod = deltaTime;
	}
	else
		dPeriod = dDesiredPeriod;

	if (bForceIntegralDowntime)
		dDowntime = ROUNDBY(dDesiredDowntime, deltaTime);
	else
		dDowntime = dDesiredDowntime;

	return dPeriod > 0;
}

void ScheduledTiming::RevalidateParameters(double deltaTime, bool bForceIntegralPeriod, bool bForceIntegralDowntime)
{
	if (dDesiredDowntime > dDesiredPeriod)
		dDesiredDowntime = dDesiredPeriod;

	if (bForceIntegralDowntime)
		dDowntime = ROUNDBY(dDesiredDowntime, deltaTime);
	else
		dDowntime = dDesiredDowntime;

	if (bForceIntegralPeriod)
		dPeriod = ROUNDBY(dDesiredPeriod, deltaTime);
	else
		dPeriod = dDesiredPeriod;
}