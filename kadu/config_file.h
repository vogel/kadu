/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONFIG_FILE_H
#define CONFIG_FILE_H

#include <qvaluelist.h>
#include <qstring.h>

struct KaduConfigParam {
	QString name;
	QString value;
};
	
struct KaduConfigGroup {
	QString name;
	QValueList<KaduConfigParam> params;
};

class KaduConfig {
	public:
		KaduConfig(const QString &filename);
		void sync();
		void setGroup(const QString &name);

	private:
		QValueList<KaduConfigGroup> groups;
		KaduConfigGroup activegroup;
};

#endif
