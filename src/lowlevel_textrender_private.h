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
#ifdef USE_HARFBUZZ
	bool setKerning(bool kerning);
	bool setLigatures(bool liga);
#endif
private:
	// Freetype fields
	FT_Library m_ft_library;
	FT_Face m_ft_face;
#ifdef USE_HARFBUZZ
	// HarfBuzz fields
	hb_buffer_t* m_hb_buffer;
	hb_font_t* m_hb_font;
	hb_feature_t* m_hb_features;
	int m_hb_features_count;
	int m_hb_feature_kern_pos;
	int m_hb_feature_liga_pos;
#endif
};

#endif // LOWVEVEL_TEXTRENDER_PRIVATE_H
