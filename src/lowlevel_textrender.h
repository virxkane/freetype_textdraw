/***************************************************************************
 *   Copyright (C) 2018-2019 by Chernov A.A.                               *
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

#ifndef LOWLEVEL_TEXTRENDER_H
#define LOWLEVEL_TEXTRENDER_H

#include <QtWidgets/QWidget>

class QPaintEvent;
class QImage;

class GLowLevelTextRenderPrivate;

class GLowLevelTextRender: public QWidget
{
	Q_OBJECT
public:
	enum AntialiasingMode
	{
		AntialiasNone,
		AntialiasGray,
		AntialiasLCD,
		AntialiasLCD_V
	};
	enum HintingMode
	{
		HintingNone,
		HintingByteCode,
		HintingAuto
	};
public:
	explicit GLowLevelTextRender(QWidget *parent = nullptr);
	virtual ~GLowLevelTextRender();
	bool setFontFace(const QString& fileName);
	bool setFontPointSize(float size, int dpi = 96);
	bool setFontPixelSize(int size);
	AntialiasingMode antialiasingMode() const
	{
		return m_antialiasingMode;
	}
	void setAntialiasingMode(AntialiasingMode mode);
	HintingMode hintingMode() const
	{
		return m_hintingMode;
	}
	void setHintingMode(HintingMode hinting);
	bool isKerning() const
	{
		return m_useKerning;
	}
	void setKerning(bool kerning);
	bool ligaturesAllowed() const
	{
		return m_allowLigatures;
	}
	void setAllowLigatures(bool ligatures);
	QColor textColor() const
	{
		return m_textColor;
	}
	void setTextColor(const QColor& textColor);
	double gammaCorrection() const
	{
		return m_gamma;
	}
	void setGammaCorrection(double gamma);
	QColor backgroundColor() const
	{
		return m_backgroundColor;
	}
	void setBackgroundColor(const QColor& color);
	QStringList getSupportedLanguages();
protected:
	virtual void paintEvent(QPaintEvent *event);
public slots:
	bool renderText(const QString& text);
	bool renderText();
private:
	GLowLevelTextRenderPrivate* m_d;
	float m_fontSize;
	QString m_text;
	QColor m_textColor;
	QColor m_backgroundColor;
	AntialiasingMode m_antialiasingMode;
	HintingMode m_hintingMode;
	bool m_useKerning;
	bool m_allowLigatures;
	double m_gamma;
	QImage* m_offscreen;
};

#endif // LOWLEVEL_TEXTRENDER_H
