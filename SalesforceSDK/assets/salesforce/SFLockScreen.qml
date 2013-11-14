import bb.cascades 1.2

Sheet {
    id:lockScreenSheet
    Page {
        Container {
            TextField {
                id:passcodeField
                inputMode: TextFieldInputMode.Password
                input {                
                    submitKey: SubmitKey.Done;
                    onSubmitted: {
                        lockScreenSheet.validatePasscode(passcodeField.text)
                    }
                    flags: TextInputFlag.SpellCheckOff | TextInputFlag.PredictionOff
                }  
            }
            Button {
                id:forgotPasscodeBtn
                text:qsTr("Forgot Passcode");
                onClicked: {
                    SFSecurityLockout.reset();
                    SFAuthenticationManager.logout();
                }
            }
            Label {
                id:infoLabel
            }
        }
        titleBar: TitleBar {
            title: qsTr("Verify Passcode")
            acceptAction: ActionItem {
                title: "Done"
                onTriggered: {
                    lockScreenSheet.validatePasscode(passcodeField.text)
                }
            }
        }
    }
    peekEnabled:false;
    
    onOpened: {
        passcodeField.requestFocus();    
    }
    
    function validatePasscode(passcode){
        if (!SFSecurityLockout.validatePasscode(passcode)){
         	infoLabel.text = qsTr("The passcode you entered was invalid");
            passcodeField.requestFocus();
            passcodeField.editor.setSelection(0, passcodeField.text.length); 
        }else{
            passcodeField.text = "";
        }
    }
}