#pragma once

#include <taglib/apetag.h>

#include "track/taglib/trackmetadata_common.h"

class QImage;
namespace TagLib {
namespace APE {
class Tag;
} // namespace APE
} // namespace TagLib

namespace mixxx {
class TrackMetadata;

namespace taglib {

namespace ape {

void importTrackMetadataFromTag(
        TrackMetadata* pTrackMetadata,
        const TagLib::APE::Tag& tag);

bool importCoverImageFromTag(
        QImage* pCoverArt,
        const TagLib::APE::Tag& tag);

bool exportTrackMetadataIntoTag(
        TagLib::APE::Tag* pTag,
        const TrackMetadata& trackMetadata);

} // namespace ape

} // namespace taglib

} // namespace mixxx
