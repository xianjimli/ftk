
#include "ftk_log.h"
#include "ftk_util.h"
#include "ftk_font_win32.h"

typedef struct _PrivInfo
{
	HDC hdc;
	HFONT hfont;
	HFONT old_hfont;
	int data_size;
	char* buf;
	char* data;
	int size;
	char* filename;
}PrivInfo;

static Ret ftk_font_win32_load(FtkFont* thiz, const char* filename, int bold, int italic, size_t size)
{
	LOGFONT lf;
	DECL_PRIV(thiz, priv);

	ftk_logd("%s: win32 font: %s\n", __func__, filename);

	priv->hdc = CreateCompatibleDC(NULL);
	if (priv->hdc == NULL) {
		ftk_loge("%s: CreateCompatibleDC() failed\n", __func__);
		return RET_FAIL;
	}

	SetMapperFlags(priv->hdc, 0);
	SetMapMode(priv->hdc, MM_TEXT);

	memset(&lf, 0, sizeof(LOGFONT));

	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfOutPrecision = OUT_OUTLINE_PRECIS;
	lf.lfPitchAndFamily = DEFAULT_PITCH|FF_DONTCARE;
	lf.lfQuality = PROOF_QUALITY;
	lf.lfItalic = (BYTE) (italic ? TRUE:FALSE);
	lf.lfWeight = bold ? FW_BOLD:FW_NORMAL;
	lf.lfWidth = size;
	lf.lfHeight = size;
	strcpy(lf.lfFaceName, "ËÎÌו");

	priv->hfont = CreateFontIndirect(&lf);
	if (priv->hfont == NULL) {
		ftk_loge("%s: CreateFontIndirect() failed\n", __func__);
		DeleteDC(priv->hdc);
		return RET_FAIL;
	}

	priv->old_hfont = SelectObject(priv->hdc, priv->hfont);
	if (priv->old_hfont == NULL) {
		ftk_loge("%s: SelectObject() failed\n", __func__);
		DeleteObject(priv->hfont);
		DeleteDC(priv->hdc);
		return RET_FAIL;
	}

	return RET_OK;
}

static int ftk_font_win32_height(FtkFont* thiz)
{
	DECL_PRIV(thiz, priv);
	return priv->size;
}

static Ret ftk_font_win32_lookup (FtkFont* thiz, unsigned short code, FtkGlyph* glyph)
{
	int i, j, k, n;
	char unicode[8], ansi[8];
	unsigned int c;
	MAT2 mat = {{0,1}, {0,0}, {0,0}, {0,1}};
	GLYPHMETRICS gm;
	DECL_PRIV(thiz, priv);

	memset(unicode, 0, sizeof(unicode));
	memcpy(unicode, &code, sizeof(code));
	memset(ansi, 0, sizeof(ansi));

	if (WideCharToMultiByte(CP_ACP, 0, (LPCWSTR) unicode, -1, (LPSTR) ansi, sizeof(ansi), NULL, NULL) == 0) {
		ftk_loge("%s: WideCharToMultiByte() failed\n", __func__);
		return RET_FAIL;
	}

	c = 0;
	if (ansi[1] != '\0') {
		*((char *) &c) = ansi[1];
		*(((char *) &c) + 1) = ansi[0];
	} else {
		*((char *) &c) = ansi[0];
	}

	if (GetGlyphOutline(priv->hdc, c, GGO_BITMAP, &gm, priv->data_size, priv->buf, &mat) == GDI_ERROR) {
		ftk_loge("%s: GetGlyphOutline() failed\n", __func__);
		return RET_FAIL;
	}

#if 0
	ftk_logd("GetGlyphOutline: %s %d %d %d %d %d %d\n", ansi, gm.gmBlackBoxX, gm.gmBlackBoxY, gm.gmptGlyphOrigin.x, gm.gmptGlyphOrigin.y, gm.gmCellIncX, gm.gmCellIncY);
#endif

	n = ((gm.gmBlackBoxX + 31) >> 5) << 2;

	for (i = 0; i < (int) gm.gmBlackBoxY; i++) {
		for (j = 0; j < n; j++) {
			unsigned char c = priv->buf[i * n + j];
			for (k = 0; k < 8; k++) {
				if (c & (0x80 >> k)) {
					priv->data[i * n * 8 + j * 8 + k] = 0xFF;
				} else {
					priv->data[i * n * 8 + j * 8 + k] = 0;
				}
			}
		}
	}

	if (gm.gmCellIncX > priv->size) {
		glyph->x = (gm.gmBlackBoxX - gm.gmCellIncX) - 10;
	} else {
		glyph->x = -(gm.gmBlackBoxX + gm.gmCellIncX);
	}
	glyph->y = (char) gm.gmptGlyphOrigin.y;
	glyph->w = n * 8;
	glyph->h = gm.gmBlackBoxY;
	glyph->data = priv->data;
	glyph->code = code;

	return RET_OK;
}

void ftk_font_win32_destroy(FtkFont* thiz)
{
	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);
		FTK_FREE(priv->filename);
		FTK_FREE(priv->buf);
		FTK_FREE(priv->data);
		SelectObject(priv->hdc, priv->old_hfont);
		DeleteObject(priv->hfont);
		DeleteDC(priv->hdc);
		FTK_ZFREE(thiz, sizeof(FtkFont) + sizeof(PrivInfo));
	}

	return;
}

FtkFont* ftk_font_create(const char* filename, FtkFontDesc* font_desc)
{
	FtkFont* thiz = NULL;
	int size = ftk_font_desc_get_size(font_desc);
	int bold = ftk_font_desc_is_bold(font_desc);
	int italic = ftk_font_desc_is_italic(font_desc);
	return_val_if_fail(filename != NULL, NULL);

	size = 12;

	thiz = FTK_ZALLOC(sizeof(FtkFont) + sizeof(PrivInfo));
	if(thiz != NULL)
	{
		DECL_PRIV(thiz, priv);
		thiz->height = ftk_font_win32_height;
		thiz->lookup = ftk_font_win32_lookup;
		thiz->destroy= ftk_font_win32_destroy;

		if(ftk_font_win32_load(thiz, filename, bold, italic, size) != RET_OK)
		{
			FTK_ZFREE(thiz, sizeof(FtkFont) + sizeof(PrivInfo));
		}
		else
		{
			priv->size = size;
			priv->filename = FTK_STRDUP(filename);
			priv->data_size = 128 * 128;
			priv->buf = FTK_ALLOC(priv->data_size);
			priv->data = FTK_ALLOC(priv->data_size);
		}
	}

	return thiz;
}
