/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _MEDIAPLAYER_EXPORTS_H

#include "exports.h"

#ifdef KADULIB
#define MEDIAPLAYERAPI KADU_EXPORT
#else
#ifdef MEDIAPLAYER_BUILD
#define MEDIAPLAYERAPI KADU_EXPORT
#else
#define MEDIAPLAYERAPI KADU_IMPORT
#endif
#endif // KADULIB

#define _MEDIAPLAYER_EXPORTS_H
#endif
