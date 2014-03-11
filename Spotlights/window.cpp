#include "window.h"

#include <QtGui>

Window::Window(QWidget *parent)
    : QWidget(parent)
{
    glwidget = new GLWidget();
    QLabel* spot_label = new QLabel("Spotlight position: 2, 2, 0");
    QSlider* spotx_slider = new QSlider();
    spotx_slider->setOrientation(Qt::Horizontal);
    spotx_slider->setRange(-3, 3);
    spotx_slider->setSliderPosition(2);
    QSlider* spoty_slider = new QSlider();
    spoty_slider->setOrientation(Qt::Horizontal);
    spoty_slider->setRange(0, 5);
    spoty_slider->setSliderPosition(2);
    QSlider* spotz_slider = new QSlider();
    spotz_slider->setOrientation(Qt::Horizontal);
    spotz_slider->setRange(-4, 4);
    spotz_slider->setSliderPosition(0);

    QLabel* spot_dir_label = new QLabel("Spotlight direction: -1, -1, 0");
    QSlider* spotdirx_slider = new QSlider();
    spotdirx_slider->setOrientation(Qt::Horizontal);
    spotdirx_slider->setRange(-5, 5);
    spotdirx_slider->setSliderPosition(-1);
    QSlider* spotdiry_slider = new QSlider();
    spotdiry_slider->setOrientation(Qt::Horizontal);
    spotdiry_slider->setRange(-5, 5);
    spotdiry_slider->setSliderPosition(-1);
    QSlider* spotdirz_slider = new QSlider();
    spotdirz_slider->setOrientation(Qt::Horizontal);
    spotdirz_slider->setRange(-5, 5);
    spotdirz_slider->setSliderPosition(0);

    QLabel* spot_cutoff_label = new QLabel("Spotlight cutoff: 0.5");
    QSlider* spot_cutoff_slider = new QSlider();
    spot_cutoff_slider->setOrientation(Qt::Horizontal);
    spot_cutoff_slider->setRange(10,20);
    spot_cutoff_slider->setSliderPosition(10);

    QLabel* spot_exp_label = new QLabel("Spotlight exponent: 8");
    QSlider* spot_exp_slider = new QSlider();
    spot_exp_slider->setOrientation(Qt::Horizontal);
    spot_exp_slider->setRange(1,5);
    spot_exp_slider->setSliderPosition(2);

    QLabel* linear_label = new QLabel("Linear attenuation: 0.2");
    QSlider* linear_slider = new QSlider();
    linear_slider->setOrientation(Qt::Horizontal);
    linear_slider->setRange(0,5);
    linear_slider->setSliderPosition(2);
    
    QVBoxLayout* vbox = new QVBoxLayout();
    vbox->addWidget(glwidget);
    vbox->addWidget(spot_label);
    vbox->addWidget(spotx_slider);
    vbox->addWidget(spoty_slider);
    vbox->addWidget(spotz_slider);
    vbox->addWidget(spot_dir_label);
    vbox->addWidget(spotdirx_slider);
    vbox->addWidget(spotdiry_slider);
    vbox->addWidget(spotdirz_slider);
    vbox->addWidget(spot_cutoff_label);
    vbox->addWidget(spot_cutoff_slider);
    vbox->addWidget(spot_exp_label);
    vbox->addWidget(spot_exp_slider);
    vbox->addWidget(linear_label);
    vbox->addWidget(linear_slider);

    connect(spotx_slider, SIGNAL(valueChanged(int)), glwidget, SLOT(spotX(int)));
    connect(spoty_slider, SIGNAL(valueChanged(int)), glwidget, SLOT(spotY(int)));
    connect(spotz_slider, SIGNAL(valueChanged(int)), glwidget, SLOT(spotZ(int)));
    connect(spotdirx_slider, SIGNAL(valueChanged(int)), glwidget, SLOT(spotDirX(int)));
    connect(spotdiry_slider, SIGNAL(valueChanged(int)), glwidget, SLOT(spotDirY(int)));
    connect(spotdirz_slider, SIGNAL(valueChanged(int)), glwidget, SLOT(spotDirZ(int)));
    connect(spot_cutoff_slider, SIGNAL(valueChanged(int)), glwidget, SLOT(spotCutoff(int)));
    connect(spot_exp_slider, SIGNAL(valueChanged(int)), glwidget, SLOT(spotExponent(int)));
    connect(linear_slider, SIGNAL(valueChanged(int)), glwidget, SLOT(linearAttenuation(int)));
    connect(glwidget, SIGNAL(spotPosLabel(QString)), spot_label, SLOT(setText(const QString&)));
    connect(glwidget, SIGNAL(spotDirLabel(QString)), spot_dir_label, SLOT(setText(const QString&)));
    connect(glwidget, SIGNAL(spotCutoffLabel(QString)), spot_cutoff_label, SLOT(setText(const QString&)));
    connect(glwidget, SIGNAL(spotExpLabel(QString)), spot_exp_label, SLOT(setText(const QString&)));
    connect(glwidget, SIGNAL(linearLabel(QString)), linear_label, SLOT(setText(const QString&)));
    setLayout(vbox);
}

void Window::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Escape)
		close();
	else
		QWidget::keyPressEvent(e);
}
