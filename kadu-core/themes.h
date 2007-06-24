#ifndef KADU_THEMES_H
#define KADU_THEMES_H

#include <qmap.h>
#include <qobject.h>
#include <qstring.h>
#include <qstringlist.h>

class Themes : public QObject
{
	Q_OBJECT

	QStringList ThemesList;
	QStringList ThemesPaths;
	QStringList additional;
	QString ConfigName, Name, ActualTheme;
	QMap<QString, QString> entries;
	QStringList getSubDirs(const QString& path) const;
	QString fixFileName(const QString& path,const QString& fn) const;

public:
	Themes(const QString& name, const QString& configname, const char *cname = 0);
	QStringList defaultKaduPathsWithThemes() const;
	const QStringList &themes() const;
	const QString &theme() const;
	const QStringList &paths() const;
	const QStringList &additionalPaths() const;
	QString themePath(const QString& theme = QString::null) const;
	QString getThemeEntry(const QString& name) const;
	const QMap<QString, QString> & getEntries() { return entries; }

public slots:
	void setTheme(const QString& theme);
	void setPaths(const QStringList& paths);

signals:
	void themeChanged(const QString& theme);
	void pathsChanged(const QStringList& list);
};

#endif
