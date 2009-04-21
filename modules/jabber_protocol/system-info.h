#ifndef SYSTEMINFO_H
#define SYSTEMINFO_H

#include <QtCore/QObject>
#include <QtCore/QString>

class SystemInfo : public QObject
{
	Q_OBJECT
public:
	static SystemInfo* instance();
	const QString& os() const { return os_str_; }
	int timezoneOffset() const { return timezone_offset_; }
	const QString& timezoneString() const { return timezone_str_; }
	const QString localHostName();

private:
	SystemInfo();

	static SystemInfo* instance_;
	int timezone_offset_;
	QString timezone_str_;
	QString os_str_;
};

#endif
