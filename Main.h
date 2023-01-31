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
#include <mysql/mysql.h>

typedef std::int64_t int64;
typedef std::int32_t int32;
typedef std::int16_t int16;
typedef std::int8_t int8;
typedef std::uint64_t uint64;
typedef std::uint32_t uint32;
typedef std::uint16_t uint16;
typedef std::uint8_t uint8;

#define MAX_EXPANSION 3
#define MAX_LOCALE 7
#define MAX_QUEST_PARTS 7

enum CacheType
{
    CACHE_NONE = 0,
    CACHE_QUEST = 1,
    CACHE_NPC = 2,
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

typedef std::map<uint32, std::map<uint32, QuestStrings> > QuestStringsMap;

class WowheadCacheEntry
{
public:
    explicit WowheadCacheEntry(uint32 id, CacheType type, uint32 expansion = 0, uint32 locale = 0);

    // load info
    virtual void LoadCacheData(uint32 expansion, uint32 locale) {}
    void LoadCacheDataAll()
    {
        for (auto e = 1; e <= MAX_EXPANSION; ++e) // expansion
        {
            for (auto i = 1; i <= MAX_LOCALE; ++i) // locales
            {
                LoadCacheData(e, i);
            }
        }
    }

    void SetCurLocale(uint32 locale) { curLocale = locale; }
    void SetCurExpansion(uint32 expansion) { curExpansion = expansion; }
    [[nodiscard]] uint32 GetCurLocale() const { return curLocale; }
    [[nodiscard]] uint32 GetCurExpansion() const { return curExpansion; }
    [[nodiscard]] bool IsLoaded() const { return isLoaded; };
    void SetLoaded(bool loaded) { isLoaded = loaded; }
    CacheType GetCacheType() { return cacheType; };
    [[nodiscard]] uint32 GetEntry() const { return entry; }

private:
    uint32 entry;
    CacheType cacheType;
    bool isLoaded;
    uint32 curLocale;
    uint32 curExpansion;
};

QuestStrings LoadQuestCacheStrings(const std::string& whPage, uint32 locale);

class WowheadQuestInfo: public WowheadCacheEntry
{
public:
    explicit WowheadQuestInfo(uint32 id, uint32 expansion = 0, uint32 locale = 0) : WowheadCacheEntry(id, CACHE_QUEST, expansion, locale) {}
    void LoadCacheData(uint32 expansion, uint32 locale) override;

public:
    std::string GetTitle(uint32 expansion = 0, uint32 locale = 0);
    std::string GetObjectives(uint32 expansion = 0, uint32 locale = 0);
    std::string GetDetails(uint32 expansion = 0, uint32 locale = 0);
    std::string GetRequestItemText(uint32 expansion = 0, uint32 locale = 0);
    std::string GetOfferRewardText(uint32 expansion = 0, uint32 locale = 0);
    std::string GetEndText(uint32 expansion = 0, uint32 locale = 0);
    std::string GetObjective(uint32 index, uint32 expansion = 0, uint32 locale = 0);
private:
    QuestStringsMap questTexts;
};

WowheadQuestInfo* LoadQuestCache(uint32 id);

std::map<uint32, WowheadQuestInfo*> questWowheadInfoList;

#endif //WH_PARSER_MAIN_H
