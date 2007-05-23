//Header file for structs useful to Random Failure, Scheduled Events, and Combined downtime
//It could be useful to integrate the others to use this down the track. For now, only combined
//downtime is using this.

#ifndef __TIMINGSTRUCTS_H
#define __TIMINGSTRUCTS_H

#ifndef __MD_HEADERS_H
#include "md_headers.h"
#endif

template <class T> T ROUNDBY(T a, T b) { return a/b - (int)(a/b) < 0.5 ? b * (int)(a/b) : b * (int)(a/b + 1); }

enum PDFType { PDFType_Constant, PDFType_Exponential, PDFType_Normal };

enum DT_RunningStates {
	DTRS_MinorMaintenance,	DTRS_MajorMaintenance,
	DTRS_MinorFailure,		DTRS_MajorFailure,
	DTRS_Running,			DTRS_Off };

struct FailureTiming
{
	FailureTiming();
    double dAvgDowntime, dDowntimeStdDev;
	double dAvgUptime, dUptimeStdDev;
	
    PDFType eFailureType;
	PDFType eRepairType;

	double dNextFailure;	//When the task will next fail
	double dRepairsDone;	//When the task will be repaired
	double dBackedUpDowntime;	//How much downtime the task needs in actual iterations

	long lFailureCount;

	void Reset();
	void Update(double deltaTime);
	bool ValidateDataFields();
	void RevalidateParameters();

protected:
	void Repair();
	void Fail();
	double CalculateVariable(double Average, double Dev, PDFType PDF);
};

struct ScheduledTiming
{
	ScheduledTiming();
    double dDesiredDowntime, dDowntime;
    double dDesiredPeriod, dPeriod;
    double dOffset;
	double dNextShutdown;
	
	double dBackedUpDowntime;

	void Reset();
	void Update(double deltaTime);
	bool ValidateDataFields(double deltaTime, bool bForceIntegralPeriod, bool bForceIntegralDowntime);
	void RevalidateParameters(double deltaTime, bool bForceIntegralPeriod, bool bForceIntegralDowntime);
};

#endif