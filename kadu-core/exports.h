/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _EXPORTS_H

#ifdef _WIN32
#define KADU_IMPORT __declspec(dllimport)
#define KADU_EXPORT __declspec(dllexport)
#else
#define KADU_IMPORT
#define KADU_EXPORT
#endif

#ifdef KADULIB
#define KADUAPI KADU_EXPORT
#else
#define KADUAPI KADU_IMPORT
#endif

#else
#define _EXPORTS_H
#endif
