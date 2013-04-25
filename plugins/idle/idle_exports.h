/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef IDLE_EXPORTS_H
#define IDLE_EXPORTS_H

#include <QtCore/QtGlobal>

#ifdef idle_EXPORTS
#define IDLEAPI Q_DECL_EXPORT
#else
#define IDLEAPI Q_DECL_IMPORT
#endif

#endif // IDLE_EXPORTS_H
