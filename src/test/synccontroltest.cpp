// Tests for Master Sync.
// There are no tests for actual deck playback, since I don't know how to mock that out yet.
// The following manual tests should probably be performed:
// * Quantize mode nudges tracks in sync, whether internal or deck master.
// * Flinging tracks with the waveform should work.
// * vinyl??

#include <gtest/gtest-message.h>
#include <gtest/gtest-test-part.h>

#include <memory>

#include "engine/channels/enginedeck.h"
#include "engine/enginebuffer.h"
#include "engine/sync/synccontrol.h"
#include "gtest/gtest_pred_impl.h"
#include "test/mockedenginebackendtest.h"

class SyncControlTest : public MockedEngineBackendTest {
};

TEST_F(SyncControlTest, TestDetermineBpmMultiplier) {
    EXPECT_EQ(SyncControl::kBpmUnity,
            m_pChannel1->getEngineBuffer()->m_pSyncControl->determineBpmMultiplier(70, 80));
    EXPECT_EQ(SyncControl::kBpmHalve,
            m_pChannel1->getEngineBuffer()->m_pSyncControl->determineBpmMultiplier(70, 160));
    EXPECT_EQ(SyncControl::kBpmDouble,
            m_pChannel1->getEngineBuffer()->m_pSyncControl->determineBpmMultiplier(70, 40));
}
