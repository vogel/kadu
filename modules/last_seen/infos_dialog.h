#ifndef INFOS_DIALOG_H
#define INFOS_DIALOG_H

#include <QtGui/QDialog>

#include "infos.h"

class InfosDataFile;

/*!
 * Creates a QListView within itself. It also updates the "last seen" times.
 * \brief Dialog window for "last seen", IP, DNS.
 */
class InfosDialog : public QDialog
{
	Q_OBJECT
	public:
		/*! Default constructor. */
		InfosDialog(const LastSeen &lastSeen, QWidget *parent = 0, Qt::WindowFlags f = 0 );

		/*! Default destructor. */
		~InfosDialog();
};

#endif
