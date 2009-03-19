#include "image-dialog.h"

ImageDialog::ImageDialog(QWidget *parent)
	: QFileDialog(parent)
{
// 	PixmapPreview* pp = new PixmapPreview();
	setFilter(qApp->translate("ImageDialog", "Images")+" (*.png *.PNG *.jpg *.JPG *.jpeg *.JPEG *.gif *.GIF *.bmp *.BMP)");
// 	setContentsPreviewEnabled(true);
// 	setContentsPreview(pp, pp);
// 	setPreviewMode(Q3FileDialog::Contents);
}