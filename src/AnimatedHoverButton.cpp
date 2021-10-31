/**
MIT License

Copyright (c) 2018 Michael Scopchanov

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "AnimatedHoverButton.h"
#include <QVariantAnimation>
#include <QStyleOptionButton>
#include <QStylePainter>
#include <QEvent>

AnimatedHoverButton::AnimatedHoverButton(const QString &text, QWidget *parent) :
	QPushButton(text, parent),
	m_transition(nullptr),
	m_duration(1000),
	m_currentColor(Qt::transparent)
{

}

void AnimatedHoverButton::setTransitionDuration(int duration)
{
	m_duration = duration;
}

bool AnimatedHoverButton::event(QEvent *event)
{
	switch (event->type()) {
		case QEvent::HoverEnter:
			animateHover(true);
			break;
		case QEvent::HoverLeave:
			animateHover(false);
			break;
		default:
			break;
	}

	return QPushButton::event(event);
}

void AnimatedHoverButton::paintEvent(QPaintEvent */*event*/)
{
	QStylePainter painter(this);
	QStyleOptionButton option;

	initStyleOption(&option);

	option.state &= ~QStyle::State_MouseOver;

	painter.drawControl(QStyle::CE_PushButton, option);
	painter.setOpacity(0.25);
	painter.fillRect(rect(), m_currentColor);
}

void AnimatedHoverButton::animateHover(bool in)
{
	if (m_transition)
		m_transition->stop();

	m_transition = new QVariantAnimation(this);
	m_transition->setDuration(m_duration);
	m_transition->setStartValue(m_currentColor);
	m_transition->setEndValue(in ? palette().highlight().color()
								 : Qt::transparent);

	connect(m_transition, &QVariantAnimation::valueChanged,
			this, [this](const QVariant &value){
		m_currentColor = value.value<QColor>();
		repaint();
	});

	connect(m_transition, &QVariantAnimation::destroyed,
			this, [this](){
		m_transition = nullptr;
		repaint();
	});

	m_transition->start(QAbstractAnimation::DeleteWhenStopped);
}
