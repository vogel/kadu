/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QT4DOCKAPI_EXPORTS_H
#define QT4DOCKAPI_EXPORTS_H

#include <QtCore/QtGlobal>

#ifdef qt4_docking_EXPORTS
#define QT4DOCKAPI Q_DECL_EXPORT
#else
#define QT4DOCKAPI Q_DECL_IMPORT
#endif

#endif // QT4DOCKAPI_EXPORTS_H
