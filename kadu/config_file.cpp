#include <qstring.h>
#include <qvaluelist.h>

#include "config_file.h"

ConfigFile::ConfigFile(const QString &filaname) {

}

void ConfigFile::sync() {
}

void ConfigFile::setGroup(const QString &name) {
	struct KaduConfigGroup newgroup;
	int i;

	for (i = 0; i < groups.count(); i++)
		if (name == groups[i].name)
			break;
	if (i == groups.count()) {
		newgroup.name = name;
		groups.append(newgroup);
		for (i = 0; i < groups.count(); i++)
			if (name == groups[i].name)
				break;
		}
	activegroup = &(groups[i]);
}
