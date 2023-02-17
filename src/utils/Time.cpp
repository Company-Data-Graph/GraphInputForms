#include "Time.hpp"
#include <cstring>
#include <time.h>
#include <stdlib.h>

namespace DataGraph::utils
{
char* strptime(const char* s, const char* format, struct tm* tm)
{
	memset(tm, 0, sizeof(*tm));

	while (*format)
	{
		if (*format != '%')
		{
			if (*s != *format)
			{
				return 0;
			}
			++s;
			++format;
			continue;
		}

		++format;

		switch (*format)
		{
		case 'Y':
			tm->tm_year = strtol(s, (char**)&s, 10) - 1900;
			break;
		case 'm':
			tm->tm_mon = strtol(s, (char**)&s, 10) - 1;
			break;
		case 'd':
			tm->tm_mday = strtol(s, (char**)&s, 10);
			break;
		case 'H':
			tm->tm_hour = strtol(s, (char**)&s, 10);
			break;
		case 'M':
			tm->tm_min = strtol(s, (char**)&s, 10);
			break;
		case 'S':
			tm->tm_sec = strtol(s, (char**)&s, 10);
			break;
		default:
			return 0;
		}

		format++;
	}

	return (char*)s;
}
}  // namespace DataGraph::utils
