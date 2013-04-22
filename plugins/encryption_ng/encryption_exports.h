/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ENCRYPTION_EXPORTS_H
#define ENCRYPTION_EXPORTS_H

#include <QtCore/QtGlobal>

#ifdef encryption_ng_EXPORTS
#define ENCRYPTIONAPI Q_DECL_EXPORT
#else
#define ENCRYPTIONAPI Q_DECL_IMPORT
#endif

#endif // ENCRYPTION_EXPORTS_H
