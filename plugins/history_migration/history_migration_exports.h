/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _HISTORY_MIGRATION_EXPORTS_H

#include "exports.h"

#ifdef KADULIB
#define HISTORY_MIGRATION_API KADU_EXPORT
#else
#ifdef HISTORY_MIGRATION_BUILD
#define HISTORY_MIGRATION_API KADU_EXPORT
#else
#define HISTORY_MIGRATION_API KADU_IMPORT
#endif
#endif // KADULIB

#define _HISTORY_MIGRATION_EXPORTS_H
#endif
