/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _NOTIFY_EXPORTS_H

#include "exports.h"

#ifdef KADULIB
#define NOTIFYAPI KADUAPI
#else
#ifdef NOTIFY_BUILD
#define NOTIFYAPI KADU_EXPORT
#else
#define NOTIFYAPI KADU_IMPORT
#endif
#endif

#else
#define _NOTIFY_EXPORTS_H
#endif
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _NOTIFY_EXPORTS_H

#include "exports.h"

#ifdef KADULIB
#define NOTIFYAPI KADUAPI
#else
#ifdef NOTIFY_BUILD
#define NOTIFYAPI KADU_EXPORT
#else
#define NOTIFYAPI KADU_IMPORT
#endif
#endif

#else
#define _NOTIFY_EXPORTS_H
#endif
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _NOTIFY_EXPORTS_H

#include "exports.h"

#ifdef KADULIB
#define NOTIFYAPI KADUAPI
#else
#ifdef NOTIFY_BUILD
#define NOTIFYAPI KADU_EXPORT
#else
#define NOTIFYAPI KADU_IMPORT
#endif
#endif

#else
#define _NOTIFY_EXPORTS_H
#endif
