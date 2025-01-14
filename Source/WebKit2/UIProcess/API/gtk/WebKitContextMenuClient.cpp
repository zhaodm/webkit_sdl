/*
 * Copyright (C) 2012 Igalia S.L.
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
 */

#include "config.h"
#include "WebKitContextMenuClient.h"

#include "WebKitPrivate.h"
#include "WebKitWebViewBasePrivate.h"
#include "WebKitWebViewPrivate.h"

using namespace WebKit;

static void getContextMenuFromProposedMenu(WKPageRef, WKArrayRef proposedMenu, WKArrayRef*, WKHitTestResultRef hitTestResult, WKTypeRef userData, const void* clientInfo)
{
    GRefPtr<GVariant> variant;
    if (userData) {
        ASSERT(WKGetTypeID(userData) == WKStringGetTypeID());
        CString userDataString = toImpl(static_cast<WKStringRef>(userData))->string().utf8();
        variant = adoptGRef(g_variant_parse(nullptr, userDataString.data(), userDataString.data() + userDataString.length(), nullptr, nullptr));
    }
    webkitWebViewPopulateContextMenu(WEBKIT_WEB_VIEW(clientInfo), toImpl(proposedMenu), toImpl(hitTestResult), variant.get());
}

void attachContextMenuClientToView(WebKitWebView* webView)
{
    WKPageContextMenuClientV3 wkContextMenuClient = {
        {
            3, // version
            webView, // clientInfo
        },
        0, // getContextMenuFromProposedMenu_deprecatedForUseWithV0
        0, // customContextMenuItemSelected
        0, // contextMenuDismissed
        getContextMenuFromProposedMenu,
        0, // showContextMenu
        0, // hideContextMenu
    };
    WKPageRef wkPage = toAPI(webkitWebViewBaseGetPage(WEBKIT_WEB_VIEW_BASE(webView)));
    WKPageSetPageContextMenuClient(wkPage, &wkContextMenuClient.base);
}

