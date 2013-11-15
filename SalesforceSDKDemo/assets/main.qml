import bb.cascades 1.0
import "./salesforce"
import sf 1.0

TabbedPane {
    id: tabbedPane
    property Sheet settingsSheet;
    showTabsOnActionBar: true
    
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
                    settingsSheet = settingsSheetDef.createObject(tabbedPane);
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
                                imageSource: "asset:///images/ic_previous.png"
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
    
    Tab {
        id: basicTab
        objectName: "basicTab"
        title: qsTr("Basic")
        imageSource: "asset:///images/icon_salesforce.png"
        content: NavigationPane {
            id:mainNav
        }
        
        onTriggered: {
            tabbedPane.showTabsOnActionBar = true;
        }
        
        onCreationCompleted: {
            var basicPage = basicPageDef.createObject(mainNav);
            mainNav.push(basicPage);
        }
        
        attachedObjects: [
            ComponentDefinition {
                id: basicPageDef
                BasicPage {}
            }
        ]
    }
}