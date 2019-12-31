////////////////////////////////////////////////////////////////////////
// Controller: Reloop Digital Jockey 2
// URL:        na razie nic
// Author:     DJ aK
// Credits:    Uwe Klotz a/k/a tapir (baseline: Denon MC6000MK2 script)
////////////////////////////////////////////////////////////////////////

var RDJ2 = {};


////////////////////////////////////////////////////////////////////////
// Tunable constants                                                  //
////////////////////////////////////////////////////////////////////////

RDJ2.JOG_SPIN_CUE_PEAK = 0.2; // [0.0, 1.0]
RDJ2.JOG_SPIN_CUE_EXPONENT = 0.7; // 1.0 = linear response

RDJ2.JOG_SPIN_PLAY_PEAK = 0.3; // [0.0, 1.0]
RDJ2.JOG_SPIN_PLAY_EXPONENT = 0.7; // 1.0 = linear response

RDJ2.JOG_SCRATCH_RPM = 33.333333; // 33 1/3
RDJ2.JOG_SCRATCH_ALPHA = 0.125; // 1/8
RDJ2.JOG_SCRATCH_BETA = RDJ2.JOG_SCRATCH_ALPHA / 32.0;
RDJ2.JOG_SCRATCH_RAMP = true; // required for back spins

// Seeking: Number of revolutions needed to seek from the beginning
// to the end of the track.
RDJ2.JOG_SEEK_REVOLUTIONS = 2;


////////////////////////////////////////////////////////////////////////
// Fixed constants                                                    //
////////////////////////////////////////////////////////////////////////

// Controller constants
RDJ2.DECK_COUNT = 2;
RDJ2.JOG_RESOLUTION = 600; // measured/estimated


// Jog constants
RDJ2.MIDI_JOG_DELTA_BIAS = 0x40; // center value of relative movements
RDJ2.MIDI_JOG_DELTA_RANGE = 0x3F; // both forward (= positive) and reverse (= negative)

// Mixxx constants
RDJ2.MIXXX_JOG_RANGE = 3.0;


////////////////////////////////////////////////////////////////////////
// Logging functions                                                  //
////////////////////////////////////////////////////////////////////////

RDJ2.logDebug = function (msg) {
    if (RDJ2.debug) {
        print("[" + RDJ2.id + " DEBUG] " + msg);
    }
};

RDJ2.logInfo = function (msg) {
    print("[" + RDJ2.id + " INFO] " + msg);
};

RDJ2.logWarning = function (msg) {
    print("[" + RDJ2.id + " WARNING] " + msg);
};

RDJ2.logError = function (msg) {
    print("[" + RDJ2.id + " ERROR] " + msg);
};


////////////////////////////////////////////////////////////////////////
// Buttons                                                            //
////////////////////////////////////////////////////////////////////////

RDJ2.MIDI_BUTTON_ON = 0x7F;
RDJ2.MIDI_BUTTON_OFF = 0x00;

RDJ2.isButtonPressed = function (midiValue) {
    switch (midiValue) {
        case RDJ2.MIDI_BUTTON_ON:
            return true;
        case RDJ2.MIDI_BUTTON_OFF:
            return false;
        default:
            RDJ2.logError("Unexpected MIDI button value: " + midiValue);
            return undefined;
    }
};

/* This map is necessary as Reloop has designed the controller in such
   a way that not all buttons/knobs have the same offset comparing
   CH0 and CH1. By looking at the MIDI messages sent by the controller,
   we can see that the hardware is designed as symmetric halves.
   
   In other words, constant offset in hardware corresponds to symmetric
   halves, but the controller layout is not fully symmetric.
   (e.x. ACTIVATE 1 buttons)

   Thus we need a map to preserve object oriented approach .*/
RDJ2.BUTTONMAP_CH0_CH1 = {
    play: [0x19, 0x55],
    play2: [0x19, 0x55]
}

////////////////////////////////////////////////////////////////////////
// Controls                                                           //
////////////////////////////////////////////////////////////////////////

//code removed - looks as deprecated

////////////////////////////////////////////////////////////////////////
// Decks                                                              //
////////////////////////////////////////////////////////////////////////

/* Management */

//TODO REMOVE
// RDJ2.decksByGroup = {};

// RDJ2.getDeckByGroup = function (group) {
//     var deck = RDJ2.decksByGroup[group];
//     if (undefined === deck) {
//         RDJ2.logError("No deck found for " + group);
//     }
//     return deck;
// };

/* Constructor */

RDJ2.Deck = function (number) {
    RDJ2.logDebug("Creating Deck " + number);

    this.number = number;
    this.group = "[Channel" + number + "]";
    this.filterGroup = "[QuickEffectRack1_" + this.group + "_Effect1]";
    this.jogTouchState = false;
    this.rateDirBackup = this.getValue("rate_dir");
    this.setValue("rate_dir", -1);
    this.vinylMode = undefined;
    this.syncMode = undefined;

    components.Deck.call(this, 1);

    this.playButton = new components.PlayButton([0x90, RDJ2.BUTTONMAP_CH0_CH1.play[number - 1]]);
    //this.cueButton = new components.CueButton([0x90, 0x02]);
    //this.syncButton = new components.SyncButton([0x90, 0x03]);

    // Set the group properties of the above Components and connect their output callback functions
    // Without this, the group property for each Component would have to be specified to its
    // constructor.
    this.reconnectComponents(function (component) {
        if (component.group === undefined) {
            // 'this' inside a function passed to reconnectComponents refers to the ComponentContainer
            // so 'this' refers to the custom Deck object being constructed
            component.group = this.currentDeck;
        }
    });
};

// give our custom Deck all the methods of the generic Deck in the Components library
RDJ2.Deck.prototype = Object.create(components.Deck.prototype);

/* Values & Parameters */

RDJ2.Deck.prototype.getValue = function (key) {
    return engine.getValue(this.group, key);
};

RDJ2.Deck.prototype.setValue = function (key, value) {
    engine.setValue(this.group, key, value);
};

RDJ2.Deck.prototype.toggleValue = function (key) {
    this.setValue(key, !this.getValue(key));
};

RDJ2.Deck.prototype.setParameter = function (key, param) {
    engine.setParameter(this.group, key, param);
};

RDJ2.Deck.prototype.triggerValue = function (key) {
    engine.trigger(this.group, key);
};

/* Cue & Play */

RDJ2.Deck.prototype.isPlaying = function () {
    return this.getValue("play");
};

/* Pitch Bend / Track Search */

RDJ2.Deck.prototype.onBendPlusButton = function (isButtonPressed) {
    if (this.isPlaying()) {
        this.setValue("fwd", false);
        /*if (this.getShiftState()) {
            this.setValue("rate_temp_up_small", isButtonPressed);
        } else*/ {
            this.setValue("rate_temp_up", isButtonPressed);
        }
    } else {
        this.setValue("fwd", isButtonPressed);
    }
};

RDJ2.Deck.prototype.onBendMinusButton = function (isButtonPressed) {
    if (this.isPlaying()) {
        this.setValue("back", false);
        /*if (this.getShiftState()) {
            this.setValue("rate_temp_down_small", isButtonPressed);
        } else*/ {
            this.setValue("rate_temp_down", isButtonPressed);
        }
    } else {
        this.setValue("back", isButtonPressed);
    }
};

/* Vinyl Mode (Scratching) */

RDJ2.Deck.prototype.onVinylModeValue = function () {
    //this.vinylModeLed.setStateBoolean(this.vinylMode);
};

RDJ2.Deck.prototype.enableScratching = function () {
};

RDJ2.Deck.prototype.disableScratching = function () {
};

RDJ2.Deck.prototype.updateVinylMode = function () {
    if (this.vinylMode && this.jogTouchState) {
        engine.scratchEnable(this.number,
            RDJ2.JOG_RESOLUTION,
            RDJ2.JOG_SCRATCH_RPM,
            RDJ2.JOG_SCRATCH_ALPHA,
            RDJ2.JOG_SCRATCH_BETA,
            RDJ2.JOG_SCRATCH_RAMP);
    } else {
        engine.scratchDisable(this.number,
            RDJ2.JOG_SCRATCH_RAMP);
    }
    this.onVinylModeValue();
};

RDJ2.Deck.prototype.setVinylMode = function (vinylMode) {
    this.vinylMode = vinylMode;
    this.updateVinylMode();
};

RDJ2.Deck.prototype.toggleVinylMode = function () {
    this.setVinylMode(!this.vinylMode);
};

RDJ2.Deck.prototype.enableVinylMode = function () {
    this.setVinylMode(true);
};

RDJ2.Deck.prototype.disableVinylMode = function () {
    this.setVinylMode(false);
};

RDJ2.Deck.prototype.onVinylButton = function (isButtonPressed) {
    this.toggleVinylMode();
};

/* Jog Wheel */

RDJ2.Deck.prototype.touchJog = function (isJogTouched) {
    this.jogTouchState = isJogTouched;
    this.updateVinylMode();
};

RDJ2.Deck.prototype.spinJog = function (jogDelta) {
    if (/*this.getShiftState() &&*/ this.jogTouchState && !this.isPlaying()) {
        // fast track seek (strip search)
        var playPos = engine.getValue(this.group, "playposition");
        if (undefined !== playPos) {
            var seekPos = playPos + (jogDelta / (RDJ2.JOG_RESOLUTION * RDJ2.JOG_SEEK_REVOLUTIONS));
            this.setValue("playposition", Math.max(0.0, Math.min(1.0, seekPos)));
        }
    } else {
        if (engine.isScratching(this.number)) {
            engine.scratchTick(this.number, jogDelta);
        } else {
            var normalizedDelta = jogDelta / RDJ2.MIDI_JOG_DELTA_RANGE;
            var scaledDelta;
            var jogExponent;
            if (this.isPlaying()) {
                // bending
                scaledDelta = normalizedDelta / RDJ2.JOG_SPIN_PLAY_PEAK;
                jogExponent = RDJ2.JOG_SPIN_PLAY_EXPONENT;
            } else {
                // cueing
                scaledDelta = normalizedDelta / RDJ2.JOG_SPIN_CUE_PEAK;
                jogExponent = RDJ2.JOG_SPIN_CUE_EXPONENT;
            }
            var direction;
            var scaledDeltaAbs;
            if (scaledDelta < 0.0) {
                direction = -1.0;
                scaledDeltaAbs = -scaledDelta;
            } else {
                direction = 1.0;
                scaledDeltaAbs = scaledDelta;
            }
            var scaledDeltaPow = direction * Math.pow(scaledDeltaAbs, jogExponent);
            var jogValue = RDJ2.MIXXX_JOG_RANGE * scaledDeltaPow;
            this.setValue("jog", jogValue);
        }
    }
};

/* MIDI Input Callbacks */

RDJ2.Deck.prototype.recvBendPlusButton = function (channel, control, value) {
    this.onBendPlusButton(RDJ2.isButtonPressed(value));
};

RDJ2.Deck.prototype.recvBendMinusButton = function (channel, control, value) {
    this.onBendMinusButton(RDJ2.isButtonPressed(value));
};

RDJ2.Deck.prototype.recvJogTouch = function (channel, control, value) {
    this.touchJog(RDJ2.isButtonPressed(value));
};

RDJ2.Deck.prototype.recvJogSpin = function (channel, control, value) {
    this.spinJog(RDJ2.getJogDeltaValue(value));
};


////////////////////////////////////////////////////////////////////////
// Controller functions                                               //
////////////////////////////////////////////////////////////////////////

RDJ2.group = "[Master]";

RDJ2.getValue = function (key) {
    return engine.getValue(RDJ2.group, key);
};

RDJ2.setValue = function (key, value) {
    engine.setValue(RDJ2.group, key, value);
};

RDJ2.getJogDeltaValue = function (value) {
    if (0x00 === value) {
        return 0x00;
    } else {
        return value - RDJ2.MIDI_JOG_DELTA_BIAS;
    }
};


////////////////////////////////////////////////////////////////////////
// MIDI callback functions without a group                            //
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
// MIDI [Channel<n>] callback functions                               //
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
// Mixxx connected controls callback functions                        //
////////////////////////////////////////////////////////////////////////

//some out of context code was here (?)

////////////////////////////////////////////////////////////////////////
// Mixxx Callback Functions                                           //
////////////////////////////////////////////////////////////////////////

RDJ2.init = function (id, debug) {
    RDJ2.id = id;
    RDJ2.debug = debug;
    
    RDJ2.logInfo("Initializing controller");

    // left deck
    RDJ2.leftDeck = new RDJ2.Deck(1);
    // right deck
    RDJ2.rightDeck = new RDJ2.Deck(2);
};

RDJ2.shutdown = function () {
    RDJ2.logInfo("Shutting down controller");
};