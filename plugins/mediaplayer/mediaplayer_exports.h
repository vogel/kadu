/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MEDIAPLAYER_EXPORTS_H
#define MEDIAPLAYER_EXPORTS_H

#include <QtCore/QtGlobal>

#ifdef mediaplayer_EXPORTS
#define MEDIAPLAYERAPI Q_DECL_EXPORT
#else
#define MEDIAPLAYERAPI Q_DECL_IMPORT
#endif

#endif // MEDIAPLAYER_EXPORTS_H
