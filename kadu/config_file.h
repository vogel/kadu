#ifndef CONFIG_FILE_H
#define CONFIG_FILE_H

#include <qvaluelist.h>
#include <qstring.h>
#include <qrect.h>
#include <qsize.h>
#include <qcolor.h>
#include <qfont.h>


struct ConfigFileEntry {
	QString name;
	QString value;
};
	
struct ConfigFileGroup {
	QString name;
	QValueList<ConfigFileEntry> entries;
};

/**
	Klasa reprezentuj±ca plik z zapisem konfiguracji programu
**/
class ConfigFile {

	private:
		void read();
		void write();
		bool changeEntry(const QString &name, const QString &value);
		QString getEntry(const QString &name, bool *ok = 0) const;

		QString filename;
		QValueList<ConfigFileGroup> groups;
		struct ConfigFileGroup *activegroup;
		void setGroup(const QString &name);


	public:
		ConfigFile(const QString &filename);
		void sync();
		
		QValueList<ConfigFileEntry> getGroupSection(const QString& name);

		void writeEntry(const QString &group,const QString &name, const QString &value);
		void writeEntry(const QString &group,const QString &name, const char *value);
		void writeEntry(const QString &group,const QString &name, const int value);
		void writeEntry(const QString &group,const QString &name, const double value);
		void writeEntry(const QString &group,const QString &name, const bool value);
		void writeEntry(const QString &group,const QString &name, const QRect &value);
		void writeEntry(const QString &group,const QString &name, const QSize &value);
		void writeEntry(const QString &group,const QString &name, const QColor &value);
		void writeEntry(const QString &group,const QString &name, const QFont &value);
		void writeEntry(const QString &group,const QString &name, const QPoint &value);

		QString readEntry(const QString &group,const QString &name, const QString &def = QString::null);
		int readNumEntry(const QString &group,const QString &name, int def = 0);
		double readDoubleNumEntry(const QString &group,const QString &name, double def = 0.0);
		bool readBoolEntry(const QString &group,const QString &name, bool def = false);
		QRect readRectEntry(const QString &group,const QString &name, const QRect *def = 0L);
		QSize readSizeEntry(const QString &group,const QString &name, const QSize *def = 0L);
		QColor readColorEntry(const QString &group,const QString &name, const QColor *def = 0L);
		QFont readFontEntry(const QString &group,const QString &name, const QFont *def = 0L);
		QPoint readPointEntry(const QString &group,const QString &name, const QPoint *def = 0L);
		
		void addVariable(const QString &group,const QString &name, const QString &value);
		void addVariable(const QString &group,const QString &name, const char *value);
		void addVariable(const QString &group,const QString &name, const int value);
		void addVariable(const QString &group,const QString &name, const double value);
		void addVariable(const QString &group,const QString &name, const bool value);
		void addVariable(const QString &group,const QString &name, const QRect &value);
		void addVariable(const QString &group,const QString &name, const QSize &value);
		void addVariable(const QString &group,const QString &name, const QColor &value);
		void addVariable(const QString &group,const QString &name, const QFont &value);
		void addVariable(const QString &group,const QString &name, const QPoint &value);


};

extern ConfigFile config_file;

#endif
