#ifndef DATE_TIME
#define DATE_TIME

#include "base.h"

QString printDateTime(const QDateTime &datetime);
QString timestamp(time_t = 0);
QDateTime currentDateTime();
void KADUAPI getTime(time_t *sec, int *msec);

#endif
