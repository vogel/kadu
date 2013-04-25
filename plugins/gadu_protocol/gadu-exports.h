/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GADU_EXPORTS_H
#define GADU_EXPORTS_H

#include <QtCore/QtGlobal>

#ifdef gadu_protocol_EXPORTS
#define GADUAPI Q_DECL_EXPORT
#else
#define GADUAPI Q_DECL_IMPORT
#endif

#endif // GADU_EXPORTS_H
