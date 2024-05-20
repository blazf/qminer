#include <base.h>
#include <qminer.h>
#include <qminer_storage.h>

#if defined(GLib_MACOSX)

#include <mach/mach.h>

void ReportMemory(TStr Msg = "") {
    mach_task_basic_info info;
    mach_msg_type_number_t infoCount = MACH_TASK_BASIC_INFO_COUNT;

    if (task_info(mach_task_self(), MACH_TASK_BASIC_INFO, (task_info_t)&info, &infoCount) != KERN_SUCCESS) {
        printf("Error getting task info\n");
    } else {
		printf("Memory[%s]:  size=%10.2fMB\n", Msg.CStr(), (double)info.resident_size / (double)TInt::Mega);
	}
}

#elif defined(GLib_GLIBC)

void ReportMemory(TStr Msg = "") {
	TSysMemStat MemStat;
	printf("Memory[%s]:  size=%10.2fMB\n", Msg.CStr(), (double)MemStat.Resident.Val / (double)TInt::Mega);
}

#endif

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

void TestStoreCache() {
	printf("------ Testing store cache ------\n");
	TQm::TEnv::InitLogger(1, "std", true);

	const uint64 StoreCacheSize = 10 * TInt::Mega;
	const uint64 IndexCacheSize = 10 * TInt::Mega;
	TQm::PBase Base = TQm::TStorage::LoadBase("./test_base/", faRdOnly, IndexCacheSize, StoreCacheSize);
	ReportMemory("on_load");

	TQm::PStore ArticleStore = Base->GetStoreByStoreNm("Article");
	TRnd Rnd(1); uint64 Recs = 0, DataSize = 0;
	while (Recs < 100*(uint64)TInt::Mega) {
		const uint64 RecId = Rnd.GetUniDevInt64(ArticleStore->GetRecs());
		// load cache fields
		const TStr Url = ArticleStore->GetFieldNmStr(RecId, "URI");
		const TStr Title = ArticleStore->GetFieldNmStr(RecId, "Title");
		const TStr Body = ArticleStore->GetFieldNmStr(RecId, "Body");
		const TStr Image = ArticleStore->GetFieldNmStr(RecId, "Image");
		const TStr Details = ArticleStore->GetFieldNmStr(RecId, "Details");
		const TStr ExtractedDates = ArticleStore->GetFieldNmStr(RecId, "ExtractedDates");
		const TStr Date = ArticleStore->GetFieldNmStr(RecId, "Date");
		const uchar Flags = ArticleStore->GetFieldNmByte(RecId, "Flags");
		Recs++;
		DataSize += Url.Len() + Title.Len() + Body.Len() + Image.Len() + Details.Len() + ExtractedDates.Len() + Date.Len() + sizeof(Flags);
		// print status every 1000 records
		if (Recs % 100000 == 0) {
			ReportMemory(TStr::Fmt("recs=%llu, data=%.2fMB", Recs, (double)DataSize / (double)TInt::Mega));
		}
	}
}

void TestIndexCache() {

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
		} else if (Env.IsArgStr("--store")) {
			printf("Testing store cache ...\n");
			TestStoreCache();
		} else if (Env.IsArgStr("--index")) {
			printf("Testing index cache ...\n");
			TestIndexCache();
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
