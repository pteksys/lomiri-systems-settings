import QtQuick 2.12
import Qt.labs.folderlistmodel 1.0
import SystemSettings 1.0
import SystemSettings.ListItems 1.0 as SettingsListItems
import Lomiri.Components 1.3
import Lomiri.SystemSettings.StorageAbout 1.0

ItemPage {
    id: licensesPage
    objectName: "licensesPage"
    title: i18n.tr("Software licenses")
    flickable: softwareList

    LomiriStorageAboutPanel {
        id: backendInfo
    }

    FolderListModel {
        id: folderModel
        folder: mountPoint + "/usr/share/doc"
    }

    ListView {
        id: softwareList
        anchors.fill: parent
        maximumFlickVelocity: height * 10
        flickDeceleration: height * 2

        model: folderModel
        delegate: SettingsListItems.SingleValueProgression {
            text: fileName
            onClicked: pageStack.addPageToNextColumn(
                licensesPage, Qt.resolvedUrl("License.qml"), {binary: fileName}
            )
        }

    }
}
