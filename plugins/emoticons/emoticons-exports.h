/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef EMOTICONS_EXPORTS_H
#define EMOTICONS_EXPORTS_H

#include <QtCore/QtGlobal>

#ifdef emoticons_EXPORTS
#	define EMOTICONSAPI Q_DECL_EXPORT

#	ifdef KADU_EXPORT_TESTS
#		define EMOTICONSAPI_TESTS Q_DECL_EXPORT
#	else
#		define EMOTICONSAPI_TESTS
#	endif
#else
#	define EMOTICONSAPI Q_DECL_IMPORT

#	ifdef KADU_EXPORT_TESTS
#		define EMOTICONSAPI_TESTS Q_DECL_EXPORT
#	else
#		define EMOTICONSAPI_TESTS
#	endif
#endif

#endif // EMOTICONS_EXPORTS_H
