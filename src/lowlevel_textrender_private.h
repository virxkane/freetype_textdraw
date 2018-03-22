#ifndef LOWVEVEL_TEXTRENDER_PRIVATE_H
#define LOWVEVEL_TEXTRENDER_PRIVATE_H

// Freetype
#include <ft2build.h>
#include FT_FREETYPE_H

#ifdef USE_HARFBUZZ
// HarfBuzz
#include <hb.h>
#endif

#include <QtCore/QString>

class GLowLevelTextRenderPrivate
{
	friend class GLowLevelTextRender;
public:
	GLowLevelTextRenderPrivate();
	virtual ~GLowLevelTextRenderPrivate();
	bool initFreetype();
	bool freetypeIsInited()
	{
		return m_ft_library != 0;
	}
	bool setFontFace(const QString& fileName);
	bool setFontPointSize(float size, int dpi = 96);
	bool setFontPixelSize(int size);
private:
	// Freetype fields
	FT_Library m_ft_library;
	FT_Face m_ft_face;
#ifdef USE_HARFBUZZ
	// HarfBuzz fields
	hb_buffer_t* m_hb_buffer;
	hb_font_t* m_hb_font;
#endif
};

#endif // LOWVEVEL_TEXTRENDER_PRIVATE_H
