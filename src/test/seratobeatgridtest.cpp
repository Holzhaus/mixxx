#include <gtest/gtest.h>
#include <taglib/textidentificationframe.h>
#include <taglib/tstring.h>

#include <QDir>
#include <QtDebug>

#include "mixxxtest.h"
#include "track/serato/beatgrid.h"
#include "util/memory.h"

namespace {

class SeratoBeatGridTest : public MixxxTest {
  protected:
    bool parseBeatGridData(const QByteArray& inputValue,
            bool valid,
            mixxx::taglib::FileType fileType,
            QByteArray* output = nullptr) {
        mixxx::SeratoBeatGrid seratoBeatGrid;
        bool parseOk = mixxx::SeratoBeatGrid::parse(
                &seratoBeatGrid, inputValue, fileType);
        EXPECT_EQ(valid, parseOk);
        if (!parseOk) {
            return false;
        }
        QByteArray outputValue = seratoBeatGrid.dump(fileType);
        EXPECT_EQ(inputValue, outputValue);
        if (inputValue != outputValue) {
            if (output) {
                output->clear();
                output->append(outputValue);
            }
            return false;
        }
        return true;
    }

    void parseBeatGridDataInDirectory(
            QDir dir, mixxx::taglib::FileType fileType) {
        EXPECT_TRUE(dir.exists());
        dir.setFilter(QDir::Files);
        dir.setNameFilters(QStringList() << "*.octet-stream");

        QFileInfoList fileList = dir.entryInfoList();
        EXPECT_FALSE(fileList.isEmpty());
        for (const QFileInfo& fileInfo : fileList) {
            qDebug() << "--- File:" << fileInfo.fileName();
            QFile file(dir.filePath(fileInfo.fileName()));
            bool openOk = file.open(QIODevice::ReadOnly);
            EXPECT_TRUE(openOk);
            if (!openOk) {
                continue;
            }
            QByteArray data = file.readAll();
            QByteArray actualData;
            if (!parseBeatGridData(data, true, fileType, &actualData)) {
                QFile outfile(file.fileName() + QStringLiteral(".actual"));
                openOk = outfile.open(QIODevice::WriteOnly);
                EXPECT_TRUE(openOk);
                if (!openOk) {
                    continue;
                }
                EXPECT_EQ(actualData.size(), outfile.write(actualData));
            }
        }
    }

    void parseEmptyBeatGridData(mixxx::taglib::FileType fileType) {
        QByteArray inputValue;
        mixxx::SeratoBeatGrid seratoBeatGrid;
        mixxx::SeratoBeatGrid::parse(&seratoBeatGrid, inputValue, fileType);
        QByteArray outputValue = seratoBeatGrid.dump(fileType);
        EXPECT_EQ(inputValue, outputValue);
    }
};

TEST_F(SeratoBeatGridTest, ParseBeatGridDataMP3) {
    QDir dir(dataDir());
    ASSERT_TRUE(dir.cd("serato/mp3/beatgrid"));
    parseBeatGridDataInDirectory(dir, mixxx::taglib::FileType::MP3);
}

TEST_F(SeratoBeatGridTest, ParseEmptyDataMP3) {
    parseEmptyBeatGridData(mixxx::taglib::FileType::MP3);
}

TEST_F(SeratoBeatGridTest, ParseBeatGridDataMP4) {
    QDir dir(dataDir());
    ASSERT_TRUE(dir.cd("serato/mp4/beatgrid"));
    parseBeatGridDataInDirectory(dir, mixxx::taglib::FileType::MP4);
}

TEST_F(SeratoBeatGridTest, ParseEmptyDataMP4) {
    parseEmptyBeatGridData(mixxx::taglib::FileType::MP4);
}

TEST_F(SeratoBeatGridTest, ParseBeatGridDataFLAC) {
    QDir dir(dataDir());
    ASSERT_TRUE(dir.cd("serato/flac/beatgrid"));
    parseBeatGridDataInDirectory(dir, mixxx::taglib::FileType::FLAC);
}

TEST_F(SeratoBeatGridTest, ParseEmptyDataFLAC) {
    parseEmptyBeatGridData(mixxx::taglib::FileType::FLAC);
}

} // namespace
