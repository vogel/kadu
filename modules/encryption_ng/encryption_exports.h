/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _ENCRYPTION_EXPORTS_H

#include "exports.h"

#ifdef KADULIB
#define ENCRYPTIONAPI KADU_EXPORT
#else
#ifdef ENCRYPTION_BUILD
#define ENCRYPTIONAPI KADU_EXPORT
#else
#define ENCRYPTIONAPI KADU_IMPORT
#endif
#endif // KADULIB

#define _ENCRYPTION_EXPORTS_H
#endif
