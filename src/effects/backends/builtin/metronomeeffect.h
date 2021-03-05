#pragma once

#include <QMap>

#include "effects/backends/effectprocessor.h"
#include "engine/effects/engineeffect.h"
#include "engine/effects/engineeffectparameter.h"
#include "engine/filters/enginefilterpansingle.h"
#include "util/class.h"
#include "util/defs.h"
#include "util/sample.h"
#include "util/types.h"

class MetronomeGroupState final : public EffectState {
  public:
    MetronomeGroupState(const mixxx::EngineParameters& bufferParameters)
            : EffectState(bufferParameters),
              m_framesSinceClickStart(0) {
    }
    ~MetronomeGroupState() {
    }

    SINT m_framesSinceClickStart;
};

class MetronomeEffect : public EffectProcessorImpl<MetronomeGroupState> {
  public:
    MetronomeEffect() = default;
    virtual ~MetronomeEffect();

    static QString getId();
    static EffectManifestPointer getManifest();

    void loadEngineEffectParameters(
            const QMap<QString, EngineEffectParameterPointer>& parameters) override;

    void processChannel(
            MetronomeGroupState* pState,
            const CSAMPLE* pInput,
            CSAMPLE* pOutput,
            const mixxx::EngineParameters& bufferParameters,
            const EffectEnableState enableState,
            const GroupFeatureState& groupFeatures) override;

  private:
    EngineEffectParameterPointer m_pBpmParameter;
    EngineEffectParameterPointer m_pSyncParameter;

    DISALLOW_COPY_AND_ASSIGN(MetronomeEffect);
};