/* time.c - time functions.
*
* LICENSE:
*
*   Copyright (C) 1991, 1993, 1997, 1998, 2002 Free Software Foundation, Inc.
*
*   Based on an implementation from the GNU C Library.
*
*/

#include <time.h>

static unsigned long
_mktime (unsigned int year, unsigned int mon,
         unsigned int day, unsigned int hour,
         unsigned int min, unsigned int sec)
{
	if (0 >= (int) (mon -= 2)) {    /* 1..12 -> 11,12,1..10 */
		mon += 12;                  /* Puts Feb last since it has leap day */
		year -= 1;
	}

	return (((
		(unsigned long)
		(year/4 - year/100 + year/400 + 367*mon/12 + day) +
		year*365 - 719499
		)*24 + hour /* now have hours */
		)*60 + min /* now have minutes */
        )*60 + sec; /* finally seconds */
}


#define SECS_PER_HOUR   (60 * 60)
#define SECS_PER_DAY    (SECS_PER_HOUR * 24)

/* Nonzero if YEAR is a leap year (every 4 years,
 * except every 100th isn't, and every 400th is).
 */
# define __isleap(year) \
((year) % 4 == 0 && ((year) % 100 != 0 || (year) % 400 == 0))


static const unsigned short int __mon_yday[2][13] =
{
    /* Normal years.  */
    { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 },
		/* Leap years.  */
    { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 }
};


/* Compute the `struct tm' representation of *T,
 * offset OFFSET seconds east of UTC,
 * and store year, yday, mon, mday, wday, hour, min, sec into *TP.
 * Return nonzero if successful.
 */
static int
__offtime (t, offset, tp)
const time_t *t;
long int offset;
struct tm *tp;
{
	long int days, rem, y;
	const unsigned short int *ip;

	days = *t / SECS_PER_DAY;
	rem = *t % SECS_PER_DAY;
	rem += offset;
	while (rem < 0)
    {
		rem += SECS_PER_DAY;
		--days;
    }
	while (rem >= SECS_PER_DAY)
    {
		rem -= SECS_PER_DAY;
		++days;
    }
	tp->tm_hour = rem / SECS_PER_HOUR;
	rem %= SECS_PER_HOUR;
	tp->tm_min = rem / 60;
	tp->tm_sec = rem % 60;
	/* January 1, 1970 was a Thursday.  */
	tp->tm_wday = (4 + days) % 7;
	if (tp->tm_wday < 0)
		tp->tm_wday += 7;
	y = 1970;

#define DIV(a, b) ((a) / (b) - ((a) % (b) < 0))
#define LEAPS_THRU_END_OF(y) (DIV (y, 4) - DIV (y, 100) + DIV (y, 400))

	while (days < 0 || days >= (__isleap (y) ? 366 : 365))
    {
		/* Guess a corrected year, assuming 365 days per year.  */
		long int yg = y + days / 365 - (days % 365 < 0);

		/* Adjust DAYS and Y to match the guessed year.  */
		days -= ((yg - y) * 365
			+ LEAPS_THRU_END_OF (yg - 1)
			- LEAPS_THRU_END_OF (y - 1));
		y = yg;
    }
	tp->tm_year = y - 1900;
	if (tp->tm_year != y - 1900)
    {
		/* The year cannot be represented due to overflow.  */
		return 0;
    }
	tp->tm_yday = days;
	ip = __mon_yday[__isleap(y)];
	for (y = 11; days < (long int) ip[y]; --y)
		continue;
	days -= ip[y];
	tp->tm_mon = y;
	tp->tm_mday = days + 1;
	return 1;
}


struct tm *gmtime(const time_t *timep)
{
	static struct tm t;
	__offtime(timep, 0, &t);
	return &t;
}


/*
 * TODO: implement get_bias using a java syscall
 *
 * This function is missing the implementation of get_bias which
 * should return the the difference of the current timezone in minutes.
 * It should also return whether the current machine is in day light saving
 * time.
 */
#if 0
struct tm *localtime(const time_t *timep)
{
	static struct tm t;
	int bias = -get_bias(&t.tm_isdst);
	__offtime(timep, -bias * 60, &t);
	return &t;
}
#endif

time_t timegm(struct tm *_tm)
{
	return _mktime(_tm->tm_year + 1900, _tm->tm_mon + 1, _tm->tm_mday, _tm->tm_hour,
		_tm->tm_min, _tm->tm_sec);
}
