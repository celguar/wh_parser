#ifndef WH_PARSER_MAIN_H
#define WH_PARSER_MAIN_H

#include <iostream>
#include "cpr/cpr.h"
#include "gumbo.h"
#include <windows.h>
#include <chrono>
#include <thread>
#include <sstream>
#include <filesystem>
#include <utility>
#include <mysql/mysql.h>
#include <regex>

typedef std::int64_t int64;
typedef std::int32_t int32;
typedef std::int16_t int16;
typedef std::int8_t int8;
typedef std::uint64_t uint64;
typedef std::uint32_t uint32;
typedef std::uint16_t uint16;
typedef std::uint8_t uint8;

#define MAX_EXPANSION 3
#define MAX_LOCALE 8
#define MAX_QUEST_PARTS 7
#define SKIP_NONDB 1

enum TypeId
{
    TYPE_NONE  = 0,
    TYPE_QUEST = 1,
    TYPE_NPC   = 2,
    TYPE_ITEM = 3,
    TYPE_OBJECT = 4,
};

enum EntrySource
{
    SRC_NO = 0,
    SRC_WH = 1,
    SRC_DB = 2,
};

struct ItemPage
{
    uint32 entry;
    std::string text;
};

struct ItemStrings
{
    std::string name;
    std::string description;
    std::vector<ItemPage> pages;

    static bool IsMissingPages(const std::vector<ItemPage>& version1, const std::vector<ItemPage>& version2, std::vector<ItemPage>* version1MissingPages = nullptr, std::vector<ItemPage>* version2MissingPages = nullptr);
    static bool HasSamePages(const std::vector<ItemPage>& version1, const std::vector<ItemPage>& version2, std::vector<std::pair<ItemPage, ItemPage>>* = nullptr);
};

struct QuestStrings
{
    std::string title;
    std::string details;
    std::string objectives;
    std::string requestItemText;
    std::string offerRewardText;
    std::string endText;
    std::string objectiveList[4];
};

struct ProjectInfo
{
    std::string host;
    std::string port;
    std::string user;
    std::string password;
    std::string projectName;
};

class DatabaseConnect
{
public:
    explicit DatabaseConnect(const char *host, uint32 port, const char *user, const char *pass, const char *dbname);

    virtual ~DatabaseConnect();

    [[nodiscard]] bool IsConnected() const { return isConnected; }
    bool IsEntryExistInDb(TypeId type, uint32 id);
    bool RunStringStmt(const std::string& command, std::vector<std::string>& result, int32 numStrings = 1, bool silent = false);
    bool RunIntStmt(const std::string& command, std::vector<int>& result, int32 numStrings = 1, bool silent = false);
    bool GetDbString(const std::string& command, std::string& result, bool silent = false);
    bool GetDbStrings(const std::string& command, std::vector<std::string>& result, int numStrings, bool silent = false);
    bool GetDbInt(const std::string& command, int& result, bool silent = false);

    MYSQL* GetMysql() { return mysql; }

private:
    bool isConnected;
    MYSQL* mysql;
};

typedef std::map<uint32, std::map<uint32, QuestStrings> > QuestStringsMap;
typedef std::map<uint32, std::map<uint32, ItemStrings> > ItemStringsMap;

class WowGameEntry
{
public:
    explicit WowGameEntry(uint32 id, TypeId type, uint32 expansion = 0, uint32 locale = 0, EntrySource src = SRC_NO);

    // load info
    virtual void LoadEntryData(uint32 expansion, uint32 locale) {}
    void LoadEntryDataAllVersions()
    {
        for (auto e = 1; e <= MAX_EXPANSION; ++e) // expansion
        {
            for (auto i = 1; i <= MAX_LOCALE; ++i) // locales
            {
                LoadEntryData(e, i);
            }
        }
    }

    void SetCurLocale(uint32 locale) { curLocale = locale; }
    void SetCurExpansion(uint32 expansion) { curExpansion = expansion; }
    void SetCurVersion(uint32 expansion, uint32 locale) { curExpansion = expansion; curLocale = locale; }
    [[nodiscard]] uint32 GetCurLocale() const { return curLocale; }
    [[nodiscard]] uint32 GetCurExpansion() const { return curExpansion; }
    [[nodiscard]] bool IsLoaded(uint32 expansion, uint32 locale) const { return isLoaded[expansion][locale]; };
    void SetLoaded(uint32 expansion, uint32 locale, bool loaded) { isLoaded[expansion][locale] = loaded; }
    TypeId GetCacheType() { return typeId; };
    [[nodiscard]] uint32 GetEntry() const { return entry; }
    EntrySource GetSource() { return source; }

private:
    uint32 entry;
    TypeId typeId;
    bool isLoaded[MAX_EXPANSION + 1][MAX_LOCALE + 1];
    uint32 curLocale;
    uint32 curExpansion;
    EntrySource source;
};

ItemStrings LoadItemCacheStrings(const std::string& whPage, uint32 locale);

class ItemInfo : public WowGameEntry
{
public:
    explicit ItemInfo(uint32 id, uint32 expansion = 0, uint32 locale = 0, EntrySource src = SRC_NO) : WowGameEntry(id, TYPE_ITEM, expansion, locale, src) {}

public:
    std::string GetName(uint32 expansion = 0, uint32 locale = 0);
    std::string GetDescription(uint32 expansion = 0, uint32 locale = 0);
    const std::vector<ItemPage>& GetPages(uint32 expansion = 0, uint32 locale = 0);
    std::string GetItemPart(const std::string& iPart, uint32 expansion = 0, uint32 locale = 0);
    void SetItemTexts(uint32 expansion, uint32 locale, ItemStrings iStrings) { itemTexts[expansion][locale] = std::move(iStrings); }

private:
    ItemStringsMap itemTexts;
};

class WowheadItemInfo : public ItemInfo
{
public:
    explicit WowheadItemInfo(uint32 id, uint32 expansion = 0, uint32 locale = 0) : ItemInfo(id, expansion, locale, SRC_WH) {}
    void LoadEntryData(uint32 expansion, uint32 locale) override;
};

class DatabaseItemInfo : public ItemInfo
{
public:
    explicit DatabaseItemInfo(uint32 id, uint32 expansion = 0, uint32 locale = 0) : ItemInfo(id, expansion, locale, SRC_DB) {}
    void LoadEntryData(uint32 expansion, uint32 locale) override;
};

QuestStrings LoadQuestCacheStrings(const std::string& whPage, uint32 locale);

class QuestInfo: public WowGameEntry
{
public:
    explicit QuestInfo(uint32 id, uint32 expansion = 0, uint32 locale = 0, EntrySource src = SRC_NO) : WowGameEntry(id, TYPE_QUEST, expansion, locale, src) {}

public:
    std::string GetTitle(uint32 expansion = 0, uint32 locale = 0);
    std::string GetObjectives(uint32 expansion = 0, uint32 locale = 0);
    std::string GetDetails(uint32 expansion = 0, uint32 locale = 0);
    std::string GetRequestItemText(uint32 expansion = 0, uint32 locale = 0);
    std::string GetOfferRewardText(uint32 expansion = 0, uint32 locale = 0);
    std::string GetEndText(uint32 expansion = 0, uint32 locale = 0);
    std::string GetObjective(uint32 index, uint32 expansion = 0, uint32 locale = 0);
    std::string GetQuestPart(const std::string& qPart, uint32 expansion = 0, uint32 locale = 0);
    void SetQuestTexts(uint32 expansion, uint32 locale, QuestStrings qStrings) { questTexts[expansion][locale] = std::move(qStrings); }

private:
    QuestStringsMap questTexts;
};

class WowheadQuestInfo: public QuestInfo
{
public:
    explicit WowheadQuestInfo(uint32 id, uint32 expansion = 0, uint32 locale = 0) : QuestInfo(id, expansion, locale, SRC_WH) {}
    void LoadEntryData(uint32 expansion, uint32 locale) override;
};

class DatabaseQuestInfo: public QuestInfo
{
public:
    explicit DatabaseQuestInfo(uint32 id, uint32 expansion = 0, uint32 locale = 0) : QuestInfo(id, expansion, locale, SRC_DB) {}
    void LoadEntryData(uint32 expansion, uint32 locale) override;
};

class DataManager
{
public:
    DataManager()
    {
        mysqlCon[0] = nullptr;
        mysqlCon[1] = nullptr;
        mysqlCon[2] = nullptr;
        isInitialized = false;
    };
    virtual ~DataManager()
    {
        delete mysqlCon[0];
        delete mysqlCon[1];
        delete mysqlCon[2];
    };
    static DataManager& instance()
    {
        static DataManager instance;
        return instance;
    }

    void Initialize();
    DatabaseConnect* GetCon(uint32 expansion) { return mysqlCon[expansion - 1]; }
    std::map<uint32, WowheadQuestInfo*> questWowheadInfoList;
    std::map<uint32, DatabaseQuestInfo*> questDatabaseInfoList;
    std::map<uint32, WowheadItemInfo*> itemWowheadInfoList;
    std::map<uint32, DatabaseItemInfo*> itemDatabaseInfoList;
    bool IsDbOn(uint32 expansion) { return mysqlCon[expansion - 1] != nullptr; }
    std::string getProjectName() const { return projectInfo.projectName; };

private:
    ProjectInfo projectInfo;
    DatabaseConnect* mysqlCon[MAX_EXPANSION]{};
    bool isInitialized;
};

#define sDataMgr DataManager::instance()

#endif //WH_PARSER_MAIN_H
