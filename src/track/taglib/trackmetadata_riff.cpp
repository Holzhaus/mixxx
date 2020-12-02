#include "track/taglib/trackmetadata_riff.h"

#include <infotag.h>

#include "track/taglib/trackmetadata_common.h"

namespace mixxx {
class TrackMetadata;

namespace taglib {

namespace riff {

void importTrackMetadataFromTag(
    TrackMetadata* pTrackMetadata,
     const TagLib::RIFF::Info::Tag& tag) {
    // Just delegate to the common import function
    taglib::importTrackMetadataFromTag(pTrackMetadata, tag);
}

} // namespace riff

} // namespace taglib

} // namespace mixxx
