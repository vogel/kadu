/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _QT4DOCKAPI_EXPORTS_H

#include "exports.h"

#ifdef KADULIB
#define QT4DOCKAPI KADU_EXPORT
#else
#ifdef QT4DOCKAPI_BUILD
#define QT4DOCKAPI KADU_EXPORT
#else
#define QT4DOCKAPI KADU_IMPORT
#endif
#endif // KADULIB

#define _QT4DOCKAPI_EXPORTS_H
#endif
