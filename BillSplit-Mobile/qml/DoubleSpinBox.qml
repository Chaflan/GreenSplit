import QtQuick 2.0
import QtQuick.Controls 2.1

// A homebrew to make up for QML's lack of a fundamental control
// mostly adapted from the documentation, but that version doesn't work
SpinBox {
    id: spinbox
    from: 0
    value: 0
    to: 9999999
    stepSize: 100
    anchors.centerIn: parent
    editable: true

    property int decimals: 2
    property real realValue: value / 100  // Doesn't work

    validator: DoubleValidator {
        bottom: Math.min(spinbox.from, spinbox.to)
        top:  Math.max(spinbox.from, spinbox.to)
    }

    textFromValue: function(value, locale) {
        return Number(value / 100).toLocaleString(locale, 'f', spinbox.decimals)
    }

    valueFromText: function(text, locale) {
        return Number.fromLocaleString(locale, text) * 100
    }

    onDisplayTextChanged: {
        value = Number(displayText * 100)
    }
}




//import QtQuick 2.0
//import QtQuick.Controls 2.2
////import QmlUtils 1.0

//Item
//{
//    id: doublespinbox
//    width: 140
//    height: 40
//    property int decimals: 1
//    property alias value: valuePreview.value
//    property real from: 0
//    property real to: 99
//    property real stepSize: 1
//    property alias editable: spinbox.editable
//    property alias font: spinbox.font

//    SpinBox
//    {
//        id: spinbox
//        property bool init: false
//        property real factor: Math.pow(10, decimals)

//        function setValue(preview)
//        {
//            init = true
//            value = preview.value * factor
//            init = false
//            preview.value = value / factor
//        }

////        DoubleValuePreview
////        {
////            id: valuePreview
////            onValuePreview: spinbox.setValue(preview)
////        }

//        anchors.fill: parent
//        editable: true
//        stepSize: doublespinbox.stepSize * factor
//        to : doublespinbox.to * factor
//        from : doublespinbox.from * factor

//        onValueChanged:
//        {
//            if (init)
//                return

//            valuePreview.setValueDirect(value / factor)
//        }

//        validator: DoubleValidator
//        {
//            bottom: Math.min(spinbox.from, spinbox.to)
//            top: Math.max(spinbox.from, spinbox.to)
//        }

//        textFromValue: function(value, locale)
//        {
//            return Number(value / factor).toLocaleString(locale, 'f', doublespinbox.decimals)
//        }

//        valueFromText: function(text, locale)
//        {
//            doublespinbox.value = Number.fromLocaleString(locale, text)
//            return doublespinbox.value * factor
//        }
//    }
//}
