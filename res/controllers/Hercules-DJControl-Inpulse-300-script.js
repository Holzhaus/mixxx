
var DJCi300 = {};
///////////////////////////////////////////////////////////////
//                       USER OPTIONS                        //
///////////////////////////////////////////////////////////////

// How fast scratching is.
DJCi300.scratchScale = 1.0;

// How much faster seeking (shift+scratch) is than scratching.
DJCi300.scratchShiftMultiplier = 4;

// How fast bending is.
DJCi300.bendScale = 1.0;

// DJControl_Inpulse_300_script.js
//
// ***************************************************************************
// * Mixxx mapping script file for the Hercules DJControl Inpulse 300.
// * Author: DJ Phatso, contributions by Kerrick Staley
// * Version 1.2 (December 2019)
// * Forum: https://www.mixxx.org/forums/viewtopic.php?f=7&t=12599
// * Wiki: https://mixxx.org/wiki/doku.php/hercules_djcontrol_inpulse_300
//
// Changes to v1.2
// - Code cleanup.
//
// Changes to v1.1
// - Fix seek-to-start and cue-master behavior.
// - Tweak scratch, seek, and bend behavior.
// - Controller knob/slider values are queried on startup, so MIXXX is synced.
// - Fixed vinyl button behavior the first time it's pressed.
//
// v1.0 : Original forum release
//
// TODO: Functions that could be implemented to the script:
//
// * ROLL: Keep SLIP active (if already enabled) when exiting from rolls
//
// * SLICER/SLICER LOOP
//
// * TONEPLAY
//
// * FX:
//	- Potentially use 1 FX rack for FX pads and another for the Controlled FX
//  - See how to preselect effects for a rack
// * Fix behavior when adjusting tempo slider after pressing [Sync] (tempo adjustment should be relative, not absolute).
// ****************************************************************************

DJCi300.kScratchActionNone = 0;
DJCi300.kScratchActionScratch = 1;
DJCi300.kScratchActionSeek = 2;
DJCi300.kScratchActionBend = 3;

//function DJCi300() {}


DJCi300.vuMeterUpdate = function(value, group, control) {
    value = (value * 127) + 5;
    switch (control) {
    case "VuMeterL":
        midi.sendShortMsg(0xB0, 0x40, value);
        break;
    case "VuMeterR":
        midi.sendShortMsg(0xB0, 0x41, value);
        break;
    }
};

DJCi300.vuMeterUpdateDA = function(value, group, control) {
    value = (value * 127) + 5;
    switch (control) {
    case "[Channel1]", "VuMeter":
        midi.sendShortMsg(0xB1, 0x40, value);
        break;
    }
};

DJCi300.vuMeterUpdateDB = function(value, group, control) {
    value = (value * 127) + 5;
    switch (control) {
    case "[Channel2]", "VuMeter":
        midi.sendShortMsg(0xB2, 0x40, value);
        break;
    }
};




DJCi300.init = function() {
    DJCi300.scratchButtonState = true;
    DJCi300.scratchAction = {
        1: DJCi300.kScratchActionNone,
        2: DJCi300.kScratchActionNone
    };


    // Turn On Vinyl buttons LED(one for each deck).
    midi.sendShortMsg(0x91, 0x03, 0x7F);
    midi.sendShortMsg(0x92, 0x03, 0x7F);

    //Turn On Browser button LED
    midi.sendShortMsg(0x90, 0x05, 0x10);



    // Connect the VUMeters
    engine.connectControl("[Channel1]", "VuMeter", "DJCi300.vuMeterUpdateDA");
    engine.connectControl("[Channel2]", "VuMeter", "DJCi300.vuMeterUpdateDB");
    engine.connectControl("[Master]", "VuMeterL", "DJCi300.vuMeterUpdate");
    engine.connectControl("[Master]", "VuMeterR", "DJCi300.vuMeterUpdate");

    // Connect the Browser LEDs
    engine.getValue("[Library]", "MoveFocus");
    engine.getValue("[Master]", "maximize_library");

    // Ask the controller to send all current knob/slider values over MIDI, which will update
    // the corresponding GUI controls in MIXXX.
    midi.sendShortMsg(0xB0, 0x7F, 0x7F);
};


// The Vinyl button, used to enable or disable scratching on the jog wheels (One per deck).

DJCi300.vinylButtonDA = function(channel, control, value, _status, _group) {
    if (value) {
        if (DJCi300.scratchButtonState) {
            DJCi300.scratchButtonState = false;
            midi.sendShortMsg(0x91, 0x03, 0x00);

        } else {
            DJCi300.scratchButtonState = true;
            midi.sendShortMsg(0x91, 0x03, 0x7F);
        }
    }
};

DJCi300.vinylButtonDB = function(channel, control, value, _status, _group) {
    if (value) {
        if (DJCi300.scratchButtonState) {
            DJCi300.scratchButtonState = false;
            midi.sendShortMsg(0x92, 0x03, 0x00);

        } else {
            DJCi300.scratchButtonState = true;
            midi.sendShortMsg(0x92, 0x03, 0x7F);
        }
    }
};


DJCi300._scratchEnable = function(deck) {
    var alpha = 1.0/8;
    var beta = alpha/32;
    engine.scratchEnable(deck, 248, 33 + 1/3, alpha, beta);
};

DJCi300._convertWheelRotation = function(value) {
    // When you rotate the jogwheel, the controller always sends either 0x1
    // (clockwise) or 0x7F (counter clockwise). 0x1 should map to 1, 0x7F
    // should map to -1 (IOW it's 7-bit signed).
    return value < 0x40 ? 1 : -1;
};

// The touch action on the jog wheel's top surface
DJCi300.wheelTouch = function(channel, control, value, _status, _group) {
    var deck = channel;
    if (value > 0) {
        //  Touching the wheel.
        if (engine.getValue("[Channel" + deck + "]", "play") !== 1 || DJCi300.scratchButtonState) {
            DJCi300._scratchEnable(deck);
            DJCi300.scratchAction[deck] = DJCi300.kScratchActionScratch;
        } else {
            DJCi300.scratchAction[deck] = DJCi300.kScratchActionBend;
        }
    } else {
        // Released the wheel.
        engine.scratchDisable(deck);
        DJCi300.scratchAction[deck] = DJCi300.kScratchActionNone;
    }
};


// The touch action on the jog wheel's top surface while holding shift
DJCi300.wheelTouchShift = function(channel, control, value, _status, _group) {
    var deck = channel - 3;
    // We always enable scratching regardless of button state.
    if (value > 0) {
        DJCi300._scratchEnable(deck);
        DJCi300.scratchAction[deck] = DJCi300.kScratchActionSeek;
    } else {
        // Released the wheel.
        engine.scratchDisable(deck);
        DJCi300.scratchAction[deck] = DJCi300.kScratchActionNone;
    }
};

// Scratching on the jog wheel (rotating it while pressing the top surface)
DJCi300._scratchWheelImpl = function(deck, value) {
    var interval = DJCi300._convertWheelRotation(value);
    var scratchAction = DJCi300.scratchAction[deck];

    if (scratchAction === DJCi300.kScratchActionScratch) {
        engine.scratchTick(deck, interval * DJCi300.scratchScale);
    } else if (scratchAction === DJCi300.kScratchActionSeek) {
        engine.scratchTick(deck,
            interval *  DJCi300.scratchScale *
            DJCi300.scratchShiftMultiplier);
    } else {
        DJCi300._bendWheelImpl(deck, value);
    }
};

// Scratching on the jog wheel (rotating it while pressing the top surface)
DJCi300.scratchWheel = function(channel, control, value, _status, _group) {
    var deck = channel;
    DJCi300._scratchWheelImpl(deck, value);
};

// Seeking on the jog wheel (rotating it while pressing the top surface and holding Shift)
DJCi300.scratchWheelShift = function(channel, control, value, _status, _group) {
    var deck = channel - 3;
    DJCi300._scratchWheelImpl(deck, value);
};

DJCi300._bendWheelImpl = function(deck, value) {
    var interval = DJCi300._convertWheelRotation(value);
    engine.setValue("[Channel" + deck + "]", "jog",
        interval * DJCi300.bendScale);
};

// Bending on the jog wheel (rotating using the edge)
DJCi300.bendWheel = function(channel, control, value, _status, _group) {
    var deck = channel;
    DJCi300._bendWheelImpl(deck, value);
};

DJCi300.scratchPad = function(channel, control, value, _status, _group) {
    var deck = channel;
    DJCi300._scratchWheelImpl(deck, value);
};

DJCi300.shutdown = function() {
    midi.sendShortMsg(0xB0, 0x7F, 0x00);
};
