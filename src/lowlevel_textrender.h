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
	double m_gamma;
	QImage* m_offscreen;
};

#endif // LOWLEVEL_TEXTRENDER_H
