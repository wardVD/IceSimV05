#include <QGraphicsScene>
#include <QFileDialog>

#include "I3GLWidget.h"
#include "ScreenshotDialog.h"
#include "ScreenshotDialog.moc"

ScreenshotDialog::ScreenshotDialog( I3GLWidget* gl ) :
	QDialog(),
	gl_( gl ),
	scene_( new QGraphicsScene(this) )
{
	ui.setupUi( this );

	ui.graphicsView->setScene( scene_ );

	QSize maxdim = gl_->maxScreenshotDims();
	ui.widthSpinner->setMaximum( maxdim.width() );
	ui.heightSpinner->setMaximum( maxdim.height() );

	ui.dpiSpinner->setValue( gl->referenceDpi() );

	connect( ui.scaleSpinner, SIGNAL(valueChanged(double)), SLOT(updatePreview()) );
	connect( ui.dpiSpinner, SIGNAL(valueChanged(int)), SLOT(rescale()) );
	connect( ui.widthSpinner, SIGNAL(valueChanged(int)), SLOT(rescale()) );
	connect( ui.heightSpinner, SIGNAL(valueChanged(int)), SLOT(rescale()) );
	connect( ui.dpiSpinner, SIGNAL(valueChanged(int)), SLOT(rescale()) );
	connect( ui.gammaSpinner, SIGNAL(valueChanged(double)), SLOT(rescale()) );
	connect( ui.fitInView, SIGNAL(toggled(bool)), SLOT(changeView(bool)) );

	rescale();
	updatePreview();
}

void ScreenshotDialog::accept(){
	int w = ui.widthSpinner->value();
	int h = ui.heightSpinner->value();
	int dpi = ui.dpiSpinner->value();
	double scale = ui.scaleSpinner->value();
	double gamma = ui.gammaSpinner->value();
	// obey the convention that an unmodified dpi means no dpi setting is intended
	if( dpi == gl_->referenceDpi() )
		dpi = 0;
	QString fileName = QFileDialog::getSaveFileName( this, "Save Screenshot",
	                                                 "ShovelScreen.png",
	                                                 "Images (*.png *.xpm *.jpg)" );
	if( fileName.length() ){
		gl_->screenshot_engine( w, h, fileName.toStdString(), dpi, scale, gamma );
		QDialog::accept();
	}
}

void ScreenshotDialog::changeView( bool fitInView ){
	if( fitInView ){
		ui.graphicsView->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
		ui.graphicsView->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
		ui.graphicsView->fitInView( image_rect_, Qt::KeepAspectRatio );
	}
	else{
		ui.graphicsView->setHorizontalScrollBarPolicy( Qt::ScrollBarAsNeeded );
		ui.graphicsView->setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded );
		ui.graphicsView->resetMatrix();
	}
}

void ScreenshotDialog::rescale(){
	ui.scaleSpinner->setValue( double(ui.dpiSpinner->value()) / gl_->referenceDpi() );
	updatePreview();
}

void ScreenshotDialog::updatePreview(){
	int w = ui.widthSpinner->value();
	int h = ui.heightSpinner->value();
	double scale = ui.scaleSpinner->value();
	QImage img = gl_->screenshotData( w, h, scale );

	scene_->clear();
	scene_->addPixmap( QPixmap::fromImage(img) );
	scene_->setSceneRect( img.rect() );
	image_rect_ = img.rect();
	if( ui.fitInView->isChecked() )
		ui.graphicsView->fitInView( image_rect_, Qt::KeepAspectRatio );

	ui.sizeLabel->setText( QString("Preview size: %1 X %2").arg(w).arg(h) );
}

void ScreenshotDialog::resizeEvent( QResizeEvent* e ){
	QDialog::resizeEvent(e);
	if( ui.fitInView->isChecked() )
		ui.graphicsView->fitInView( image_rect_, Qt::KeepAspectRatio );
}

void ScreenshotDialog::showEvent( QShowEvent* e ){
	QDialog::showEvent(e);
	if( ui.fitInView->isChecked() )
		ui.graphicsView->fitInView( image_rect_, Qt::KeepAspectRatio );
}
