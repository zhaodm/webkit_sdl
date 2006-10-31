/*
 * Copyright (C) 2006 Apple Computer, Inc.  All rights reserved.
 * Copyright (C) 2006 Michael Emmel mike.emmel@gmail.com 
 * All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include "CString.h"
#include "Node.h"
#include "TextField.h"
#include "FileChooser.h"
#include "Font.h"
#include "ListBox.h"
#include "IntPoint.h"
#include "Widget.h"
#include "GraphicsContext.h"
#include "Slider.h"
#include "Cursor.h"
#include "loader.h"
#include "FrameView.h"
#include "KURL.h"
#include "PlatformScrollBar.h"
#include "ScrollBar.h"
#include "Path.h"
#include "PlatformMouseEvent.h"
#include "CookieJar.h"
#include "Screen.h"
#include "History.h"
#include "Language.h"
#include "LocalizedStrings.h"
#include "PlugInInfoStore.h"
#include "RenderTheme.h"
#include "FrameGdk.h"
#include "FrameLoadRequest.h"
#include "RenderThemeGdk.h"
#include "TextBoundaries.h"
#include "AXObjectCache.h"
#include "EditCommand.h"
#include "Icon.h"
#include "IconLoader.h"
#include "IconDatabase.h"
#include "CachedResource.h"

using namespace WebCore;

namespace WebCore {
struct PlatformDataStruct
{
};

struct PlatformResponseStruct
{
};
}

#define notImplemented() do { fprintf(stderr, "FIXME: UNIMPLEMENTED: %s in %s:%d\n", __FUNCTION__, __FILE__, __LINE__); } while(0)

void FrameView::updateBorder() { notImplemented(); }
bool FrameView::passMousePressEventToScrollbar(MouseEventWithHitTestResults&, PlatformScrollbar*) { return false; }
bool FrameView::passMousePressEventToSubframe(MouseEventWithHitTestResults&, Frame*) { return true; }
bool FrameView::passMouseReleaseEventToSubframe(MouseEventWithHitTestResults&, Frame*) { return true; }
bool FrameView::passMouseMoveEventToSubframe(MouseEventWithHitTestResults&, Frame*) { return true; }
bool FrameView::passWheelEventToSubframe(PlatformWheelEvent&, Frame*) { return true; }

void Widget::enableFlushDrawing() { notImplemented(); }
bool Widget::isEnabled() const { notImplemented(); return 0; }
Widget::FocusPolicy Widget::focusPolicy() const { notImplemented(); return NoFocus; }
void Widget::disableFlushDrawing() { notImplemented(); }
GraphicsContext* Widget::lockDrawingFocus() { notImplemented(); return 0; }
void Widget::unlockDrawingFocus(GraphicsContext*) { notImplemented(); }
void Widget::removeFromParent() { }

void TextField::selectAll() { notImplemented(); }
void TextField::addSearchResult() { notImplemented(); }
int TextField::selectionStart() const { notImplemented(); return 0; }
bool TextField::hasSelectedText() const { notImplemented(); return 0; }
String TextField::selectedText() const { notImplemented(); return String(); }
void TextField::setAutoSaveName(String const&) { notImplemented(); }
bool TextField::checksDescendantsForFocus() const { notImplemented(); return false; }
void TextField::setSelection(int, int) { notImplemented(); }
void TextField::setMaxResults(int) { notImplemented(); }
bool TextField::edited() const { notImplemented(); return 0; }

Slider::Slider() { notImplemented(); }
IntSize Slider::sizeHint() const { notImplemented(); return IntSize(); }
void Slider::setValue(double) { notImplemented(); }
void Slider::setMaxValue(double) { notImplemented(); }
void Slider::setMinValue(double) { notImplemented(); }
Slider::~Slider() { notImplemented(); }
void Slider::setFont(WebCore::Font const&) { notImplemented(); }
double Slider::value() const { notImplemented(); return 0; }

void ListBox::setSelected(int, bool) { notImplemented(); }
IntSize ListBox::sizeForNumberOfLines(int) const { notImplemented(); return IntSize(); }
bool ListBox::isSelected(int) const { notImplemented(); return 0; }
void ListBox::appendItem(DeprecatedString const&, ListBoxItemType, bool) { notImplemented(); }
void ListBox::doneAppendingItems() { notImplemented(); }
void ListBox::setWritingDirection(TextDirection) { notImplemented(); }
void ListBox::setEnabled(bool) { notImplemented(); }
void ListBox::clear() { notImplemented(); }
bool ListBox::checksDescendantsForFocus() const { notImplemented(); return 0; }

Widget::FocusPolicy Slider::focusPolicy() const { notImplemented(); return NoFocus; }
Widget::FocusPolicy ListBox::focusPolicy() const { notImplemented(); return NoFocus; }
Widget::FocusPolicy TextField::focusPolicy() const { notImplemented(); return NoFocus; }

Cursor::Cursor(Image*, const IntPoint&) { notImplemented(); }

PlatformMouseEvent::PlatformMouseEvent(const CurrentEventTag&) { notImplemented(); }
String WebCore::searchableIndexIntroduction() { notImplemented(); return String(); }

int WebCore::findNextSentenceFromIndex(UChar const*, int, int, bool) { notImplemented(); return 0; }
void WebCore::findSentenceBoundary(UChar const*, int, int, int*, int*) { notImplemented(); }
int WebCore::findNextWordFromIndex(UChar const*, int, int, bool) { notImplemented(); return 0; }

namespace WebCore {

Vector<char> ServeSynchronousRequest(Loader*,DocLoader*,ResourceHandle*,KURL&,DeprecatedString&) { notImplemented(); return Vector<char>(); }

}

void FrameGdk::focusWindow() { notImplemented(); }
void FrameGdk::unfocusWindow() { notImplemented(); }
bool FrameGdk::locationbarVisible() { notImplemented(); return 0; }
void FrameGdk::issueRedoCommand(void) { notImplemented(); }
KJS::Bindings::Instance* FrameGdk::getObjectInstanceForWidget(Widget *) { notImplemented(); return 0; }
KJS::Bindings::Instance* FrameGdk::getEmbedInstanceForWidget(Widget *) { notImplemented(); return 0; }
KJS::Bindings::RootObject* FrameGdk::bindingRootObject() { notImplemented(); return 0; }
Widget* FrameGdk::createJavaAppletWidget(const IntSize&, Element*, const HashMap<String, String>&) { notImplemented(); return 0; }
bool FrameGdk::canRedo() const { notImplemented(); return 0; }
bool FrameGdk::canUndo() const { notImplemented(); return 0; }
void FrameGdk::registerCommandForRedo(PassRefPtr<WebCore::EditCommand>) { notImplemented(); }
bool FrameGdk::runJavaScriptPrompt(String const&, String const&, String &) { notImplemented(); return 0; }
bool FrameGdk::shouldInterruptJavaScript() { notImplemented(); return false; }
void FrameGdk::print() { notImplemented(); }
KJS::Bindings::Instance* FrameGdk::getAppletInstanceForWidget(Widget*) { notImplemented(); return 0; }
bool FrameGdk::passMouseDownEventToWidget(Widget*) { notImplemented(); return 0; }
void FrameGdk::issueCutCommand() { notImplemented(); }
void FrameGdk::issueCopyCommand() { notImplemented(); }
void FrameGdk::issueUndoCommand() { notImplemented(); }
String FrameGdk::mimeTypeForFileName(String const&) const { notImplemented(); return String(); }
void FrameGdk::issuePasteCommand() { notImplemented(); }
void FrameGdk::scheduleClose() { notImplemented(); }
void FrameGdk::markMisspellings(WebCore::Selection const&) { notImplemented(); }
bool FrameGdk::menubarVisible() { notImplemented(); return 0; }
bool FrameGdk::personalbarVisible() { notImplemented(); return 0; }
bool FrameGdk::statusbarVisible() { notImplemented(); return 0; }
bool FrameGdk::toolbarVisible() { notImplemented(); return 0; }
void FrameGdk::issueTransposeCommand() { notImplemented(); }
bool FrameGdk::canPaste() const { notImplemented(); return 0; }
enum WebCore::ObjectContentType FrameGdk::objectContentType(KURL const&, String const&) { notImplemented(); return (ObjectContentType)0; }
bool FrameGdk::canGoBackOrForward(int) const { notImplemented(); return 0; }
void FrameGdk::issuePasteAndMatchStyleCommand() { notImplemented(); }
bool FrameGdk::isLoadTypeReload() { notImplemented(); return false; }
KURL FrameGdk::originalRequestURL() const { return KURL(); }
Plugin* FrameGdk::createPlugin(Element*, KURL const&, const Vector<String>&, const Vector<String>&, String const&) { notImplemented(); return 0; }

int WebCore::screenDepthPerComponent(const Page*) { notImplemented(); return 0; }
bool WebCore::screenIsMonochrome(const Page*) { notImplemented(); return false; }

/********************************************************/
/* Completely empty stubs (mostly to allow DRT to run): */
/********************************************************/
static Cursor localCursor;
const Cursor& WebCore::moveCursor() { return localCursor; }

bool AXObjectCache::gAccessibilityEnabled = false;

bool WebCore::historyContains(DeprecatedString const&) { return false; }
String WebCore::submitButtonDefaultLabel() { return "Submit"; }
String WebCore::inputElementAltText() { return DeprecatedString(); }
String WebCore::resetButtonDefaultLabel() { return "Reset"; }
String WebCore::fileButtonChooseFileLabel() { return "Browse..."; }
String WebCore::fileButtonNoFileSelectedLabel() { return String(); }

String WebCore::defaultLanguage() { return "en"; }

void WebCore::findWordBoundary(UChar const* str, int len, int position, int* start, int* end) {*start = position; *end = position; }

PluginInfo*PlugInInfoStore::createPluginInfoForPluginAtIndex(unsigned) { return 0;}
unsigned PlugInInfoStore::pluginCount() const { return 0; }
bool WebCore::PlugInInfoStore::supportsMIMEType(const WebCore::String&) { return false; }
void WebCore::refreshPlugins(bool) { }

void WebCore::ResourceHandle::assembleResponseHeaders() const { }
void WebCore::ResourceHandle::retrieveCharset() const { }

void FrameGdk::restoreDocumentState() { }
void FrameGdk::partClearedInBegin() { }
void FrameGdk::createEmptyDocument() { }
String FrameGdk::overrideMediaType() const { return String(); }
void FrameGdk::handledOnloadEvents() { }
Range* FrameGdk::markedTextRange() const { return 0; }
bool FrameGdk::lastEventIsMouseUp() const { return false; }
void FrameGdk::addMessageToConsole(String const&, unsigned int, String const&) { }
bool FrameGdk::shouldChangeSelection(const Selection&, const Selection&, WebCore::EAffinity, bool) const { return true; }
bool FrameGdk::shouldChangeSelection(const Selection&) const { return true; }
void FrameGdk::respondToChangedSelection(WebCore::Selection const&, bool) { }
Frame* FrameGdk::createFrame(const KURL& url, const String& name, Element* ownerElement, const String& referrer) { return 0; }

void FrameGdk::saveDocumentState() { }
void FrameGdk::registerCommandForUndo(PassRefPtr<WebCore::EditCommand>) { }
void FrameGdk::clearUndoRedoOperations(void) { }
String FrameGdk::incomingReferrer() const { return String(); }
void FrameGdk::markMisspellingsInAdjacentWords(WebCore::VisiblePosition const&) { }
void FrameGdk::respondToChangedContents(const Selection&) { }

namespace WebCore {

bool CheckIfReloading(DocLoader*) { return false; }
time_t CacheObjectExpiresTime(DocLoader*, PlatformResponse) { return 0; }
void CheckCacheObjectStatus(DocLoader*, CachedResource*) { }
DeprecatedString ResponseURL(PlatformResponse) { return DeprecatedString(); }
bool IsResponseURLEqualToURL(PlatformResponse , const String& URL) { return false; }
bool ResponseIsMultipart(PlatformResponse) { return false; }
DeprecatedString ResponseMIMEType(PlatformResponse) { return DeprecatedString(); }
void CachedResource::setResponse(PlatformResponse) { notImplemented(); }
void CachedResource::setAllData(PlatformData) { notImplemented(); }

}

void Widget::setEnabled(bool) { }
void Widget::paint(GraphicsContext*, IntRect const&) { }
void Widget::setIsSelected(bool) { }
void Widget::invalidate() { }
void Widget::invalidateRect(const IntRect& r) { }

void ScrollView::addChild(Widget*) { }
void ScrollView::removeChild(Widget*) { }
void ScrollView::scrollPointRecursively(int x, int y) { }
bool ScrollView::inWindow() const { return true; }
void ScrollView::wheelEvent(PlatformWheelEvent&) { }
void ScrollView::updateScrollbars() { }
int ScrollView::updateScrollInfo(short type, int current, int max, int pageSize) { return 0; }
IntPoint ScrollView::windowToContents(const IntPoint& point) const { return point; }
IntPoint ScrollView::contentsToWindow(const IntPoint& point) const { return point; }
PlatformScrollbar* ScrollView::scrollbarUnderMouse(const PlatformMouseEvent& mouseEvent) { return 0; }

void GraphicsContext::addRoundedRectClip(const IntRect& rect, const IntSize& topLeft, const IntSize& topRight,
        const IntSize& bottomLeft, const IntSize& bottomRight) { notImplemented(); }
void GraphicsContext::addInnerRoundedRectClip(const IntRect& rect, int thickness) { notImplemented(); }
void GraphicsContext::setShadow(IntSize const&, int, Color const&) { }
void GraphicsContext::clearShadow() { }
void GraphicsContext::beginTransparencyLayer(float) { }
void GraphicsContext::endTransparencyLayer() { }
void GraphicsContext::clearRect(const FloatRect&) { }
void GraphicsContext::strokeRect(const FloatRect&, float) { }
void GraphicsContext::setLineWidth(float) { }
void GraphicsContext::setLineCap(LineCap) { }
void GraphicsContext::setLineJoin(LineJoin) { }
void GraphicsContext::setMiterLimit(float) { }
void GraphicsContext::setAlpha(float) { }
void GraphicsContext::setCompositeOperation(CompositeOperator) { }
void GraphicsContext::clip(const Path&) { }
void GraphicsContext::rotate(float) { }
void GraphicsContext::scale(const FloatSize&) { }

Path::Path(){ }
Path::~Path(){ }
Path::Path(const Path&){ }
bool Path::contains(const FloatPoint&, WindRule rule) const{ return false; }
void Path::translate(const FloatSize&){ }
FloatRect Path::boundingRect() const { return FloatRect(); }
Path& Path::operator=(const Path&){ return * this; }
void Path::clear() { }
void Path::moveTo(const FloatPoint&) { }
void Path::addLineTo(const FloatPoint&) { }
void Path::addQuadCurveTo(const FloatPoint&, const FloatPoint&) { }
void Path::addBezierCurveTo(const FloatPoint&, const FloatPoint&, const FloatPoint&) { }
void Path::addArcTo(const FloatPoint&, const FloatPoint&, float) { }
void Path::closeSubpath() { }
void Path::addArc(const FloatPoint&, float, float, float, bool) { }
void Path::addRect(const FloatRect&) { }
void Path::addEllipse(const FloatRect&) { }

TextField::TextField(TextField::Type) { }
TextField::~TextField() { }
void TextField::setFont(WebCore::Font const&) { }
void TextField::setAlignment(HorizontalAlignment) { }
void TextField::setWritingDirection(TextDirection) { }
int TextField::maxLength() const { return 0; }
void TextField::setMaxLength(int) { }
String TextField::text() const { return String(); }
void TextField::setText(String const&) { }
int TextField::cursorPosition() const { return 0; }
void TextField::setCursorPosition(int) { }
void TextField::setEdited(bool) { }
void TextField::setReadOnly(bool) { }
void TextField::setPlaceholderString(String const&) { }
void TextField::setColors(Color const&, Color const&) { }
IntSize TextField::sizeForCharacterWidth(int) const { return IntSize(); }
int TextField::baselinePosition(int) const { return 0; }
void TextField::setLiveSearch(bool) { }

PlatformScrollbar::PlatformScrollbar(ScrollbarClient* client, ScrollbarOrientation orientation, ScrollbarControlSize controlSize) : Scrollbar(client, orientation, controlSize) { }
PlatformScrollbar::~PlatformScrollbar() { }
int PlatformScrollbar::width() const { return 0; }
int PlatformScrollbar::height() const { return 0; }
void PlatformScrollbar::setEnabled(bool) { }
void PlatformScrollbar::paint(GraphicsContext*, const IntRect& damageRect) { }
void PlatformScrollbar::updateThumbPosition() { }
void PlatformScrollbar::updateThumbProportion() { }
void PlatformScrollbar::setRect(const IntRect&) { }

ListBox::ListBox() { }
ListBox::~ListBox() { }
void ListBox::setSelectionMode(ListBox::SelectionMode) { }
void ListBox::setFont(WebCore::Font const&) { }

FileChooser::FileChooser(Document*, RenderFileUploadControl*) { notImplemented(); }
FileChooser::~FileChooser() { notImplemented(); }
PassRefPtr<FileChooser> FileChooser::create(Document*, RenderFileUploadControl*) { notImplemented(); return 0; }
void FileChooser::openFileChooser() { notImplemented(); }
String FileChooser::basenameForWidth(int width) const { notImplemented(); return String(); }
void FileChooser::disconnectUploadControl() { notImplemented(); }
void FileChooser::chooseFile(const String& filename) { notImplemented(); }

Color WebCore::focusRingColor() { return 0xFF0000FF; }
void WebCore::setFocusRingColorChangeFunction(void (*)()) { }

void Frame::setNeedsReapplyStyles() { }

void Image::drawTiled(GraphicsContext*, const FloatRect&, const FloatRect&, TileRule, TileRule, CompositeOperator) { }

void RenderThemeGdk::setCheckboxSize(RenderStyle*) const { }
bool RenderThemeGdk::paintButton(RenderObject*, const RenderObject::PaintInfo&, const IntRect&) { return false; }
void RenderThemeGdk::setRadioSize(RenderStyle*) const { }
void RenderThemeGdk::adjustTextFieldStyle(CSSStyleSelector*, RenderStyle*, Element* e) const {}
bool RenderThemeGdk::paintTextField(RenderObject*, const RenderObject::PaintInfo&, const IntRect&) { return false; }

Icon::Icon() { notImplemented(); }
Icon::~Icon() { notImplemented(); }
PassRefPtr<Icon> Icon::newIconForFile(const String& filename) { notImplemented(); return PassRefPtr<Icon>(new Icon()); }
void Icon::paint(GraphicsContext*, const IntRect&) { notImplemented(); }

void IconLoader::stopLoading() { notImplemented(); } 
void IconLoader::startLoading() { notImplemented(); } 
IconLoader* IconLoader::createForFrame(Frame *frame) { return 0; } 

bool IconDatabase::isIconExpiredForIconURL(const String& url) { return false; }
bool IconDatabase::hasEntryForIconURL(const String& url) { return false; }
IconDatabase* IconDatabase::sharedIconDatabase() { return 0; }
bool IconDatabase::setIconURLForPageURL(const String& iconURL, const String& pageURL) { return false; }

FloatRect Font::selectionRectForComplexText(const TextRun&, const TextStyle&, const IntPoint&, int) const { return FloatRect(); }
void Font::drawComplexText(GraphicsContext*, const TextRun&, const TextStyle&, const FloatPoint&) const { notImplemented(); }
float Font::floatWidthForComplexText(const TextRun&, const TextStyle&) const { notImplemented(); return 0; }
int Font::offsetForPositionForComplexText(const TextRun&, const TextStyle&, int, bool) const { notImplemented(); return 0; }

namespace WebCore {
Vector<String> supportedKeySizes() { notImplemented(); return Vector<String>(); }
String signedPublicKeyAndChallengeString(unsigned keySizeIndex, const String &challengeString, const KURL &url) { return String(); }

}
