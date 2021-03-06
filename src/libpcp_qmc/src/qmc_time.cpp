/*
 * Copyright (c) 2006, Ken McDonell.  All Rights Reserved.
 * Copyright (c) 2006-2007, Aconex.  All Rights Reserved.
 * 
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 */
#include <QtGui/QIcon>
#include "qmc_time.h"

//
// Map icon type name to QIcon
//
extern QIcon *QmcTime::icon(QmcTime::Icon type)
{
    static QIcon icons[QmcTime::IconCount];
    static int setup;

    if (!setup) {
	setup = 1;
	icons[QmcTime::ForwardOn] = QIcon(":play_on.png");
	icons[QmcTime::ForwardOff] = QIcon(":play_off.png");
	icons[QmcTime::StoppedOn] = QIcon(":stop_on.png");
	icons[QmcTime::StoppedOff] = QIcon(":stop_off.png");
	icons[QmcTime::BackwardOn] = QIcon(":back_on.png");
	icons[QmcTime::BackwardOff] = QIcon(":back_off.png");
	icons[QmcTime::FastForwardOn] = QIcon(":fastfwd_on.png");
	icons[QmcTime::FastForwardOff] = QIcon(":fastfwd_off.png");
	icons[QmcTime::FastBackwardOn] = QIcon(":fastback_on.png");
	icons[QmcTime::FastBackwardOff] = QIcon(":fastback_off.png");
	icons[QmcTime::StepForwardOn] = QIcon(":stepfwd_on.png");
	icons[QmcTime::StepForwardOff] = QIcon(":stepfwd_off.png");
	icons[QmcTime::StepBackwardOn] = QIcon(":stepback_on.png");
	icons[QmcTime::StepBackwardOff] = QIcon(":stepback_off.png");
    }
    return &icons[type];
}

//
// Test for not-zeroed timeval
//
int QmcTime::timevalNonZero(struct timeval *a)
{
    return (a->tv_sec != 0 || a->tv_usec != 0);
}

//
// a := a + b for struct timevals
//
void QmcTime::timevalAdd(struct timeval *a, struct timeval *b)
{
    a->tv_usec += b->tv_usec;
    if (a->tv_usec > 1000000) {
	a->tv_usec -= 1000000;
	a->tv_sec++;
    }
    a->tv_sec += b->tv_sec;
}

//
// a := a - b for struct timevals, result is never less than zero
//
void QmcTime::timevalSub(struct timeval *a, struct timeval *b)
{
    a->tv_usec -= b->tv_usec;
    if (a->tv_usec < 0) {
	a->tv_usec += 1000000;
	a->tv_sec--;
    }
    a->tv_sec -= b->tv_sec;
    if (a->tv_sec < 0) {
	/* clip negative values at zero */
	a->tv_sec = 0;
	a->tv_usec = 0;
    }
}

//
// a : b for struct timevals ... <0 for a<b, ==0 for a==b, >0 for a>b
//
int QmcTime::timevalCompare(struct timeval *a, struct timeval *b)
{
    int res = (int)(a->tv_sec - b->tv_sec);
    if (res == 0)
	res = (int)(a->tv_usec - b->tv_usec);
    return res;
}

//
// Conversion from seconds (double precision) to struct timeval
//
void QmcTime::secondsToTimeval(double value, struct timeval *tv)
{
    tv->tv_sec = (time_t)value;
    tv->tv_usec = (long)(((value - (double)tv->tv_sec) * 1000000.0));
}

//
// Conversion from struct timeval to seconds (double precision)
//
double QmcTime::secondsFromTimeval(struct timeval *tv)
{
    return (double)tv->tv_sec + ((double)tv->tv_usec / 1000000.0);
}

//
// Conversion from other time units into seconds
//
double QmcTime::unitsToSeconds(double value, QmcTime::DeltaUnits units)
{
    if (units == QmcTime::Milliseconds)
	return value / 1000.0;
    else if (units == QmcTime::Minutes)
	return value * 60.0;
    else if (units == QmcTime::Hours)
	return value * (60.0 * 60.0);
    else if (units == QmcTime::Days)
	return value * (60.0 * 60.0 * 24.0);
    else if (units == QmcTime::Weeks)
	return value * (60.0 * 60.0 * 24.0 * 7.0);
    return value;
}

//
// Conversion from seconds into other time units
//
double QmcTime::secondsToUnits(double value, QmcTime::DeltaUnits units)
{
    switch (units) {
    case Milliseconds:
	value = value * 1000.0;
	break;
    case Minutes:
	value = value / 60.0;
	break;
    case Hours:
	value = value / (60.0 * 60.0);
	break;
    case Days:
	value = value / (60.0 * 60.0 * 24.0);
	break;
    case Weeks:
	value = value / (60.0 * 60.0 * 24.0 * 7.0);
	break;
    case Seconds:
    default:
	break;
    }
    return value;
}

double QmcTime::deltaValue(QString delta, QmcTime::DeltaUnits units)
{
    return QmcTime::secondsToUnits(delta.trimmed().toDouble(), units);
}

QString QmcTime::deltaString(double value, QmcTime::DeltaUnits units)
{
    QString delta;

    value = QmcTime::secondsToUnits(value, units);
    if ((double)(int)value == value)
	delta.sprintf("%.2f", value);
    else
	delta.sprintf("%.6f", value);
    return delta;
}
