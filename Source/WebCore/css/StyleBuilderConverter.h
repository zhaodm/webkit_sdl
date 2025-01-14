/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
 * Copyright (C) 2014 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef StyleBuilderConverter_h
#define StyleBuilderConverter_h

#include "BasicShapeFunctions.h"
#include "CSSCalculationValue.h"
#include "CSSFunctionValue.h"
#include "CSSGridLineNamesValue.h"
#include "CSSGridTemplateAreasValue.h"
#include "CSSImageGeneratorValue.h"
#include "CSSImageSetValue.h"
#include "CSSImageValue.h"
#include "CSSPrimitiveValue.h"
#include "CSSReflectValue.h"
#include "Length.h"
#include "Pair.h"
#include "QuotesData.h"
#include "Settings.h"
#include "StyleResolver.h"
#include "TransformFunctions.h"

namespace WebCore {

// Note that we assume the CSS parser only allows valid CSSValue types.
class StyleBuilderConverter {
public:
    static Length convertLength(StyleResolver&, CSSValue&);
    static Length convertLengthOrAuto(StyleResolver&, CSSValue&);
    static Length convertLengthSizing(StyleResolver&, CSSValue&);
    static Length convertLengthMaxSizing(StyleResolver&, CSSValue&);
    template <typename T> static T convertComputedLength(StyleResolver&, CSSValue&);
    template <typename T> static T convertLineWidth(StyleResolver&, CSSValue&);
    static float convertSpacing(StyleResolver&, CSSValue&);
    static LengthSize convertRadius(StyleResolver&, CSSValue&);
    static TextDecoration convertTextDecoration(StyleResolver&, CSSValue&);
    template <typename T> static T convertNumber(StyleResolver&, CSSValue&);
    static short convertWebkitHyphenateLimitLines(StyleResolver&, CSSValue&);
    template <CSSPropertyID property> static NinePieceImage convertBorderImage(StyleResolver&, CSSValue&);
    template <CSSPropertyID property> static NinePieceImage convertBorderMask(StyleResolver&, CSSValue&);
    template <CSSPropertyID property> static PassRefPtr<StyleImage> convertStyleImage(StyleResolver&, CSSValue&);
    static TransformOperations convertTransform(StyleResolver&, CSSValue&);
    static String convertString(StyleResolver&, CSSValue&);
    static String convertStringOrAuto(StyleResolver&, CSSValue&);
    static String convertStringOrNone(StyleResolver&, CSSValue&);
    static TextEmphasisPosition convertTextEmphasisPosition(StyleResolver&, CSSValue&);
    static ETextAlign convertTextAlign(StyleResolver&, CSSValue&);
    static PassRefPtr<ClipPathOperation> convertClipPath(StyleResolver&, CSSValue&);
    static EResize convertResize(StyleResolver&, CSSValue&);
    static int convertMarqueeRepetition(StyleResolver&, CSSValue&);
    static int convertMarqueeSpeed(StyleResolver&, CSSValue&);
    static PassRefPtr<QuotesData> convertQuotes(StyleResolver&, CSSValue&);
    static TextUnderlinePosition convertTextUnderlinePosition(StyleResolver&, CSSValue&);
    static PassRefPtr<StyleReflection> convertReflection(StyleResolver&, CSSValue&);
    static IntSize convertInitialLetter(StyleResolver&, CSSValue&);
    static float convertTextStrokeWidth(StyleResolver&, CSSValue&);
    static LineBoxContain convertLineBoxContain(StyleResolver&, CSSValue&);
    static TextDecorationSkip convertTextDecorationSkip(StyleResolver&, CSSValue&);
    static PassRefPtr<ShapeValue> convertShapeValue(StyleResolver&, CSSValue&);
#if ENABLE(CSS_GRID_LAYOUT)
    static bool convertGridTrackSize(StyleResolver&, CSSValue&, GridTrackSize&);
    static bool convertGridPosition(StyleResolver&, CSSValue&, GridPosition&);
    static GridAutoFlow convertGridAutoFlow(StyleResolver&, CSSValue&);
#endif // ENABLE(CSS_GRID_LAYOUT)

private:
    friend class StyleBuilderCustom;

    static Length convertToRadiusLength(CSSToLengthConversionData&, CSSPrimitiveValue&);
    static TextEmphasisPosition valueToEmphasisPosition(CSSPrimitiveValue&);
    static TextDecorationSkip valueToDecorationSkip(const CSSPrimitiveValue&);
#if ENABLE(CSS_GRID_LAYOUT)
    static bool createGridTrackBreadth(CSSPrimitiveValue&, StyleResolver&, GridLength&);
    static bool createGridTrackSize(CSSValue&, GridTrackSize&, StyleResolver&);
    static bool createGridTrackList(CSSValue&, Vector<GridTrackSize>& trackSizes, NamedGridLinesMap&, OrderedNamedGridLinesMap&, StyleResolver&);
    static bool createGridPosition(CSSValue&, GridPosition&);
    static void createImplicitNamedGridLinesFromGridArea(const NamedGridAreaMap&, NamedGridLinesMap&, GridTrackSizingDirection);
#endif // ENABLE(CSS_GRID_LAYOUT)
};

inline Length StyleBuilderConverter::convertLength(StyleResolver& styleResolver, CSSValue& value)
{
    auto& primitiveValue = downcast<CSSPrimitiveValue>(value);
    CSSToLengthConversionData conversionData = styleResolver.useSVGZoomRulesForLength() ?
        styleResolver.state().cssToLengthConversionData().copyWithAdjustedZoom(1.0f)
        : styleResolver.state().cssToLengthConversionData();

    if (primitiveValue.isLength()) {
        Length length = primitiveValue.computeLength<Length>(conversionData);
        length.setHasQuirk(primitiveValue.isQuirkValue());
        return length;
    }

    if (primitiveValue.isPercentage())
        return Length(primitiveValue.getDoubleValue(), Percent);

    if (primitiveValue.isCalculatedPercentageWithLength())
        return Length(primitiveValue.cssCalcValue()->createCalculationValue(conversionData));

    ASSERT_NOT_REACHED();
    return Length(0, Fixed);
}

inline Length StyleBuilderConverter::convertLengthOrAuto(StyleResolver& styleResolver, CSSValue& value)
{
    if (downcast<CSSPrimitiveValue>(value).getValueID() == CSSValueAuto)
        return Length(Auto);
    return convertLength(styleResolver, value);
}

inline Length StyleBuilderConverter::convertLengthSizing(StyleResolver& styleResolver, CSSValue& value)
{
    auto& primitiveValue = downcast<CSSPrimitiveValue>(value);
    switch (primitiveValue.getValueID()) {
    case CSSValueInvalid:
        return convertLength(styleResolver, value);
    case CSSValueIntrinsic:
        return Length(Intrinsic);
    case CSSValueMinIntrinsic:
        return Length(MinIntrinsic);
    case CSSValueWebkitMinContent:
        return Length(MinContent);
    case CSSValueWebkitMaxContent:
        return Length(MaxContent);
    case CSSValueWebkitFillAvailable:
        return Length(FillAvailable);
    case CSSValueWebkitFitContent:
        return Length(FitContent);
    case CSSValueAuto:
        return Length(Auto);
    default:
        ASSERT_NOT_REACHED();
        return Length();
    }
}

inline Length StyleBuilderConverter::convertLengthMaxSizing(StyleResolver& styleResolver, CSSValue& value)
{
    if (downcast<CSSPrimitiveValue>(value).getValueID() == CSSValueNone)
        return Length(Undefined);
    return convertLengthSizing(styleResolver, value);
}

template <typename T>
inline T StyleBuilderConverter::convertComputedLength(StyleResolver& styleResolver, CSSValue& value)
{
    return downcast<CSSPrimitiveValue>(value).computeLength<T>(styleResolver.state().cssToLengthConversionData());
}

template <typename T>
inline T StyleBuilderConverter::convertLineWidth(StyleResolver& styleResolver, CSSValue& value)
{
    auto& primitiveValue = downcast<CSSPrimitiveValue>(value);
    switch (primitiveValue.getValueID()) {
    case CSSValueThin:
        return 1;
    case CSSValueMedium:
        return 3;
    case CSSValueThick:
        return 5;
    case CSSValueInvalid: {
        // Any original result that was >= 1 should not be allowed to fall below 1.
        // This keeps border lines from vanishing.
        T result = convertComputedLength<T>(styleResolver, value);
        if (styleResolver.state().style()->effectiveZoom() < 1.0f && result < 1.0) {
            T originalLength = primitiveValue.computeLength<T>(styleResolver.state().cssToLengthConversionData().copyWithAdjustedZoom(1.0));
            if (originalLength >= 1.0)
                return 1;
        }
        return result;
    }
    default:
        ASSERT_NOT_REACHED();
        return 0;
    }
}

inline float StyleBuilderConverter::convertSpacing(StyleResolver& styleResolver, CSSValue& value)
{
    auto& primitiveValue = downcast<CSSPrimitiveValue>(value);
    if (primitiveValue.getValueID() == CSSValueNormal)
        return 0.f;

    CSSToLengthConversionData conversionData = styleResolver.useSVGZoomRulesForLength() ?
        styleResolver.state().cssToLengthConversionData().copyWithAdjustedZoom(1.0f)
        : styleResolver.state().cssToLengthConversionData();
    return primitiveValue.computeLength<float>(conversionData);
}

inline Length StyleBuilderConverter::convertToRadiusLength(CSSToLengthConversionData& conversionData, CSSPrimitiveValue& value)
{
    if (value.isPercentage())
        return Length(value.getDoubleValue(), Percent);
    if (value.isCalculatedPercentageWithLength())
        return Length(value.cssCalcValue()->createCalculationValue(conversionData));
    return value.computeLength<Length>(conversionData);
}

inline LengthSize StyleBuilderConverter::convertRadius(StyleResolver& styleResolver, CSSValue& value)
{
    auto& primitiveValue = downcast<CSSPrimitiveValue>(value);
    Pair* pair = primitiveValue.getPairValue();
    if (!pair || !pair->first() || !pair->second())
        return LengthSize(Length(0, Fixed), Length(0, Fixed));

    CSSToLengthConversionData conversionData = styleResolver.state().cssToLengthConversionData();
    Length radiusWidth = convertToRadiusLength(conversionData, *pair->first());
    Length radiusHeight = convertToRadiusLength(conversionData, *pair->second());

    ASSERT(!radiusWidth.isNegative());
    ASSERT(!radiusHeight.isNegative());
    if (radiusWidth.isZero() || radiusHeight.isZero())
        return LengthSize(Length(0, Fixed), Length(0, Fixed));

    return LengthSize(radiusWidth, radiusHeight);
}

inline TextDecoration StyleBuilderConverter::convertTextDecoration(StyleResolver&, CSSValue& value)
{
    TextDecoration result = RenderStyle::initialTextDecoration();
    if (is<CSSValueList>(value)) {
        for (auto& currentValue : downcast<CSSValueList>(value))
            result |= downcast<CSSPrimitiveValue>(currentValue.get());
    }
    return result;
}

template <typename T>
inline T StyleBuilderConverter::convertNumber(StyleResolver&, CSSValue& value)
{
    auto& primitiveValue = downcast<CSSPrimitiveValue>(value);
    if (primitiveValue.getValueID() == CSSValueAuto)
        return -1;
    return primitiveValue.getValue<T>(CSSPrimitiveValue::CSS_NUMBER);
}

inline short StyleBuilderConverter::convertWebkitHyphenateLimitLines(StyleResolver&, CSSValue& value)
{
    auto& primitiveValue = downcast<CSSPrimitiveValue>(value);
    if (primitiveValue.getValueID() == CSSValueNoLimit)
        return -1;
    return primitiveValue.getValue<short>(CSSPrimitiveValue::CSS_NUMBER);
}

template <CSSPropertyID property>
inline NinePieceImage StyleBuilderConverter::convertBorderImage(StyleResolver& styleResolver, CSSValue& value)
{
    NinePieceImage image;
    styleResolver.styleMap()->mapNinePieceImage(property, &value, image);
    return image;
}

template <CSSPropertyID property>
inline NinePieceImage StyleBuilderConverter::convertBorderMask(StyleResolver& styleResolver, CSSValue& value)
{
    NinePieceImage image;
    image.setMaskDefaults();
    styleResolver.styleMap()->mapNinePieceImage(property, &value, image);
    return image;
}

template <CSSPropertyID property>
inline PassRefPtr<StyleImage> StyleBuilderConverter::convertStyleImage(StyleResolver& styleResolver, CSSValue& value)
{
    return styleResolver.styleImage(property, value);
}

inline TransformOperations StyleBuilderConverter::convertTransform(StyleResolver& styleResolver, CSSValue& value)
{
    TransformOperations operations;
    transformsForValue(value, styleResolver.state().cssToLengthConversionData(), operations);
    return operations;
}

inline String StyleBuilderConverter::convertString(StyleResolver&, CSSValue& value)
{
    return downcast<CSSPrimitiveValue>(value).getStringValue();
}

inline String StyleBuilderConverter::convertStringOrAuto(StyleResolver& styleResolver, CSSValue& value)
{
    if (downcast<CSSPrimitiveValue>(value).getValueID() == CSSValueAuto)
        return nullAtom;
    return convertString(styleResolver, value);
}

inline String StyleBuilderConverter::convertStringOrNone(StyleResolver& styleResolver, CSSValue& value)
{
    if (downcast<CSSPrimitiveValue>(value).getValueID() == CSSValueNone)
        return nullAtom;
    return convertString(styleResolver, value);
}

inline TextEmphasisPosition StyleBuilderConverter::valueToEmphasisPosition(CSSPrimitiveValue& primitiveValue)
{
    ASSERT(primitiveValue.isValueID());

    switch (primitiveValue.getValueID()) {
    case CSSValueOver:
        return TextEmphasisPositionOver;
    case CSSValueUnder:
        return TextEmphasisPositionUnder;
    case CSSValueLeft:
        return TextEmphasisPositionLeft;
    case CSSValueRight:
        return TextEmphasisPositionRight;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return RenderStyle::initialTextEmphasisPosition();
}

inline TextEmphasisPosition StyleBuilderConverter::convertTextEmphasisPosition(StyleResolver&, CSSValue& value)
{
    if (is<CSSPrimitiveValue>(value))
        return valueToEmphasisPosition(downcast<CSSPrimitiveValue>(value));

    TextEmphasisPosition position = 0;
    for (auto& currentValue : downcast<CSSValueList>(value))
        position |= valueToEmphasisPosition(downcast<CSSPrimitiveValue>(currentValue.get()));
    return position;
}

inline ETextAlign StyleBuilderConverter::convertTextAlign(StyleResolver& styleResolver, CSSValue& value)
{
    auto& primitiveValue = downcast<CSSPrimitiveValue>(value);
    ASSERT(primitiveValue.isValueID());

    if (primitiveValue.getValueID() != CSSValueWebkitMatchParent)
        return primitiveValue;

    auto* parentStyle = styleResolver.parentStyle();
    if (parentStyle->textAlign() == TASTART)
        return parentStyle->isLeftToRightDirection() ? LEFT : RIGHT;
    if (parentStyle->textAlign() == TAEND)
        return parentStyle->isLeftToRightDirection() ? RIGHT : LEFT;
    return parentStyle->textAlign();
}

inline PassRefPtr<ClipPathOperation> StyleBuilderConverter::convertClipPath(StyleResolver& styleResolver, CSSValue& value)
{
    if (is<CSSPrimitiveValue>(value)) {
        auto& primitiveValue = downcast<CSSPrimitiveValue>(value);
        if (primitiveValue.primitiveType() == CSSPrimitiveValue::CSS_URI) {
            String cssURLValue = primitiveValue.getStringValue();
            URL url = styleResolver.document().completeURL(cssURLValue);
            // FIXME: It doesn't work with external SVG references (see https://bugs.webkit.org/show_bug.cgi?id=126133)
            return ReferenceClipPathOperation::create(cssURLValue, url.fragmentIdentifier());
        }
        ASSERT(primitiveValue.getValueID() == CSSValueNone);
        return nullptr;
    }

    CSSBoxType referenceBox = BoxMissing;
    RefPtr<ClipPathOperation> operation;

    for (auto& currentValue : downcast<CSSValueList>(value)) {
        auto& primitiveValue = downcast<CSSPrimitiveValue>(currentValue.get());
        if (primitiveValue.isShape()) {
            ASSERT(!operation);
            operation = ShapeClipPathOperation::create(basicShapeForValue(styleResolver.state().cssToLengthConversionData(), primitiveValue.getShapeValue()));
        } else {
            ASSERT(primitiveValue.getValueID() == CSSValueContentBox
                   || primitiveValue.getValueID() == CSSValueBorderBox
                   || primitiveValue.getValueID() == CSSValuePaddingBox
                   || primitiveValue.getValueID() == CSSValueMarginBox
                   || primitiveValue.getValueID() == CSSValueFill
                   || primitiveValue.getValueID() == CSSValueStroke
                   || primitiveValue.getValueID() == CSSValueViewBox);
            ASSERT(referenceBox == BoxMissing);
            referenceBox = primitiveValue;
        }
    }
    if (operation)
        downcast<ShapeClipPathOperation>(*operation).setReferenceBox(referenceBox);
    else {
        ASSERT(referenceBox != BoxMissing);
        operation = BoxClipPathOperation::create(referenceBox);
    }

    return operation.release();
}

inline EResize StyleBuilderConverter::convertResize(StyleResolver& styleResolver, CSSValue& value)
{
    auto& primitiveValue = downcast<CSSPrimitiveValue>(value);

    EResize resize = RESIZE_NONE;
    if (primitiveValue.getValueID() == CSSValueAuto) {
        if (Settings* settings = styleResolver.document().settings())
            resize = settings->textAreasAreResizable() ? RESIZE_BOTH : RESIZE_NONE;
    } else
        resize = primitiveValue;

    return resize;
}

inline int StyleBuilderConverter::convertMarqueeRepetition(StyleResolver&, CSSValue& value)
{
    auto& primitiveValue = downcast<CSSPrimitiveValue>(value);
    if (primitiveValue.getValueID() == CSSValueInfinite)
        return -1; // -1 means repeat forever.

    ASSERT(primitiveValue.isNumber());
    return primitiveValue.getIntValue();
}

inline int StyleBuilderConverter::convertMarqueeSpeed(StyleResolver&, CSSValue& value)
{
    auto& primitiveValue = downcast<CSSPrimitiveValue>(value);
    int speed = 85;
    if (CSSValueID ident = primitiveValue.getValueID()) {
        switch (ident) {
        case CSSValueSlow:
            speed = 500; // 500 msec.
            break;
        case CSSValueNormal:
            speed = 85; // 85msec. The WinIE default.
            break;
        case CSSValueFast:
            speed = 10; // 10msec. Super fast.
            break;
        default:
            ASSERT_NOT_REACHED();
            break;
        }
    } else if (primitiveValue.isTime())
        speed = primitiveValue.computeTime<int, CSSPrimitiveValue::Milliseconds>();
    else {
        // For scrollamount support.
        ASSERT(primitiveValue.isNumber());
        speed = primitiveValue.getIntValue();
    }
    return speed;
}

inline PassRefPtr<QuotesData> StyleBuilderConverter::convertQuotes(StyleResolver&, CSSValue& value)
{
    if (is<CSSPrimitiveValue>(value)) {
        ASSERT(downcast<CSSPrimitiveValue>(value).getValueID() == CSSValueNone);
        return QuotesData::create(Vector<std::pair<String, String>>());
    }

    CSSValueList& list = downcast<CSSValueList>(value);
    Vector<std::pair<String, String>> quotes;
    quotes.reserveInitialCapacity(list.length() / 2);
    for (unsigned i = 0; i < list.length(); i += 2) {
        CSSValue* first = list.itemWithoutBoundsCheck(i);
        // item() returns null if out of bounds so this is safe.
        CSSValue* second = list.item(i + 1);
        if (!second)
            break;
        String startQuote = downcast<CSSPrimitiveValue>(*first).getStringValue();
        String endQuote = downcast<CSSPrimitiveValue>(*second).getStringValue();
        quotes.append(std::make_pair(startQuote, endQuote));
    }
    return QuotesData::create(quotes);
}

inline TextUnderlinePosition StyleBuilderConverter::convertTextUnderlinePosition(StyleResolver&, CSSValue& value)
{
    // This is true if value is 'auto' or 'alphabetic'.
    if (is<CSSPrimitiveValue>(value))
        return downcast<CSSPrimitiveValue>(value);

    unsigned combinedPosition = 0;
    for (auto& currentValue : downcast<CSSValueList>(value)) {
        TextUnderlinePosition position = downcast<CSSPrimitiveValue>(currentValue.get());
        combinedPosition |= position;
    }
    return static_cast<TextUnderlinePosition>(combinedPosition);
}

inline PassRefPtr<StyleReflection> StyleBuilderConverter::convertReflection(StyleResolver& styleResolver, CSSValue& value)
{
    if (is<CSSPrimitiveValue>(value)) {
        ASSERT(downcast<CSSPrimitiveValue>(value).getValueID() == CSSValueNone);
        return nullptr;
    }

    CSSReflectValue& reflectValue = downcast<CSSReflectValue>(value);

    RefPtr<StyleReflection> reflection = StyleReflection::create();
    reflection->setDirection(*reflectValue.direction());

    if (reflectValue.offset())
        reflection->setOffset(reflectValue.offset()->convertToLength<FixedIntegerConversion | PercentConversion | CalculatedConversion>(styleResolver.state().cssToLengthConversionData()));

    NinePieceImage mask;
    mask.setMaskDefaults();
    styleResolver.styleMap()->mapNinePieceImage(CSSPropertyWebkitBoxReflect, reflectValue.mask(), mask);
    reflection->setMask(mask);

    return reflection.release();
}

inline IntSize StyleBuilderConverter::convertInitialLetter(StyleResolver&, CSSValue& value)
{
    auto& primitiveValue = downcast<CSSPrimitiveValue>(value);

    if (primitiveValue.getValueID() == CSSValueNormal)
        return IntSize();

    Pair* pair = primitiveValue.getPairValue();
    ASSERT(pair);
    ASSERT(pair->first());
    ASSERT(pair->second());

    return IntSize(pair->first()->getIntValue(), pair->second()->getIntValue());
}

inline float StyleBuilderConverter::convertTextStrokeWidth(StyleResolver& styleResolver, CSSValue& value)
{
    auto& primitiveValue = downcast<CSSPrimitiveValue>(value);

    float width = 0;
    switch (primitiveValue.getValueID()) {
    case CSSValueThin:
    case CSSValueMedium:
    case CSSValueThick: {
        double result = 1.0 / 48;
        if (primitiveValue.getValueID() == CSSValueMedium)
            result *= 3;
        else if (primitiveValue.getValueID() == CSSValueThick)
            result *= 5;
        Ref<CSSPrimitiveValue> emsValue(CSSPrimitiveValue::create(result, CSSPrimitiveValue::CSS_EMS));
        width = convertComputedLength<float>(styleResolver, emsValue);
        break;
    }
    case CSSValueInvalid: {
        width = convertComputedLength<float>(styleResolver, primitiveValue);
        break;
    }
    default:
        ASSERT_NOT_REACHED();
        return 0;
    }

    return width;
}

inline LineBoxContain StyleBuilderConverter::convertLineBoxContain(StyleResolver&, CSSValue& value)
{
    if (is<CSSPrimitiveValue>(value)) {
        ASSERT(downcast<CSSPrimitiveValue>(value).getValueID() == CSSValueNone);
        return LineBoxContainNone;
    }

    return downcast<CSSLineBoxContainValue>(value).value();
}

inline TextDecorationSkip StyleBuilderConverter::valueToDecorationSkip(const CSSPrimitiveValue& primitiveValue)
{
    ASSERT(primitiveValue.isValueID());

    switch (primitiveValue.getValueID()) {
    case CSSValueAuto:
        return TextDecorationSkipAuto;
    case CSSValueNone:
        return TextDecorationSkipNone;
    case CSSValueInk:
        return TextDecorationSkipInk;
    case CSSValueObjects:
        return TextDecorationSkipObjects;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return TextDecorationSkipNone;
}

inline TextDecorationSkip StyleBuilderConverter::convertTextDecorationSkip(StyleResolver&, CSSValue& value)
{
    if (is<CSSPrimitiveValue>(value))
        return valueToDecorationSkip(downcast<CSSPrimitiveValue>(value));

    TextDecorationSkip skip = RenderStyle::initialTextDecorationSkip();
    for (auto& currentValue : downcast<CSSValueList>(value))
        skip |= valueToDecorationSkip(downcast<CSSPrimitiveValue>(currentValue.get()));
    return skip;
}

#if ENABLE(CSS_SHAPES)
static inline bool isImageShape(const CSSValue& value)
{
    return is<CSSImageValue>(value)
#if ENABLE(CSS_IMAGE_SET)
        || is<CSSImageSetValue>(value)
#endif 
        || is<CSSImageGeneratorValue>(value);
}

inline PassRefPtr<ShapeValue> StyleBuilderConverter::convertShapeValue(StyleResolver& styleResolver, CSSValue& value)
{
    if (is<CSSPrimitiveValue>(value)) {
        ASSERT(downcast<CSSPrimitiveValue>(value).getValueID() == CSSValueNone);
        return nullptr;
    }

    if (isImageShape(value))
        return ShapeValue::createImageValue(styleResolver.styleImage(CSSPropertyWebkitShapeOutside, value));

    RefPtr<BasicShape> shape;
    CSSBoxType referenceBox = BoxMissing;
    for (auto& currentValue : downcast<CSSValueList>(value)) {
        CSSPrimitiveValue& primitiveValue = downcast<CSSPrimitiveValue>(currentValue.get());
        if (primitiveValue.isShape())
            shape = basicShapeForValue(styleResolver.state().cssToLengthConversionData(), primitiveValue.getShapeValue());
        else if (primitiveValue.getValueID() == CSSValueContentBox
            || primitiveValue.getValueID() == CSSValueBorderBox
            || primitiveValue.getValueID() == CSSValuePaddingBox
            || primitiveValue.getValueID() == CSSValueMarginBox)
            referenceBox = primitiveValue;
        else {
            ASSERT_NOT_REACHED();
            return nullptr;
        }
    }

    if (shape)
        return ShapeValue::createShapeValue(shape.release(), referenceBox);

    if (referenceBox != BoxMissing)
        return ShapeValue::createBoxShapeValue(referenceBox);

    ASSERT_NOT_REACHED();
    return nullptr;
}
#endif // ENABLE(CSS_SHAPES)

#if ENABLE(CSS_GRID_LAYOUT)
bool StyleBuilderConverter::createGridTrackBreadth(CSSPrimitiveValue& primitiveValue, StyleResolver& styleResolver, GridLength& workingLength)
{
    if (primitiveValue.getValueID() == CSSValueWebkitMinContent) {
        workingLength = Length(MinContent);
        return true;
    }

    if (primitiveValue.getValueID() == CSSValueWebkitMaxContent) {
        workingLength = Length(MaxContent);
        return true;
    }

    if (primitiveValue.isFlex()) {
        // Fractional unit.
        workingLength.setFlex(primitiveValue.getDoubleValue());
        return true;
    }

    workingLength = primitiveValue.convertToLength<FixedIntegerConversion | PercentConversion | CalculatedConversion | AutoConversion>(styleResolver.state().cssToLengthConversionData());
    if (workingLength.length().isUndefined())
        return false;

    if (primitiveValue.isLength())
        workingLength.length().setHasQuirk(primitiveValue.isQuirkValue());

    return true;
}

bool StyleBuilderConverter::createGridTrackSize(CSSValue& value, GridTrackSize& trackSize, StyleResolver& styleResolver)
{
    if (is<CSSPrimitiveValue>(value)) {
        GridLength workingLength;
        if (!createGridTrackBreadth(downcast<CSSPrimitiveValue>(value), styleResolver, workingLength))
            return false;

        trackSize.setLength(workingLength);
        return true;
    }

    CSSValueList& arguments = *downcast<CSSFunctionValue>(value).arguments();
    ASSERT_WITH_SECURITY_IMPLICATION(arguments.length() == 2);

    GridLength minTrackBreadth;
    GridLength maxTrackBreadth;
    if (!createGridTrackBreadth(downcast<CSSPrimitiveValue>(*arguments.itemWithoutBoundsCheck(0)), styleResolver, minTrackBreadth) || !createGridTrackBreadth(downcast<CSSPrimitiveValue>(*arguments.itemWithoutBoundsCheck(1)), styleResolver, maxTrackBreadth))
        return false;

    trackSize.setMinMax(minTrackBreadth, maxTrackBreadth);
    return true;
}

bool StyleBuilderConverter::createGridTrackList(CSSValue& value, Vector<GridTrackSize>& trackSizes, NamedGridLinesMap& namedGridLines, OrderedNamedGridLinesMap& orderedNamedGridLines, StyleResolver& styleResolver)
{
    // Handle 'none'.
    if (is<CSSPrimitiveValue>(value))
        return downcast<CSSPrimitiveValue>(value).getValueID() == CSSValueNone;

    if (!is<CSSValueList>(value))
        return false;

    unsigned currentNamedGridLine = 0;
    for (auto& currentValue : downcast<CSSValueList>(value)) {
        if (is<CSSGridLineNamesValue>(currentValue.get())) {
            for (auto& currentGridLineName : downcast<CSSGridLineNamesValue>(currentValue.get())) {
                String namedGridLine = downcast<CSSPrimitiveValue>(currentGridLineName.get()).getStringValue();
                NamedGridLinesMap::AddResult result = namedGridLines.add(namedGridLine, Vector<unsigned>());
                result.iterator->value.append(currentNamedGridLine);
                OrderedNamedGridLinesMap::AddResult orderedResult = orderedNamedGridLines.add(currentNamedGridLine, Vector<String>());
                orderedResult.iterator->value.append(namedGridLine);
            }
            continue;
        }

        ++currentNamedGridLine;
        GridTrackSize trackSize;
        if (!createGridTrackSize(currentValue, trackSize, styleResolver))
            return false;

        trackSizes.append(trackSize);
    }

    // The parser should have rejected any <track-list> without any <track-size> as
    // this is not conformant to the syntax.
    ASSERT(!trackSizes.isEmpty());
    return true;
}

bool StyleBuilderConverter::createGridPosition(CSSValue& value, GridPosition& position)
{
    // We accept the specification's grammar:
    // auto | <custom-ident> | [ <integer> && <custom-ident>? ] | [ span && [ <integer> || <custom-ident> ] ]
    if (is<CSSPrimitiveValue>(value)) {
        auto& primitiveValue = downcast<CSSPrimitiveValue>(value);
        // We translate <ident> to <string> during parsing as it makes handling it simpler.
        if (primitiveValue.isString()) {
            position.setNamedGridArea(primitiveValue.getStringValue());
            return true;
        }

        ASSERT(primitiveValue.getValueID() == CSSValueAuto);
        return true;
    }

    auto& values = downcast<CSSValueList>(value);
    ASSERT(values.length());

    auto it = values.begin();
    CSSPrimitiveValue* currentValue = &downcast<CSSPrimitiveValue>(it->get());
    bool isSpanPosition = false;
    if (currentValue->getValueID() == CSSValueSpan) {
        isSpanPosition = true;
        ++it;
        currentValue = it != values.end() ? &downcast<CSSPrimitiveValue>(it->get()) : nullptr;
    }

    int gridLineNumber = 0;
    if (currentValue && currentValue->isNumber()) {
        gridLineNumber = currentValue->getIntValue();
        ++it;
        currentValue = it != values.end() ? &downcast<CSSPrimitiveValue>(it->get()) : nullptr;
    }

    String gridLineName;
    if (currentValue && currentValue->isString()) {
        gridLineName = currentValue->getStringValue();
        ++it;
    }

    ASSERT(it == values.end());
    if (isSpanPosition)
        position.setSpanPosition(gridLineNumber ? gridLineNumber : 1, gridLineName);
    else
        position.setExplicitPosition(gridLineNumber, gridLineName);

    return true;
}

void StyleBuilderConverter::createImplicitNamedGridLinesFromGridArea(const NamedGridAreaMap& namedGridAreas, NamedGridLinesMap& namedGridLines, GridTrackSizingDirection direction)
{
    for (auto& area : namedGridAreas) {
        GridSpan areaSpan = direction == ForRows ? area.value.rows : area.value.columns;
        {
            auto& startVector = namedGridLines.add(area.key + "-start", Vector<unsigned>()).iterator->value;
            startVector.append(areaSpan.resolvedInitialPosition.toInt());
            std::sort(startVector.begin(), startVector.end());
        }
        {
            auto& endVector = namedGridLines.add(area.key + "-end", Vector<unsigned>()).iterator->value;
            endVector.append(areaSpan.resolvedFinalPosition.next().toInt());
            std::sort(endVector.begin(), endVector.end());
        }
    }
}

inline bool StyleBuilderConverter::convertGridTrackSize(StyleResolver& styleResolver, CSSValue& value, GridTrackSize& trackSize)
{
    return createGridTrackSize(value, trackSize, styleResolver);
}

inline bool StyleBuilderConverter::convertGridPosition(StyleResolver&, CSSValue& value, GridPosition& gridPosition)
{
    return createGridPosition(value, gridPosition);
}

inline GridAutoFlow StyleBuilderConverter::convertGridAutoFlow(StyleResolver&, CSSValue& value)
{
    auto& list = downcast<CSSValueList>(value);
    if (!list.length())
        return RenderStyle::initialGridAutoFlow();

    CSSPrimitiveValue& first = downcast<CSSPrimitiveValue>(*list.item(0));
    CSSPrimitiveValue* second = downcast<CSSPrimitiveValue>(list.item(1));

    GridAutoFlow autoFlow = RenderStyle::initialGridAutoFlow();
    switch (first.getValueID()) {
    case CSSValueRow:
        if (second && second->getValueID() == CSSValueDense)
            autoFlow =  AutoFlowRowDense;
        else
            autoFlow = AutoFlowRow;
        break;
    case CSSValueColumn:
        if (second && second->getValueID() == CSSValueDense)
            autoFlow = AutoFlowColumnDense;
        else
            autoFlow = AutoFlowColumn;
        break;
    default:
        ASSERT_NOT_REACHED();
        break;
    }

    return autoFlow;
}
#endif // ENABLE(CSS_GRID_LAYOUT)

} // namespace WebCore

#endif // StyleBuilderConverter_h
