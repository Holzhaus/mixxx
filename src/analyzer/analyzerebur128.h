#ifndef ANALYZER_ANALYZEREBUR128_H_
#define ANALYZER_ANALYZEREBUR128_H_

#include <ebur128.h>

#include "analyzer/analyzer.h"
#include "preferences/replaygainsettings.h"
#include "preferences/usersettings.h"
#include "track/track_decl.h"
#include "util/types.h"

class AnalyzerEbur128 : public Analyzer {
  public:
    AnalyzerEbur128(UserSettingsPointer pConfig);
    ~AnalyzerEbur128() override;

    static bool isEnabled(const ReplayGainSettings& rgSettings) {
        return rgSettings.isAnalyzerEnabled(2);
    }

    bool initialize(TrackPointer tio, int sampleRate, int totalSamples) override;
    bool processSamples(const CSAMPLE* pIn, const int iLen) override;
    void storeResults(TrackPointer tio) override;
    void cleanup() override;

  private:
    ReplayGainSettings m_rgSettings;
    ebur128_state* m_pState;
};

#endif /* ANALYZER_ANALYZEREBUR128_H_ */
