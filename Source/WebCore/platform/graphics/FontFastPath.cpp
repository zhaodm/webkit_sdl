/**
 * Copyright (C) 2003, 2006, 2010 Apple Inc. All rights reserved.
 * Copyright (C) 2008 Holger Hans Peter Freyther
 * Copyright (C) 2009 Torch Mobile, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#include "config.h"
#include "Font.h"

#include "FloatRect.h"
#include "FontCache.h"
#include "FontGlyphs.h"
#include "GlyphBuffer.h"
#include "LayoutRect.h"
#include "SimpleFontData.h"
#include "TextRun.h"
#include "WidthIterator.h"
#include <wtf/MainThread.h>
#include <wtf/MathExtras.h>
#include <wtf/unicode/CharacterNames.h>

using namespace WTF;
using namespace Unicode;

namespace WebCore {

bool Font::primaryFontHasGlyphForCharacter(UChar32 character) const
{
    return primaryFont()->glyphForCharacter(character);
}

// FIXME: This function may not work if the emphasis mark uses a complex script, but none of the
// standard emphasis marks do so.
bool Font::getEmphasisMarkGlyphData(const AtomicString& mark, GlyphData& glyphData) const
{
    if (mark.isEmpty())
        return false;

    UChar32 character = mark[0];

    if (U16_IS_SURROGATE(character)) {
        if (!U16_IS_SURROGATE_LEAD(character))
            return false;

        if (mark.length() < 2)
            return false;

        UChar low = mark[1];
        if (!U16_IS_TRAIL(low))
            return false;

        character = U16_GET_SUPPLEMENTARY(character, low);
    }

    glyphData = glyphDataForCharacter(character, false, EmphasisMarkVariant);
    return true;
}

int Font::emphasisMarkAscent(const AtomicString& mark) const
{
    FontCachePurgePreventer purgePreventer;
    
    GlyphData markGlyphData;
    if (!getEmphasisMarkGlyphData(mark, markGlyphData))
        return 0;

    const SimpleFontData* markFontData = markGlyphData.fontData;
    ASSERT(markFontData);
    if (!markFontData)
        return 0;

    return markFontData->fontMetrics().ascent();
}

int Font::emphasisMarkDescent(const AtomicString& mark) const
{
    FontCachePurgePreventer purgePreventer;
    
    GlyphData markGlyphData;
    if (!getEmphasisMarkGlyphData(mark, markGlyphData))
        return 0;

    const SimpleFontData* markFontData = markGlyphData.fontData;
    ASSERT(markFontData);
    if (!markFontData)
        return 0;

    return markFontData->fontMetrics().descent();
}

int Font::emphasisMarkHeight(const AtomicString& mark) const
{
    FontCachePurgePreventer purgePreventer;

    GlyphData markGlyphData;
    if (!getEmphasisMarkGlyphData(mark, markGlyphData))
        return 0;

    const SimpleFontData* markFontData = markGlyphData.fontData;
    ASSERT(markFontData);
    if (!markFontData)
        return 0;

    return markFontData->fontMetrics().height();
}

float Font::getGlyphsAndAdvancesForSimpleText(const TextRun& run, int from, int to, GlyphBuffer& glyphBuffer, ForTextEmphasisOrNot forTextEmphasis) const
{
    float initialAdvance;

    WidthIterator it(this, run, 0, false, forTextEmphasis);
    // FIXME: Using separate glyph buffers for the prefix and the suffix is incorrect when kerning or
    // ligatures are enabled.
    GlyphBuffer localGlyphBuffer;
    it.advance(from, &localGlyphBuffer);
    float beforeWidth = it.m_runWidthSoFar;
    it.advance(to, &glyphBuffer);

    if (glyphBuffer.isEmpty())
        return 0;

    float afterWidth = it.m_runWidthSoFar;

    if (run.rtl()) {
        float finalRoundingWidth = it.m_finalRoundingWidth;
        it.advance(run.length(), &localGlyphBuffer);
        initialAdvance = finalRoundingWidth + it.m_runWidthSoFar - afterWidth;
    } else
        initialAdvance = beforeWidth;

    if (run.rtl())
        glyphBuffer.reverse(0, glyphBuffer.size());

    return initialAdvance;
}

float Font::drawSimpleText(GraphicsContext* context, const TextRun& run, const FloatPoint& point, int from, int to) const
{
    // This glyph buffer holds our glyphs+advances+font data for each glyph.
    GlyphBuffer glyphBuffer;

    float startX = point.x() + getGlyphsAndAdvancesForSimpleText(run, from, to, glyphBuffer);

    if (glyphBuffer.isEmpty())
        return 0;

    FloatPoint startPoint(startX, point.y());
    drawGlyphBuffer(context, run, glyphBuffer, startPoint);

    return startPoint.x() - startX;
}

void Font::drawEmphasisMarksForSimpleText(GraphicsContext* context, const TextRun& run, const AtomicString& mark, const FloatPoint& point, int from, int to) const
{
    GlyphBuffer glyphBuffer;
    float initialAdvance = getGlyphsAndAdvancesForSimpleText(run, from, to, glyphBuffer, ForTextEmphasis);

    if (glyphBuffer.isEmpty())
        return;

    drawEmphasisMarks(context, run, glyphBuffer, mark, FloatPoint(point.x() + initialAdvance, point.y()));
}

void Font::drawGlyphBuffer(GraphicsContext* context, const TextRun& run, const GlyphBuffer& glyphBuffer, FloatPoint& point) const
{
#if !ENABLE(SVG_FONTS)
    UNUSED_PARAM(run);
#endif

    // Draw each contiguous run of glyphs that use the same font data.
    const SimpleFontData* fontData = glyphBuffer.fontDataAt(0);
    FloatSize offset = glyphBuffer.offsetAt(0);
    FloatPoint startPoint(point.x(), point.y() - glyphBuffer.initialAdvance().height());
    float nextX = startPoint.x() + glyphBuffer.advanceAt(0).width();
    float nextY = startPoint.y() + glyphBuffer.advanceAt(0).height();
    int lastFrom = 0;
    int nextGlyph = 1;
#if ENABLE(SVG_FONTS)
    TextRun::RenderingContext* renderingContext = run.renderingContext();
#endif
    while (nextGlyph < glyphBuffer.size()) {
        const SimpleFontData* nextFontData = glyphBuffer.fontDataAt(nextGlyph);
        FloatSize nextOffset = glyphBuffer.offsetAt(nextGlyph);

        if (nextFontData != fontData || nextOffset != offset) {
#if ENABLE(SVG_FONTS)
            if (renderingContext && fontData->isSVGFont())
                renderingContext->drawSVGGlyphs(context, fontData, glyphBuffer, lastFrom, nextGlyph - lastFrom, startPoint);
            else
#endif
                drawGlyphs(context, fontData, glyphBuffer, lastFrom, nextGlyph - lastFrom, startPoint);

            lastFrom = nextGlyph;
            fontData = nextFontData;
            offset = nextOffset;
            startPoint.setX(nextX);
            startPoint.setY(nextY);
        }
        nextX += glyphBuffer.advanceAt(nextGlyph).width();
        nextY += glyphBuffer.advanceAt(nextGlyph).height();
        nextGlyph++;
    }

#if ENABLE(SVG_FONTS)
    if (renderingContext && fontData->isSVGFont())
        renderingContext->drawSVGGlyphs(context, fontData, glyphBuffer, lastFrom, nextGlyph - lastFrom, startPoint);
    else
#endif
    {
        drawGlyphs(context, fontData, glyphBuffer, lastFrom, nextGlyph - lastFrom, startPoint);
        point.setX(nextX);
    }
}

inline static float offsetToMiddleOfGlyph(const SimpleFontData* fontData, Glyph glyph)
{
    if (fontData->platformData().orientation() == Horizontal) {
        FloatRect bounds = fontData->boundsForGlyph(glyph);
        return bounds.x() + bounds.width() / 2;
    }
    // FIXME: Use glyph bounds once they make sense for vertical fonts.
    return fontData->widthForGlyph(glyph) / 2;
}

inline static float offsetToMiddleOfGlyphAtIndex(const GlyphBuffer& glyphBuffer, size_t i)
{
    return offsetToMiddleOfGlyph(glyphBuffer.fontDataAt(i), glyphBuffer.glyphAt(i));
}

void Font::drawEmphasisMarks(GraphicsContext* context, const TextRun& run, const GlyphBuffer& glyphBuffer, const AtomicString& mark, const FloatPoint& point) const
{
    FontCachePurgePreventer purgePreventer;
    
    GlyphData markGlyphData;
    if (!getEmphasisMarkGlyphData(mark, markGlyphData))
        return;

    const SimpleFontData* markFontData = markGlyphData.fontData;
    ASSERT(markFontData);
    if (!markFontData)
        return;

    Glyph markGlyph = markGlyphData.glyph;
    Glyph spaceGlyph = markFontData->spaceGlyph();

    float middleOfLastGlyph = offsetToMiddleOfGlyphAtIndex(glyphBuffer, 0);
    FloatPoint startPoint(point.x() + middleOfLastGlyph - offsetToMiddleOfGlyph(markFontData, markGlyph), point.y());

    GlyphBuffer markBuffer;
    for (int i = 0; i + 1 < glyphBuffer.size(); ++i) {
        float middleOfNextGlyph = offsetToMiddleOfGlyphAtIndex(glyphBuffer, i + 1);
        float advance = glyphBuffer.advanceAt(i).width() - middleOfLastGlyph + middleOfNextGlyph;
        markBuffer.add(glyphBuffer.glyphAt(i) ? markGlyph : spaceGlyph, markFontData, advance);
        middleOfLastGlyph = middleOfNextGlyph;
    }
    markBuffer.add(glyphBuffer.glyphAt(glyphBuffer.size() - 1) ? markGlyph : spaceGlyph, markFontData, 0);

    drawGlyphBuffer(context, run, markBuffer, startPoint);
}

float Font::floatWidthForSimpleText(const TextRun& run, HashSet<const SimpleFontData*>* fallbackFonts, GlyphOverflow* glyphOverflow) const
{
    WidthIterator it(this, run, fallbackFonts, glyphOverflow);
    GlyphBuffer glyphBuffer;
    it.advance(run.length(), (typesettingFeatures() & (Kerning | Ligatures)) ? &glyphBuffer : 0);

    if (glyphOverflow) {
        glyphOverflow->top = std::max<int>(glyphOverflow->top, ceilf(-it.minGlyphBoundingBoxY()) - (glyphOverflow->computeBounds ? 0 : fontMetrics().ascent()));
        glyphOverflow->bottom = std::max<int>(glyphOverflow->bottom, ceilf(it.maxGlyphBoundingBoxY()) - (glyphOverflow->computeBounds ? 0 : fontMetrics().descent()));
        glyphOverflow->left = ceilf(it.firstGlyphOverflow());
        glyphOverflow->right = ceilf(it.lastGlyphOverflow());
    }

    return it.m_runWidthSoFar;
}

void Font::adjustSelectionRectForSimpleText(const TextRun& run, LayoutRect& selectionRect, int from, int to) const
{
    GlyphBuffer glyphBuffer;
    WidthIterator it(this, run);
    it.advance(from, &glyphBuffer);
    float beforeWidth = it.m_runWidthSoFar;
    it.advance(to, &glyphBuffer);
    float afterWidth = it.m_runWidthSoFar;
    float totalWidth = -1;

    if (run.rtl()) {
        it.advance(run.length(), &glyphBuffer);
        totalWidth = it.m_runWidthSoFar;
        selectionRect.move(totalWidth - afterWidth, 0);
    } else
        selectionRect.move(beforeWidth, 0);
    selectionRect.setWidth(afterWidth - beforeWidth);
}

int Font::offsetForPositionForSimpleText(const TextRun& run, float x, bool includePartialGlyphs) const
{
    float delta = x;

    WidthIterator it(this, run);
    GlyphBuffer localGlyphBuffer;
    unsigned offset;
    if (run.rtl()) {
        delta -= floatWidthForSimpleText(run);
        while (1) {
            offset = it.m_currentCharacter;
            float w;
            if (!it.advanceOneCharacter(w, localGlyphBuffer))
                break;
            delta += w;
            if (includePartialGlyphs) {
                if (delta - w / 2 >= 0)
                    break;
            } else {
                if (delta >= 0)
                    break;
            }
        }
    } else {
        while (1) {
            offset = it.m_currentCharacter;
            float w;
            if (!it.advanceOneCharacter(w, localGlyphBuffer))
                break;
            delta -= w;
            if (includePartialGlyphs) {
                if (delta + w / 2 <= 0)
                    break;
            } else {
                if (delta <= 0)
                    break;
            }
        }
    }

    return offset;
}

}
