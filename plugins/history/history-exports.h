/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#pragma once

#include <QtCore/QtGlobal>

#ifdef HISTORY_EXPORTS
#define HISTORYAPI Q_DECL_EXPORT
#else
#define HISTORYAPI Q_DECL_IMPORT
#endif
