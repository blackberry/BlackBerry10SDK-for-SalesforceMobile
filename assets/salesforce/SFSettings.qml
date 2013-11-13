import bb.cascades 1.0

Page {
    Container {
        RadioGroup {
            id: connectionsGroup
            objectName: "connectionsGroup"

            Option {
                id: productionHost
                value: "login.salesforce.com"
                text: "Production"    
                selected: SFAccountManager.getLoginHostFromAppSettings() ==  value? true : false                            
            }
            
            Option {
                id: sandboxHost
                value: "test.salesforce.com"
                text: "Sandbox"                
                selected: SFAccountManager.getLoginHostFromAppSettings() ==  value? true : false                            
            }
            
            Option {
                id: customHost
                value: "CUSTOM"
                text: "Custom Host"                
                selected: SFAccountManager.getLoginHostFromAppSettings() ==  value? true : false                            
            }
            onSelectedOptionChanged: {
                SFAccountManager.setLoginHostToAppSettings(selectedOption.value);
            }
        } // RadioGroup
        
        TextField {
            id:customHostValue
            text: SFAccountManager.getCustomHostFromAppSettings();
            input {                
                submitKey: SubmitKey.Done;
                onSubmitted: {
                    SFAccountManager.setCustomHostToAppSettings(customHostValue.text);
                }
                flags: TextInputFlag.SpellCheckOff | TextInputFlag.PredictionOff
            }        
            onTextChanged: {
                console.debug("on touch exit");
                SFAccountManager.setCustomHostToAppSettings(customHostValue.text);                
            }
        }
    }
}