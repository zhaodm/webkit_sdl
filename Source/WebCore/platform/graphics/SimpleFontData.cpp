/*
 * Copyright (C) 2005, 2008, 2010, 2015 Apple Inc. All rights reserved.
 * Copyright (C) 2006 Alexey Proskuryakov
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer. 
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution. 
 * 3.  Neither the name of Apple Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "SimpleFontData.h"

#if PLATFORM(IOS) || (PLATFORM(MAC) && __MAC_OS_X_VERSION_MIN_REQUIRED > 1080)
#include "CoreTextSPI.h"
#endif
#include "Font.h"
#include "FontCache.h"
#include "OpenTypeMathData.h"
#include <wtf/MathExtras.h>
#include <wtf/NeverDestroyed.h>

#if ENABLE(OPENTYPE_VERTICAL)
#include "OpenTypeVerticalData.h"
#endif

namespace WebCore {

unsigned GlyphPage::s_count = 0;

const float smallCapsFontSizeMultiplier = 0.7f;
const float emphasisMarkFontSizeMultiplier = 0.5f;

SimpleFontData::SimpleFontData(const FontPlatformData& platformData, bool isCustomFont, bool isLoading, bool isTextOrientationFallback)
    : m_maxCharWidth(-1)
    , m_avgCharWidth(-1)
    , m_platformData(platformData)
    , m_treatAsFixedPitch(false)
    , m_isCustomFont(isCustomFont)
    , m_isLoading(isLoading)
    , m_isTextOrientationFallback(isTextOrientationFallback)
    , m_isBrokenIdeographFallback(false)
    , m_mathData(nullptr)
#if ENABLE(OPENTYPE_VERTICAL)
    , m_verticalData(0)
#endif
    , m_hasVerticalGlyphs(false)
{
    platformInit();
    platformGlyphInit();
    platformCharWidthInit();
#if ENABLE(OPENTYPE_VERTICAL)
    if (platformData.orientation() == Vertical && !isTextOrientationFallback) {
        m_verticalData = platformData.verticalData();
        m_hasVerticalGlyphs = m_verticalData.get() && m_verticalData->hasVerticalMetrics();
    }
#endif
}

SimpleFontData::SimpleFontData(std::unique_ptr<AdditionalFontData> fontData, float fontSize, bool syntheticBold, bool syntheticItalic)
    : m_platformData(FontPlatformData(fontSize, syntheticBold, syntheticItalic))
    , m_fontData(WTF::move(fontData))
    , m_treatAsFixedPitch(false)
    , m_isCustomFont(true)
    , m_isLoading(false)
    , m_isTextOrientationFallback(false)
    , m_isBrokenIdeographFallback(false)
    , m_mathData(nullptr)
#if ENABLE(OPENTYPE_VERTICAL)
    , m_verticalData(0)
#endif
    , m_hasVerticalGlyphs(false)
#if PLATFORM(IOS)
    , m_shouldNotBeUsedForArabic(false)
#endif
{
    m_fontData->initializeFontData(this, fontSize);
}

// Estimates of avgCharWidth and maxCharWidth for platforms that don't support accessing these values from the font.
void SimpleFontData::initCharWidths()
{
    auto* glyphPageZero = glyphPage(0);

    // Treat the width of a '0' as the avgCharWidth.
    if (m_avgCharWidth <= 0.f && glyphPageZero) {
        static const UChar32 digitZeroChar = '0';
        Glyph digitZeroGlyph = glyphPageZero->glyphDataForCharacter(digitZeroChar).glyph;
        if (digitZeroGlyph)
            m_avgCharWidth = widthForGlyph(digitZeroGlyph);
    }

    // If we can't retrieve the width of a '0', fall back to the x height.
    if (m_avgCharWidth <= 0.f)
        m_avgCharWidth = m_fontMetrics.xHeight();

    if (m_maxCharWidth <= 0.f)
        m_maxCharWidth = std::max(m_avgCharWidth, m_fontMetrics.floatAscent());
}

void SimpleFontData::platformGlyphInit()
{
    auto* glyphPageZero = glyphPage(0);
    if (!glyphPageZero) {
        m_spaceGlyph = 0;
        m_spaceWidth = 0;
        m_zeroGlyph = 0;
        m_adjustedSpaceWidth = 0;
        determinePitch();
        m_zeroWidthSpaceGlyph = 0;
        return;
    }

    // Ask for the glyph for 0 to avoid paging in ZERO WIDTH SPACE. Control characters, including 0,
    // are mapped to the ZERO WIDTH SPACE glyph.
    m_zeroWidthSpaceGlyph = glyphPageZero->glyphDataForCharacter(0).glyph;

    // Nasty hack to determine if we should round or ceil space widths.
    // If the font is monospace or fake monospace we ceil to ensure that 
    // every character and the space are the same width.  Otherwise we round.
    m_spaceGlyph = glyphPageZero->glyphDataForCharacter(' ').glyph;
    float width = widthForGlyph(m_spaceGlyph);
    m_spaceWidth = width;
    m_zeroGlyph = glyphPageZero->glyphDataForCharacter('0').glyph;
    m_fontMetrics.setZeroWidth(widthForGlyph(m_zeroGlyph));
    determinePitch();
    m_adjustedSpaceWidth = m_treatAsFixedPitch ? ceilf(width) : roundf(width);

    // Force the glyph for ZERO WIDTH SPACE to have zero width, unless it is shared with SPACE.
    // Helvetica is an example of a non-zero width ZERO WIDTH SPACE glyph.
    // See <http://bugs.webkit.org/show_bug.cgi?id=13178> and SimpleFontData::isZeroWidthSpaceGlyph()
    if (m_zeroWidthSpaceGlyph == m_spaceGlyph)
        m_zeroWidthSpaceGlyph = 0;
}

SimpleFontData::~SimpleFontData()
{
    if (!m_fontData)
        platformDestroy();
}

const SimpleFontData* SimpleFontData::simpleFontDataForCharacter(UChar32) const
{
    return this;
}

const SimpleFontData& SimpleFontData::simpleFontDataForFirstRange() const
{
    return *this;
}

static bool fillGlyphPage(GlyphPage& pageToFill, unsigned offset, unsigned length, UChar* buffer, unsigned bufferLength, const SimpleFontData* fontData)
{
#if ENABLE(SVG_FONTS)
    if (SimpleFontData::AdditionalFontData* additionalFontData = fontData->fontData())
        return additionalFontData->fillSVGGlyphPage(&pageToFill, offset, length, buffer, bufferLength, fontData);
#endif
    bool hasGlyphs = pageToFill.fill(offset, length, buffer, bufferLength, fontData);
#if ENABLE(OPENTYPE_VERTICAL)
    if (hasGlyphs && fontData->verticalData())
        fontData->verticalData()->substituteWithVerticalGlyphs(fontData, &pageToFill, offset, length);
#endif
    return hasGlyphs;
}

static RefPtr<GlyphPage> createAndFillGlyphPage(unsigned pageNumber, const SimpleFontData* fontData)
{
#if PLATFORM(IOS)
    // FIXME: Times New Roman contains Arabic glyphs, but Core Text doesn't know how to shape them. See <rdar://problem/9823975>.
    // Once we have the fix for <rdar://problem/9823975> then remove this code together with SimpleFontData::shouldNotBeUsedForArabic()
    // in <rdar://problem/12096835>.
    if (pageNumber == 6 && fontData->shouldNotBeUsedForArabic())
        return nullptr;
#endif

    unsigned start = pageNumber * GlyphPage::size;
    UChar buffer[GlyphPage::size * 2 + 2];
    unsigned bufferLength;
    // Fill in a buffer with the entire "page" of characters that we want to look up glyphs for.
    if (start < 0x10000) {
        bufferLength = GlyphPage::size;
        for (unsigned i = 0; i < GlyphPage::size; i++)
            buffer[i] = start + i;

        if (start == 0) {
            // Control characters must not render at all.
            for (unsigned i = 0; i < 0x20; ++i)
                buffer[i] = zeroWidthSpace;
            for (unsigned i = 0x7F; i < 0xA0; i++)
                buffer[i] = zeroWidthSpace;
            buffer[softHyphen] = zeroWidthSpace;

            // \n, \t, and nonbreaking space must render as a space.
            buffer[(int)'\n'] = ' ';
            buffer[(int)'\t'] = ' ';
            buffer[noBreakSpace] = ' ';
        } else if (start == (leftToRightMark & ~(GlyphPage::size - 1))) {
            // LRM, RLM, LRE, RLE, ZWNJ, ZWJ, and PDF must not render at all.
            buffer[leftToRightMark - start] = zeroWidthSpace;
            buffer[rightToLeftMark - start] = zeroWidthSpace;
            buffer[leftToRightEmbed - start] = zeroWidthSpace;
            buffer[rightToLeftEmbed - start] = zeroWidthSpace;
            buffer[leftToRightOverride - start] = zeroWidthSpace;
            buffer[rightToLeftOverride - start] = zeroWidthSpace;
            buffer[zeroWidthNonJoiner - start] = zeroWidthSpace;
            buffer[zeroWidthJoiner - start] = zeroWidthSpace;
            buffer[popDirectionalFormatting - start] = zeroWidthSpace;
        } else if (start == (objectReplacementCharacter & ~(GlyphPage::size - 1))) {
            // Object replacement character must not render at all.
            buffer[objectReplacementCharacter - start] = zeroWidthSpace;
        } else if (start == (zeroWidthNoBreakSpace & ~(GlyphPage::size - 1))) {
            // ZWNBS/BOM must not render at all.
            buffer[zeroWidthNoBreakSpace - start] = zeroWidthSpace;
        }
    } else {
        bufferLength = GlyphPage::size * 2;
        for (unsigned i = 0; i < GlyphPage::size; i++) {
            int c = i + start;
            buffer[i * 2] = U16_LEAD(c);
            buffer[i * 2 + 1] = U16_TRAIL(c);
        }
    }

    // Now that we have a buffer full of characters, we want to get back an array
    // of glyph indices. This part involves calling into the platform-specific
    // routine of our glyph map for actually filling in the page with the glyphs.
    // Success is not guaranteed. For example, Times fails to fill page 260, giving glyph data
    // for only 128 out of 256 characters.
    RefPtr<GlyphPage> glyphPage;
    if (GlyphPage::mayUseMixedFontDataWhenFilling(buffer, bufferLength, fontData))
        glyphPage = GlyphPage::createForMixedFontData();
    else
        glyphPage = GlyphPage::createForSingleFontData(fontData);

    bool haveGlyphs = fillGlyphPage(*glyphPage, 0, GlyphPage::size, buffer, bufferLength, fontData);
    if (!haveGlyphs)
        return nullptr;

    glyphPage->setImmutable();
    return glyphPage;
}

const GlyphPage* SimpleFontData::glyphPage(unsigned pageNumber) const
{
    if (pageNumber == 0) {
        if (!m_glyphPageZero)
            m_glyphPageZero = createAndFillGlyphPage(0, this);
        return m_glyphPageZero.get();
    }
    auto addResult = m_glyphPages.add(pageNumber, nullptr);
    if (addResult.isNewEntry)
        addResult.iterator->value = createAndFillGlyphPage(pageNumber, this);

    return addResult.iterator->value.get();
}

Glyph SimpleFontData::glyphForCharacter(UChar32 character) const
{
    auto* page = glyphPage(character / GlyphPage::size);
    if (!page)
        return 0;
    return page->glyphAt(character % GlyphPage::size);
}

GlyphData SimpleFontData::glyphDataForCharacter(UChar32 character) const
{
    auto* page = glyphPage(character / GlyphPage::size);
    if (!page)
        return GlyphData();
    return page->glyphDataForCharacter(character);
}

bool SimpleFontData::isSegmented() const
{
    return false;
}

PassRefPtr<SimpleFontData> SimpleFontData::verticalRightOrientationFontData() const
{
    if (!m_derivedFontData)
        m_derivedFontData = std::make_unique<DerivedFontData>(isCustomFont());
    if (!m_derivedFontData->verticalRightOrientation) {
        FontPlatformData verticalRightPlatformData(m_platformData);
        verticalRightPlatformData.setOrientation(Horizontal);
        m_derivedFontData->verticalRightOrientation = create(verticalRightPlatformData, isCustomFont(), false, true);
    }
    return m_derivedFontData->verticalRightOrientation;
}

PassRefPtr<SimpleFontData> SimpleFontData::uprightOrientationFontData() const
{
    if (!m_derivedFontData)
        m_derivedFontData = std::make_unique<DerivedFontData>(isCustomFont());
    if (!m_derivedFontData->uprightOrientation)
        m_derivedFontData->uprightOrientation = create(m_platformData, isCustomFont(), false, true);
    return m_derivedFontData->uprightOrientation;
}

PassRefPtr<SimpleFontData> SimpleFontData::smallCapsFontData(const FontDescription& fontDescription) const
{
    if (!m_derivedFontData)
        m_derivedFontData = std::make_unique<DerivedFontData>(isCustomFont());
    if (!m_derivedFontData->smallCaps)
        m_derivedFontData->smallCaps = createScaledFontData(fontDescription, smallCapsFontSizeMultiplier);

    return m_derivedFontData->smallCaps;
}

PassRefPtr<SimpleFontData> SimpleFontData::emphasisMarkFontData(const FontDescription& fontDescription) const
{
    if (!m_derivedFontData)
        m_derivedFontData = std::make_unique<DerivedFontData>(isCustomFont());
    if (!m_derivedFontData->emphasisMark)
        m_derivedFontData->emphasisMark = createScaledFontData(fontDescription, emphasisMarkFontSizeMultiplier);

    return m_derivedFontData->emphasisMark;
}

PassRefPtr<SimpleFontData> SimpleFontData::brokenIdeographFontData() const
{
    if (!m_derivedFontData)
        m_derivedFontData = std::make_unique<DerivedFontData>(isCustomFont());
    if (!m_derivedFontData->brokenIdeograph) {
        m_derivedFontData->brokenIdeograph = create(m_platformData, isCustomFont(), false);
        m_derivedFontData->brokenIdeograph->m_isBrokenIdeographFallback = true;
    }
    return m_derivedFontData->brokenIdeograph;
}

PassRefPtr<SimpleFontData> SimpleFontData::nonSyntheticItalicFontData() const
{
    if (!m_derivedFontData)
        m_derivedFontData = std::make_unique<DerivedFontData>(isCustomFont());
    if (!m_derivedFontData->nonSyntheticItalic) {
        FontPlatformData nonSyntheticItalicFontPlatformData(m_platformData);
#if PLATFORM(COCOA)
        nonSyntheticItalicFontPlatformData.m_syntheticOblique = false;
#endif
        m_derivedFontData->nonSyntheticItalic = create(nonSyntheticItalicFontPlatformData, isCustomFont());
    }
    return m_derivedFontData->nonSyntheticItalic;
}

#ifndef NDEBUG
String SimpleFontData::description() const
{
    if (isSVGFont())
        return "[SVG font]";
    if (isCustomFont())
        return "[custom font]";

    return platformData().description();
}
#endif

const OpenTypeMathData* SimpleFontData::mathData() const
{
    if (m_isLoading)
        return nullptr;
    if (!m_mathData) {
        m_mathData = OpenTypeMathData::create(m_platformData);
        if (!m_mathData->hasMathData())
            m_mathData.clear();
    }
    return m_mathData.get();
}

SimpleFontData::DerivedFontData::~DerivedFontData()
{
#if PLATFORM(COCOA)
    if (compositeFontReferences) {
        // FIXME: Why don't we use WebKit types here?
        CFDictionaryRef dictionary = CFDictionaryRef(compositeFontReferences.get());
        CFIndex count = CFDictionaryGetCount(dictionary);
        if (count > 0) {
            Vector<SimpleFontData*, 2> stash(count);
            SimpleFontData** fonts = stash.data();
            CFDictionaryGetKeysAndValues(dictionary, 0, (const void **)fonts);
            // This deletes the fonts.
            while (count-- > 0 && *fonts)
                adoptRef(*fonts++);
        }
    }
#endif
}

PassRefPtr<SimpleFontData> SimpleFontData::createScaledFontData(const FontDescription& fontDescription, float scaleFactor) const
{
    // FIXME: Support scaled fonts that used AdditionalFontData.
    if (m_fontData)
        return 0;

    return platformCreateScaledFontData(fontDescription, scaleFactor);
}

bool SimpleFontData::applyTransforms(GlyphBufferGlyph* glyphs, GlyphBufferAdvance* advances, size_t glyphCount, TypesettingFeatures typesettingFeatures) const
{
    // We need to handle transforms on SVG fonts internally, since they are rendered internally.
    ASSERT(!isSVGFont());
#if PLATFORM(IOS) || (PLATFORM(MAC) && __MAC_OS_X_VERSION_MIN_REQUIRED > 1080)
    CTFontTransformOptions options = (typesettingFeatures & Kerning ? kCTFontTransformApplyPositioning : 0) | (typesettingFeatures & Ligatures ? kCTFontTransformApplyShaping : 0);
    return CTFontTransformGlyphs(m_platformData.ctFont(), glyphs, reinterpret_cast<CGSize*>(advances), glyphCount, options);
#else
    UNUSED_PARAM(glyphs);
    UNUSED_PARAM(advances);
    UNUSED_PARAM(glyphCount);
    UNUSED_PARAM(typesettingFeatures);
    return false;
#endif
}

RefPtr<SimpleFontData> SimpleFontData::systemFallbackFontDataForCharacter(UChar32 c, const FontDescription& description, bool isForPlatformFont) const
{
    UChar codeUnits[2];
    int codeUnitsLength;
    if (c <= 0xFFFF) {
        codeUnits[0] = Font::normalizeSpaces(c);
        codeUnitsLength = 1;
    } else {
        codeUnits[0] = U16_LEAD(c);
        codeUnits[1] = U16_TRAIL(c);
        codeUnitsLength = 2;
    }

    return fontCache().systemFallbackForCharacters(description, this, isForPlatformFont, codeUnits, codeUnitsLength);
}

} // namespace WebCore
