#ifndef CONFIG_FILE_H
#define CONFIG_FILE_H

#include <qvaluelist.h>
#include <qstring.h>
#include <qrect.h>
#include <qsize.h>
#include <qcolor.h>
#include <qfont.h>
#include <qmap.h>

/**
	Klasa reprezentuj±ca plik z zapisem konfiguracji programu
**/
class ConfigFile {

	private:
		void read();
		void write(const QString &f=QString::null) const;
		bool changeEntry(const QString &group, const QString &name, const QString &value);
		QString getEntry(const QString &group, const QString &name, bool *ok = 0) const;

		QString filename;
		QMap<QString, QMap<QString, QString> > groups;
		mutable QString activeGroupName;
		mutable QMap<QString, QString> *activeGroup;

	public:
		/**
			otwiera plik filename i go wczytuje
		**/
		ConfigFile(const QString &filename);
		
		/**
			zapisuje na dysk zawarto¶æ konfiguracji
		**/
		void sync() const;
		
		/**
			zapisuje do wybranego pliku kopiê konfiguracji
		**/
		void saveTo(const QString &filename) const;
		
		/**
			zwraca ca³± sekcjê "name"
		**/
		QMap<QString, QString>& getGroupSection(const QString& name);
		
		/**
			zwraca listê sekcji
		**/
		QStringList getGroupList() const;

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

		QString readEntry(const QString &group,const QString &name, const QString &def = QString::null) const;
		int readNumEntry(const QString &group,const QString &name, int def = 0) const;
		unsigned int readUnsignedNumEntry(const QString &group,const QString &name, unsigned int def = 0) const;
		double readDoubleNumEntry(const QString &group,const QString &name, double def = 0.0) const;
		bool readBoolEntry(const QString &group,const QString &name, bool def = false) const;
		QRect readRectEntry(const QString &group,const QString &name, const QRect *def = 0L) const;
		QSize readSizeEntry(const QString &group,const QString &name, const QSize *def = 0L) const;
		QColor readColorEntry(const QString &group,const QString &name, const QColor *def = 0L) const;
		QFont readFontEntry(const QString &group,const QString &name, const QFont *def = 0L) const;
		QPoint readPointEntry(const QString &group,const QString &name, const QPoint *def = 0L) const;
		
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
