import "." as Skin
import Mixxx 0.1 as Mixxx

Skin.Button {
    id: root

    property alias group: control.group
    property alias key: control.key
    property bool toggleable: false

    function toggle() {
        control.value = !control.value;
    }

    //highlight: control.value
    onPressed: {
        if (toggleable)
            toggle();
        else
            control.value = 1;
    }
    onReleased: {
        if (!toggleable)
            control.value = 0;

    }

    Mixxx.ControlProxy {
        id: control
    }

}
