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

struct ConfigFileParam {
	QString name;
	QString value;
};
	
struct ConfigFileGroup {
	QString name;
	QValueList<ConfigFileParam> params;
};

class ConfigFile {
	public:
		ConfigFile(const QString &filename);
		void sync();
		void setGroup(const QString &name);

	private:
		QValueList<ConfigFileGroup> groups;
		struct ConfigFileGroup *activegroup;
};

#endif
