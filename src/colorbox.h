/***************************************************************************
 *   Copyright (C) 2002-2019 by Chernov A.A.                               *
 *   valexlin@gmail.com                                                    *
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 ***************************************************************************/

#ifndef COLORBOX_H
#define COLORBOX_H

#include <QtWidgets/QFrame>

class QHBoxLayout;
class QToolButton;

class QColorComboBox;

/**
\brief Элемент управления для выбора цвета.

Этот элемент управления позволяет выбирать различные цвета. Он состоит из двух компонентов:
QComboBox и QToolButton. Можно выбрать цвет из списка, или если нужного цвета нет, выбрать
цвет с помощью стандартного диалога выбора цвета, нажатием на кнопку QToolButton, при этом
выбранный цвет будет отображен в конце списка с пометкой "custom". Если пользователь не
выбирает цвет, то используется цвет "по умолчанию", представленый самым первым элементом списка
и задавемый функцией setDefaultColor(QColor& c). Всего в списке находится 16 цветов, с учётом
элементов "default" и "custom" всего в QComboBox 18 элементов.

\author Чернов А.А.
*/

class QColorBox : public QFrame
{
Q_OBJECT
public:
	/**
	 * Конструктор.
	 * Создает элемент управления.
	 * \param parent	родительский виджет, обязательно должен существовать;
	 */
	QColorBox(QWidget *parent);
	/**
	 * Деструктор.
	 * Удаляет все внутренние переменные.
	 */
	~QColorBox();
	/**
	 * Возвращает выбранный цвет.
	 */
	QColor color();
	/**
	 * Устанавливает текущий выбранный цвет.
	 * Если этот цвет c найден в списке QComboBox, то выделяется соответствующий
	 * элемент списка, иначе устанавливается как дополнительный цвет. См. setCustomColor().
	 * \param c		устанавливаемый цвет.
	 */
	void setColor(const QColor& c);
	/**
	 * Устанавливает цвет по умолчанию. Это цвет находится в самом начале списка цветов.
	 * Таким образом, если пользователь не выбирает цвет в списке, то именно этот цвет
	 * возвращется функцией color().
	 * \param c		устанавливаемый цвет.
	 */
	void setDefaultColor(const QColor& c);
signals:
	/**
	 * Этот сигнал вызывается, когда изменяется значение цвета.
	 * Другими словами значения возвращаемые функцией color()
	 * до и после этого сигнала разные.
	 */
	void colorChanged(QColor);
protected:
	/**
	 * Устанавливает дополнительный цвет. Этот цвет позднее может быть изменён
	 * пользователем. По умолчанию QColor(0, 0, 0).
	 * \param c		устанавливаемый цвет.
	 */
	void setCustomColor(const QColor& c);
	/**
	 * Для внутреннего использования. Заполняет список цветов.
	 */
	void fill();
protected slots:
	virtual void languageChange();
	void slotSelColor();
	void slotChanged(int);
private:
	QHBoxLayout* layout;
	QColorComboBox* comboBox;
	QToolButton* tool;
	QColor defaultColor;
	QColor customColor;
};

#endif
