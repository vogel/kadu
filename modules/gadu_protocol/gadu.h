/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GADU_H
#define GADU_H

#include <QtGui/QPixmap>
#include <QtNetwork/QHostAddress>

#include <libgadu.h>

#include "protocols/protocol.h"
#include "protocols/status.h"

#include "gadu_exports.h"

typedef uin_t UinType;

class QTimer;

class GaduAccountData;
class GaduContactAccountData;
class GaduProtocolSocketNotifiers;

struct SearchRecord;
struct SearchResult;
typedef class QList<SearchResult> SearchResults;

#endif
