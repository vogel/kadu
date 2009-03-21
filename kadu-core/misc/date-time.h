/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef DATE_TIME
#define DATE_TIME

#include <time.h>

#include "exports.h"

class QDateTime;
class QString;

QString printDateTime(const QDateTime &datetime);
QString timestamp(time_t = 0);
QDateTime currentDateTime();
void KADUAPI getTime(time_t *sec, int *msec);

#endif // DATE_TIME
