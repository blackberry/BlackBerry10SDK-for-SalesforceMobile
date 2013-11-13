import bb.cascades 1.2

Sheet {
    id:createPassCodeSheet
    Page {
        Container {
            Label {
                text:"New Passcode"
            }
            TextField {
                id:newPasscode
                inputMode: TextFieldInputMode.Password
                input {                
                    flags: TextInputFlag.SpellCheckOff | TextInputFlag.PredictionOff
                }
                onFocusedChanged: {
                    if (focused) {
                        newPasscode.editor.setSelection(0, newPasscode.text.length);
                    }
                }
            }            
            Label {
                text:"Confirm Passcode"
            }
            TextField {
                id: confirmedPasscode
                inputMode: TextFieldInputMode.Password
                input {                
                    submitKey: SubmitKey.Done;
                    onSubmitted: {
                        createPassCodeSheet.confirmPasscode();
                    }
                    flags: TextInputFlag.SpellCheckOff | TextInputFlag.PredictionOff
                }  
                onFocusedChanged: {
                    if (focused) {
                        confirmedPasscode.editor.setSelection(0, confirmedPasscode.text.length);
                    }
                }      
            }
            Label {
                id:infoLabel;
            }
        }
        titleBar: TitleBar {
            title: qsTr("Create Passcode")
            acceptAction: ActionItem {
                title: "Done"
                onTriggered: {
                    createPassCodeSheet.confirmPasscode();
                }
            }
        } 
    }//page
    peekEnabled: false;
    
    onOpened: {
        newPasscode.requestFocus();
    }
    function confirmPasscode() {
        if (newPasscode.text == confirmedPasscode.text){
            if (newPasscode.text.length >= SFSecurityLockout.requiredPasscodeLength()){                
                SFSecurityLockout.savePasscode(newPasscode.text);   
                newPasscode.text = "";
                confirmedPasscode.text = "";   
            }else{
                infoLabel.setText(qsTr("Your passcode must be at least " + SFSecurityLockout.requiredPasscodeLength() + " characters long"));
                newPasscode.requestFocus();
            }
        }else{
            infoLabel.setText(qsTr("Passcodes do not match"));
            newPasscode.requestFocus();   
        }
    }
}