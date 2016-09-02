#ifndef I3_SHOVEL_SCREENSHOT_DIALOG_H
#define I3_SHOVEL_SCREENSHOT_DIALOG_H

#include <QDialog>

#include "ui_screenshot.h"

namespace Ui{ class ScreenshotDialog; }
class I3GLWidget;
class QGraphicsScene;

class ScreenshotDialog : public QDialog {

Q_OBJECT;

public:
	ScreenshotDialog( I3GLWidget* gl );

protected:
	void resizeEvent( QResizeEvent* e );
	void showEvent( QShowEvent* e );

protected Q_SLOTS:
	void changeView( bool );

public Q_SLOTS:
	void rescale();
	void updatePreview();
	void accept();

private:
	Ui::ScreenshotDialog ui;
	I3GLWidget* gl_;
	QGraphicsScene* scene_;
	QRect image_rect_;

};

#endif /* I3_SHOVEL_SCREENSHOT_DIALOG_H */
