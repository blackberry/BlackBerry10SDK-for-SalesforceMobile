/*
 * Copyright (c) 2011-2013 BlackBerry Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import bb.cascades 1.2
import "./salesforce"

Page {
    id: page
    objectName: "page";
    property Sheet settingsSheet;
    Menu.definition: MenuDefinition {        
        // Add any remaining actions
        actions: [
            ActionItem {
                title: "Logout"
                imageSource: "asset:///images/icon_logout.png"
                onTriggered: {
                    console.debug("logout action triggered");
                    SFAuthenticationManager.logout();
                }
            }
        ]
        settingsAction: SettingsActionItem {
            onTriggered: {
                console.debug("setting action triggered");
                if (!settingsSheet){
                    settingsSheet = settingsSheetDef.createObject(page);
                }
                settingsSheet.open();
            }
        }
        helpAction: HelpActionItem {
            onTriggered: {
                console.debug("help action triggered");
            }
        }
        //Tim: this is for the setting page
        attachedObjects: [
            ComponentDefinition {
                id: settingsSheetDef
                Sheet {
                    id: settingsSheet
                    peekEnabled: false; 
                    content: SFSettings { 
                        titleBar: TitleBar {
                            title: qsTr("Settings")
                            dismissAction: ActionItem {
                                title: "Back"
                                imageSource: "asset:///RestExplorer/images/icon_previous.png"
                                onTriggered: {
                                    settingsSheet.close();
                                }
                            }
                        } 
                        
                        paneProperties: NavigationPaneProperties {
                            backButton: ActionItem {
                                title: "Back"
                            }
                        }
                    } 
                    //Tim: settings page is created new every time, so destroy it when it's popped
                    onClosed: {
                        console.debug("destroy settings page");
                        settingsSheet.destroy();
                        SFAccountManager.updateLoginHost();
                    }
                }
            }
        ]
    }
    SFHybridWebView{}
}