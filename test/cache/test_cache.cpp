#include <base.h>
#include <qminer.h>
#include <qminer_storage.h>

void CreateTestStore() {
	printf("------ Creating test store ------\n");
	TQm::TEnv::InitLogger(1, "std", true);

	// load schema
    const TStr StoreJsonFNm = "./news.def";
	if (!TFile::Exists(StoreJsonFNm)) {
        TQm::TEnv::Logger->OnStatusFmt("could not locate the news.def file");
        return;
    }
    PJsonVal StoreDefVal = TJsonVal::GetValFromStr(TStr::LoadTxt(StoreJsonFNm));

	// create base
	const uint64 StoreCacheSize = 100 * TInt::Mega;
	const uint64 IndexCacheSize = 100 * TInt::Mega;
	TQm::PBase Base = TQm::TStorage::NewBase("./test_base/", StoreDefVal, IndexCacheSize, StoreCacheSize,
		true, TStrUInt64H(), TStrUInt64H(), false, 128, true);

	// load articles
	TStrV ArticleFNmV; TStrSet UriSet;
	ArticleFNmV.Add("./data/2023-06-08 Articles.json");
	ArticleFNmV.Add("./data/2023-06-09 Articles.json");
	ArticleFNmV.Add("./data/2023-06-10 Articles.json");
	ArticleFNmV.Add("./data/2023-06-11 Articles.json");
	TQm::PStore ArticleStore = Base->GetStoreByStoreNm("Article");
	for (const TStr& ArticleFNm : ArticleFNmV) {
		TQm::TEnv::Logger->OnStatusFmt("Loading %s batch of articles ...", ArticleFNm.CStr());
		TFIn FIn(ArticleFNm);
		TStr Line;
		uint64 LastTm = TTm::GetCurUniMSecs();
		while (FIn.GetNextLn(Line)) {
			PJsonVal ArticleJson = TJsonVal::GetValFromStr(Line);
			// check if URI is in the set
			if (!UriSet.Empty()) {
				const TStr Uri = ArticleJson->GetObjStr("URI");
				if (!UriSet.IsKey(Uri)) { continue; }
			}
			// add record
			const uint64 ArticleId = ArticleStore->AddRec(ArticleJson);
			// print status
			if (ArticleId % 10000 == 0) {
				double Diff = (TTm::GetCurUniMSecs() - LastTm) / 1000.0;
				TQm::TEnv::Logger->OnStatusFmt("%llu (%.0fs)", ArticleId, Diff);
				LastTm = TTm::GetCurUniMSecs();
			}
		}
		// update last time
		double Diff = (TTm::GetCurUniMSecs() - LastTm) / 1000.0;
		TQm::TEnv::Logger->OnStatusFmt("%llu (%.0fs)", ArticleStore->GetRecs(), Diff);
	}

	// save base
	TQm::TStorage::SaveBase(Base);
}

int main(int argc, char* argv[]) {
	TEnv Env(argc, argv, TNotify::StdNotify);
	TQm::TEnv::Init();
	TQm::TEnv::InitLogger(1, "std", true);

	// prepare unicode base
	if (!TUnicodeDef::IsDef()) {
		TUnicodeDef::Load("./UnicodeDef.Bin");
		if (!TUnicodeDef::IsDef()) {
			TNotify::StdNotify->OnNotify(ntInfo, "Unable to load ./UnicodeDef.Bin. Exiting...");
			return 1;
		}
	}

    try {
		if (Env.IsArgStr("--create")) {
			printf("Creating test store ...\n");
			CreateTestStore();
		} else {
			printf("No arguments provided.\n");
		}
    } catch (PExcept E) {
        TNotify::StdNotify->OnStatus("Error: " + E->GetMsgStr());
        TNotify::StdNotify->OnStatus("Trace: " + E->GetLocStr());
    }
    catch (...) {
        printf("general exception");
    }

	printf("Done.\n");
}
