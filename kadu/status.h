#ifndef STATUS_H
#define STATUS_H

#include <qstring.h>
#include <qstringlist.h>

#define GG_STATUS_INVISIBLE2 0x0009 /* g³upy... */

extern QString gg_icons[];
extern int gg_statuses[];
extern const char *statustext[];
extern QStringList defaultdescriptions;
bool ifStatusWithDescription(int status);
bool isAvailableStatus(unsigned int);
int statusGGToStatusNr(int);

#endif
