/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _GADU_EXPORTS_H

#include "exports.h"

#ifdef KADULIB
#define GADUAPI KADU_EXPORT
#else
#ifdef GADU_BUILD
#define GADUAPI KADU_EXPORT
#else
#define GADUAPI KADU_IMPORT
#endif
#endif // KADULIB

#define _GADU_EXPORTS_H
#endif
