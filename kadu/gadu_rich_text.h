#ifndef GADU_RICH_TEXT_H
#define GADU_RICH_TEXT_H

#include <qstring.h>
#include "gadu.h"

QString formatGGMessage(const QString &msg, int formats_length, void *formats, UinType sender);
QString unformatGGMessage(const QString &msg, int &formats_length, void *&formats);

#endif
