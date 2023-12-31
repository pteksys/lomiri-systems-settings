import QtQuick 2.12
import QtQuick.Layouts 1.1
import Lomiri.Components 1.3
import Lomiri.Components.Popups 1.3
import Lomiri.Content 1.3

PopupBase  {
    id: picker
    anchorToKeyboard: false

    signal fileImportSignal (var file)
    property var activeTransfer

    Rectangle {
        anchors.fill: parent

        ContentTransferHint {
            id: transferHint
            anchors.fill: parent
            activeTransfer: picker.activeTransfer
        }

        ContentStore {
            id: appStore
            scope: ContentScope.App
        }

        ContentPeerPicker {
            id: peerPicker
            anchors.fill: parent
            visible: true
            contentType: ContentType.Documents
            handler: ContentHandler.Source
            onPeerSelected: {
                peer.selectionType = ContentTransfer.Single;
                picker.activeTransfer = peer.request(appStore);
            }
            onCancelPressed: PopupUtils.close(picker)

            Component.onCompleted: {
                peerPicker.forceActiveFocus();
            }
        }
    }

    Connections {
        target: picker.activeTransfer ? picker.activeTransfer : null
        onStateChanged: {
            if (picker.activeTransfer.state === ContentTransfer.Charged) {
                if (picker.activeTransfer.items.length > 0) {
                    var fileUrl = picker.activeTransfer.items[0].url;
                    picker.fileImportSignal(
                        fileUrl.toString().replace("file://", "")
                    );
                    PopupUtils.close(picker);
                }
            } else if (picker.activeTransfer.state === ContentTransfer.Aborted){
                picker.fileImportSignal(false);
                PopupUtils.close(picker);
            }
        }
    }
}
