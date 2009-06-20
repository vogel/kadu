/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef IMAGE_DIALOG_H
#define IMAGE_DIALOG_H

#include <QtGui/QFileDialog>

class ImageDialog : public QFileDialog
{
public:
	ImageDialog(QWidget *parent);

};

#endif // IMAGE_DIALOG_H
