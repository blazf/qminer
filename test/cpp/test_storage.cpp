#include <qminer.h>
#include <qminer_storage.h>

#include "microtest.h"

using namespace TQm;
using namespace TQm::TStorage;

const uint64 StoreCacheSize = 100 * TInt::Mega;
const uint64 IndexCacheSize = 100 * TInt::Mega;

PJsonVal MakeArticleSmall(const int& Id) {
    PJsonVal ArticleVal = TJsonVal::NewObj();
    ArticleVal->AddToObj("URI", TStr::Fmt("URI_%d", Id));
    ArticleVal->AddToObj("Url", "The Url");
    ArticleVal->AddToObj("Title", "The Title");
    ArticleVal->AddToObj("Body", "The Body");
    return ArticleVal;
}

TStr MakeStr(const int& Len) {
    TStr Str = "1234567890";
    TChA ChA; while (ChA.Len() < Len) { ChA += Str; }
    return ChA;
}

PJsonVal MakeArticleBig(const int& Id, const int& FieldLen) {
    const TStr FieldStr = MakeStr(FieldLen);
    PJsonVal ArticleVal = TJsonVal::NewObj();
    ArticleVal->AddToObj("URI", TStr::Fmt("URI_%d", Id));
    ArticleVal->AddToObj("Url", FieldStr);
    ArticleVal->AddToObj("Title", FieldStr);
    ArticleVal->AddToObj("Body", FieldStr);
    return ArticleVal;
}

PJsonVal MakeArticleBigMany(const int& Id, const int& FieldLen) {
    const TStr FieldStr = MakeStr(FieldLen);
    PJsonVal ArticleVal = TJsonVal::NewObj();
    ArticleVal->AddToObj("URI", TStr::Fmt("URI_%d", Id));
    ArticleVal->AddToObj("Url", FieldStr);
    ArticleVal->AddToObj("Title", FieldStr);
    ArticleVal->AddToObj("Body01", FieldStr);
    ArticleVal->AddToObj("Body02", FieldStr);
    ArticleVal->AddToObj("Body03", FieldStr);
    ArticleVal->AddToObj("Body04", FieldStr);
    ArticleVal->AddToObj("Body05", FieldStr);
    ArticleVal->AddToObj("Body06", FieldStr);
    ArticleVal->AddToObj("Body07", FieldStr);
    ArticleVal->AddToObj("Body08", FieldStr);
    ArticleVal->AddToObj("Body09", FieldStr);
    ArticleVal->AddToObj("Body10", FieldStr);
    return ArticleVal;
}

double FileSize() {
    return TFile::GetSize("./test/cpp/data/ArticlePgBlob.bin000") * 1.0;
}

TEST(TInit) {
    TUnicodeDef::Load("./src/glib/bin/UnicodeDef.Bin");
    TQm::TEnv::Init();
	TQm::TEnv::InitLogger(0, "std", true);
}

TEST(TStore1) {
    return;

    TStr StoreJsonFNm = "./test/cpp/files/store1.def";
    PJsonVal StoreDefVal = TJsonVal::GetValFromStr(TStr::LoadTxt(StoreJsonFNm));

    int ArticleId = 0;
    uint64 Records = 0;
    const int ArticleFieldLen = 1000;
    const int Recs = 100000;
    const double PercDelete = 0.5;
    const double PercAdd = 1.0;
    const int Iters = 100;

    try {
        PBase Base = NewBase("./test/cpp/data/", StoreDefVal, IndexCacheSize,
            StoreCacheSize, true, TStrUInt64H(), TStrUInt64H(), false, 128, true);
        PStore ArticleStore = Base->GetStoreByStoreNm("Article");

        for (int i = 0; i < Recs; i++) {
            ArticleStore->AddRec(MakeArticleBig(ArticleId++, ArticleFieldLen));
        }
        printf("Added %d records\n", Recs);

        Records = ArticleStore->GetRecs();
        SaveBase(Base);
    } catch (PExcept Except) {
        printf("Exception: %s\n", Except->GetMsgStr().CStr());
    }

    const double RecSize = FileSize() / Records;
    printf("File size: %.0fMB    %.0f / article\n", FileSize() / (1024.0*1024.0), FileSize() / Records);

    for (int IterN = 0; IterN < Iters; IterN++) {
        printf("Iter: %d\n", IterN);

        try {
            PBase Base = LoadBase("./test/cpp/data/", faUpdate, IndexCacheSize,
                StoreCacheSize, TStrUInt64H(), TStrUInt64H(), false, 128);
            PStore ArticleStore = Base->GetStoreByStoreNm("Article");

            PRecSet DeleteSet = ArticleStore->GetRndRecs((int)(PercDelete * Recs));
            TUInt64V DelRecIdV; DeleteSet->GetRecIdV(DelRecIdV);
            ArticleStore->DeleteRecs(DelRecIdV);

            Records = ArticleStore->GetRecs();
            SaveBase(Base);
        } catch (PExcept Except) {
            printf("Exception: %s\n", Except->GetMsgStr().CStr());
        }

        printf("File size: %.0fMB  ...  %.1fx\n", FileSize() / (1024.0*1024.0), (FileSize() / Records) / RecSize);

        try {
            PBase Base = LoadBase("./test/cpp/data/", faUpdate, IndexCacheSize,
                StoreCacheSize, TStrUInt64H(), TStrUInt64H(), false, 128);
            PStore ArticleStore = Base->GetStoreByStoreNm("Article");

            for (int i = 0; i < (int)(PercAdd * Recs); i++) {
                ArticleStore->AddRec(MakeArticleBig(ArticleId++, ArticleFieldLen));
            }

            Records = ArticleStore->GetRecs();
            SaveBase(Base);
        } catch (PExcept Except) {
            printf("Exception: %s\n", Except->GetMsgStr().CStr());
        }

        printf("File size: %.0fMB  ...  %.1fx\n", FileSize() / (1024.0*1024.0), (FileSize() / Records) / RecSize);
    }
}


TEST(TStore2) {
    return;

    TStr StoreJsonFNm = "./test/cpp/files/store1.def";
    PJsonVal StoreDefVal = TJsonVal::GetValFromStr(TStr::LoadTxt(StoreJsonFNm));

    try {
        PBase Base = NewBase("./test/cpp/data/", StoreDefVal, IndexCacheSize,
            StoreCacheSize, true, TStrUInt64H(), TStrUInt64H(), false, 128, true);
        PStore ArticleStore = Base->GetStoreByStoreNm("Article");

        ArticleStore->AddRec(MakeArticleBig(123, 10000));

        printf("Records: %d\n", (int)ArticleStore->GetRecs());

        SaveBase(Base);
    } catch (PExcept Except) {
        printf("Exception: %s\n", Except->GetMsgStr().CStr());
    }

    try {
        PBase Base = LoadBase("./test/cpp/data/", faUpdate, IndexCacheSize,
            StoreCacheSize, TStrUInt64H(), TStrUInt64H(), false, 128);
        PStore ArticleStore = Base->GetStoreByStoreNm("Article");

        TUInt64V DelRecIdV; ArticleStore->GetAllRecs()->GetRecIdV(DelRecIdV);
        ArticleStore->DeleteRecs(DelRecIdV);
        printf("Records: %d\n", (int)ArticleStore->GetRecs());

        SaveBase(Base);
    } catch (PExcept Except) {
        printf("Exception: %s\n", Except->GetMsgStr().CStr());
    }
}

TEST(TStore3) {
    return;

    TStr StoreJsonFNm = "./test/cpp/files/store1.def";
    PJsonVal StoreDefVal = TJsonVal::GetValFromStr(TStr::LoadTxt(StoreJsonFNm));

    try {
        PBase Base = NewBase("./test/cpp/data/", StoreDefVal, IndexCacheSize,
            StoreCacheSize, true, TStrUInt64H(), TStrUInt64H(), false, 128, true);
        PStore ArticleStore = Base->GetStoreByStoreNm("ArticleMany");

        ArticleStore->AddRec(MakeArticleBigMany(123, 500));

        printf("Records: %d\n", (int)ArticleStore->GetRecs());

        SaveBase(Base);
    } catch (PExcept Except) {
        printf("Exception: %s\n", Except->GetMsgStr().CStr());
    }
}
