#ifndef XLFD_CHOOSER_H
#define XLFD_CHOOSER_H

#include <qobject.h>
#include <qprocess.h>
#include <qstring.h>
#include <qvaluelist.h>

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
		QValueList<SearchPosition> queries;
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
