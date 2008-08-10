/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _DOCKING_EXPORTS_H

#include "exports.h"

#ifdef KADULIB
#define DOCKINGAPI KADUAPI
#else
#ifdef DOCKING_BUILD
#define DOCKINGAPI KADU_EXPORT
#else
#define DOCKINGAPI KADU_IMPORT
#endif // DOCKINGAPI
#endif // KADULIB

#define _DOCKING_EXPORTS_H
#endif
