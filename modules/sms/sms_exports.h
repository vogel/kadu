/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _SMS_EXPORTS_H

#include "exports.h"

#ifdef KADULIB
#define SMSAPI KADU_EXPORT
#else
#ifdef SMS_BUILD
#define SMSAPI KADU_EXPORT
#else
#define SMSAPI KADU_IMPORT
#endif
#endif // KADULIB

#else
#define _SMS_EXPORTS_H
#endif
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _SMS_EXPORTS_H

#include "exports.h"

#ifdef KADULIB
#define SMSAPI KADU_EXPORT
#else
#ifdef SMS_BUILD
#define SMSAPI KADU_EXPORT
#else
#define SMSAPI KADU_IMPORT
#endif
#endif // KADULIB

#else
#define _SMS_EXPORTS_H
#endif
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _SMS_EXPORTS_H

#include "exports.h"

#ifdef KADULIB
#define SMSAPI KADU_EXPORT
#else
#ifdef SMS_BUILD
#define SMSAPI KADU_EXPORT
#else
#define SMSAPI KADU_IMPORT
#endif
#endif // KADULIB

#else
#define _SMS_EXPORTS_H
#endif
