/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _SOUND_EXPORTS_H

#include "exports.h"
#ifdef KADULIB
#define SOUNDAPI KADU_EXPORT
#else
#ifdef SOUND_BUILD
#define SOUNDAPI KADU_EXPORT
#else
#define SOUNDAPI KADU_IMPORT
#endif
#endif

#define _SOUND_EXPORTS_H
#endif
