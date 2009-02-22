/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _DCC_EXPORTS_H

#include "exports.h"
#ifdef KADULIB
#define DCCAPI KADU_EXPORT
#else
#ifdef DCC_BUILD
#define DCCAPI KADU_EXPORT
#else
#define DCCAPI KADU_IMPORT
#endif
#endif

#define _DCC_EXPORTS_H
#endif
