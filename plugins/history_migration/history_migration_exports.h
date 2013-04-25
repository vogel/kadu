/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef HISTORY_MIGRATION_EXPORTS_H
#define HISTORY_MIGRATION_EXPORTS_H

#include <QtCore/QtGlobal>

#ifdef history_migration_EXPORTS
#define HISTORY_MIGRATION_API Q_DECL_EXPORT
#else
#define HISTORY_MIGRATION_API Q_DECL_IMPORT
#endif

#endif // HISTORY_MIGRATION_EXPORTS_H
