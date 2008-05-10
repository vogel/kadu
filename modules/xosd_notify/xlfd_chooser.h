#ifndef XLFD_CHOOSER_H
#define XLFD_CHOOSER_H

#include <QObject>
#include <QProcess>
#include <QString>
#include <QList>

class XLFDChooser : public QObject
{
	Q_OBJECT
	private:
		struct SearchPosition
		{
			QProcess *proc;
			QString initial;
			QString pattern;
			QObject *receiver;
			char *slot;
			SearchPosition();
		};
		QList<SearchPosition> queries;
	signals:
		void fontSelected(const QString &);
	private slots:
		void processExited();

	public:
		XLFDChooser(QObject *parent=0, const char *name=0);
		virtual ~XLFDChooser();
		
		void getFont(QObject *receiver, char *slot, const QString &initial, const QString &pattern=QString::null);
};

#endif
