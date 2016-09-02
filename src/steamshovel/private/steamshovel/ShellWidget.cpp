#include "ShellWidget.h"
#include <QtGui/QVBoxLayout>

ShellWidget::ShellWidget( QWidget* parent ) :
    QWidget( parent )
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    // Python code later adds a RichWidget from IPython as a child 
}
