#include "effects/backends/builtin/phasereffect.h"

#include <QDebug>

#include "util/math.h"

namespace {
constexpr unsigned int updateCoef = 32;
constexpr auto kDoublePi = static_cast<CSAMPLE>(2.0 * M_PI);
} // namespace

// static
QString PhaserEffect::getId() {
    return "org.mixxx.effects.phaser";
}

// static
EffectManifestPointer PhaserEffect::getManifest() {
    EffectManifestPointer pManifest(new EffectManifest());
    pManifest->setId(getId());
    pManifest->setName(QObject::tr("Phaser"));
    pManifest->setShortName(QObject::tr("Phaser"));
    pManifest->setAuthor("The Mixxx Team");
    pManifest->setVersion("1.0");
    pManifest->setDescription(QObject::tr(
            "Mixes the input signal with a copy passed through a series of "
            "all-pass filters to create comb filtering"));
    pManifest->setEffectRampsFromDry(true);

    EffectManifestParameterPointer period = pManifest->addParameter();
    period->setId("lfo_period");
    period->setName(QObject::tr("Period"));
    period->setShortName(QObject::tr("Period"));
    period->setDescription(QObject::tr(
            "Period of the LFO (low frequency oscillator)\n"
            "1/4 - 4 beats rounded to 1/2 beat if tempo is detected\n"
            "1/4 - 4 seconds if no tempo is detected"));
    period->setValueScaler(EffectManifestParameter::ValueScaler::LINEAR);
    period->setSemanticHint(EffectManifestParameter::SemanticHint::UNKNOWN);
    period->setUnitsHint(EffectManifestParameter::UnitsHint::BEATS);
    period->setRange(0.0, 1.0, 4.0);

    EffectManifestParameterPointer fb = pManifest->addParameter();
    fb->setId("feedback");
    fb->setName(QObject::tr("Feedback"));
    fb->setShortName(QObject::tr("Feedback"));
    fb->setDescription(QObject::tr(
            "Controls how much of the output signal is looped"));
    fb->setValueScaler(EffectManifestParameter::ValueScaler::LINEAR);
    fb->setSemanticHint(EffectManifestParameter::SemanticHint::UNKNOWN);
    fb->setUnitsHint(EffectManifestParameter::UnitsHint::UNKNOWN);
    fb->setRange(-1.0, 0.0, 1.0);

    EffectManifestParameterPointer range = pManifest->addParameter();
    range->setId("range");
    range->setName(QObject::tr("Range"));
    range->setShortName(QObject::tr("Range"));
    range->setDescription(QObject::tr(
            "Controls the frequency range across which the notches sweep."));
    range->setValueScaler(EffectManifestParameter::ValueScaler::LINEAR);
    range->setSemanticHint(EffectManifestParameter::SemanticHint::UNKNOWN);
    range->setUnitsHint(EffectManifestParameter::UnitsHint::UNKNOWN);
    range->setRange(0.05, 1.0, 1.0);

    EffectManifestParameterPointer stages = pManifest->addParameter();
    stages->setId("stages");
    stages->setName(QObject::tr("Stages"));
    stages->setShortName(QObject::tr("Stages"));
    stages->setDescription(QObject::tr(
            "Number of stages")); // stages of what?
    stages->setValueScaler(EffectManifestParameter::ValueScaler::LINEAR);
    stages->setSemanticHint(EffectManifestParameter::SemanticHint::UNKNOWN);
    stages->setUnitsHint(EffectManifestParameter::UnitsHint::UNKNOWN);
    stages->setRange(1.0, 3.5, 6.0);

    EffectManifestParameterPointer depth = pManifest->addParameter();
    depth->setId("depth");
    depth->setName(QObject::tr("Depth"));
    depth->setShortName(QObject::tr("Depth"));
    depth->setDescription(QObject::tr(
            "Intensity of the effect"));
    depth->setValueScaler(EffectManifestParameter::ValueScaler::LINEAR);
    depth->setSemanticHint(EffectManifestParameter::SemanticHint::UNKNOWN);
    depth->setUnitsHint(EffectManifestParameter::UnitsHint::UNKNOWN);
    depth->setDefaultLinkType(EffectManifestParameter::LinkType::LINKED);
    depth->setRange(0.5, 0.5, 1.0);

    EffectManifestParameterPointer triplet = pManifest->addParameter();
    triplet->setId("triplet");
    triplet->setName(QObject::tr("Triplets"));
    triplet->setShortName(QObject::tr("Triplets"));
    triplet->setDescription(QObject::tr(
            "Divide rounded 1/2 beats of the Period parameter by 3."));
    triplet->setValueScaler(EffectManifestParameter::ValueScaler::TOGGLE);
    triplet->setSemanticHint(EffectManifestParameter::SemanticHint::UNKNOWN);
    triplet->setUnitsHint(EffectManifestParameter::UnitsHint::UNKNOWN);
    triplet->setRange(0, 0, 1);

    EffectManifestParameterPointer stereo = pManifest->addParameter();
    stereo->setId("stereo");
    stereo->setName(QObject::tr("Stereo"));
    stereo->setShortName(QObject::tr("Stereo"));
    stereo->setDescription(QObject::tr(
            "Sets the LFOs (low frequency oscillators) for the left and right "
            "channels out of phase with each others"));
    stereo->setValueScaler(EffectManifestParameter::ValueScaler::TOGGLE);
    stereo->setSemanticHint(EffectManifestParameter::SemanticHint::UNKNOWN);
    stereo->setUnitsHint(EffectManifestParameter::UnitsHint::UNKNOWN);
    stereo->setRange(0, 0, 1);

    return pManifest;
}

void PhaserEffect::loadEngineEffectParameters(
        const QMap<QString, EngineEffectParameterPointer>& parameters) {
    m_pStagesParameter = parameters.value("stages");
    m_pLFOPeriodParameter = parameters.value("lfo_period");
    m_pDepthParameter = parameters.value("depth");
    m_pFeedbackParameter = parameters.value("feedback");
    m_pRangeParameter = parameters.value("range");
    m_pTripletParameter = parameters.value("triplet");
    m_pStereoParameter = parameters.value("stereo");
}

PhaserEffect::~PhaserEffect() {
    //qDebug() << debugString() << "destroyed";
}

void PhaserEffect::processChannel(
        PhaserGroupState* pState,
        const CSAMPLE* pInput,
        CSAMPLE* pOutput,
        const mixxx::EngineParameters& bufferParameters,
        const EffectEnableState enableState,
        const GroupFeatureState& groupFeatures) {
    if (enableState == EffectEnableState::Enabling) {
        pState->clear();
    }

    CSAMPLE depth = 0;
    if (enableState != EffectEnableState::Disabling) {
        depth = static_cast<CSAMPLE>(m_pDepthParameter->value());
    }

    double periodParameter = m_pLFOPeriodParameter->value();
    double periodSamples;
    if (groupFeatures.has_beat_length_sec) {
        // periodParameter is a number of beats
        periodParameter = std::max(roundToFraction(periodParameter, 2.0), 1 / 4.0);
        if (m_pTripletParameter->toBool()) {
            periodParameter /= 3.0;
        }
        periodSamples = periodParameter * groupFeatures.beat_length_sec *
                bufferParameters.sampleRate();
    } else {
        // periodParameter is a number of seconds
        periodSamples = std::max(periodParameter, 1 / 4.0) * bufferParameters.sampleRate();
    }
    // freqSkip is used to calculate the phase independently for each channel,
    // so do not multiply periodSamples by the number of channels.
    const auto freqSkip = static_cast<CSAMPLE>(1.0f / periodSamples * kDoublePi);

    const auto feedback = static_cast<CSAMPLE>(m_pFeedbackParameter->value());
    const auto range = static_cast<CSAMPLE>(m_pRangeParameter->value());
    const auto stages = static_cast<int>(2 * m_pStagesParameter->value());

    CSAMPLE* oldInLeft = pState->oldInLeft;
    CSAMPLE* oldOutLeft = pState->oldOutLeft;
    CSAMPLE* oldInRight = pState->oldInRight;
    CSAMPLE* oldOutRight = pState->oldOutRight;

    // Using two sets of coefficients for left and right channel
    CSAMPLE filterCoefLeft = 0;
    CSAMPLE filterCoefRight = 0;

    CSAMPLE left = 0, right = 0;

    CSAMPLE_GAIN oldDepth = pState->oldDepth;
    const CSAMPLE_GAIN depthDelta = (depth - oldDepth) / bufferParameters.framesPerBuffer();
    const CSAMPLE_GAIN depthStart = oldDepth + depthDelta;

    const auto stereoCheck = static_cast<int>(m_pStereoParameter->value());
    int counter = 0;

    for (SINT i = 0;
            i < bufferParameters.samplesPerBuffer();
            i += bufferParameters.channelCount()) {
        left = pInput[i] + tanh(left * feedback);
        right = pInput[i + 1] + tanh(right * feedback);

        // For stereo enabled, the channels are out of phase
        pState->leftPhase = fmodf(pState->leftPhase + freqSkip, kDoublePi);
        pState->rightPhase = fmodf(
                pState->rightPhase + freqSkip + static_cast<float>(M_PI) * stereoCheck,
                kDoublePi);

        // Updating filter coefficients once every 'updateCoef' samples to avoid
        // extra computing
        if ((counter++) % updateCoef == 0) {
            const auto delayLeft = static_cast<CSAMPLE>(0.5 + 0.5 * sin(pState->leftPhase));
            const auto delayRight = static_cast<CSAMPLE>(0.5 + 0.5 * sin(pState->rightPhase));

            // Coefficient computing based on the following:
            // https://ccrma.stanford.edu/~jos/pasp/Classic_Virtual_Analog_Phase.html
            CSAMPLE wLeft = range * delayLeft;
            CSAMPLE wRight = range * delayRight;

            CSAMPLE tanwLeft = tanh(wLeft / 2);
            CSAMPLE tanwRight = tanh(wRight / 2);

            filterCoefLeft = (1.0f - tanwLeft) / (1.0f + tanwLeft);
            filterCoefRight = (1.0f - tanwRight) / (1.0f + tanwRight);
        }

        left = processSample(left, oldInLeft, oldOutLeft, filterCoefLeft, stages);
        right = processSample(right, oldInRight, oldOutRight, filterCoefRight, stages);

        const CSAMPLE_GAIN depth = depthStart + depthDelta * (i / bufferParameters.channelCount());

        // Computing output combining the original and processed sample
        pOutput[i] = pInput[i] * (1.0f - 0.5f * depth) + left * depth * 0.5f;
        pOutput[i + 1] = pInput[i + 1] * (1.0f - 0.5f * depth) + right * depth * 0.5f;
    }

    pState->oldDepth = depth;
}