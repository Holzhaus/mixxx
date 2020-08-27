#include "macro_test.h"

TEST(MacroTest, SerializeMacroActions) {
    QList<MacroAction> actions{MacroAction(0, 1)};

    QString filename(QDir::currentPath() % "/src/test/macros/macro_proto");
    ASSERT_TRUE(QFile::exists(filename));
    QFile file(filename);
    file.open(QIODevice::OpenModeFlag::ReadOnly);
    QByteArray content = file.readAll();
    QByteArray serialized = Macro::serialize(actions);
    EXPECT_EQ(serialized.length(), content.length());
    EXPECT_EQ(serialized, content);
    QList<MacroAction> deserialized = Macro::deserialize(serialized);
    EXPECT_EQ(deserialized.size(), 1);
    EXPECT_EQ(deserialized, actions);
}

TEST(MacroTest, CreateMacroAndChangeLabel) {
    Macro macro;
    EXPECT_FALSE(macro.isDirty());
    EXPECT_TRUE(macro.isEmpty());
    macro.clear();
    EXPECT_EQ(macro, Macro());

    macro.setLabel("hello");
    EXPECT_TRUE(macro.isDirty());
    EXPECT_NE(macro, Macro());
}
