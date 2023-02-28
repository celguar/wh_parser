#include "Main.h"

#define STRING_SIZE 100000
#define NOT_FOUND_TEXT "database-detail-page-not-found-message"
#define MSG_DELAY 100

unsigned int check_int_input(const char* arg)
{
    std::istringstream ss(arg);
    unsigned int x;
    if (!(ss >> x))
        return 0; // std::cerr << "Invalid number: " << arg << '\n';
    else if (!ss.eof())
        return 0; // std::cerr << "Trailing characters after number: " << arg << '\n';
    return x;
}

void msg_delay_set(const std::string& message, int delay)
{
    fprintf(stderr, "%s", message.c_str());
    std::chrono::milliseconds duration(delay);
    std::this_thread::sleep_for(duration);
}

void msg_delay(std::string message, ...)
{
    va_list vl;
    va_start(vl, message);

    /*const char* cur;
    do
    {
        cur = va_arg(vl, const char*);
        if (cur)
            addStrategy(cur);
    } while (cur);*/

    vfprintf(stderr, message.c_str(), vl);
    va_end(vl);
    //std::cerr << message.c_str();
    std::chrono::milliseconds duration(MSG_DELAY);
    std::this_thread::sleep_for(duration);
}

void msg_nodelay(std::string message, ...)
{
    va_list vl;
            va_start(vl, message);

    /*const char* cur;
    do
    {
        cur = va_arg(vl, const char*);
        if (cur)
            addStrategy(cur);
    } while (cur);*/


    vfprintf(stderr, message.c_str(), vl);
    va_end(vl);
}

char* strstri(const char* haystack, const char* needle)
{
    if (!*needle)
    {
        return (char*)haystack;
    }
    for (; *haystack; ++haystack)
    {
        if (tolower(*haystack) == tolower(*needle))
        {
            const char* h = haystack, * n = needle;
            for (; *h && *n; ++h, ++n)
            {
                if (tolower(*h) != tolower(*n))
                {
                    break;
                }
            }
            if (!*n)
            {
                return (char*)haystack;
            }
        }
    }
    return nullptr;
}

void writeToFile(const char* text, const std::string& filename, const std::string& path = "")
{
    if (filename.empty())
        return;

    if (!path.empty())
    {
        std::filesystem::create_directories("./" + path);
        //fs::create_directories("./" + path);
        //int makeFolder = _mkdir(path.c_str());
    }

    std::wofstream fileOut(path + filename + ".txt");
    fileOut << text;
    fileOut.close();
   // end

    /*FILE* stream;
    fopen_s(&stream, filename.c_str(), "w+,ccs=UTF-8");
    if (stream)
    {
        fwprintf_s(stream, L"%hs", text);
        fclose(stream);
    }*/
}

std::string readFromFile(const std::string& path)
{
    std::ifstream ifs(path);
    std::string content((std::istreambuf_iterator<char>(ifs)),
        (std::istreambuf_iterator<char>()));

    return content;
}

std::string localeName(unsigned int locId)
{
    switch (locId)
    {
        case 1:
            return "en";
        case 2:
            return "ko";
        case 3:
            return "fr";
        case 4:
            return "de";
        case 5:
            return "cn";
        case 6:
            return "sp";
        case 7:
            return "ru";
        default:
            return "";
    }
}

std::string expansionName(unsigned int expId)
{
    std::string expansionPrefix = "classic";
    if (expId == 2)
        expansionPrefix = "tbc";
    if (expId == 3)
        expansionPrefix = "wotlk";
    if (expId == 4)
        expansionPrefix = "retail";

    return expansionPrefix;
}

std::string typeName(TypeId typeId)
{
    switch (typeId)
    {
    case TYPE_QUEST:
        return "quest";
    case TYPE_NPC:
        return "npc";
    default:
        return "quest";
    }
}

uint32 localeRealId(uint32 locale)
{
    switch (locale)
    {
        case 1:
        default:
            return 0;
            break;
        case 2:
        case 3:
        case 4:
        case 5:
            return locale - 1;
            break;
        case 6:
            return 6;
            break;
        case 7:
            return 8;
            break;
    }
}

uint32 questTextId(const std::string& partName)
{
    if (partName == "title")
        return 1;
    if (partName == "objectives")
        return 2;
    if (partName == "details")
        return 3;
    if (partName == "requestItemText")
        return 4;
    if (partName == "offerRewardText")
        return 5;
    if (partName == "endText")
        return 6;
    if (partName == "objectiveList")
        return 7;

    return 0;
}

std::string questColumnName(const std::string& partName, uint32 locale = 1)
{
    if (partName == "title")
        return locale == 1 ? "Title" : ("Title_loc" + std::to_string(localeRealId(locale)));
    if (partName == "objectives")
        return locale == 1 ? "Objectives" : ("Objectives_loc" + std::to_string(localeRealId(locale)));
    if (partName == "details")
        return locale == 1 ? "Details" : ("Details_loc" + std::to_string(localeRealId(locale)));
    if (partName == "requestItemText")
        return locale == 1 ? "RequestItemsText" : ("RequestItemsText_loc" + std::to_string(localeRealId(locale)));
    if (partName == "offerRewardText")
        return locale == 1 ? "OfferRewardText" : ("OfferRewardText_loc" + std::to_string(localeRealId(locale)));
    if (partName == "endText")
        return locale == 1 ? "EndText" : ("EndText_loc" + std::to_string(localeRealId(locale)));

    if (partName == "objectiveText1")
        return locale == 1 ? "ObjectiveText1" : ("ObjectiveText1_loc" + std::to_string(localeRealId(locale)));
    if (partName == "objectiveText2")
        return locale == 1 ? "ObjectiveText2" : ("ObjectiveText2_loc" + std::to_string(localeRealId(locale)));
    if (partName == "objectiveText3")
        return locale == 1 ? "ObjectiveText3" : ("ObjectiveText3_loc" + std::to_string(localeRealId(locale)));
    if (partName == "objectiveText4")
        return locale == 1 ? "ObjectiveText4" : ("ObjectiveText4_loc" + std::to_string(localeRealId(locale)));

    return "";
}

std::string questAllColumns(uint32 locale = 1)
{
    if (locale == 1)
        return "Title, Objectives, Details, RequestItemsText, OfferRewardText, EndText, ObjectiveText1, objectiveText2, objectiveText3, ObjectiveText4";

    return "Title_loc" + std::to_string(localeRealId(locale)) +
    ", Objectives_loc" + std::to_string(localeRealId(locale)) +
    ", Details_loc" + std::to_string(localeRealId(locale)) +
    ", RequestItemsText_loc" + std::to_string(localeRealId(locale)) +
    ", OfferRewardText_loc" + std::to_string(localeRealId(locale)) +
    ", EndText_loc" + std::to_string(localeRealId(locale)) +
    ", ObjectiveText1_loc" + std::to_string(localeRealId(locale)) +
    ", ObjectiveText2_loc" + std::to_string(localeRealId(locale)) +
    ", ObjectiveText3_loc" + std::to_string(localeRealId(locale)) +
    ", ObjectiveText4_loc" + std::to_string(localeRealId(locale));
}

const char* dbName(uint32 expansion)
{
    const char* dbName = nullptr;
    if (expansion == 1)
        dbName = "classicmangos";
    if (expansion == 2)
        dbName = "tbcmangos";
    if (expansion == 3)
        dbName = "wotlkmangos";

    return dbName;
}

const char* questColumnName(const std::string& partName)
{
    if (partName == "title")
        return "Title";
    if (partName == "objectives")
        return "Objectives";
    if (partName == "details")
        return "Details";
    if (partName == "requestItemText")
        return "RequestItemsText";
    if (partName == "offerRewardText")
        return "OfferRewardText";
    if (partName == "endText")
        return "EndText";
    if (partName == "objectiveList")
        return "ObjectiveText";

    return nullptr;
}

void replaceNewLine(std::string& theText)
{
    const std::regex re("\\n");
    theText = std::regex_replace(theText, re, "$B");
}

void replaceNameTag(std::string& theText, uint32 locale = 0)
{
    std::vector<std::string> nameTagsUp;
    nameTagsUp.emplace_back("<Name>");
    nameTagsUp.emplace_back("<Nombre>");
    nameTagsUp.emplace_back("<Nom>");
    nameTagsUp.emplace_back("<Имя>");

    std::vector<std::string> nameTagsLow;
    nameTagsLow.emplace_back("<name>");
    nameTagsLow.emplace_back("<nombre>");
    nameTagsLow.emplace_back("<nom>");
    nameTagsLow.emplace_back("<имя>");

    for (const auto& name : nameTagsUp)
    {
        const std::regex re(name);
        theText = std::regex_replace(theText, re, "$N");
    }
    for (const auto& name : nameTagsLow)
    {
        const std::regex re(name);
        theText = std::regex_replace(theText, re, "$N");
    }

    // find buggy russian tags
    if (locale == 7)
    {
        const std::regex re1(R"(\|\d-\d\(\$N\))");
        theText = std::regex_replace(theText, re1, "$N");
        const std::regex re2(R"(\|\d-\d\(\$n\))");
        theText = std::regex_replace(theText, re2, "$N");
    }
}

void replaceRaceTag(std::string& theText, uint32 locale = 0)
{
    std::vector<std::string> nameTagsUp;
    nameTagsUp.emplace_back("<Race>");
    nameTagsUp.emplace_back("<Raza>");
    nameTagsUp.emplace_back("<Раса>");

    std::vector<std::string> nameTagsLow;
    nameTagsLow.emplace_back("<race>");
    nameTagsLow.emplace_back("<raza>");
    nameTagsLow.emplace_back("<раса>");

    for (const auto& name : nameTagsUp)
    {
        const std::regex re(name);
        theText = std::regex_replace(theText, re, "$R");
    }
    for (const auto& name : nameTagsLow)
    {
        const std::regex re(name);
        theText = std::regex_replace(theText, re, "$R");
    }

    if (locale == 7)
    {
        std::vector<std::string> posClasses;
        posClasses.emplace_back("человек");
        posClasses.emplace_back("орк");
        posClasses.emplace_back("нежить");
        posClasses.emplace_back("таурен");
        posClasses.emplace_back("гном");
        posClasses.emplace_back("дворф");
        posClasses.emplace_back("тролль");
        posClasses.emplace_back("ночной эльф");
        posClasses.emplace_back("дреней");
        posClasses.emplace_back("эльф крови");
        posClasses.emplace_back("Человек");
        posClasses.emplace_back("Орк");
        posClasses.emplace_back("Нежить");
        posClasses.emplace_back("Таурен");
        posClasses.emplace_back("Гном");
        posClasses.emplace_back("Дворф");
        posClasses.emplace_back("Тролль");
        posClasses.emplace_back("Ночной эльф");
        posClasses.emplace_back("Дреней");
        posClasses.emplace_back("Эльф крови");

        for (const auto& className : posClasses)
        {
            const std::regex re("\\|\\d-\\d\\(" + className + "\\)");
            theText = std::regex_replace(theText, re, "$R");
        }

        for (auto className : posClasses)
        {
            className[0] = toupper(className[0]);
            const std::regex re("\\|\\d-\\d\\(" + className + "\\)");
            theText = std::regex_replace(theText, re, "$R");
        }

        const std::regex re1(R"(\|\d-\d\(\$R\))");
        theText = std::regex_replace(theText, re1, "$R");
        const std::regex re2(R"(\|\d-\d\(\$r\))");
        theText = std::regex_replace(theText, re2, "$R");
        const std::regex re3(R"(\|\d-\d\(\$R)");
        theText = std::regex_replace(theText, re3, "$R");
        const std::regex re4(R"(\|\d-\d\(\$r)");
        theText = std::regex_replace(theText, re4, "$R");
    }
}

void replaceClassTag(std::string& theText, uint32 locale = 0)
{
    std::vector<std::string> nameTagsUp;
    nameTagsUp.emplace_back("<Class>");
    nameTagsUp.emplace_back("<Clase>");
    nameTagsUp.emplace_back("<Classe>");
    nameTagsUp.emplace_back("<Класс>");

    std::vector<std::string> nameTagsLow;
    nameTagsLow.emplace_back("<class>");
    nameTagsLow.emplace_back("<clase>");
    nameTagsLow.emplace_back("<classe>");
    nameTagsLow.emplace_back("<класс>");

    for (const auto& name : nameTagsUp)
    {
        const std::regex re(name);
        theText = std::regex_replace(theText, re, "$C");
    }
    for (const auto& name : nameTagsLow)
    {
        const std::regex re(name);
        theText = std::regex_replace(theText, re, "$C");
    }

    if (locale == 7)
    {
        std::vector<std::string> posClasses;
        posClasses.emplace_back("чернокнижник");
        posClasses.emplace_back("жрец");
        posClasses.emplace_back("воин");
        posClasses.emplace_back("друид");
        posClasses.emplace_back("паладин");
        posClasses.emplace_back("разбойник");
        posClasses.emplace_back("шаман");
        posClasses.emplace_back("маг");
        posClasses.emplace_back("охотник");
        posClasses.emplace_back("рыцарь смерти");
        posClasses.emplace_back("Чернокнижник");
        posClasses.emplace_back("Жрец");
        posClasses.emplace_back("Воин");
        posClasses.emplace_back("Друид");
        posClasses.emplace_back("Паладин");
        posClasses.emplace_back("Разбойник");
        posClasses.emplace_back("Шаман");
        posClasses.emplace_back("Маг");
        posClasses.emplace_back("Охотник");
        posClasses.emplace_back("Рыцарь смерти");

        for (const auto& className : posClasses)
        {
            std::string rege = "\\|\\d-\\d\\(" + className + "\\)";
            const std::regex re(rege);
            theText = std::regex_replace(theText, re, "$C");
        }

        const std::regex re1(R"(\|\d-\d\(\$C\))");
        theText = std::regex_replace(theText, re1, "$C");
        const std::regex re2(R"(\|\d-\d\(\$c\))");
        theText = std::regex_replace(theText, re2, "$C");
        const std::regex re3(R"(\|\d-\d\(\$C)");
        theText = std::regex_replace(theText, re3, "$C");
        const std::regex re4(R"(\|\d-\d\(\$c)");
        theText = std::regex_replace(theText, re4, "$C");
    }
}

bool hasNameTag(QuestInfo* qInfo, const std::string& partName, bool dbOnly = false, uint32 expansion = 0, uint32 locale = 0)
{
    std::string questText = qInfo->GetQuestPart(partName, expansion, locale);
    if (questText.empty())
        return false;

    std::vector<std::string> nameTags;
    nameTags.emplace_back("$N");
    nameTags.emplace_back("$n");
    if (!dbOnly)
    {
        nameTags.emplace_back("<name>");
        nameTags.emplace_back("<nombre>");
        nameTags.emplace_back("<Name>");
        nameTags.emplace_back("<Nombre>");
        nameTags.emplace_back("<nom>");
        nameTags.emplace_back("<Nom>");
        nameTags.emplace_back("<имя>");
        nameTags.emplace_back("<Имя>");
    }
    return std::any_of(nameTags.begin(), nameTags.end(), [questText](const std::string& str){return questText.find(str) != std::string::npos;});
}

bool hasRaceTag(QuestInfo* qInfo, const std::string& partName, bool dbOnly = false, uint32 expansion = 0, uint32 locale = 0)
{
    std::string questText = qInfo->GetQuestPart(partName, expansion, locale);
    if (questText.empty())
        return false;

    std::vector<std::string> nameTags;
    nameTags.emplace_back("$R");
    nameTags.emplace_back("$r");
    if (!dbOnly)
    {
        nameTags.emplace_back("<race>");
        nameTags.emplace_back("<Race>");
        nameTags.emplace_back("<raza>");
        nameTags.emplace_back("<Raza>");
        nameTags.emplace_back("<раса>");
        nameTags.emplace_back("<Раса>");
    }

    // find buggy russian tags
    if (locale == 7)
    {
        std::vector<std::string> posClasses;
        posClasses.emplace_back("человек");
        posClasses.emplace_back("орк");
        posClasses.emplace_back("нежить");
        posClasses.emplace_back("таурен");
        posClasses.emplace_back("гном");
        posClasses.emplace_back("дворф");
        posClasses.emplace_back("тролль");
        posClasses.emplace_back("ночной эльф");
        posClasses.emplace_back("дреней");
        posClasses.emplace_back("эльф крови");
        posClasses.emplace_back("Человек");
        posClasses.emplace_back("Орк");
        posClasses.emplace_back("Нежить");
        posClasses.emplace_back("Таурен");
        posClasses.emplace_back("Гном");
        posClasses.emplace_back("Дворф");
        posClasses.emplace_back("Тролль");
        posClasses.emplace_back("Ночной эльф");
        posClasses.emplace_back("Дреней");
        posClasses.emplace_back("Эльф крови");

        for (const auto& className : posClasses)
        {
            //nameTags.emplace_back("|3-6(" + className + ")");
            const std::regex re("\\|\\d-\\d\\(" + className + "\\)");
            if (std::regex_search(questText, re))
                return true;
        }
    }

    return std::any_of(nameTags.begin(), nameTags.end(), [questText](const std::string& str){return questText.find(str) != std::string::npos;});
}

bool hasClassTag(QuestInfo* qInfo, const std::string& partName, bool dbOnly = false, uint32 expansion = 0, uint32 locale = 0)
{
    std::string questText = qInfo->GetQuestPart(partName, expansion, locale);
    if (questText.empty())
        return false;

    std::vector<std::string> nameTags;
    nameTags.emplace_back("$C");
    nameTags.emplace_back("$c");
    if (!dbOnly)
    {
        nameTags.emplace_back("<class>");
        nameTags.emplace_back("<Class>");
        nameTags.emplace_back("<clase>");
        nameTags.emplace_back("<Clase>");
        nameTags.emplace_back("<classe>");
        nameTags.emplace_back("<Classe>");
        nameTags.emplace_back("<класс>");
        nameTags.emplace_back("<Класс>");
    }

    // find buggy russian tags
    if (locale == 7)
    {
        std::vector<std::string> posClasses;
        posClasses.emplace_back("чернокнижник");
        posClasses.emplace_back("жрец");
        posClasses.emplace_back("воин");
        posClasses.emplace_back("друид");
        posClasses.emplace_back("паладин");
        posClasses.emplace_back("разбойник");
        posClasses.emplace_back("шаман");
        posClasses.emplace_back("маг");
        posClasses.emplace_back("охотник");
        posClasses.emplace_back("рыцарь смерти");
        posClasses.emplace_back("Чернокнижник");
        posClasses.emplace_back("Жрец");
        posClasses.emplace_back("Воин");
        posClasses.emplace_back("Друид");
        posClasses.emplace_back("Паладин");
        posClasses.emplace_back("Разбойник");
        posClasses.emplace_back("Шаман");
        posClasses.emplace_back("Маг");
        posClasses.emplace_back("Охотник");
        posClasses.emplace_back("Рыцарь смерти");

        for (const auto& className : posClasses)
        {
            //nameTags.emplace_back("|3-6(" + className + ")");
            const std::regex re("\\|\\d-\\d\\(" + className + "\\)");
            if (std::regex_search(questText, re))
                return true;
        }
    }

    //msg = std::regex_replace(msg, std::regex("%s"), name);

    return std::any_of(nameTags.begin(), nameTags.end(), [questText](const std::string& str){return questText.find(str) != std::string::npos;});
}

bool hasGenderTag(QuestInfo* qInfo, const std::string& partName, bool dbOnly = false, uint32 expansion = 0, uint32 locale = 0)
{
    std::string questText = qInfo->GetQuestPart(partName, expansion, locale);
    if (questText.empty())
        return false;

    std::vector<std::string> nameTags;
    nameTags.emplace_back("$C");
    nameTags.emplace_back("$c");
    if (!dbOnly)
    {
        nameTags.emplace_back("<class>");
        nameTags.emplace_back("<Class>");
        nameTags.emplace_back("<clase>");
        nameTags.emplace_back("<Clase>");
        nameTags.emplace_back("<classe>");
        nameTags.emplace_back("<Classe>");
        nameTags.emplace_back("<класс>");
        nameTags.emplace_back("<Класс>");
    }

    return std::any_of(nameTags.begin(), nameTags.end(), [questText](const std::string& str){return questText.find(str) != std::string::npos;});
}

std::string updateQuestFromWhQuery(WowheadQuestInfo* whInfo, DatabaseQuestInfo* dbInfo, const std::string& partName, uint32 expansion, uint32 locale)
{
    if (!whInfo || !dbInfo || !whInfo->IsLoaded(whInfo->GetCurExpansion(), whInfo->GetCurLocale()) || !dbInfo->IsLoaded(dbInfo->GetCurExpansion(), dbInfo->GetCurLocale()))
        return "";

    std::string tableName = locale == 1 ? "quest_template" : "locales_quest";
    std::string colName = questColumnName(partName, locale);
    std::string whText = whInfo->GetQuestPart(partName, expansion, locale);
    if (whText.empty())
        return "";
    //std::string dbText = dbInfo->GetQuestPart(partName, expansion, locale);

    // replace nameTags
    //replaceNameTag(whText, locale);
    //replaceClassTag(whText, locale);
    //replaceRaceTag(whText, locale);
    replaceNewLine(whText);

    char *tStr = new char[strlen(whText.c_str())*2+1];
    mysql_real_escape_string(sDataMgr.GetCon(expansion)->GetMysql(), tStr, whText.c_str(), strlen(whText.c_str()));
    std::string replaceString(tStr);
    delete [] tStr;

    std::string command = "UPDATE " + tableName + " SET " + colName + " = '" + replaceString + "' WHERE entry = '" + std::to_string(dbInfo->GetEntry()) + "';";
    return command;
}

std::string updateQuestFromTextQuery(const std::string& replace, DatabaseQuestInfo* dbInfo, const std::string& partName, uint32 expansion, uint32 locale)
{
    if (!dbInfo || replace.empty() || !dbInfo->IsLoaded(dbInfo->GetCurExpansion(), dbInfo->GetCurLocale()))
        return "";

    std::string tableName = locale == 1 ? "quest_template" : "locales_quest";
    std::string colName = questColumnName(partName, locale);
    std::string replaceText = replace;
    //std::string dbText = dbInfo->GetQuestPart(partName, expansion, locale);

    // replace nameTags
    //replaceNameTag(replaceText, locale);
    //replaceClassTag(replaceText, locale);
    //replaceRaceTag(replaceText, locale);
    replaceNewLine(replaceText);

    char *tStr = new char[strlen(replaceText.c_str())*2+1];
    mysql_real_escape_string(sDataMgr.GetCon(expansion)->GetMysql(), tStr, replaceText.c_str(), strlen(replaceText.c_str()));
    std::string replaceString(tStr);
    delete [] tStr;

    std::string command = "UPDATE " + tableName + " SET " + colName + " = '" + replaceString + "' WHERE entry = '" + std::to_string(dbInfo->GetEntry()) + "';";
    return command;
}

std::string updateQuestFromTextQueryNoTags(const std::string& replace, DatabaseQuestInfo* dbInfo, const std::string& partName, uint32 expansion, uint32 locale)
{
    if (!dbInfo || replace.empty() || !dbInfo->IsLoaded(dbInfo->GetCurExpansion(), dbInfo->GetCurLocale()))
        return "";

    std::string tableName = locale == 1 ? "quest_template" : "locales_quest";
    std::string colName = questColumnName(partName, locale);

    char *tStr = new char[strlen(replace.c_str())*2+1];
    mysql_real_escape_string(sDataMgr.GetCon(expansion)->GetMysql(), tStr, replace.c_str(), strlen(replace.c_str()));
    std::string replaceString(tStr);
    delete [] tStr;

    std::string command = "UPDATE " + tableName + " SET " + colName + " = '" + replaceString + "' WHERE entry = '" + std::to_string(dbInfo->GetEntry()) + "';";
    return command;
}

bool TestDbConnect()
{
    auto* mysql = new MYSQL;
    mysql_init(mysql);
    mysql_options(mysql, MYSQL_READ_DEFAULT_GROUP, "your_prog_name");
    if (!mysql_real_connect(mysql, "127.0.0.1", "root", "123456", "classicmangos", 3310, NULL, 0)) {
        fprintf(stderr, "Failed to connect to database: Error: %s\n",
                mysql_error(mysql));

        mysql_close(mysql);
        return false;
    }
    mysql_close(mysql);
    return true;
}

bool DatabaseConnect::RunStringStmt(const std::string &command, std::vector<std::string> &result, int32 numStrings, bool silent)
{
    MYSQL_STMT* stmt;
    auto* bind = new MYSQL_BIND[numStrings];
    //MYSQL_BIND    bind[arrSize];
    MYSQL_RES* prepare_meta_result;
    MYSQL_TIME    ts;
    auto* length = new unsigned long [numStrings];
    int           param_count, column_count, row_count;
    //short         small_data;
    //int           int_data;
    std::map<uint32, char[STRING_SIZE]> str_data;
    //char          str_data[STRING_SIZE];
    bool* is_null = new bool[numStrings];
    bool* error = new bool[numStrings];
    //bool          is_null[arrSize];
    //bool          error[arrSize];

    /* Prepare a SELECT query to fetch data from test_table */
    stmt = mysql_stmt_init(mysql);
    if (!stmt)
    {
        fprintf(stderr, " mysql_stmt_init(), out of memory\n");
        delete[] bind; delete[] length; delete[] is_null; delete[] error;
        return false;
    }
    if (mysql_stmt_prepare(stmt, command.c_str(), strlen(command.c_str())))
    {
        fprintf(stderr, " mysql_stmt_prepare(), SELECT failed\n");
        fprintf(stderr, " %s\n", mysql_stmt_error(stmt));
        delete[] bind; delete[] length; delete[] is_null; delete[] error;
        return false;
    }
    //fprintf(stdout, " prepare, SELECT successful\n");

    /* Get the parameter count from the statement */
    param_count = mysql_stmt_param_count(stmt);
    //fprintf(stdout, " total parameters in SELECT: %d\n", param_count);

    if (param_count != 0) /* validate parameter count */
    {
        fprintf(stderr, " invalid parameter count returned by MySQL\n");
        delete[] bind; delete[] length; delete[] is_null; delete[] error;
        return false;
    }

    /* Execute the SELECT query */
    if (mysql_stmt_execute(stmt))
    {
        fprintf(stderr, " mysql_stmt_execute(), failed\n");
        fprintf(stderr, " %s\n", mysql_stmt_error(stmt));
        delete[] bind; delete[] length; delete[] is_null; delete[] error;
        return false;
    }

    /* Fetch result set meta information */
    prepare_meta_result = mysql_stmt_result_metadata(stmt);
    if (!prepare_meta_result)
    {
        fprintf(stderr,
                " mysql_stmt_result_metadata(), \
           returned no meta information\n");
        fprintf(stderr, " %s\n", mysql_stmt_error(stmt));
        delete[] bind; delete[] length; delete[] is_null; delete[] error;
        return false;
    }

    /* Get total columns in the query */
    column_count = mysql_num_fields(prepare_meta_result);
    //fprintf(stdout,
    //        " total columns in SELECT statement: %d\n",
    //        column_count);

    if (column_count != numStrings) /* validate column count */
    {
        if (!silent) fprintf(stderr, " invalid column count returned by MySQL\n");
        delete[] bind; delete[] length; delete[] is_null; delete[] error;
        return false;
    }

    /* Bind the result buffers for all 4 columns before fetching them */

    //memset(bind, 0, sizeof(bind));
    for (auto i = 0; i < numStrings; ++i)
    {
        /* STRING COLUMN */
        bind[i].buffer_type = MYSQL_TYPE_STRING;
        bind[i].buffer = (char*)(str_data[i]);
        bind[i].buffer_length = STRING_SIZE;
        bind[i].is_null = &is_null[i];
        bind[i].length = &length[i];
        bind[i].error = &error[i];
    }

    /* Bind the result buffers */
    if (mysql_stmt_bind_result(stmt, bind))
    {
        fprintf(stderr, " mysql_stmt_bind_result() failed\n");
        fprintf(stderr, " %s\n", mysql_stmt_error(stmt));
        delete[] bind; delete[] length; delete[] is_null; delete[] error;
        return false;
    }

    /* Now buffer all results to client (optional step) */
    if (mysql_stmt_store_result(stmt))
    {
        fprintf(stderr, " mysql_stmt_store_result() failed\n");
        fprintf(stderr, " %s\n", mysql_stmt_error(stmt));
        delete[] bind; delete[] length; delete[] is_null; delete[] error;
        return false;
    }

    /* Fetch all rows */
    row_count = 0;
    //fprintf(stdout, "Fetching results ...\n");
    while (!mysql_stmt_fetch(stmt))
    {
        row_count++;
        //fprintf(stdout, "  row %d\n", row_count);

        for (auto i = 0; i < numStrings; ++i)
        {
            /* column 1 */
            //fprintf(stdout, "   column1 (string)   : ");
            if (!is_null[i])
            {
                //fprintf(stdout, " NULL\n");
                result.emplace_back(str_data[i]);
            }
            else
                result.emplace_back("");
        }
    }

    /* Validate rows fetched */
    //fprintf(stdout, " total rows fetched: %d\n", row_count);
    if (row_count < 1)
    {
        if (!silent) fprintf(stderr, " MySQL failed to return all rows\n");
    }

    /* Free the prepared result metadata */
    mysql_free_result(prepare_meta_result);

    /* Close the statement */
    if (mysql_stmt_close(stmt))
    {
        /* mysql_stmt_close() invalidates stmt, so call          */
        /* mysql_error(mysql) rather than mysql_stmt_error(stmt) */
        fprintf(stderr, " failed while closing the statement\n");
        fprintf(stderr, " %s\n", mysql_error(mysql));
        delete[] bind; delete[] length; delete[] is_null; delete[] error;
        return false;
    }

    delete[] bind; delete[] length; delete[] is_null; delete[] error;
    return true;
}

bool DatabaseConnect::RunIntStmt(const std::string &command, std::vector<int> &result, int32 numStrings)
{
    MYSQL_STMT* stmt;
    auto* bind = new MYSQL_BIND[numStrings];
    MYSQL_RES* prepare_meta_result;
    MYSQL_TIME    ts;
    auto* length = new unsigned long [numStrings];
    int           param_count, column_count, row_count;
    short         small_data;
    int           int_data;
    char          str_data[STRING_SIZE];
    bool* is_null = new bool[numStrings];
    bool* error = new bool[numStrings];

    /* Prepare a SELECT query to fetch data from test_table */
    stmt = mysql_stmt_init(mysql);
    if (!stmt)
    {
        fprintf(stderr, " mysql_stmt_init(), out of memory\n");
        return false;
    }
    if (mysql_stmt_prepare(stmt, command.c_str(), strlen(command.c_str())))
    {
        fprintf(stderr, " mysql_stmt_prepare(), SELECT failed\n");
        fprintf(stderr, " %s\n", mysql_stmt_error(stmt));
        return false;
    }
    //fprintf(stdout, " prepare, SELECT successful\n");

    /* Get the parameter count from the statement */
    param_count = mysql_stmt_param_count(stmt);
    //fprintf(stdout, " total parameters in SELECT: %d\n", param_count);

    if (param_count != 0) /* validate parameter count */
    {
        fprintf(stderr, " invalid parameter count returned by MySQL\n");
        return false;
    }

    /* Execute the SELECT query */
    if (mysql_stmt_execute(stmt))
    {
        fprintf(stderr, " mysql_stmt_execute(), failed\n");
        fprintf(stderr, " %s\n", mysql_stmt_error(stmt));
        return false;
    }

    /* Fetch result set meta information */
    prepare_meta_result = mysql_stmt_result_metadata(stmt);
    if (!prepare_meta_result)
    {
        fprintf(stderr,
                " mysql_stmt_result_metadata(), \
           returned no meta information\n");
        fprintf(stderr, " %s\n", mysql_stmt_error(stmt));
        return false;
    }

    /* Get total columns in the query */
    column_count = mysql_num_fields(prepare_meta_result);
    //fprintf(stdout,
    //        " total columns in SELECT statement: %d\n",
    //        column_count);

    if (column_count != numStrings) /* validate column count */
    {
        fprintf(stderr, " invalid column count returned by MySQL\n");
        return false;
    }

    /* Bind the result buffers for all 4 columns before fetching them */

    //memset(bind, 0, sizeof(bind));
    for (auto i = 0; i < numStrings; ++i)
    {
        /* INTEGER COLUMN */
        bind[i].buffer_type = MYSQL_TYPE_LONG;
        bind[i].buffer = (char*)&int_data;
        bind[i].is_null = &is_null[i];
        bind[i].length = &length[i];
        bind[i].error = &error[i];
    }

    /* Bind the result buffers */
    if (mysql_stmt_bind_result(stmt, bind))
    {
        fprintf(stderr, " mysql_stmt_bind_result() failed\n");
        fprintf(stderr, " %s\n", mysql_stmt_error(stmt));
        return false;
    }

    /* Now buffer all results to client (optional step) */
    if (mysql_stmt_store_result(stmt))
    {
        fprintf(stderr, " mysql_stmt_store_result() failed\n");
        fprintf(stderr, " %s\n", mysql_stmt_error(stmt));
        return false;
    }

    /* Fetch all rows */
    row_count = 0;
    //fprintf(stdout, "Fetching results ...\n");
    while (!mysql_stmt_fetch(stmt))
    {
        row_count++;
        //fprintf(stdout, "  row %d\n", row_count);

        for (auto i = 0; i < numStrings; ++i)
        {
            /* column 1 */
            //fprintf(stdout, "   column1 (string)   : ");
            if (!is_null[i])
            {
                //fprintf(stdout, " NULL\n");
                result.emplace_back(int_data);
            }
        }
    }

    /* Validate rows fetched */
    //fprintf(stdout, " total rows fetched: %d\n", row_count);
    if (row_count < 1)
    {
        fprintf(stderr, " MySQL failed to return all rows\n");
    }

    /* Free the prepared result metadata */
    mysql_free_result(prepare_meta_result);


    /* Close the statement */
    if (mysql_stmt_close(stmt))
    {
        /* mysql_stmt_close() invalidates stmt, so call          */
        /* mysql_error(mysql) rather than mysql_stmt_error(stmt) */
        fprintf(stderr, " failed while closing the statement\n");
        fprintf(stderr, " %s\n", mysql_error(mysql));
        return false;
    }

    return true;
}

bool DatabaseConnect::GetDbString(const std::string& command, std::string& result, bool silent)
{
    std::vector<std::string> strVec;
    RunStringStmt(command, strVec, 1, silent);
    if (strVec.empty())
        return false;

    if (!strVec[0].empty()) {
        result = strVec[0];
        return true;
    }
    return false;
}

bool DatabaseConnect::GetDbStrings(const std::string& command, std::vector<std::string>& result, int numStrings, bool silent)
{
    std::vector<std::string> strVec;
    RunStringStmt(command, strVec, numStrings, silent);
    for (const auto& entry : strVec)
    {
        if (!entry.empty())
        {
            result = strVec;
            return true;
        }
    }

    return false;
}

bool DatabaseConnect::GetDbInt(const std::string& command, int& result)
{
    std::vector<int> strVec;
    RunIntStmt(command, strVec, 1);
    if (strVec.empty())
        return false;

    result = strVec[0];
    return true;
}

bool DatabaseConnect::IsEntryExistInDb(TypeId type, uint32 id)
{
    if (!IsConnected())
    {
        fprintf(stderr, "Failed to connect to database");
        return false;
    }
    else
    {
        std::string command;
        if (type == TYPE_QUEST)
            command = "SELECT Title FROM quest_template WHERE entry = " + std::to_string(id);

        std::string result;
        return GetDbString(command, result, true);
    }
    return false;
}

std::string loadFromDB()
{
    auto* mysql = new MYSQL;
    mysql_init(mysql);
    mysql_options(mysql, MYSQL_READ_DEFAULT_GROUP, "your_prog_name");
    if (!mysql_real_connect(mysql, "127.0.0.1", "root", "123456", "tbcmangos", 3310, NULL, 0))
    {
        fprintf(stderr, "Failed to connect to database: Error: %s\n",
            mysql_error(mysql));
    }
    else
    {
        std::string command = "SELECT Title_loc3 FROM locales_quest WHERE entry = 2";
        msg_delay("> DB: connection successful!");

        MYSQL_STMT* stmt;
        MYSQL_BIND    bind[1];
        MYSQL_RES* prepare_meta_result;
        MYSQL_TIME    ts;
        unsigned long length[4];
        int           param_count, column_count, row_count;
        short         small_data;
        int           int_data;
        char          str_data[STRING_SIZE];
        bool          is_null[4];
        bool          error[4];

        /* Prepare a SELECT query to fetch data from test_table */
        stmt = mysql_stmt_init(mysql);
        if (!stmt)
        {
            fprintf(stderr, " mysql_stmt_init(), out of memory\n");
            exit(0);
        }
        if (mysql_stmt_prepare(stmt, command.c_str(), strlen(command.c_str())))
        {
            fprintf(stderr, " mysql_stmt_prepare(), SELECT failed\n");
            fprintf(stderr, " %s\n", mysql_stmt_error(stmt));
            exit(0);
        }
        fprintf(stdout, " prepare, SELECT successful\n");

        /* Get the parameter count from the statement */
        param_count = mysql_stmt_param_count(stmt);
        fprintf(stdout, " total parameters in SELECT: %d\n", param_count);

        if (param_count != 0) /* validate parameter count */
        {
            fprintf(stderr, " invalid parameter count returned by MySQL\n");
            exit(0);
        }

        /* Execute the SELECT query */
        if (mysql_stmt_execute(stmt))
        {
            fprintf(stderr, " mysql_stmt_execute(), failed\n");
            fprintf(stderr, " %s\n", mysql_stmt_error(stmt));
            exit(0);
        }

        /* Fetch result set meta information */
        prepare_meta_result = mysql_stmt_result_metadata(stmt);
        if (!prepare_meta_result)
        {
            fprintf(stderr,
                " mysql_stmt_result_metadata(), \
           returned no meta information\n");
            fprintf(stderr, " %s\n", mysql_stmt_error(stmt));
            exit(0);
        }

        /* Get total columns in the query */
        column_count = mysql_num_fields(prepare_meta_result);
        fprintf(stdout,
            " total columns in SELECT statement: %d\n",
            column_count);

        if (column_count != 1) /* validate column count */
        {
            fprintf(stderr, " invalid column count returned by MySQL\n");
            exit(0);
        }

        /* Bind the result buffers for all 4 columns before fetching them */

        memset(bind, 0, sizeof(bind));

        /* STRING COLUMN */
        bind[0].buffer_type = MYSQL_TYPE_STRING;
        bind[0].buffer = (char*)str_data;
        bind[0].buffer_length = STRING_SIZE;
        bind[0].is_null = &is_null[0];
        bind[0].length = &length[0];
        bind[0].error = &error[0];

        ///* INTEGER COLUMN */
        //bind[0].buffer_type = MYSQL_TYPE_LONG;
        //bind[0].buffer = (char*)&int_data;
        //bind[0].is_null = &is_null[0];
        //bind[0].length = &length[0];
        //bind[0].error = &error[0];

        ///* STRING COLUMN */
        //bind[1].buffer_type = MYSQL_TYPE_STRING;
        //bind[1].buffer = (char*)str_data;
        //bind[1].buffer_length = STRING_SIZE;
        //bind[1].is_null = &is_null[1];
        //bind[1].length = &length[1];
        //bind[1].error = &error[1];

        ///* SMALLINT COLUMN */
        //bind[2].buffer_type = MYSQL_TYPE_SHORT;
        //bind[2].buffer = (char*)&small_data;
        //bind[2].is_null = &is_null[2];
        //bind[2].length = &length[2];
        //bind[2].error = &error[2];

        ///* TIMESTAMP COLUMN */
        //bind[3].buffer_type = MYSQL_TYPE_TIMESTAMP;
        //bind[3].buffer = (char*)&ts;
        //bind[3].is_null = &is_null[3];
        //bind[3].length = &length[3];
        //bind[3].error = &error[3];

        /* Bind the result buffers */
        if (mysql_stmt_bind_result(stmt, bind))
        {
            fprintf(stderr, " mysql_stmt_bind_result() failed\n");
            fprintf(stderr, " %s\n", mysql_stmt_error(stmt));
            exit(0);
        }

        /* Now buffer all results to client (optional step) */
        if (mysql_stmt_store_result(stmt))
        {
            fprintf(stderr, " mysql_stmt_store_result() failed\n");
            fprintf(stderr, " %s\n", mysql_stmt_error(stmt));
            exit(0);
        }

        /* Fetch all rows */
        row_count = 0;
        fprintf(stdout, "Fetching results ...\n");
        while (!mysql_stmt_fetch(stmt))
        {
            row_count++;
            fprintf(stdout, "  row %d\n", row_count);

            /* column 1 */
            fprintf(stdout, "   column1 (string)   : ");
            if (is_null[0])
                fprintf(stdout, " NULL\n");
            else
                fprintf(stdout, " %s(%ld)\n", str_data, length[0]);

            ///* column 1 */
            //fprintf(stdout, "   column1 (integer)  : ");
            //if (is_null[0])
            //    fprintf(stdout, " NULL\n");
            //else
            //    fprintf(stdout, " %d(%ld)\n", int_data, length[0]);

            ///* column 2 */
            //fprintf(stdout, "   column2 (string)   : ");
            //if (is_null[1])
            //    fprintf(stdout, " NULL\n");
            //else
            //    fprintf(stdout, " %s(%ld)\n", str_data, length[1]);

            ///* column 3 */
            //fprintf(stdout, "   column3 (smallint) : ");
            //if (is_null[2])
            //    fprintf(stdout, " NULL\n");
            //else
            //    fprintf(stdout, " %d(%ld)\n", small_data, length[2]);

            ///* column 4 */
            //fprintf(stdout, "   column4 (timestamp): ");
            //if (is_null[3])
            //    fprintf(stdout, " NULL\n");
            //else
            //    fprintf(stdout, " %04d-%02d-%02d %02d:%02d:%02d (%ld)\n",
            //        ts.year, ts.month, ts.day,
            //        ts.hour, ts.minute, ts.second,
            //        length[3]);
            //fprintf(stdout, "\n");
        }

        /* Validate rows fetched */
        fprintf(stdout, " total rows fetched: %d\n", row_count);
        if (row_count < 1)
        {
            fprintf(stderr, " MySQL failed to return all rows\n");
        }

        /* Free the prepared result metadata */
        mysql_free_result(prepare_meta_result);


        /* Close the statement */
        if (mysql_stmt_close(stmt))
        {
            /* mysql_stmt_close() invalidates stmt, so call          */
            /* mysql_error(mysql) rather than mysql_stmt_error(stmt) */
            fprintf(stderr, " failed while closing the statement\n");
            fprintf(stderr, " %s\n", mysql_error(mysql));
            exit(0);
        }
    }
    return "";
}

std::string load_WH_page(TypeId type, unsigned int id, unsigned int& error_code, unsigned int locale = 0, unsigned int expansion = 0)
{
    // select locale prefix
    std::string localePrefix;
    switch (locale)
    {
    case 1:
    default:
        break;
    case 2:
        localePrefix = "/ko";
        break;
    case 3:
        localePrefix = "/fr";
        break;
    case 4:
        localePrefix = "/de";
        break;
    case 5:
        localePrefix = "/cn";
        break;
    case 6:
        localePrefix = "/es";
        break;
    case 7:
        localePrefix = "/ru";
        break;
    }

    // select expansion prefix
    std::string expansionPrefix = "classic";
    if (expansion == 2)
        expansionPrefix = "tbc";
    if (expansion == 3)
        expansionPrefix = "wotlk";
    if (expansion == 4)
        expansionPrefix = "";

    cpr::Response response = cpr::Get(cpr::Url{ (std::string("https://") + std::string("wowhead.com/") + expansionPrefix + localePrefix + "/" + typeName(type) + "=" + std::to_string(id))}, cpr::UserAgent{"Mozilla / 5.0 (Windows NT 10.0; Win64; x64) AppleWebKit / 537.36 (KHTML, like Gecko) Chrome / 99.0.4844.51 Safari / 537.36"});
    error_code = response.status_code;
    return response.text;
}

bool isNotFound(const std::string& wh_page)
{
    // "database-detail-page-not-found-message";
    return wh_page.find(NOT_FOUND_TEXT) != std::string::npos;
}

std::string loadPageOrCache(TypeId type, unsigned int id, unsigned int expansion, unsigned int locale, bool silent = false)
{
    std::string cacheLocation = "cache/" + expansionName(expansion) + "/" + localeName(locale) + "/" + typeName(type) + "/";
    if (!silent)
        msg_delay("\n> " + typeName(type) + " #" + std::to_string(id) + " " + expansionName(expansion) + "-" + localeName(locale) + " : Loading... \n");

    // Load Wowhead Page or Cache
    std::string wh_page = readFromFile(cacheLocation + std::to_string(id) + ".txt");
    if (!wh_page.empty())
    {
        if (!silent)
            msg_delay("> " + typeName(type) + " #" + std::to_string(id) + " " + expansionName(expansion) + "-" + localeName(locale) + " : Loaded from Cache! \n");
    }
    else
    {
        unsigned int error_code;
        wh_page = load_WH_page(type, id, error_code, locale, expansion);
        if (error_code >= 400)
        {
            if (!silent)
                msg_delay("> " + typeName(type) + " #" + std::to_string(id) + " " + expansionName(expansion) + "-" + localeName(locale) + " Not Found! (404) \n");
        }
        else if (isNotFound(wh_page))
        {
            if (!silent)
                msg_delay("> " + typeName(type) + " #" + std::to_string(id) + " " + expansionName(expansion) + "-" + localeName(locale) + " Not Found! (Not Found) \n");

            return "";
        }
        else
        {
            if (!silent)
            {
                msg_delay("> " + typeName(type) + " #" + std::to_string(id) + " " + expansionName(expansion) + "-" + localeName(locale) + " Loaded from WH! \n");
                msg_delay("> " + typeName(type) + " #" + std::to_string(id) + " " + expansionName(expansion) + "-" + localeName(locale) + " sCaching... \n");
            }
            writeToFile(wh_page.c_str(), std::to_string(id), cacheLocation);
        }
    }
    return wh_page;
}

void cachePagesRange(TypeId type, unsigned int min_id, unsigned int max_id, unsigned int locale = 0, unsigned int expansion = 0)
{
    msg_delay("> Cache: Loading %s pages \n", typeName(type).c_str());
    if (max_id > 0)
        msg_delay("> Cache: Entries: #%d - #%d \n", min_id, max_id);
    else
        msg_delay("> Cache: Entry: #%d \n", min_id);
    if (locale > 0)
        msg_delay("> Cache: Locale: %s \n", localeName(locale).c_str());
    else
        msg_delay("> Cache: Locale: all \n");
    if (expansion > 0)
        msg_delay("> Cache: Version: %s \n", expansionName(expansion).c_str());
    else
        msg_delay("> Cache: Version: all \n");

    msg_delay("> Cache: Starting... \n");

    if (min_id < 1)
    {
        msg_delay("> Cache: invalid minId! \n");
        return;
    }
    /*if (max_id < 1)
    {
        msg_delay("> Cache: invalid minId! \n");
        return;
    }*/
    if (max_id > 0 && max_id < min_id)
    {
        msg_delay("> Cache: maxId is less than minId! \n");
        return;
    }

    // load if not cached
    if (max_id == 0)
    {
        for (auto e = 1; e < 4 + 1; ++e) // expansion
        {
            if (expansion != 0 && e != expansion)
                continue;

            bool noEnglishPage = false; // speed up missing quests process
            for (auto i = 1; i < 7 + 1; ++i) // locales
            {
                if (locale != 0 && i != locale)
                    continue;

                if (noEnglishPage)
                {
                    noEnglishPage = false;
                    break;
                }

                std::string cacheLocation = "cache/" + expansionName(e) + "/" + localeName(i) + "/" + typeName(type) + "/";
                std::string wh_page = readFromFile(cacheLocation + std::to_string(min_id) + ".txt");
                if (!wh_page.empty())
                    msg_delay("\n> " + typeName(type) + " #" + std::to_string(min_id) + " " + expansionName(e) + "-" + localeName(i) + ": Already cached, skipping...");
                else
                {
                    std::string page = loadPageOrCache(type, min_id, e, i, true);
                    if (!page.empty())
                        msg_delay("\n> " + typeName(type) + " #" + std::to_string(min_id) + " " + expansionName(e) + "-" + localeName(i));
                    else
                    {
                        msg_delay("\n> " + typeName(type) + " #" + std::to_string(min_id) + " " + expansionName(e) + "-" + localeName(i) + " (error)");
                        if (locale == 0 && i == 1)
                            noEnglishPage = true;
                    }
                }

            }
        }
        msg_delay("\n\n Done!");
        /*std::string cacheLocation = "cache/" + expansionName(expansion) + "/" + localeName(locale) + "/" + type + "/";
        std::string wh_page = readFromFile(cacheLocation + std::to_string(min_id) + ".txt");
        if (!wh_page.empty())
            msg_delay("> " + type + " #" + std::to_string(min_id) + "(" + expansionName(expansion) + "/" + localeName(locale) + "): Already cached, skipping... \n");*/
        //else
        //{
        //    if (locale == 0 || expansion == 0) // all locales/exp
        //    {
        //        msg_delay("> " + type + " #" + std::to_string(min_id) + ": ");
        //        for (auto i = 1; i < 7 + 1; ++i) // locales
        //        {
        //            if (locale != 0 && i != locale)
        //                continue;

        //            //msg_delay(localeName(i) + ": ");

        //            for (auto e = 1; e < 4 + 1; ++e) // expansion
        //            {
        //                if (expansion != 0 && e != expansion)
        //                    continue;

        //                //msg_delay(expansionName(e));

        //                std::string page = loadPageOrCache(type, min_id, i, e, true);
        //                if (!page.empty())
        //                    msg_delay("(" + (((expansion != 0) || (expansion == 0 && i == 1)) ? expansionName(e) : "") + "/" + (((locale != 0) || (locale == 0 && e == 1)) ? localeName(i) : "") + ")");
        //                else
        //                    msg_delay("(" + (((expansion != 0) || (expansion == 0 && i == 1)) ? expansionName(e) : "") + "/" + (((locale != 0) || (locale == 0 && e == 1)) ? localeName(i) : "") + ")" + " (error), ");
        //            }
        //        }
        //        msg_delay(" Done! \n");
        //    }
        //    else
        //        std::string page = loadPageOrCache(type, min_id, locale, expansion);
        //}
    }
    else
    {
        for (auto id = min_id; id < max_id + 1; ++id)
        {
            if (id > min_id)
                msg_delay("\n");

            for (auto e = 1; e < 4 + 1; ++e) // expansion
            {
                if (expansion != 0 && e != expansion)
                    continue;

                bool noEnglishPage = false; // speed up missing quests process
                for (auto i = 1; i < 7 + 1; ++i) // locales
                {
                    if (locale != 0 && i != locale)
                        continue;

                    if (noEnglishPage)
                    {
                        noEnglishPage = false;
                        break;
                    }

                    std::string cacheLocation = "cache/" + expansionName(e) + "/" + localeName(i) + "/" + typeName(type) + "/";
                    std::string wh_page = readFromFile(cacheLocation + std::to_string(id) + ".txt");
                    if (!wh_page.empty())
                        msg_delay("\n> " + typeName(type) + " # (" + std::to_string(id) + "/" + std::to_string(max_id) + ") " + expansionName(e) + "-" + localeName(i) + ": Already cached, skipping...");
                    else
                    {
                        std::string page = loadPageOrCache(type, id, e, i, true);
                        if (!page.empty())
                            msg_delay("\n> " + typeName(type) + " # (" + std::to_string(id) + "/" + std::to_string(max_id) + ") " + expansionName(e) + "-" + localeName(i));
                        else
                        {
                            msg_delay("\n> " + typeName(type) + " # (" + std::to_string(id) + "/" + std::to_string(max_id) + ") " + expansionName(e) + "-" + localeName(i) + " (error)");
                            if (locale == 0 && i == 1)
                                noEnglishPage = true;
                        }
                    }
                }
            }
        }
        msg_delay("\n\n Done!");
    }
}

const char* parse_details(std::ostringstream& query_result, GumboNode* node, GumboTag tag, const char* attr_name, const char* attr_value, const char* contents, bool between = false, const char* attr_second_name = NULL, const char* attr_second_value = NULL, const char* attr_second_alt_value = NULL, unsigned int steps_back = 0, bool unique = false, bool hasChildren = true, GumboTag preTag = GUMBO_TAG_UNKNOWN, GumboNodeType preType = GUMBO_NODE_ELEMENT, bool searchChildren = false, unsigned int childIndex = 0, bool fromEnd = false, const char* ignoreContent = "")
{
    if (node->type != GUMBO_NODE_ELEMENT)
        return NULL;

    // check tag
    if (tag != GUMBO_TAG_UNKNOWN && node->v.element.tag != tag)
        return NULL;

    // check attributes
    if (attr_name != NULL && attr_value != NULL)
    {
        GumboAttribute* lt_attr = gumbo_get_attribute(&node->v.element.attributes, attr_name);
        if (lt_attr && (strcmp(lt_attr->value, attr_value) == 0))
        {
            GumboNode* title_text = static_cast<GumboNode*>(node->v.element.children.data[0]);
            // check contents
            if (!between && !searchChildren && title_text && (contents == NULL || (contents != NULL && strcmp(title_text->v.text.text, contents) == 0)))
            {
                GumboVector* children = &node->v.element.children;
                for (unsigned int i = 0; i < children->length; ++i)
                {
                    GumboNode* childNode = static_cast<GumboNode*>(children->data[i]);
                    if (childNode)
                    {
                        if (childNode->v.element.tag == GUMBO_TAG_BR)
                        {
                            // TODO replace here or not
                            query_result << "\n";
                            //query_result << "$B";
                            //std::cout << "\n";
                        }
                        else if (childNode->v.element.tag == GUMBO_TAG_A)
                        {
                            // Bug - gives random huge children length if has <a> tag in middle
                            GumboVector* childrenA = &childNode->v.element.children;
                            if (childrenA->length > 10)
                            {
                                query_result << childNode->v.text.text;
                                continue;
                            }

                            for (unsigned int e = 0; e < childrenA->length; ++e)
                            {
                                GumboNode* link_node = static_cast<GumboNode*>(childrenA->data[e]);
                                if (link_node)
                                {
                                    query_result << link_node->v.text.text;
                                }
                            }
                            /*GumboNode* link_node = static_cast<GumboNode*>(childNode->v.element.children.data[0]);
                            if (link_node)
                            {
                                query_result << link_node->v.text.text;
                            }*/
                        }
                        else
                        {
                            query_result << childNode->v.text.text;
                        }
                    }
                }
            }
            // search children (used for quest EndText (main objective) and quest ObjectiveText1-4. They don't exist together)
            if (!between && searchChildren/* && title_text && (contents == NULL || (contents != NULL && strcmp(title_text->v.text.text, contents) == 0))*/)
            {
                GumboVector* children = &node->v.element.children;
                for (unsigned int i = 0; i < children->length; ++i)
                {
                    GumboNode* childNode = static_cast<GumboNode*>(children->data[i]);
                    if (childNode)
                    {
                        if (childNode->v.element.tag == GUMBO_TAG_TBODY)
                        {
                            GumboVector* tableChildren = &childNode->v.element.children;
                            // save <tr> indexes
                            std::vector<uint32_t> trIndexes;
                            for (unsigned int f = 0; f < tableChildren->length; ++f)
                            {
                                // filter stuff
                                bool foundTag = false;
                                GumboNode* tableChild = static_cast<GumboNode*>(tableChildren->data[f]);
                                if (tableChild && tableChild->v.element.tag == GUMBO_TAG_TR)
                                {
                                    if (!ignoreContent)
                                        trIndexes.push_back(f);
                                    else
                                    {
                                        // hackfix - <a> link to NPC</a> slain
                                        /*if (preTag == GUMBO_TAG_A)
                                        {
                                            std::string checkstr = tableChild->v.text.text;
                                            if (checkstr.find("slain") != std::string::npos)
                                                return NULL;
                                        }*/

                                        GumboVector* rowChildren = &tableChild->v.element.children;
                                        for (unsigned int r = 0; r < rowChildren->length; ++r)
                                        {
                                            GumboNode* rowChild = static_cast<GumboNode*>(rowChildren->data[r]);
                                            // Filter Quest Objectives 1-4. They should not have <p style="height:26px">
                                            if (rowChild && preTag == GUMBO_TAG_A)
                                            {
                                                if (rowChild->v.element.tag == GUMBO_TAG_P)
                                                    return NULL;

                                                // hackfix - <a> link to NPC</a> slain
                                                /*std::string checkstr = rowChild->v.text.text;
                                                if (checkstr.find("</a> ") != std::string::npos)
                                                    return NULL;*/

                                                GumboVector* colChildren = &rowChild->v.element.children;
                                                for (unsigned int c = 0; c < colChildren->length; ++c)
                                                {
                                                    GumboNode* colChild = static_cast<GumboNode*>(colChildren->data[c]);
                                                    if (colChild && colChild->v.element.tag == GUMBO_TAG_P)
                                                        return NULL;

                                                    // filter Provided Item: <a class=qX>
                                                    if (colChild && colChild->v.element.tag == GUMBO_TAG_A)
                                                    {
                                                        GumboAttribute* s_attr = gumbo_get_attribute(&colChild->v.element.attributes, "class");
                                                        if (s_attr &&
                                                                ((strcmp(s_attr->value, "q0") == 0) ||
                                                                (strcmp(s_attr->value, "q1") == 0) ||
                                                                (strcmp(s_attr->value, "q2") == 0) ||
                                                                (strcmp(s_attr->value, "q3") == 0) ||
                                                                (strcmp(s_attr->value, "q4") == 0) ||
                                                                (strcmp(s_attr->value, "q5") == 0)))
                                                            return NULL;
                                                    }
                                                }

                                                if (rowChild && rowChild->v.element.tag == GUMBO_TAG_TH)
                                                {
                                                    // filter Provided Item: <a class=q1>
                                                    if (preTag == GUMBO_TAG_A)
                                                    {
                                                        GumboAttribute* s_attr = gumbo_get_attribute(&rowChild->v.element.attributes, "class");
                                                        if (s_attr && (strcmp(s_attr->value, "icon-list-icon") == 0))
                                                            return NULL;
                                                    }

                                                    for (unsigned int c = 0; c < colChildren->length; ++c)
                                                    {
                                                        GumboNode* colChild = static_cast<GumboNode*>(colChildren->data[c]);
                                                        if (colChild)
                                                        {
                                                            if (colChild->v.element.tag == GUMBO_TAG_P)
                                                            {
                                                                GumboAttribute* s_attr = gumbo_get_attribute(&colChild->v.element.attributes, "style");
                                                                if (s_attr && (strcmp(s_attr->value, "height:26px") == 0))
                                                                    return NULL;
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            // Filter content of Quest EndText/Objectives 1-4
                                            if (!foundTag && rowChild && rowChild->v.element.tag == GUMBO_TAG_TD)
                                            {
                                                GumboVector* colChildren = &rowChild->v.element.children;
                                                for (unsigned int c = 0; c < colChildren->length; ++c)
                                                {
                                                    GumboNode* colChild = static_cast<GumboNode*>(colChildren->data[c]);
                                                    if (colChild && colChild->v.element.tag == preTag)
                                                    {
                                                        GumboNode* link_node = static_cast<GumboNode*>(colChild->v.element.children.data[0]);
                                                        if (link_node)
                                                        {
                                                            std::string checkstr = link_node->v.text.text;
                                                            if (checkstr.find(ignoreContent) != std::string::npos)
                                                            {
                                                                //std::cout << "found ignore content, skipping tag";
                                                                foundTag = true;
                                                            }
                                                            else if (checkstr.find(" (") != std::string::npos)
                                                            {
                                                                //std::cout << "found ignore content, skipping tag";
                                                                foundTag = true;
                                                            }
                                                            else
                                                            {
                                                                foundTag = true;
                                                                trIndexes.push_back(f);
                                                            }
                                                        }
                                                    }
                                                    if (foundTag)
                                                        break;
                                                }
                                            }
                                            if (foundTag)
                                                break;
                                        }
                                    }

                                    if (foundTag)
                                    {
                                        foundTag = false;
                                        if (unique)
                                            continue;
                                    }
                                }
                            }

                            for (unsigned int f = 0; f < tableChildren->length; ++f)
                            {
                                GumboNode* tableChild = static_cast<GumboNode*>(tableChildren->data[f]);
                                if (tableChild && tableChild->v.element.tag == GUMBO_TAG_TR)
                                {
                                    if (std::find(trIndexes.begin(), trIndexes.end(), f) == trIndexes.end())
                                        continue;
                                    if (unique && fromEnd && f != trIndexes[trIndexes.size() - 1 - childIndex])
                                        continue;
                                    if (unique && !fromEnd && f != trIndexes[childIndex])
                                        continue;

                                    // Quest EndText should have quantity=1
                                    if (preTag != GUMBO_TAG_A)
                                    {
                                        GumboAttribute* t_attr = gumbo_get_attribute(&tableChild->v.element.attributes, "data-icon-list-quantity");
                                        if (t_attr && (strcmp(t_attr->value, "1") != 0))
                                            continue;
                                    }

                                    GumboVector* rowChildren = &tableChild->v.element.children;
                                    for (unsigned int r = 0; r < rowChildren->length; ++r)
                                    {
                                        GumboNode* rowChild = static_cast<GumboNode*>(rowChildren->data[r]);
                                        if (rowChild && rowChild->v.element.tag == GUMBO_TAG_TD)
                                        {
                                            GumboVector* colChildren = &rowChild->v.element.children;
                                            for (unsigned int c = 0; c < colChildren->length; ++c)
                                            {
                                                GumboNode* colChild = static_cast<GumboNode*>(colChildren->data[c]);
                                                if (colChild && colChild->v.element.tag == preTag)
                                                {
                                                    // Objectives <span> does not have class
                                                    if (preTag != GUMBO_TAG_A)
                                                    {
                                                        GumboAttribute* c_attr = gumbo_get_attribute(&colChild->v.element.attributes, "class");
                                                        if (c_attr && c_attr->value == "icon-list-quantity-wrapper")
                                                            continue;

                                                        // Quest EndText should have quantity=1
                                                        GumboAttribute* t_attr = gumbo_get_attribute(&colChild->v.element.attributes, "data-icon-list-quantity");
                                                        if (t_attr && (strcmp(t_attr->value, "1") == 0))
                                                            continue;
                                                    }

                                                    GumboNode* link_node = static_cast<GumboNode*>(colChild->v.element.children.data[0]);
                                                    if (link_node)
                                                    {
                                                        if (!unique && query_result.str().size())
                                                            query_result << ";";

                                                        query_result << link_node->v.text.text;
                                                        if (unique)
                                                            return NULL;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            // find children between two nodes
            if (between && attr_second_name && attr_second_value && title_text && (contents == NULL || (contents != NULL && strcmp(title_text->v.text.text, contents) == 0)))
            {
                //std::cout << "\n Selecting content between element with tag " << tag << " attribute: " << attr_name << ", value: " << attr_value << " content: " << contents << " and element with attribute: " << attr_second_name << ", value: " <<attr_second_value;
                // first node position
                unsigned int pos = node->index_within_parent;
                unsigned int pos_two = 0;

                // filter nodes with no children
                //std::cout << "\n First element has children: " << node->v.element.children.length;
                if (node->v.element.children.length && !hasChildren)
                    return 0;

                // filter those before preTag/preType
                bool foundPreTag = false;
                bool foundPreType = false;

                //std::cout << "\n First element position is " << pos;
                //std::cout << "\n First element content is " << title_text->v.text.text;

                // search parent for second node
                GumboNode* parentNode = node->parent;
                GumboVector* siblings = &parentNode->v.element.children;
                //std::cout << "\n Parent element has children: " << siblings->length;

                for (unsigned int i = pos + 1; i < siblings->length; ++i)
                {
                    GumboNode* childNode = static_cast<GumboNode*>(siblings->data[i]);
                    if (childNode)
                    {
                        //std::cout << "\n Loading parent child #" << i << " (content: " << childNode->v.text.text << "), type: " << childNode->type << ", tag: " << childNode->v.element.tag;

                        if (preTag != GUMBO_TAG_UNKNOWN && childNode->v.element.tag == preTag && !foundPreTag)
                        {
                            foundPreTag = true;
                            pos = i - 1;
                        }

                        if (preType != GUMBO_NODE_ELEMENT && childNode->type == preType && !foundPreType)
                        {
                            foundPreType = true;
                            pos = i - 1;
                        }

                        // skip text areas here
                        if (childNode->type != GUMBO_NODE_ELEMENT)
                            continue;

                        // check for correct second node
                        GumboAttribute* sb_attr = gumbo_get_attribute(&childNode->v.element.attributes, attr_second_name);
                        if (sb_attr && (strcmp(sb_attr->value, attr_second_value) == 0))
                        {
                            pos_two = childNode->index_within_parent;
                            //std::cout << "\n Second element Found! Position is " << pos_two << "\n";
                            break;
                        }
                        // find alt node
                        if (sb_attr && attr_second_alt_value != NULL && (strcmp(sb_attr->value, attr_second_alt_value) == 0))
                        {
                            pos_two = childNode->index_within_parent;
                            //std::cout << "\n Second element Found! Position is " << pos_two << "\n";
                            break;
                        }
                    }
                }

                if (pos_two > 0)
                {
                    // filter too close invalid nodes
                    if (pos_two - pos <= steps_back)
                        return NULL;

                    // display all nodes between 1 and 2
                    //std::cout << "Getting content between position " << pos << " and " << pos_two << " with stepback from second position: " << steps_back << "\n\n";
                    for (unsigned int i = pos + steps_back; i < (pos_two - steps_back); ++i) // TODO Check if + 1 needed sometimes
                    {
                        GumboNode* childNode = static_cast<GumboNode*>(siblings->data[i]);
                        if (childNode)
                        {
                            //if (preType != GUMBO_NODE_ELEMENT && childNode->type)
                            //std::cout << "\n Element found at pos " << i;
                            if (childNode->v.element.tag == GUMBO_TAG_BR) // TODO replace here or not
                                query_result << "\n"; //query_result << "$B";
                            else if (childNode->v.element.tag == GUMBO_TAG_A)
                            {
                                // Bug - gives random huge children length if has <a> tag in middle
                                GumboVector* childrenA = &childNode->v.element.children;
                                if (childrenA->length > 10)
                                {
                                    query_result << childNode->v.text.text;
                                    continue;
                                }
                                GumboNode* link_node = static_cast<GumboNode*>(childNode->v.element.children.data[0]);
                                if (link_node)
                                {
                                    query_result << link_node->v.text.text;
                                }
                            }
                            else if (childNode->type == GUMBO_NODE_TEXT)
                                query_result << childNode->v.text.text;
                        }
                    }
                }
            }
        }
    }

    GumboVector* children = &node->v.element.children;

    for (unsigned int i = 0; i < children->length; ++i)
    {
        GumboNode* childNode = static_cast<GumboNode*>(children->data[i]);
        parse_details(query_result, childNode, tag, attr_name, attr_value, contents, between, attr_second_name, attr_second_value, attr_second_alt_value, steps_back, unique, hasChildren, preTag, preType, searchChildren, childIndex, fromEnd, ignoreContent);
    }
    return NULL;
}

void parse_quest(std::ostringstream& query_result, GumboNode* node, unsigned int questPartID, unsigned int locale = 0)
{
    //const char* quest_details;
    GumboTag tag = GUMBO_TAG_UNKNOWN;
    GumboTag preTag = GUMBO_TAG_UNKNOWN;
    GumboNodeType preType = GUMBO_NODE_ELEMENT;
    const char* attr_name = NULL;
    const char* attr_value = NULL;
    const char* contents = NULL;
    bool between = false;
    const char* attr_second_name = NULL;
    const char* attr_second_value = NULL;
    const char* attr_second_alt_value = NULL;
    unsigned int steps_back = 0;
    bool unique = false;
    bool hasChildren = true;
    bool searchChildren = false;
    bool fromEnd = false;
    unsigned int childIndex = 0;
    const char* ignoreContent = NULL;

    switch (questPartID)
    {
    default:
    case 1: // Title
        attr_name = "class";
        attr_value = "heading-size-1";
        break;
    case 2: // Objectives
        attr_name = "class";
        attr_value = "heading-size-1";
        between = true;
        attr_second_name = "class";
        attr_second_value = "icon-list";
        steps_back = 0;
        preType = GUMBO_NODE_TEXT;
        break;
    case 3: // Details
        attr_name = "class";
        attr_value = "heading-size-3";
        between = true;
        attr_second_name = "id";
        attr_second_value = "lknlksndgg-progress";
        attr_second_alt_value = "lknlksndgg-completion";
        if (locale == 5)
            attr_second_value = "quest-reward-slider";
        steps_back = 1;
        break;
    case 4: // RequestItemsText
        attr_name = "id";
        attr_value = "lknlksndgg-progress";
        break;
    case 5: // OfferRewardText
        attr_name = "id";
        attr_value = "lknlksndgg-completion";
        break;
    case 6: // EndText
        searchChildren = true;
        attr_name = "class";
        attr_value = "icon-list";
        preTag = GUMBO_TAG_SPAN;
        ignoreContent = ": "; // Suggested Players: X
        childIndex = 0;
        fromEnd = true;
        unique = true;
        break;
    case 7: // ObjectiveText 1-4
        searchChildren = true;
        attr_name = "class";
        attr_value = "icon-list";
        preTag = GUMBO_TAG_A;
        ignoreContent = ": "; // Suggested Players: X
        childIndex = 0;
        fromEnd = true;
        break;
    }

    parse_details(query_result, node, tag, attr_name, attr_value, contents, between, attr_second_name, attr_second_value, attr_second_alt_value, steps_back, unique, hasChildren, preTag, preType, searchChildren, childIndex, fromEnd, ignoreContent);
}

std::string parse_quest(GumboNode* node, unsigned int questPartID, unsigned int locale = 1)
{
    std::ostringstream query_result;
    parse_quest(query_result, node, questPartID, locale);
    return query_result.str();
}

std::string ReadQuestText(GumboOutput* parsedPage, const std::string& questPart, uint32 locale)
{
    std::ostringstream questText;
    uint32 questPartId = questTextId(questPart);
    if (!questPartId)
        return "";

    return parse_quest(parsedPage->root, questPartId, locale);
}

std::string LoadQuestText(DatabaseConnect* dbCon, uint32 id, const std::string& questPart, uint32 locale)
{
    std::ostringstream command;
    std::string questText;
    std::string questCol = questColumnName(questPart, locale);
    std::string tableName = locale == 1 ? "quest_template" : "locales_quest";
    command << "SELECT " << questCol <<  " FROM " << tableName << " WHERE entry =" << std::to_string(id);
    dbCon->GetDbString(command.str(), questText, true);
    return questText;
}

QuestStrings LoadQuestFull(DatabaseConnect* dbCon, uint32 id, uint32 locale)
{
    QuestStrings qStrings;
    std::ostringstream command;
    std::vector<std::string> questText;
    std::string questColumns = questAllColumns(locale);
    std::string tableName = locale == 1 ? "quest_template" : "locales_quest";
    command << "SELECT " << questColumns <<  " FROM " << tableName << " WHERE entry = " << std::to_string(id);
    if (!dbCon->GetDbStrings(command.str(), questText, 10, true))
        return qStrings;

    qStrings.title = questText[0];
    qStrings.objectives = questText[1];
    qStrings.details = questText[2];
    qStrings.requestItemText = questText[3];
    qStrings.offerRewardText = questText[4];
    qStrings.endText = questText[5];
    qStrings.objectiveList[0] = questText[6];
    qStrings.objectiveList[1] = questText[7];
    qStrings.objectiveList[2] = questText[8];
    qStrings.objectiveList[3] = questText[9];

    return qStrings;
}

GumboOutput* GumboParsePage(const std::string& page)
{
    return gumbo_parse(page.c_str());
}

void GumboClosePage(GumboOutput* output)
{
    gumbo_destroy_output(&kGumboDefaultOptions, output);
}

WowheadQuestInfo* LoadWowheadQuestInfo(uint32 id, uint32 expansion, uint32 locale, bool onlyOneVersion = false)
{
    if (!id)
        return nullptr;

    // check if already saved
    if (expansion && locale && sDataMgr.questWowheadInfoList[id])
    {
        WowheadQuestInfo* savedInfo = sDataMgr.questWowheadInfoList[id];
        if (savedInfo->IsLoaded(expansion, locale))
        {
            savedInfo->SetCurExpansion(expansion);
            savedInfo->SetCurLocale(locale);
            return savedInfo;
        }
        else
        {
            savedInfo->LoadEntryData(expansion, locale);
            if (savedInfo->IsLoaded(expansion, locale))
            {
                savedInfo->SetCurExpansion(expansion);
                savedInfo->SetCurLocale(locale);
                return savedInfo;
            }
            else
                return nullptr;
        }
    }

    WowheadQuestInfo* qInfo = nullptr;
    bool isLoaded = false;
    if (sDataMgr.questDatabaseInfoList[id])
    {
        isLoaded = true;
        qInfo = sDataMgr.questWowheadInfoList[id];
    }
    else
        qInfo = new WowheadQuestInfo(id, expansion ? expansion : 1, locale ? locale : 1);

    if (onlyOneVersion && expansion && locale)
        qInfo->LoadEntryData(expansion, locale);
    else if (expansion && !locale)
    {
        for (auto i = 1; i <= MAX_LOCALE; ++i)
            qInfo->LoadEntryData(expansion, i);
    }
    else if (!expansion && locale)
    {
        for (auto i = 1; i <= MAX_EXPANSION; ++i)
            qInfo->LoadEntryData(i, locale);
    }
    else
        qInfo->LoadEntryDataAllVersions();

    // if not loaded english classic
    if (!qInfo->IsLoaded(expansion ? expansion : 1, locale ? locale : 1))
    {
        bool silent = false;
        if (!silent)
        {
            if (locale != 1)
                msg_delay("> WH " + typeName(TYPE_QUEST) + " #" + std::to_string(id) + "-" + localeName(locale ? locale : 1) + ": Translation Not Found! \n");
            else
                msg_delay("> WH " + typeName(TYPE_QUEST) + " #" + std::to_string(id) + "-" + localeName(locale ? locale : 1) + ": Quest Not Found! \n");
        }

        // erase after checking above
        if (!isLoaded)
        {
            delete qInfo;
        }
        //sDataMgr.questDatabaseInfoList[id] = nullptr;
        return nullptr;
    }

    if (!isLoaded) sDataMgr.questWowheadInfoList[id] = qInfo;
    return qInfo;
}

DatabaseQuestInfo* LoadDatabaseQuestInfo(uint32 id, uint32 expansion, uint32 locale, bool onlyOneVersion = false, bool silent = false)
{
    if (!id)
        return nullptr;

    bool allLocales = false;
    if (!expansion && !locale)
        allLocales = true;

    //DatabaseQuestInfo* questInfo = nullptr;

    // check if already saved
    if (expansion && locale && sDataMgr.questDatabaseInfoList[id])
    {
        DatabaseQuestInfo* savedInfo = sDataMgr.questDatabaseInfoList[id];
        if (savedInfo->IsLoaded(expansion, locale))
        {
            savedInfo->SetCurExpansion(expansion);
            savedInfo->SetCurLocale(locale);
            return savedInfo;
        }
        else
        {
            savedInfo->LoadEntryData(expansion, locale);
            if (savedInfo->IsLoaded(expansion, locale))
            {
                savedInfo->SetCurExpansion(expansion);
                savedInfo->SetCurLocale(locale);
                return savedInfo;
            }
            else
                return nullptr;
        }
    }

    DatabaseQuestInfo* qInfo = nullptr;
    bool isLoaded = false;
    if (sDataMgr.questDatabaseInfoList[id])
    {
        isLoaded = true;
        qInfo = sDataMgr.questDatabaseInfoList[id];
    }
    else
        qInfo = new DatabaseQuestInfo(id, expansion ? expansion : 1, locale ? locale : 1);
    // set default expansion or english
    //auto qInfo = new DatabaseQuestInfo(id, expansion ? expansion : 1, locale ? locale : 1);
    if (onlyOneVersion && expansion && locale)
        qInfo->LoadEntryData(expansion, locale);
    else if (expansion && !locale)
    {
        for (auto i = 1; i <= MAX_LOCALE; ++i)
            qInfo->LoadEntryData(expansion, i);
    }
    else if (!expansion && locale)
    {
        for (auto i = 1; i <= MAX_EXPANSION; ++i)
            qInfo->LoadEntryData(i, locale);
    }
    else
        qInfo->LoadEntryDataAllVersions();

    /*if (expansion)
        qInfo->SetCurExpansion(expansion);
    if (locale)
        qInfo->SetCurLocale(locale);*/

    if (!qInfo->IsLoaded(expansion ? expansion : 1, locale ? locale : 1))
    {
        if (!silent)
        {
            if (locale > 1)
                msg_delay("> DB " + typeName(TYPE_QUEST) + " #" + std::to_string(id) + "-" + localeName(locale ? locale : 1) + ": Translation Not Found! \n");
            else
                msg_delay("> DB " + typeName(TYPE_QUEST) + " #" + std::to_string(id) + "-" + localeName(locale ? locale : 1) + ": Quest Not Found! \n");
        }

        // erase after checking above
        //sDataMgr.questDatabaseInfoList.erase(id);
        delete qInfo;
        return nullptr;
    }


    if (!isLoaded) sDataMgr.questDatabaseInfoList[id] = qInfo;
    return qInfo;
}

const char* parse_npc(std::ostringstream& query_result, GumboNode* node, unsigned int npcPartId, unsigned int locale = 0)
{
    //const char* quest_details;

    GumboTag tag = GUMBO_TAG_UNKNOWN;
    GumboTag preTag = GUMBO_TAG_UNKNOWN;
    GumboNodeType preType = GUMBO_NODE_ELEMENT;
    const char* attr_name = NULL;
    const char* attr_value = NULL;
    const char* contents = NULL;
    bool between = false;
    const char* attr_second_name = NULL;
    const char* attr_second_value = NULL;
    const char* attr_second_alt_value = NULL;
    unsigned int steps_back = 0;
    bool unique = false;
    bool hasChildren = true;
    bool searchChildren = false;
    bool fromEnd = false;
    unsigned int childIndex = 0;
    const char* ignoreContent = NULL;

    // TODO Fix Name including Subname
    // TODO Make Subname a separate thing
    // e.g. by parsing JS code instead "name":"Paldesse","react":[1,1],"tag":"Cloth Armor Merchant","type"
    switch (npcPartId)
    {
    case 1: // Name
        attr_name = "class";
        attr_value = "heading-size-1";
        break;
        default: return nullptr;
    }

    parse_details(query_result, node, tag, attr_name, attr_value, contents, between, attr_second_name, attr_second_value, attr_second_alt_value, steps_back, unique, hasChildren, preTag, preType, searchChildren, childIndex, fromEnd, ignoreContent);

    return NULL;
}

void search_for_links(GumboNode* node)
{
    if (node->type != GUMBO_NODE_ELEMENT)
        return;

    if (node->v.element.tag == GUMBO_TAG_A)
    {
        GumboAttribute* href = gumbo_get_attribute(&node->v.element.attributes, "href");
        if (href)
            std::cout << href->value << "\n";
    }

    GumboVector* children = &node->v.element.children;
    for (unsigned int i = 0; i < children->length; i++)
    {
        search_for_links(static_cast<GumboNode*>(children->data[i]));
    }
}

int main(int argc, char* argv[])
{
    // Set console code page to UTF-8 so console known how to interpret string data
    SetConsoleOutputCP(CP_UTF8);
    // Enable buffering to prevent VS from chopping up UTF-8 byte sequences
    setvbuf(stdout, nullptr, _IOFBF, 1000);

    std::ostringstream query_result;

    unsigned int activity = 1;
    unsigned int locale = 0;
    unsigned int expansion = 0;
    unsigned int type = 0;
    unsigned int entry = 0;
    unsigned int entryPart = 0;
    unsigned int max_entry = 0;
    bool isCommandLine = false;
    if (argc > 1)
        isCommandLine = true;

    msg_delay("> DB: Initialize connection...\n");
    sDataMgr.Initialize();


    if (!isCommandLine) // no command line arguments
    {
        // Choose Activity
        std::cout << "Select option: \n";
        std::cout << "1) Test Page Info \n";
        std::cout << "2) Cache Pages \n";
        std::cout << "3) Test DB connect \n";
        std::cout << "4) Cache quest stats (Long Loading) \n";
        std::cout << "5) DB quest stats \n";
        std::cout << "6) Work on translations \n";
        std::cin >> activity;

        if (activity == 1) // run single page
        {
            // Choose parse type
            std::cout << "\nSelect parse type: \n";
            std::cout << "1) Quest \n";
            std::cout << "2) NPC \n";
            std::cin >> type;

            // Choose expansion
            std::cout << "\nSelect Expansion: \n";
            //std::cout << "0) All \n";
            std::cout << "1) Classic \n";
            std::cout << "2) TBC \n";
            std::cout << "3) WotLK \n";
            std::cout << "4) Retail \n";
            std::cin >> expansion;

            // Choose locale
            std::cout << "\nSelect Locale: \n";
            //std::cout << "0) All Locales \n";
            std::cout << "1) English (LOC 0) \n";
            std::cout << "2) Korean  (LOC 1) \n";
            std::cout << "3) French  (LOC 2) \n";
            std::cout << "4) German  (LOC 3) \n";
            std::cout << "5) Chinese (LOC 4 + 5) \n";
            std::cout << "6) Spanish (LOC 6 + 7) \n";
            std::cout << "7) Russian (LOC 8)\n";
            std::cin >> locale;

            // only quest is supported
            if (type == 0)
            {
                msg_delay("You must select what to load!\n\n");
                std::cin >> type;
            }

            // quest ID
            std::cout << "\nEnter ID: \n";
            std::cin >> entry;

            if (entry == 0)
            {
                msg_delay("You must select ID!\n\n");
                std::cin >> entry;
            }

            /*if (type == 1) // quest
            {
                std::cout << "\nWhat part of quest do you need? \n";
                std::cout << "1) Title           (Title) \n";
                std::cout << "2) Description     (Details) \n";
                std::cout << "3) Objectives      (Objectives) \n";
                std::cout << "4) Progress        (RequestItemsText) \n";
                std::cout << "5) Completion      (OfferRewardText) \n";
                std::cout << "6) Main Objective  (EndText) \n";
                std::cout << "7) List Objectives (ObjectiveText 1-4) \n";
                std::cin >> entryPart;
            }*/

            if (type == 2) // npc
            {
                std::cout << "\nWhat part of npc do you need? \n";
                std::cout << "1) Name            (Name) \n";
                std::cin >> entryPart;
            }

//            if (entryPart == 0)
//            {
//                msg_delay("You must select entry part!\n\n");
//                std::cin >> entryPart;
//            }
        }
        if (activity == 2) // cache multiple pages
        {
            // Choose parse type
            std::cout << "\nSelect parse type: \n";
            std::cout << "1) Quest \n";
            std::cout << "2) NPC \n";
            std::cin >> type;

            // only quest is supported
            if (type == 0)
            {
                msg_delay("You must select what to load!\n\n");
                std::cin >> type;
            }

            std::cout << "\nSelect Expansion: (Default: Classic) \n";
            std::cout << "0) All versions \n";
            std::cout << "1) Classic \n";
            std::cout << "2) TBC \n";
            std::cout << "3) WotLK \n";
            std::cout << "4) Retail \n";
            std::cin >> expansion;

            // Choose locale
            std::cout << "\nSelect Locale: (Default: English)\n";
            std::cout << "0) All locales \n";
            std::cout << "1) English (LOC 0) \n";
            std::cout << "2) Korean  (LOC 1) \n";
            std::cout << "3) French  (LOC 2) \n";
            std::cout << "4) German  (LOC 3) \n";
            std::cout << "5) Chinese (LOC 4 + 5) \n";
            std::cout << "6) Spanish (LOC 6 + 7) \n";
            std::cout << "7) Russian (LOC 8)\n";
            std::cin >> locale;

            // entry
            std::cout << "\nEnter ID: \n";
            std::cin >> entry;

            if (entry == 0)
            {
                msg_delay("You must select ID! \n\n");
                std::cin >> entry;
            }

            // max entry
            std::cout << "\nEnter Max ID: Default(None) \n";
            std::cin >> max_entry;

            cachePagesRange(TypeId(type), entry, max_entry, locale, expansion);
            return 1;
        }
        if (activity == 3)
        {
            msg_delay("> DB: Trying to connect...\n");
            loadFromDB();
            return 1;
        }
        if (activity == 4)
        {
            std::cout << "\nSelect Expansion: (Default: Classic) \n";
            std::cout << "0) All versions \n";
            std::cout << "1) Classic \n";
            std::cout << "2) TBC \n";
            std::cout << "3) WotLK \n";
            std::cout << "4) Retail \n";
            std::cin >> expansion;

            // Choose locale
            std::cout << "\nSelect Locale: \n";
            std::cout << "0) All locales \n";
            std::cout << "2) Korean  (LOC 1) \n";
            std::cout << "3) French  (LOC 2) \n";
            std::cout << "4) German  (LOC 3) \n";
            std::cout << "5) Chinese (LOC 4 + 5) \n";
            std::cout << "6) Spanish (LOC 6 + 7) \n";
            std::cout << "7) Russian (LOC 8)\n";
            std::cin >> locale;

            msg_delay("\n");
            msg_delay("> WH: Collecting data...\n");

            int maxQuestId[MAX_EXPANSION];
            maxQuestId[0] = 0;
            maxQuestId[1] = 0;
            maxQuestId[2] = 0;

            for (auto e = 1; e <= MAX_EXPANSION; ++e) // expansion
            {
                if (expansion && expansion != e)
                    continue;

                msg_delay("\n> %s \n", expansionName(e).c_str());
                for (auto i = 1; i <= MAX_LOCALE; ++i) // locales
                {
                    if (locale && locale != i && i != 1)
                        continue;

                    std::string cacheLocation = "cache/" + expansionName(e) + "/" + localeName(i) + "/quest/";
                    if (!std::filesystem::is_directory(cacheLocation))
                    {
                        msg_delay(">  %s %d \n", localeName(i).c_str(), 0);
                        continue;
                    }

                    auto counter = 0;
                    auto dirIter = std::filesystem::directory_iterator(cacheLocation);
                    for (auto& fl : dirIter)
                    {
                        if (fl.is_regular_file())
                        {
                            ++counter;
                            const std::filesystem::path& p(fl.path());
                            uint32 qID = stoi(p.stem().string());
                            if ((int)qID > maxQuestId[e - 1])
                                maxQuestId[e - 1] = (int)qID;
                        }
                    }
                    msg_delay(">  %s total: %d, max id: %d \n", localeName(i).c_str(), counter, maxQuestId[e - 1]);
                }
            }

            uint32 counter = 0;
            std::map<uint32, std::string> englishEndText;
            std::map<uint32, std::string> englishObjText;
            std::map<uint32, std::string> englishReqText;
            std::map<uint32, std::string> englishOfferText;
            //unsigned int limit = 10000;

            msg_delay("\n> WH: Analyzing data... \n\n");

            for (auto e = 1; e <= MAX_EXPANSION; ++e) // expansion
            {
                if (expansion && expansion != e)
                    continue;

                //if (counter > limit)
                //    break;
                msg_delay("> %s \n", expansionName(e).c_str());
                msg_delay("> WH: Loading all quests... \n");

                for (auto i = 1; i <= MAX_LOCALE; ++i) // locales
                {
                    if (locale && locale != i && i != 1)
                        continue;

                    // test non eng
                    //if (!(i == 1 || i == 7))
                    //    continue;
                    counter = 0;
                    msg_delay("\n>  %s \n", localeName(i).c_str());

                    uint32 missingTitle = 0;
                    uint32 missingEndText = 0;
                    uint32 missingObjText = 0;
                    uint32 missingReqText = 0;
                    uint32 missingOfferText = 0;
                    uint32 missingDetails = 0;
                    uint32 missingObjective1 = 0;
                    uint32 missingObjective2 = 0;
                    uint32 missingObjective3 = 0;
                    uint32 missingObjective4 = 0;

                    // has english text
                    uint32 hasEngTitle = 0;
                    uint32 hasEngEndText = 0;
                    uint32 hasEngObjText = 0;
                    uint32 hasEngReqText = 0;
                    uint32 hasEngOfferText = 0;
                    uint32 hasEngDetails = 0;
                    uint32 hasEngObjective1 = 0;
                    uint32 hasEngObjective2 = 0;
                    uint32 hasEngObjective3 = 0;
                    uint32 hasEngObjective4 = 0;

                    // wildcards
                    uint32 missingClassTag = 0;
                    uint32 missingRaceTag = 0;
                    uint32 missingNameTag = 0;

                    // write statistics details
                    std::string filesLocation = "stats/wh/" + expansionName(e) + "/" + localeName(i) + "/" + typeName(TYPE_QUEST) + "/";
                    //std::string updateStmt = readFromFile(filesLocation + "missingEngTexts.txt");
                    writeToFile("-- WH Locales Stats\n", "missingQuestText", filesLocation);
                    writeToFile("-- WH Locales Stats\n", "engQuestText", filesLocation);
                    writeToFile("-- WH Locales Stats\n", "missingTags", filesLocation);
                    std::string missingQuestText;
                    std::string engQuestText;
                    std::string missingTags;

                    std::string cacheLocation = "cache/" + expansionName(e) + "/" + localeName(i) + "/quest/";
                    if (!std::filesystem::is_directory(cacheLocation))
                        continue;

                    auto dirIter = std::filesystem::directory_iterator(cacheLocation);
                    for (auto& fl : dirIter)
                    {
                        if (fl.is_regular_file())
                        {
                            const std::filesystem::path& p(fl.path());
                            uint32 qID = stoi(p.stem().string());
                            //msg_nodelay("> WH: trying q %d \n", qID);

                            WowheadQuestInfo* qWhInfo = LoadWowheadQuestInfo(qID, e, i, true);
                            if (!qWhInfo)
                            {
                                //msg_delay("> DB " + typeName(TYPE_QUEST) + " #" + std::to_string(entry) + "-" + localeName(locale) + ": Failed to load from Database! \n");
                                continue;
                            }
                            else
                            {
                                counter++;
                                //msg_nodelay(".");// msg_delay_set(".", 50);
                                if ((counter % 1000) == 0)
                                    msg_nodelay(std::to_string(counter));
                                else if ((counter % 100) == 0)
                                    msg_nodelay(".");

                                //if ((counter % 1000) == 0)
                                //    msg_nodelay("\n");
                            }
                        }
                    }

                    msg_delay(">  loaded: %d %s quests \n", counter, localeName(i).c_str());

                    // do not check english here
                    if (i == 1)
                        continue;

                    msg_delay("\n> WH: Checking missing data... \n");

                    bool hasLocales = false;
                    counter = 0;
                    for (const auto& questInfo : sDataMgr.questWowheadInfoList)
                    {
                        WowheadQuestInfo* qWhInfo = questInfo.second;
                        if (!qWhInfo)
                            continue;

                        // skip if locale is missing
                        /*if (qWhInfo->GetTitle(e, i).empty() && qWhInfo->GetDetails(e, i).empty() && qWhInfo->GetObjective(e, i).empty())
                            continue;*/

                        hasLocales = true;
                        counter++;

                        // default english, set cur locale to current checked locale
                        qWhInfo->SetCurExpansion(e);
                        qWhInfo->SetCurLocale(i);

                        if ((counter % 1000) == 0)
                            msg_delay_set(std::to_string(counter), 10);
                        else if ((counter % 100) == 0)
                            msg_delay_set(".", 10);

                        //if ((counter % 1000) == 0)
                        //    msg_nodelay("\n");

                        //msg_nodelay(".");

                        // check if missing texts
                        if (!qWhInfo->GetTitle(e, 1).empty() && qWhInfo->GetTitle().empty())
                        {
                            missingTitle++;
                            missingQuestText += "title:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                        }
                        if (!qWhInfo->GetTitle(e, 1).empty() && !qWhInfo->GetTitle().empty() && strcmp(qWhInfo->GetTitle(e, 1).c_str(), qWhInfo->GetTitle().c_str()) == 0)
                        {
                            hasEngTitle++;
                            engQuestText += "title:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                        }

                        if (!qWhInfo->GetEndText(e, 1).empty() && qWhInfo->GetEndText().empty())
                        {
                            missingEndText++;
                            missingQuestText += "endText:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                        }
                        if (!qWhInfo->GetEndText(e, 1).empty() && !qWhInfo->GetEndText().empty() && strcmp(qWhInfo->GetEndText(e, 1).c_str(), qWhInfo->GetEndText().c_str()) == 0)
                        {
                            hasEngEndText++;
                            engQuestText += "endText:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                        }
                        if (!qWhInfo->GetEndText(e, 1).empty() && !qWhInfo->GetEndText().empty())
                        {
                            if (hasNameTag(qWhInfo, "endText", false, e, 1) && !hasNameTag(qWhInfo, "endText"))
                            {
                                missingNameTag++;
                                missingTags += "name:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                            }
                            if (hasRaceTag(qWhInfo, "endText", false, e, 1) && !hasRaceTag(qWhInfo, "endText"))
                            {
                                missingRaceTag++;
                                missingTags += "race:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                            }
                            if (hasClassTag(qWhInfo, "endText", false, e, 1) && !hasClassTag(qWhInfo, "endText"))
                            {
                                missingClassTag++;
                                missingTags += "class:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                            }
                        }

                        if (!qWhInfo->GetObjectives(e, 1).empty() && qWhInfo->GetObjectives().empty())
                        {
                            missingObjText++;
                            missingQuestText += "objective:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                        }
                        if (!qWhInfo->GetObjectives(e, 1).empty() && !qWhInfo->GetObjectives().empty() && strcmp(qWhInfo->GetObjectives(e, 1).c_str(), qWhInfo->GetObjectives().c_str()) == 0)
                        {
                            hasEngObjText++;
                            engQuestText += "objective:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                        }
                        if (!qWhInfo->GetObjectives(e, 1).empty() && !qWhInfo->GetObjectives().empty())
                        {
                            if (hasNameTag(qWhInfo, "objectives", false,e, 1) && !hasNameTag(qWhInfo, "objectives"))
                            {
                                missingNameTag++;
                                missingTags += "name:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                            }
                            if (hasRaceTag(qWhInfo, "objectives", false, e, 1) && !hasRaceTag(qWhInfo, "objectives"))
                            {
                                missingRaceTag++;
                                missingTags += "race:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                            }
                            if (hasClassTag(qWhInfo, "objectives", false, e, 1) && !hasClassTag(qWhInfo, "objectives"))
                            {
                                missingClassTag++;
                                missingTags += "class:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                            }
                        }

                        if (!qWhInfo->GetRequestItemText(e, 1).empty() && qWhInfo->GetRequestItemText().empty())
                        {
                            missingReqText++;
                            missingQuestText += "reqItem:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                        }
                        if (!qWhInfo->GetRequestItemText(e, 1).empty() && !qWhInfo->GetRequestItemText().empty() && strcmp(qWhInfo->GetRequestItemText(e, 1).c_str(), qWhInfo->GetRequestItemText().c_str()) == 0)
                        {
                            hasEngReqText++;
                            engQuestText += "reqItem:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                        }
                        if (!qWhInfo->GetRequestItemText(e, 1).empty() && !qWhInfo->GetRequestItemText().empty())
                        {
                            if (hasNameTag(qWhInfo, "requestItemText", false, e, 1) && !hasNameTag(qWhInfo, "requestItemText"))
                            {
                                missingNameTag++;
                                missingTags += "name:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                            }
                            if (hasRaceTag(qWhInfo, "requestItemText", false, e, 1) && !hasRaceTag(qWhInfo, "requestItemText"))
                            {
                                missingRaceTag++;
                                missingTags += "race:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                            }
                            if (hasClassTag(qWhInfo, "requestItemText", false, e, 1) && !hasClassTag(qWhInfo, "requestItemText"))
                            {
                                missingClassTag++;
                                missingTags += "class:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                            }
                        }

                        if (!qWhInfo->GetOfferRewardText(e, 1).empty() && qWhInfo->GetOfferRewardText().empty())
                        {
                            missingOfferText++;
                            missingQuestText += "offer:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                        }
                        if (!qWhInfo->GetOfferRewardText(e, 1).empty() && !qWhInfo->GetOfferRewardText().empty() && strcmp(qWhInfo->GetOfferRewardText(e, 1).c_str(), qWhInfo->GetOfferRewardText().c_str()) == 0)
                        {
                            hasEngOfferText++;
                            engQuestText += "offer:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                        }
                        if (!qWhInfo->GetOfferRewardText(e, 1).empty() && !qWhInfo->GetOfferRewardText().empty())
                        {
                            if (hasNameTag(qWhInfo, "offerRewardText", false, e, 1) && !hasNameTag(qWhInfo, "offerRewardText"))
                            {
                                missingNameTag++;
                                missingTags += "name:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                            }
                            if (hasRaceTag(qWhInfo, "offerRewardText", false, e, 1) && !hasRaceTag(qWhInfo, "offerRewardText"))
                            {
                                missingRaceTag++;
                                missingTags += "race:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                            }
                            if (hasClassTag(qWhInfo, "offerRewardText", false, e, 1) && !hasClassTag(qWhInfo, "offerRewardText"))
                            {
                                missingClassTag++;
                                missingTags += "class:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                            }
                        }

                        if (!qWhInfo->GetDetails(e, 1).empty() && qWhInfo->GetDetails().empty())
                        {
                            missingDetails++;
                            missingQuestText += "details:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                        }
                        if (!qWhInfo->GetDetails(e, 1).empty() && !qWhInfo->GetDetails().empty() && strcmp(qWhInfo->GetDetails(e, 1).c_str(), qWhInfo->GetDetails().c_str()) == 0)
                        {
                            hasEngDetails++;
                            engQuestText += "details:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                        }
                        if (!qWhInfo->GetDetails(e, 1).empty() && !qWhInfo->GetDetails().empty())
                        {
                            if (hasNameTag(qWhInfo, "details", false, e, 1) && !hasNameTag(qWhInfo, "details"))
                            {
                                missingNameTag++;
                                missingTags += "name:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                            }
                            if (hasRaceTag(qWhInfo, "details", false, e, 1) && !hasRaceTag(qWhInfo, "details"))
                            {
                                missingRaceTag++;
                                missingTags += "race:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                            }
                            if (hasClassTag(qWhInfo, "details", false, e, 1) && !hasClassTag(qWhInfo, "details"))
                            {
                                missingClassTag++;
                                missingTags += "class:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                            }
                        }

                        // Objective 1-4
                        if (!qWhInfo->GetObjective(0, e, 1).empty() && qWhInfo->GetObjective(0).empty())
                        {
                            missingObjective1++;
                            missingQuestText += "obj1:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                        }
                        if (!qWhInfo->GetObjective(0, e, 1).empty() && !qWhInfo->GetObjective(0).empty() && strcmp(qWhInfo->GetObjective(0, e, 1).c_str(), qWhInfo->GetObjective(0).c_str()) == 0)
                        {
                            hasEngObjective1++;
                            engQuestText += "obj1:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                        }

                        if (!qWhInfo->GetObjective(1, e, 1).empty() && qWhInfo->GetObjective(1).empty())
                        {
                            missingObjective2++;
                            missingQuestText += "obj2:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                        }
                        if (!qWhInfo->GetObjective(1, e, 1).empty() && !qWhInfo->GetObjective(1).empty() && strcmp(qWhInfo->GetObjective(1, e, 1).c_str(), qWhInfo->GetObjective(1).c_str()) == 0)
                        {
                            hasEngObjective2++;
                            engQuestText += "obj2:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                        }

                        if (!qWhInfo->GetObjective(2, e, 1).empty() && qWhInfo->GetObjective(2).empty())
                        {
                            missingObjective3++;
                            missingQuestText += "obj3:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                        }
                        if (!qWhInfo->GetObjective(2, e, 1).empty() && !qWhInfo->GetObjective(2).empty() && strcmp(qWhInfo->GetObjective(2, e, 1).c_str(), qWhInfo->GetObjective(2).c_str()) == 0)
                        {
                            hasEngObjective3++;
                            engQuestText += "obj3:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                        }

                        if (!qWhInfo->GetObjective(3, e, 1).empty() && qWhInfo->GetObjective(3).empty())
                        {
                            missingObjective4++;
                            missingQuestText += "obj4:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                        }
                        if (!qWhInfo->GetObjective(3, e, 1).empty() && !qWhInfo->GetObjective(3).empty() && strcmp(qWhInfo->GetObjective(3, e, 1).c_str(), qWhInfo->GetObjective(3).c_str()) == 0)
                        {
                            hasEngObjective4++;
                            engQuestText += "obj4:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                        }
                        /*for (auto index = 0; index < 4; ++index)
                        {
                            if (!qDbInfo->GetObjective(index, e, 1).empty() && qDbInfo->GetObjective(index).empty())
                                missingObjective[0][index]++;
                        }*/
                    }

                    if (hasLocales)
                    {
                        msg_delay("\n");
                        msg_delay(">   Missing Title: %d \n", missingTitle);
                        msg_delay(">   Missing Objectives: %d \n", missingObjText);
                        msg_delay(">   Missing Details: %d \n", missingDetails);
                        msg_delay(">   Missing RequestItemText: %d \n", missingReqText);
                        msg_delay(">   Missing OfferRewardText: %d \n", missingOfferText);
                        msg_delay(">   Missing EndText: %d \n", missingEndText);
                        msg_delay(">   Missing Objective1-4: %d - %d - %d - %d \n", missingObjective1, missingObjective2, missingObjective3, missingObjective4);

                        if (hasEngTitle)
                            msg_delay(">   English Title: %d \n", hasEngTitle);
                        if (hasEngObjText)
                            msg_delay(">   English Objectives: %d \n", hasEngObjText);
                        if (hasEngDetails)
                            msg_delay(">   English Details: %d \n", hasEngDetails);
                        if (hasEngReqText)
                            msg_delay(">   English RequestItemText: %d \n", hasEngReqText);
                        if (hasEngOfferText)
                            msg_delay(">   English OfferRewardText: %d \n", hasEngOfferText);
                        if (hasEngEndText)
                            msg_delay(">   English EndText: %d \n", hasEngEndText);

                        if (i == 3 || i == 4 || i == 6 || i == 7)
                        {
                            if (missingNameTag)
                                msg_delay(">   Missing Name Tag: %d \n", missingNameTag);
                            if (missingRaceTag)
                                msg_delay(">   Missing Race Tag: %d \n", missingRaceTag);
                            if (missingClassTag)
                                msg_delay(">   Missing Class Tag: %d \n", missingClassTag);
                        }

                        // write stats to file
                        writeToFile(missingQuestText.c_str(), "missingQuestText", filesLocation);
                        writeToFile(engQuestText.c_str(), "engQuestText", filesLocation);
                        if (i == 3 || i == 4 || i == 6 || i == 7)
                            writeToFile(missingTags.c_str(), "missingTags", filesLocation);
                    }
                }
                msg_delay("\n");
            }

            return 1;
        }
        if (activity == 5)
        {
            std::cout << "\nSelect Expansion: (Default: Classic) \n";
            std::cout << "0) All versions \n";
            std::cout << "1) Classic \n";
            std::cout << "2) TBC \n";
            std::cout << "3) WotLK \n";
            std::cout << "4) Retail \n";
            std::cin >> expansion;

            // Choose locale
            std::cout << "\nSelect Locale: \n";
            std::cout << "0) All locales \n";
            std::cout << "2) Korean  (LOC 1) \n";
            std::cout << "3) French  (LOC 2) \n";
            std::cout << "4) German  (LOC 3) \n";
            std::cout << "5) Chinese (LOC 4 + 5) \n";
            std::cout << "6) Spanish (LOC 6 + 7) \n";
            std::cout << "7) Russian (LOC 8)\n";
            std::cin >> locale;

            msg_delay("\n");
            msg_delay("> DB: Collecting data...\n");
            //msg_delay("> DB: Quests total: \n");

            int maxQuestId[MAX_EXPANSION];
            maxQuestId[0] = 0;
            maxQuestId[1] = 0;
            maxQuestId[2] = 0;

            for (auto e = 1; e <= MAX_EXPANSION; ++e) // expansion
            {
                if (expansion && expansion != e)
                    continue;

                msg_delay("\n> %s \n", expansionName(e).c_str());

                auto DbConnect = sDataMgr.GetCon(e);
                if (!DbConnect || !DbConnect->IsConnected())
                {
                    msg_delay("> DB: Failed to connect! \n");
                    //delete DbConnect;
                    return 0;
                }

                int counter = 0;
                DbConnect->GetDbInt("SELECT COUNT(*) FROM quest_template", counter);
                DbConnect->GetDbInt("SELECT MAX(entry) FROM quest_template", maxQuestId[e - 1]);
                msg_delay("> total: %d \n", counter);
                msg_delay("> max id: %d \n", maxQuestId[e - 1]);

                msg_delay("> DB: Quests translated (Title OR Details != NULL): \n");

                for (auto i = 2; i <= MAX_LOCALE; ++i) // locales
                {
                    if (locale && locale != i)
                        continue;

                    int trCount = 0;
                    DbConnect->GetDbInt("SELECT COUNT(*) FROM locales_quest WHERE Title_loc" + std::to_string(localeRealId(i)) + " IS NOT NULL"
                                                                                                                                 " OR Details_loc" + std::to_string(localeRealId(i)) + " IS NOT NULL"
                                                                                                                                 " OR Objectives_loc" + std::to_string(localeRealId(i)) + " IS NOT NULL", trCount);
                    msg_delay(">  %s %d \n", localeName(i).c_str(), trCount);
                    if (locale && trCount == 0)
                    {
                        msg_delay("> DB: no %s translations found in database! \n", localeName(i).c_str());
                        return 0;
                    }
                }

                //delete DbConnect;
            }

            msg_delay("\n> DB: Analyzing data... \n");

            for (auto e = 1; e <= MAX_EXPANSION; ++e) // expansion
            {
                if (expansion && expansion != e)
                    continue;

                msg_delay("\n> %s \n", expansionName(e).c_str());

                msg_delay("> DB: Loading all quests... \n");

                uint32 counter = 0;
                for (auto d = 1; d <= maxQuestId[e - 1]; ++d)
                {
                    DatabaseQuestInfo* qDbInfo = nullptr;
                    if (locale) // load english and locale
                    {
                        qDbInfo = LoadDatabaseQuestInfo(d, e, 1, true, true);
                        if (qDbInfo)
                            qDbInfo = LoadDatabaseQuestInfo(d, e, locale, true, true);
                    }
                    else // load all locales
                        qDbInfo = LoadDatabaseQuestInfo(d, e, 0, false, true);

                    if (!qDbInfo)
                    {
                        //msg_delay("> DB " + typeName(TYPE_QUEST) + " #" + std::to_string(entry) + "-" + localeName(locale) + ": Failed to load from Database! \n");
                        continue;
                    }
                    else
                    {
                        counter++;
                        //msg_nodelay(".");// msg_delay_set(".", 50);
                        if ((counter % 1000) == 0)
                            msg_nodelay(std::to_string(counter));
                        else if ((counter % 100) == 0)
                            msg_nodelay(".");

                        //if ((counter % 1000) == 0)
                        //    msg_nodelay("\n");
                    }
                }

                msg_delay(">  loaded: %d quests \n", counter);

                msg_delay("\n> DB: Checking missing data... \n");
                for (auto i = 2; i <= MAX_LOCALE; ++i) // locales
                {
                    if (locale && locale != i)
                        continue;

                    uint32 missingQuests = 0;

                    uint32 missingTitle = 0;
                    uint32 missingEndText = 0;
                    uint32 missingObjText = 0;
                    uint32 missingReqText = 0;
                    uint32 missingOfferText = 0;
                    uint32 missingDetails = 0;
                    uint32 missingObjective1 = 0;
                    uint32 missingObjective2 = 0;
                    uint32 missingObjective3 = 0;
                    uint32 missingObjective4 = 0;

                    // has english text
                    uint32 hasEngTitle = 0;
                    uint32 hasEngEndText = 0;
                    uint32 hasEngObjText = 0;
                    uint32 hasEngReqText = 0;
                    uint32 hasEngOfferText = 0;
                    uint32 hasEngDetails = 0;
                    uint32 hasEngObjective1 = 0;
                    uint32 hasEngObjective2 = 0;
                    uint32 hasEngObjective3 = 0;
                    uint32 hasEngObjective4 = 0;

                    // wildcards
                    uint32 missingClassTag = 0;
                    uint32 missingRaceTag = 0;
                    uint32 missingNameTag = 0;

                    // write statistics details
                    std::string filesLocation = "stats/db/" + expansionName(e) + "/" + localeName(i) + "/" + typeName(TYPE_QUEST) + "/";
                    //std::string updateStmt = readFromFile(filesLocation + "missingEngTexts.txt");
                    writeToFile("-- DB Locales Stats\n", "missingQuestText", filesLocation);
                    writeToFile("-- DB Locales Stats\n", "engQuestText", filesLocation);
                    writeToFile("-- DB Locales Stats\n", "missingTags", filesLocation);
                    std::string missingQuestText;
                    std::string engQuestText;
                    std::string missingTags;

                    bool hasLocales = false;
                    msg_delay("\n>  %s \n", localeName(i).c_str());
                    counter = 0;
                    for (const auto& questInfo : sDataMgr.questDatabaseInfoList)
                    {
                        DatabaseQuestInfo* qDbInfo = questInfo.second;
                        if (!qDbInfo)
                            continue;

                        // skip if locale is missing (not sure if need)
                        if (qDbInfo->GetTitle(e, i).empty() && qDbInfo->GetDetails(e, i).empty() && qDbInfo->GetObjective(e, i).empty() && qDbInfo->GetRequestItemText(e, i).empty() && qDbInfo->GetOfferRewardText(e, i).empty() && qDbInfo->GetEndText(e, i).empty())
                        {
                            missingQuests++;
                            continue;
                        }

                        hasLocales = true;
                        counter++;

                        // default english, set cur locale to current checked locale
                        qDbInfo->SetCurExpansion(e);
                        qDbInfo->SetCurLocale(i);

                        if ((counter % 1000) == 0)
                            msg_delay_set(std::to_string(counter), 10);
                        else if ((counter % 100) == 0)
                            msg_delay_set(".", 10);

                        //if ((counter % 1000) == 0)
                        //    msg_nodelay("\n");

                        //msg_nodelay(".");

                        // check if missing texts
                        if (!qDbInfo->GetTitle(e, 1).empty() && qDbInfo->GetTitle().empty())
                        {
                            missingTitle++;
                            missingQuestText += "title:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                        }
                        if (!qDbInfo->GetTitle(e, 1).empty() && !qDbInfo->GetTitle().empty() && strcmp(qDbInfo->GetTitle(e, 1).c_str(), qDbInfo->GetTitle().c_str()) == 0)
                        {
                            hasEngTitle++;
                            engQuestText += "title:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                        }

                        if (!qDbInfo->GetEndText(e, 1).empty() && qDbInfo->GetEndText().empty())
                        {
                            missingEndText++;
                            missingQuestText += "endText:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                        }
                        if (!qDbInfo->GetEndText(e, 1).empty() && !qDbInfo->GetEndText().empty() && strcmp(qDbInfo->GetEndText(e, 1).c_str(), qDbInfo->GetEndText().c_str()) == 0)
                        {
                            hasEngEndText++;
                            engQuestText += "endText:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                        }
                        if (!qDbInfo->GetEndText(e, 1).empty() && !qDbInfo->GetEndText().empty())
                        {
                            if (hasNameTag(qDbInfo, "endText", true, e, 1) && !hasNameTag(qDbInfo, "endText", true))
                            {
                                missingNameTag++;
                                missingTags += "name:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                            }
                            if (hasRaceTag(qDbInfo, "endText", true, e, 1) && !hasRaceTag(qDbInfo, "endText", true))
                            {
                                missingRaceTag++;
                                missingTags += "race:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                            }
                            if (hasClassTag(qDbInfo, "endText", true, e, 1) && !hasClassTag(qDbInfo, "endText", true))
                            {
                                missingClassTag++;
                                missingTags += "class:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                            }
                        }

                        if (!qDbInfo->GetObjectives(e, 1).empty() && qDbInfo->GetObjectives().empty())
                        {
                            missingObjText++;
                            missingQuestText += "objective:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                        }

                        if (!qDbInfo->GetObjectives(e, 1).empty() && !qDbInfo->GetObjectives().empty() && strcmp(qDbInfo->GetObjectives(e, 1).c_str(), qDbInfo->GetObjectives().c_str()) == 0)
                        {
                            hasEngObjText++;
                            engQuestText += "objective:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                        }
                        if (!qDbInfo->GetObjectives(e, 1).empty() && !qDbInfo->GetObjectives().empty())
                        {
                            if (hasNameTag(qDbInfo, "objectives", true, e, 1) && !hasNameTag(qDbInfo, "objectives", true))
                            {
                                missingNameTag++;
                                missingTags += "name:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                            }
                            if (hasRaceTag(qDbInfo, "objectives", true, e, 1) && !hasRaceTag(qDbInfo, "objectives", true))
                            {
                                missingRaceTag++;
                                missingTags += "race:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                            }
                            if (hasClassTag(qDbInfo, "objectives", true, e, 1) && !hasClassTag(qDbInfo, "objectives", true))
                            {
                                missingClassTag++;
                                missingTags += "class:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                            }
                        }

                        if (!qDbInfo->GetRequestItemText(e, 1).empty() && qDbInfo->GetRequestItemText().empty())
                        {
                            missingReqText++;
                            missingQuestText += "reqItem:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                        }
                        if (!qDbInfo->GetRequestItemText(e, 1).empty() && !qDbInfo->GetRequestItemText().empty() && strcmp(qDbInfo->GetRequestItemText(e, 1).c_str(), qDbInfo->GetRequestItemText().c_str()) == 0)
                        {
                            hasEngReqText++;
                            engQuestText += "reqItem:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                        }
                        if (!qDbInfo->GetRequestItemText(e, 1).empty() && !qDbInfo->GetRequestItemText().empty())
                        {
                            if (hasNameTag(qDbInfo, "requestItemText", true, e, 1) && !hasNameTag(qDbInfo, "requestItemText", true))
                            {
                                missingNameTag++;
                                missingTags += "name:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                            }
                            if (hasRaceTag(qDbInfo, "requestItemText", true, e, 1) && !hasRaceTag(qDbInfo, "requestItemText", true))
                            {
                                missingRaceTag++;
                                missingTags += "race:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                            }
                            if (hasClassTag(qDbInfo, "requestItemText", true, e, 1) && !hasClassTag(qDbInfo, "requestItemText", true))
                            {
                                missingClassTag++;
                                missingTags += "class:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                            }
                        }

                        if (!qDbInfo->GetOfferRewardText(e, 1).empty() && qDbInfo->GetOfferRewardText().empty())
                        {
                            missingOfferText++;
                            missingQuestText += "offer:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                        }
                        if (!qDbInfo->GetOfferRewardText(e, 1).empty() && !qDbInfo->GetOfferRewardText().empty() && strcmp(qDbInfo->GetOfferRewardText(e, 1).c_str(), qDbInfo->GetOfferRewardText().c_str()) == 0)
                        {
                            hasEngOfferText++;
                            engQuestText += "offer:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                        }
                        if (!qDbInfo->GetOfferRewardText(e, 1).empty() && !qDbInfo->GetOfferRewardText().empty())
                        {
                            if (hasNameTag(qDbInfo, "offerRewardText", true, e, 1) && !hasNameTag(qDbInfo, "offerRewardText", true))
                            {
                                missingNameTag++;
                                missingTags += "name:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                            }
                            if (hasRaceTag(qDbInfo, "offerRewardText", true, e, 1) && !hasRaceTag(qDbInfo, "offerRewardText", true))
                            {
                                missingRaceTag++;
                                missingTags += "race:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                            }
                            if (hasClassTag(qDbInfo, "offerRewardText", true, e, 1) && !hasClassTag(qDbInfo, "offerRewardText", true))
                            {
                                missingClassTag++;
                                missingTags += "class:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                            }
                        }

                        if (!qDbInfo->GetDetails(e, 1).empty() && qDbInfo->GetDetails().empty())
                        {
                            missingDetails++;
                            missingQuestText += "details:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                        }
                        if (!qDbInfo->GetDetails(e, 1).empty() && !qDbInfo->GetDetails().empty() && strcmp(qDbInfo->GetDetails(e, 1).c_str(), qDbInfo->GetDetails().c_str()) == 0)
                        {
                            hasEngDetails++;
                            engQuestText += "details:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                        }
                        if (!qDbInfo->GetDetails(e, 1).empty() && !qDbInfo->GetDetails().empty())
                        {
                            if (hasNameTag(qDbInfo, "details", true, e, 1) && !hasNameTag(qDbInfo, "details", true))
                            {
                                missingNameTag++;
                                missingTags += "name:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                            }
                            if (hasRaceTag(qDbInfo, "details", true, e, 1) && !hasRaceTag(qDbInfo, "details", true))
                            {
                                missingRaceTag++;
                                missingTags += "race:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                            }
                            if (hasClassTag(qDbInfo, "details", true, e, 1) && !hasClassTag(qDbInfo, "details", true))
                            {
                                missingClassTag++;
                                missingTags += "class:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                            }
                        }

                        // Objective 1-4
                        if (!qDbInfo->GetObjective(0, e, 1).empty() && qDbInfo->GetObjective(0).empty())
                        {
                            missingObjective1++;
                            missingQuestText += "obj1:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                        }
                        if (!qDbInfo->GetObjective(0, e, 1).empty() && !qDbInfo->GetObjective(0).empty() && strcmp(qDbInfo->GetObjective(0, e, 1).c_str(), qDbInfo->GetObjective(0).c_str()) == 0)
                        {
                            hasEngObjective1++;
                            engQuestText += "obj1:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                        }

                        if (!qDbInfo->GetObjective(1, e, 1).empty() && qDbInfo->GetObjective(1).empty())
                        {
                            missingObjective2++;
                            missingQuestText += "obj2:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                        }
                        if (!qDbInfo->GetObjective(1, e, 1).empty() && !qDbInfo->GetObjective(1).empty() && strcmp(qDbInfo->GetObjective(1, e, 1).c_str(), qDbInfo->GetObjective(1).c_str()) == 0)
                        {
                            hasEngObjective2++;
                            engQuestText += "obj2:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                        }

                        if (!qDbInfo->GetObjective(2, e, 1).empty() && qDbInfo->GetObjective(2).empty())
                        {
                            missingObjective3++;
                            missingQuestText += "obj3:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                        }
                        if (!qDbInfo->GetObjective(2, e, 1).empty() && !qDbInfo->GetObjective(2).empty() && strcmp(qDbInfo->GetObjective(2, e, 1).c_str(), qDbInfo->GetObjective(2).c_str()) == 0)
                        {
                            hasEngObjective3++;
                            engQuestText += "obj3:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                        }

                        if (!qDbInfo->GetObjective(3, e, 1).empty() && qDbInfo->GetObjective(3).empty())
                        {
                            missingObjective4++;
                            missingQuestText += "obj4:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                        }
                        if (!qDbInfo->GetObjective(3, e, 1).empty() && !qDbInfo->GetObjective(3).empty() && strcmp(qDbInfo->GetObjective(3, e, 1).c_str(), qDbInfo->GetObjective(3).c_str()) == 0)
                        {
                            hasEngObjective4++;
                            engQuestText += "obj4:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                        }
                        /*for (auto index = 0; index < 4; ++index)
                        {
                            if (!qDbInfo->GetObjective(index, e, 1).empty() && qDbInfo->GetObjective(index).empty())
                                missingObjective[0][index]++;
                        }*/
                    }

                    msg_delay(">  processed: %d %s quests \n", counter, localeName(i).c_str());

                    if (hasLocales)
                    {
                        msg_delay("\n");
                        msg_delay(">   Missing Title: %d \n", missingTitle);
                        msg_delay(">   Missing Objectives: %d \n", missingObjText);
                        msg_delay(">   Missing Details: %d \n", missingDetails);
                        msg_delay(">   Missing RequestItemText: %d \n", missingReqText);
                        msg_delay(">   Missing OfferRewardText: %d \n", missingOfferText);
                        msg_delay(">   Missing EndText: %d \n", missingEndText);
                        msg_delay(">   Missing Objective1-4: %d - %d - %d - %d \n", missingObjective1, missingObjective2, missingObjective3, missingObjective4);

                        if (hasEngTitle)
                            msg_delay(">   English Title: %d \n", hasEngTitle);
                        if (hasEngObjText)
                            msg_delay(">   English Objectives: %d \n", hasEngObjText);
                        if (hasEngDetails)
                            msg_delay(">   English Details: %d \n", hasEngDetails);
                        if (hasEngReqText)
                            msg_delay(">   English RequestItemText: %d \n", hasEngReqText);
                        if (hasEngOfferText)
                            msg_delay(">   English OfferRewardText: %d \n", hasEngOfferText);
                        if (hasEngEndText)
                            msg_delay(">   English EndText: %d \n", hasEngEndText);

                        if (i == 3 || i == 4 || i == 6 || i == 7)
                        {
                            if (missingNameTag)
                                msg_delay(">   Missing Name Tag: %d \n", missingNameTag);
                            if (missingRaceTag)
                                msg_delay(">   Missing Race Tag: %d \n", missingRaceTag);
                            if (missingClassTag)
                                msg_delay(">   Missing Class Tag: %d \n", missingClassTag);
                        }

                        if (missingQuests)
                            msg_delay("\n>   Missing Quests Locales: %d \n", missingQuests);

                        // write stats to file
                        writeToFile(missingQuestText.c_str(), "missingQuestText", filesLocation);
                        writeToFile(engQuestText.c_str(), "engQuestText", filesLocation);
                        if (i == 3 || i == 4 || i == 6 || i == 7)
                            writeToFile(missingTags.c_str(), "missingTags", filesLocation);
                    }
                    else
                        msg_delay(">   Missing Quests: %d \n", missingQuests);
                }
            }
            return 1;
        }
        if (activity == 6) // Work on Translations
        {
            uint32 action = 0;
            std::cout << "\nSelect Expansion: \n";
            std::cout << "1) Classic \n";
            std::cout << "2) TBC \n";
            std::cout << "3) WotLK \n";
            std::cout << "4) Retail \n";
            std::cin >> expansion;

            // Choose locale
            std::cout << "\nSelect Locale: \n";
            std::cout << "2) Korean  (LOC 1) \n";
            std::cout << "3) French  (LOC 2) \n";
            std::cout << "4) German  (LOC 3) \n";
            std::cout << "5) Chinese (LOC 4 + 5) \n";
            std::cout << "6) Spanish (LOC 6 + 7) \n";
            std::cout << "7) Russian (LOC 8)\n";
            std::cin >> locale;

            std::cout << "\nSelect Action: \n";
            std::cout << "1) Add missing English texts (BUGGY DONT USE)\n";
            std::cout << "2) Add missing locale texts TODO: manual fix of missing Name tags \n";
            std::cout << "3) Fix $n, $c, $r wildcards e.g. |3-6(жрец), <nom> etc \n";
            std::cout << "4) TODO \n";
            std::cin >> action;

            if (!locale || !expansion)
                return 0;

            // Auto add missing English texts
            if (action == 1)
            {
                // print wh cache stats
                msg_delay("\n");
                msg_delay("> WH: \n");

                int maxQuestId[MAX_EXPANSION];
                maxQuestId[0] = 0;
                maxQuestId[1] = 0;
                maxQuestId[2] = 0;

                for (auto e = 1; e <= MAX_EXPANSION; ++e) // expansion
                {
                    if (expansion && expansion != e)
                        continue;

                    msg_delay("\n> %s \n", expansionName(e).c_str());
                    std::string cacheLocation = "cache/" + expansionName(e) + "/" + localeName(1) + "/quest/";
                    if (!std::filesystem::is_directory(cacheLocation))
                    {
                        msg_delay(">  %s %d \n", localeName(1).c_str(), 0);
                        continue;
                    }

                    auto counter = 0;
                    auto dirIter = std::filesystem::directory_iterator(cacheLocation);
                    for (auto& fl : dirIter)
                    {
                        if (fl.is_regular_file())
                        {
                            ++counter;
                            const std::filesystem::path& p(fl.path());
                            uint32 qID = stoi(p.stem().string());
                            if ((int)qID > maxQuestId[e - 1])
                                maxQuestId[e - 1] = (int)qID;
                        }
                    }
                    msg_delay(">  %s total: %d, max id: %d \n", localeName(1).c_str(), counter, maxQuestId[e - 1]);
                }

                maxQuestId[0] = 0;
                maxQuestId[1] = 0;
                maxQuestId[2] = 0;

                // print db data
                msg_delay("\n");
                msg_delay("> DB: \n");

                for (auto e = 1; e <= MAX_EXPANSION; ++e) // expansion
                {
                    if (expansion && expansion != e)
                        continue;

                    msg_delay("\n> %s \n", expansionName(e).c_str());

                    auto DbConnect = sDataMgr.GetCon(e);
                    if (!DbConnect || !DbConnect->IsConnected())
                    {
                        msg_delay("> DB: Failed to connect! \n");
                        //delete DbConnect;
                        return 0;
                    }

                    int counter = 0;
                    DbConnect->GetDbInt("SELECT COUNT(*) FROM quest_template", counter);
                    DbConnect->GetDbInt("SELECT MAX(entry) FROM quest_template", maxQuestId[e - 1]);
                    msg_delay(">  %s total: %d, max id: %d \n", localeName(1).c_str(), counter, maxQuestId[e - 1]);
                }

                // do action
                msg_delay("\n");

                msg_delay("> Starting... \n");

                for (auto e = 1; e <= MAX_EXPANSION; ++e) // expansion
                {
                    if (expansion && expansion != e)
                        continue;

                    //if (counter > limit)
                    //    break;
                    //msg_delay("> %s \n", expansionName(e).c_str());
                    //msg_delay("> WH: Analyzing... \n");
                    uint32 counter = 0;

                    msg_delay("\n>  %s \n", localeName(1).c_str());

                    uint32 missingTitle = 0;
                    uint32 missingEndText = 0;
                    uint32 missingObjText = 0;
                    uint32 missingReqText = 0;
                    uint32 missingOfferText = 0;
                    uint32 missingDetails = 0;
                    uint32 missingObjective1 = 0;
                    uint32 missingObjective2 = 0;
                    uint32 missingObjective3 = 0;
                    uint32 missingObjective4 = 0;

                    uint32 missingDbQuest = 0; // wh exist, db not exist

                    std::string cacheLocation = "cache/" + expansionName(e) + "/" + localeName(1) + "/quest/";
                    if (!std::filesystem::is_directory(cacheLocation))
                        continue;

                    // read existing update file
                    std::string filesLocation = "work/" + expansionName(expansion) + "/" + localeName(1) + "/" + typeName(TYPE_QUEST) + "/";
                    //std::string updateStmt = readFromFile(filesLocation + "missingEngTexts.txt");
                    writeToFile("-- QUERIES GO BELOW\n", "missingEngTexts", filesLocation);
                    std::string updateQueries;

                    // write command
                    // writeToFile(wh_page.c_str(), std::to_string(id), cacheLocation);

                    auto dirIter = std::filesystem::directory_iterator(cacheLocation);
                    for (auto &fl: dirIter)
                    {
                        if (fl.is_regular_file())
                        {
                            const std::filesystem::path &p(fl.path());
                            uint32 qID = stoi(p.stem().string());

                            WowheadQuestInfo *qWhInfo = LoadWowheadQuestInfo(qID, e, 1, true);
                            if (!qWhInfo)
                                continue;

                            DatabaseQuestInfo* qDbInfo = LoadDatabaseQuestInfo(qID, e, 1, true, true);
                            if (!qDbInfo)
                            {
                                missingDbQuest++;
                                continue;
                            }

                            counter++;
                            if ((counter % 1000) == 0)
                                msg_delay_set(std::to_string(counter), 10);
                            else if ((counter % 100) == 0)
                                msg_delay_set(".", 10);

                            // check if missing texts
                            if (!qWhInfo->GetTitle().empty() && qDbInfo->GetTitle().empty())
                            {
                                missingTitle++;
                                updateQueries += updateQuestFromWhQuery(qWhInfo, qDbInfo, "title", 1, 1) + "\n";
                            }

                            if (!qWhInfo->GetEndText().empty() && qDbInfo->GetEndText().empty())
                            {
                                missingEndText++;
                                updateQueries += updateQuestFromWhQuery(qWhInfo, qDbInfo, "endText", 1, 1) + "\n";
                            }

                            if (!qWhInfo->GetObjectives().empty() && qDbInfo->GetObjectives().empty())
                            {
                                missingObjText++;
                                updateQueries += updateQuestFromWhQuery(qWhInfo, qDbInfo, "objectives", 1, 1) + "\n";
                            }

                            if (!qWhInfo->GetRequestItemText().empty() && qDbInfo->GetRequestItemText().empty())
                            {
                                missingReqText++;
                                updateQueries += updateQuestFromWhQuery(qWhInfo, qDbInfo, "requestItemText", 1, 1) + "\n";
                            }

                            if (!qWhInfo->GetOfferRewardText().empty() && qDbInfo->GetOfferRewardText().empty())
                            {
                                missingOfferText++;
                                updateQueries += updateQuestFromWhQuery(qWhInfo, qDbInfo, "offerRewardText", 1, 1) + "\n";
                            }

                            if (!qWhInfo->GetDetails().empty() && qDbInfo->GetDetails().empty())
                            {
                                missingDetails++;
                                updateQueries += updateQuestFromWhQuery(qWhInfo, qDbInfo, "details", 1, 1) + "\n";
                            }

                            // Objective 1-4
                            if (!qWhInfo->GetObjective(0).empty() && qDbInfo->GetObjective(0).empty())
                            {
                                missingObjective1++;
                                updateQueries += updateQuestFromWhQuery(qWhInfo, qDbInfo, "objectiveText1", 1, 1) + "\n";
                            }

                            if (!qWhInfo->GetObjective(1).empty() && qDbInfo->GetObjective(1).empty())
                            {
                                missingObjective2++;
                                updateQueries += updateQuestFromWhQuery(qWhInfo, qDbInfo, "objectiveText2", 1, 1) + "\n";
                            }

                            if (!qWhInfo->GetObjective(2).empty() && qDbInfo->GetObjective(2).empty())
                            {
                                missingObjective3++;
                                updateQueries += updateQuestFromWhQuery(qWhInfo, qDbInfo, "objectiveText3", 1, 1) + "\n";
                            }

                            if (!qWhInfo->GetObjective(3).empty() && qDbInfo->GetObjective(3).empty())
                            {
                                missingObjective4++;
                                updateQueries += updateQuestFromWhQuery(qWhInfo, qDbInfo, "objectiveText4", 1, 1) + "\n";
                            }
                        }
                    }

                    // print results
                    msg_delay("\n\n");

                    if (missingDbQuest)
                        msg_delay(">   Missing Quests in DB: %d \n\n", missingDbQuest);

                    msg_delay(">   Missing Title: %d \n", missingTitle);
                    msg_delay(">   Missing Objectives: %d \n", missingObjText);
                    msg_delay(">   Missing Details: %d \n", missingDetails);
                    msg_delay(">   Missing RequestItemText: %d \n", missingReqText);
                    msg_delay(">   Missing OfferRewardText: %d \n", missingOfferText);
                    msg_delay(">   Missing EndText: %d \n", missingEndText);
                    msg_delay(">   Missing Objective1-4: %d - %d - %d - %d \n", missingObjective1, missingObjective2, missingObjective3, missingObjective4);

                    // write queries to file
                    writeToFile(updateQueries.c_str(), "missingEngTexts", filesLocation);
                }
                // ACTION 1 END
                return 1;
            }
            // Auto add missing translations
            if (action == 2)
            {
                // print wh cache stats
                msg_delay("\n");
                msg_delay("> WH: \n");

                int maxQuestId[MAX_EXPANSION];
                maxQuestId[0] = 0;
                maxQuestId[1] = 0;
                maxQuestId[2] = 0;

                for (auto e = 1; e <= MAX_EXPANSION; ++e) // expansion
                {
                    if (expansion && expansion != e)
                        continue;

                    msg_delay("\n> %s \n", expansionName(e).c_str());
                    for (auto i = 1; i <= MAX_LOCALE; ++i) // locales
                    {
                        if (locale && locale != i && i != 1)
                            continue;

                        std::string cacheLocation = "cache/" + expansionName(e) + "/" + localeName(i) + "/quest/";
                        if (!std::filesystem::is_directory(cacheLocation))
                        {
                            msg_delay(">  %s %d \n", localeName(i).c_str(), 0);
                            continue;
                        }

                        auto counter = 0;
                        auto dirIter = std::filesystem::directory_iterator(cacheLocation);
                        for (auto& fl : dirIter)
                        {
                            if (fl.is_regular_file())
                            {
                                ++counter;
                                const std::filesystem::path& p(fl.path());
                                uint32 qID = stoi(p.stem().string());
                                if ((int)qID > maxQuestId[e - 1])
                                    maxQuestId[e - 1] = (int)qID;
                            }
                        }
                        msg_delay(">  %s total: %d, max id: %d \n", localeName(i).c_str(), counter, maxQuestId[e - 1]);
                    }
                }

                maxQuestId[0] = 0;
                maxQuestId[1] = 0;
                maxQuestId[2] = 0;

                // print db data
                msg_delay("\n");
                msg_delay("> DB: \n");

                for (auto e = 1; e <= MAX_EXPANSION; ++e) // expansion
                {
                    if (expansion && expansion != e)
                        continue;

                    msg_delay("\n> %s \n", expansionName(e).c_str());

                    auto DbConnect = sDataMgr.GetCon(e);
                    if (!DbConnect || !DbConnect->IsConnected())
                    {
                        msg_delay("> DB: Failed to connect! \n");
                        //delete DbConnect;
                        return 0;
                    }

                    int counter = 0;
                    DbConnect->GetDbInt("SELECT COUNT(*) FROM quest_template", counter);
                    DbConnect->GetDbInt("SELECT MAX(entry) FROM quest_template", maxQuestId[e - 1]);
                    msg_delay(">  %s total: %d, max id: %d \n", localeName(1).c_str(), counter, maxQuestId[e - 1]);

                    msg_delay("> DB: Quests translated (Title OR Details != NULL): \n");

                    for (auto i = 2; i <= MAX_LOCALE; ++i) // locales
                    {
                        if (locale && locale != i)
                            continue;

                        int trCount = 0;
                        DbConnect->GetDbInt("SELECT COUNT(*) FROM locales_quest WHERE Title_loc" + std::to_string(localeRealId(i)) + " IS NOT NULL"
                                                                                                                                     " OR Details_loc" + std::to_string(localeRealId(i)) + " IS NOT NULL"
                                                                                                                                     " OR Objectives_loc" + std::to_string(localeRealId(i)) + " IS NOT NULL", trCount);
                        msg_delay(">  %s %d \n", localeName(i).c_str(), trCount);
                        if (locale && trCount == 0)
                        {
                            msg_delay("> DB: no %s translations found in database! \n", localeName(i).c_str());
                            //return 0;
                        }
                    }
                }

                // do action
                msg_delay("\n");

                msg_delay("> Starting... \n");

                for (auto e = 1; e <= MAX_EXPANSION; ++e) // expansion
                {
                    if (expansion && expansion != e)
                        continue;

                    //if (counter > limit)
                    //    break;
                    //msg_delay("> %s \n", expansionName(e).c_str());
                    //msg_delay("> WH: Analyzing... \n");
                    uint32 counter = 0;

                    msg_delay("\n>  %s \n", localeName(locale).c_str());

                    uint32 missingTitle = 0;
                    uint32 missingEndText = 0;
                    uint32 missingObjText = 0;
                    uint32 missingReqText = 0;
                    uint32 missingOfferText = 0;
                    uint32 missingDetails = 0;
                    uint32 missingObjective1 = 0;
                    uint32 missingObjective2 = 0;
                    uint32 missingObjective3 = 0;
                    uint32 missingObjective4 = 0;

                    uint32 missingDbQuest = 0; // wh exist, db not exist

                    std::string cacheEngLocation = "cache/" + expansionName(e) + "/" + localeName(1) + "/quest/";
                    std::string cacheLocLocation = "cache/" + expansionName(e) + "/" + localeName(locale) + "/quest/";
                    if (!std::filesystem::is_directory(cacheEngLocation))
                        continue;
                    if (!std::filesystem::is_directory(cacheLocLocation))
                        continue;

                    // read existing update file
                    std::string filesLocation = "work/" + expansionName(expansion) + "/" + localeName(locale) + "/" + typeName(TYPE_QUEST) + "/";
                    //std::string updateStmt = readFromFile(filesLocation + "missingEngTexts.txt");
                    writeToFile("-- QUERIES GO BELOW\n", "missingLocales", filesLocation);
                    std::string updateQueries;

                    // write command
                    // writeToFile(wh_page.c_str(), std::to_string(id), cacheLocation);

                    auto dirIter = std::filesystem::directory_iterator(cacheLocLocation);
                    for (auto &fl: dirIter)
                    {
                        if (fl.is_regular_file())
                        {
                            const std::filesystem::path &p(fl.path());
                            uint32 qID = stoi(p.stem().string());

                            // load both eng and locale
                            WowheadQuestInfo* qWhEngInfo = LoadWowheadQuestInfo(qID, e, 1, true);
                            if (!qWhEngInfo)
                                continue;
                            WowheadQuestInfo* qWhLocInfo = LoadWowheadQuestInfo(qID, e, locale, true);
                            if (!qWhLocInfo)
                                continue;

                            // load english first
                            DatabaseQuestInfo* qDbLocInfo = LoadDatabaseQuestInfo(qID, e, 1, true, true);
                            if (!qDbLocInfo)
                            {
                                missingDbQuest++;
                                continue;
                            }
                            qDbLocInfo = LoadDatabaseQuestInfo(qID, e, locale, true, true);
                            if (!qDbLocInfo)
                                continue;

                            counter++;
                            if ((counter % 1000) == 0)
                                msg_delay_set(std::to_string(counter), 10);
                            else if ((counter % 100) == 0)
                                msg_delay_set(".", 10);

                            // use database english as proof that text should exist
                            // TODO fix wowhead parsing of quests that have no details

                            if (!qDbLocInfo->GetTitle(e, 1).empty())
                            {
                                if (!qWhLocInfo->GetTitle().empty() && qDbLocInfo->GetTitle().empty())
                                {
                                    if (qWhLocInfo->GetTitle(e, 1) == qWhLocInfo->GetTitle())
                                        msg_nodelay("x", qID);
                                    else
                                    {
                                        msg_nodelay("v", qID);
                                        missingTitle++;
                                        updateQueries += updateQuestFromWhQuery(qWhLocInfo, qDbLocInfo, "title", e, locale) + "\n";
                                    }
                                }
                            }

                            if (!qDbLocInfo->GetEndText(e, 1).empty())
                            {
                                if (!qWhLocInfo->GetEndText().empty() && qDbLocInfo->GetEndText().empty())
                                {
                                    if (qWhLocInfo->GetEndText(e, 1) == qWhLocInfo->GetEndText())
                                        msg_nodelay("x", qID);
                                    else
                                    {
                                        msg_nodelay("v", qID);
                                        missingEndText++;
                                        updateQueries += updateQuestFromWhQuery(qWhLocInfo, qDbLocInfo, "endText", e, locale) + "\n";
                                    }
                                }
                            }

                            if (!qDbLocInfo->GetObjectives(e, 1).empty())
                            {
                                if (!qWhLocInfo->GetObjectives().empty() && qDbLocInfo->GetObjectives().empty())
                                {
                                    if (qWhLocInfo->GetObjectives(e, 1) == qWhLocInfo->GetObjectives())
                                        msg_nodelay("x", qID);
                                    else
                                    {
                                        missingObjText++;
                                        msg_nodelay("v", qID);
                                        updateQueries += updateQuestFromWhQuery(qWhLocInfo, qDbLocInfo, "objectives", e, locale) + "\n";
                                    }
                                }
                            }

                            if (!qDbLocInfo->GetRequestItemText(e, 1).empty())
                            {
                                if (!qWhLocInfo->GetRequestItemText().empty() && qDbLocInfo->GetRequestItemText().empty())
                                {
                                    if (qWhLocInfo->GetRequestItemText(e, 1) == qWhLocInfo->GetRequestItemText())
                                        msg_nodelay("x", qID);
                                    else
                                    {
                                        missingReqText++;
                                        msg_nodelay("v", qID);
                                        updateQueries += updateQuestFromWhQuery(qWhLocInfo, qDbLocInfo, "requestItemText", e, locale) + "\n";
                                    }
                                }
                            }

                            if (!qDbLocInfo->GetOfferRewardText(e, 1).empty())
                            {
                                if (!qWhLocInfo->GetOfferRewardText().empty() && qDbLocInfo->GetOfferRewardText().empty())
                                {
                                    if (qWhLocInfo->GetOfferRewardText(e, 1) == qWhLocInfo->GetOfferRewardText())
                                        msg_nodelay("x", qID);
                                    else
                                    {
                                        msg_nodelay("v", qID);
                                        missingOfferText++;
                                        updateQueries += updateQuestFromWhQuery(qWhLocInfo, qDbLocInfo, "offerRewardText", e, locale) + "\n";
                                    }
                                }
                            }

                            if (!qDbLocInfo->GetDetails(e, 1).empty())
                            {
                                if (!qWhLocInfo->GetDetails().empty() && qDbLocInfo->GetDetails().empty())
                                {
                                    if (qWhLocInfo->GetDetails(e, 1) == qWhLocInfo->GetDetails())
                                        msg_nodelay("x", qID);
                                    else
                                    {
                                        msg_nodelay("v", qID);
                                        missingDetails++;
                                        updateQueries += updateQuestFromWhQuery(qWhLocInfo, qDbLocInfo, "details", e, locale) + "\n";
                                    }
                                }
                            }

                            // Objective 1-4
                            if (!qDbLocInfo->GetObjective(0, e, 1).empty())
                            {
                                if (!qWhLocInfo->GetObjective(0).empty() && qDbLocInfo->GetObjective(0).empty())
                                {
                                    if (qWhLocInfo->GetObjective(0, e, 1) == qWhLocInfo->GetObjective(0))
                                        msg_nodelay("x", qID);
                                    else
                                    {
                                        msg_nodelay("v", qID);
                                        missingObjective1++;
                                        updateQueries += updateQuestFromWhQuery(qWhLocInfo, qDbLocInfo, "objectiveText1", e, locale) + "\n";
                                    }
                                }
                            }

                            if (!qDbLocInfo->GetObjective(1, e, 1).empty())
                            {
                                if (!qWhLocInfo->GetObjective(1).empty() && qDbLocInfo->GetObjective(1).empty())
                                {
                                    if (qWhLocInfo->GetObjective(1, e, 1) == qWhLocInfo->GetObjective(1))
                                        msg_nodelay("x", qID);
                                    else
                                    {
                                        msg_nodelay("v", qID);
                                        missingObjective2++;
                                        updateQueries += updateQuestFromWhQuery(qWhLocInfo, qDbLocInfo, "objectiveText2", e, locale) + "\n";
                                    }
                                }
                            }

                            if (!qDbLocInfo->GetObjective(2, e, 1).empty())
                            {
                                if (!qWhLocInfo->GetObjective(2).empty() && qDbLocInfo->GetObjective(2).empty())
                                {
                                    if (qWhLocInfo->GetObjective(2, e, 1) == qWhLocInfo->GetObjective(2))
                                        msg_nodelay("x", qID);
                                    else
                                    {
                                        msg_nodelay("v", qID);
                                        missingObjective3++;
                                        updateQueries += updateQuestFromWhQuery(qWhLocInfo, qDbLocInfo, "objectiveText3", e, locale) + "\n";
                                    }
                                }
                            }

                            if (!qDbLocInfo->GetObjective(3, e, 1).empty())
                            {
                                if (!qWhLocInfo->GetObjective(3).empty() && qDbLocInfo->GetObjective(3).empty())
                                {
                                    if (qWhLocInfo->GetObjective(3, e, 1) == qWhLocInfo->GetObjective(3))
                                        msg_nodelay("x", qID);
                                    else
                                    {
                                        msg_nodelay("v", qID);
                                        missingObjective4++;
                                        updateQueries += updateQuestFromWhQuery(qWhLocInfo, qDbLocInfo, "objectiveText4", e, locale) + "\n";
                                    }
                                }
                            }
                        }
                    }

                    // print results
                    msg_delay("\n\n");

                    if (missingDbQuest)
                        msg_delay(">   Missing Quests in DB: %d \n\n", missingDbQuest);

                    msg_delay(">   Added Title: %d \n", missingTitle);
                    msg_delay(">   Added Objectives: %d \n", missingObjText);
                    msg_delay(">   Added Details: %d \n", missingDetails);
                    msg_delay(">   Added RequestItemText: %d \n", missingReqText);
                    msg_delay(">   Added OfferRewardText: %d \n", missingOfferText);
                    msg_delay(">   Added EndText: %d \n", missingEndText);
                    msg_delay(">   Added Objective1-4: %d - %d - %d - %d \n", missingObjective1, missingObjective2, missingObjective3, missingObjective4);

                    // write queries to file
                    writeToFile(updateQueries.c_str(), "missingLocales", filesLocation);
                }
                // ACTION 2 END
                return 1;
            }
            if (action == 3)
            {
                int maxQuestId[MAX_EXPANSION];
                maxQuestId[0] = 0;
                maxQuestId[1] = 0;
                maxQuestId[2] = 0;

                // print db data
                msg_delay("\n");
                msg_delay("> DB: \n");

                for (auto e = 1; e <= MAX_EXPANSION; ++e) // expansion
                {
                    if (expansion && expansion != e)
                        continue;

                    msg_delay("\n> %s \n", expansionName(e).c_str());

                    auto DbConnect = sDataMgr.GetCon(e);
                    if (!DbConnect || !DbConnect->IsConnected())
                    {
                        msg_delay("> DB: Failed to connect! \n");
                        //delete DbConnect;
                        return 0;
                    }

                    int counter = 0;
                    DbConnect->GetDbInt("SELECT COUNT(*) FROM quest_template", counter);
                    DbConnect->GetDbInt("SELECT MAX(entry) FROM quest_template", maxQuestId[e - 1]);
                    msg_delay(">  %s total: %d, max id: %d \n", localeName(1).c_str(), counter, maxQuestId[e - 1]);

                    msg_delay("> DB: Quests translated (Title OR Details != NULL): \n");

                    for (auto i = 2; i <= MAX_LOCALE; ++i) // locales
                    {
                        if (locale && locale != i)
                            continue;

                        int trCount = 0;
                        DbConnect->GetDbInt("SELECT COUNT(*) FROM locales_quest WHERE Title_loc" + std::to_string(localeRealId(i)) + " IS NOT NULL"
                                                                                                                                     " OR Details_loc" + std::to_string(localeRealId(i)) + " IS NOT NULL"
                                                                                                                                     " OR Objectives_loc" + std::to_string(localeRealId(i)) + " IS NOT NULL", trCount);
                        msg_delay(">  %s %d \n", localeName(i).c_str(), trCount);
                        if (locale && trCount == 0)
                        {
                            msg_delay("> DB: no %s translations found in database! \n", localeName(i).c_str());
                            return 0;
                        }
                    }
                }

                // do action
                msg_delay("\n");

                msg_delay("> Starting... \n");

                for (auto e = 1; e <= MAX_EXPANSION; ++e) // expansion
                {
                    if (expansion && expansion != e)
                        continue;

                    msg_delay("\n> %s \n", expansionName(e).c_str());

                    msg_delay("> DB: Fixing wildcards... \n");

                    // read existing update file
                    std::string filesLocation = "work/" + expansionName(expansion) + "/" + localeName(locale) + "/" + typeName(TYPE_QUEST) + "/";
                    //std::string updateStmt = readFromFile(filesLocation + "missingEngTexts.txt");
                    writeToFile("-- QUERIES GO BELOW\n", "missingTags", filesLocation);
                    std::string updateQueries;
                    uint32 addedNameTags = 0;
                    uint32 addedClassTags = 0;
                    uint32 addedRaceTags = 0;

                    uint32 counter = 0;
                    for (auto d = 1; d <= maxQuestId[e - 1]; ++d)
                    {
                        DatabaseQuestInfo* qDbInfo = LoadDatabaseQuestInfo(d, e, locale, true, true);
                        if (!qDbInfo || !qDbInfo->IsLoaded(e, locale))
                        {
                            //msg_delay("> DB " + typeName(TYPE_QUEST) + " #" + std::to_string(entry) + "-" + localeName(locale) + ": Failed to load from Database! \n");
                            continue;
                        }
                        else
                        {
                            counter++;
                            //msg_nodelay(".");// msg_delay_set(".", 50);
                            if ((counter % 1000) == 0)
                                msg_nodelay(std::to_string(counter));
                            else if ((counter % 100) == 0)
                                msg_nodelay(".");

                            // logic goes here

                            // Name
                            std::string details = qDbInfo->GetDetails();
                            std::string offer = qDbInfo->GetOfferRewardText();
                            std::string req = qDbInfo->GetRequestItemText();
                            if (hasNameTag(qDbInfo, "details", false, e, locale))
                            {
                                std::string replace = details;
                                replaceNameTag(details, locale);
                                if (replace != details)
                                    addedNameTags++;
                            }
                            if (hasNameTag(qDbInfo, "offerRewardText", false, e, locale))
                            {
                                std::string replace = offer;
                                replaceNameTag(offer, locale);
                                if (replace != offer)
                                    addedNameTags++;
                            }
                            if (hasNameTag(qDbInfo, "requestItemText", false, e, locale))
                            {
                                std::string replace = req;
                                replaceNameTag(req, locale);
                                if (replace != req)
                                    addedNameTags++;
                            }

                            // Race
                            if (hasRaceTag(qDbInfo, "details", false, e, locale))
                            {
                                std::string replace = details;
                                replaceRaceTag(details, locale);
                                if (replace != details)
                                    addedRaceTags++;
                            }
                            if (hasRaceTag(qDbInfo, "offerRewardText", false, e, locale))
                            {
                                std::string replace = offer;
                                replaceRaceTag(offer, locale);
                                if (replace != offer)
                                    addedRaceTags++;
                            }
                            if (hasRaceTag(qDbInfo, "requestItemText", false, e, locale))
                            {
                                std::string replace = req;
                                replaceRaceTag(req, locale);
                                if (replace != req)
                                    addedRaceTags++;
                            }

                            // Class
                            if (hasClassTag(qDbInfo, "details", false, e, locale))
                            {
                                std::string replace = details;
                                replaceClassTag(details, locale);
                                if (replace != details)
                                    addedClassTags++;
                            }
                            if (hasClassTag(qDbInfo, "offerRewardText", false, e, locale))
                            {
                                std::string replace = offer;
                                replaceClassTag(offer, locale);
                                if (replace != offer)
                                    addedClassTags++;
                            }
                            if (hasClassTag(qDbInfo, "requestItemText", false, e, locale))
                            {
                                std::string replace = req;
                                replaceClassTag(req, locale);
                                if (replace != req)
                                    addedClassTags++;
                            }

                            // print queries if needed
                            if (details != qDbInfo->GetDetails())
                                updateQueries += updateQuestFromTextQueryNoTags(details, qDbInfo, "details", e, locale) + "\n";
                            if (offer != qDbInfo->GetOfferRewardText())
                                updateQueries += updateQuestFromTextQueryNoTags(offer, qDbInfo, "offerRewardText", e, locale) + "\n";
                            if (req != qDbInfo->GetRequestItemText())
                                updateQueries += updateQuestFromTextQueryNoTags(req, qDbInfo, "requestItemText", e, locale) + "\n";
                        }


                    }

                    // print results
                    msg_delay("\n\n");


                    msg_delay(">   Added $N: %d \n", addedNameTags);
                    msg_delay(">   Added $R: %d \n", addedRaceTags);
                    msg_delay(">   Added $C: %d \n", addedClassTags);

                    // write queries to file
                    writeToFile(updateQueries.c_str(), "missingTags", filesLocation);
                }
                // ACTION 3 END
                return 1;
            }
        }

    }
    else // command line // TODO add missing functions
    {
        // locale
        locale = check_int_input(argv[1]);
        if (locale > 7 || locale < 1)
        {
            std::cerr << "Usage: " << "whparse.exe " << "LOCALE (1-7) \n";
            std::cerr << "1) English (LOC 0) \n";
            std::cerr << "2) Korean  (LOC 1) \n";
            std::cerr << "3) French  (LOC 2) \n";
            std::cerr << "4) German  (LOC 3)\n";
            std::cerr << "5) Chinese (LOC 4 + 5)\n";
            std::cerr << "6) Spanish (LOC 6 + 7)\n";
            std::cerr << "7) Russian (LOC 8)\n";
            std::cerr << std::endl;
            return 1;
        }

        if (argc < 3)
        {
            std::cerr << "Usage: " << "whparse.exe " << "LOCALE (1-7), EXPANSION (1-4) \n";
            std::cerr << "1) Classic \n";
            std::cerr << "2) TBC \n";
            std::cerr << "3) WotLK \n";
            std::cerr << "4) Retail \n";
            std::cerr << std::endl;
            return 1;
        }

        // expansion
        expansion = check_int_input(argv[2]);
        if (expansion > 3 || expansion < 1)
        {
            std::cerr << "Usage: " << "whparse.exe " << "LOCALE (1-7), EXPANSION (1-4) \n";
            std::cerr << "1) Classic \n";
            std::cerr << "2) TBC \n";
            std::cerr << "3) WotLK \n";
            std::cerr << "4) Retail \n";
            std::cerr << std::endl;
            return 1;
        }

        if (argc < 4)
        {
            std::cerr << "Usage: " << "whparse.exe " << "LOCALE (1-7), EXPANSION (1-4), TYPE (Only QUEST = 1) \n";
            std::cerr << "1) Quest \n";
            std::cerr << "2) NPC \n";
            std::cerr << std::endl;
            return 1;
        }

        // type
        type = check_int_input(argv[3]);
        if (type > 1 || type < 1)
        {
            std::cerr << "Usage: " << "whparse.exe " << "LOCALE (1-7), EXPANSION (1-4), TYPE (QUEST = 1, NPC = 2) \n";
            std::cerr << "1) Quest \n";
            std::cerr << "2) NPC \n";
            std::cerr << std::endl;
            return 1;
        }

        if (argc < 5)
        {
            std::cerr << "Usage: " << "whparse.exe " << "LOCALE (1-7), EXPANSION (1-4), TYPE (QUEST = 1, NPC = 2), ENTRY (ID) \n";
            std::cerr << "Enter ID: \n";
            std::cerr << std::endl;
            return 1;
        }

        // entry
        entry = check_int_input(argv[4]);
        if (entry < 1)
        {
            std::cerr << "Usage: " << "whparse.exe " << "LOCALE (1-7), EXPANSION (1-4), TYPE (QUEST = 1, NPC = 2), ENTRY (ID) \n";
            std::cerr << "Enter ID: \n";
            std::cerr << std::endl;
            return 1;
        }

        if (argc < 6)
        {
            std::cerr << "Usage: " << "LOCALE (1-7), EXPANSION (1-4), TYPE (QUEST = 1, NPC = 2), ENTRY (ID), ENTRYPART (Depends on type) \n";
            std::cerr << "1) Title           (Title) \n";
            std::cerr << "2) Description     (Details) \n";
            std::cerr << "3) Objectives      (Objectives) \n";
            std::cerr << "4) Progress        (RequestItemsText) \n";
            std::cerr << "5) Completion      (OfferRewardText) \n";
            std::cerr << "6) Main Objective  (EndText) \n";
            std::cerr << "7) List Objectives (ObjectiveText 1-4) \n";
            std::cerr << std::endl;
            return 1;
        }

        // entry part
        entryPart = check_int_input(argv[5]);
        if (entryPart < 1)
        {
            std::cerr << "Usage: " << "whparse.exe " << "LOCALE (1-7), EXPANSION (1-4), TYPE (QUEST = 1, NPC = 2), ENTRY (ID), ENTRYPART (Depends on type) \n";
            std::cerr << "1) Title           (Title) \n";
            std::cerr << "2) Description     (Details) \n";
            std::cerr << "3) Objectives      (Objectives) \n";
            std::cerr << "4) Progress        (RequestItemsText) \n";
            std::cerr << "5) Completion      (OfferRewardText) \n";
            std::cerr << "6) Main Objective  (EndText) \n";
            std::cerr << "7) List Objectives (ObjectiveText 1-4) \n";
            std::cerr << std::endl;
            return 1;
        }

        if (argc < 6) { // We expect 5 arguments
            std::cerr << "Usage: " << "whparse.exe " << "LOCALE (1-7), EXPANSION (1-4), TYPE (QUEST = 1, NPC = 2), ENTRY (ID), ENTRYPART (Depends on type)" << std::endl;
            return 1;
        }
    }

    if (activity == 1)
    {
        // QUEST
        if (type == 1)
        {
            // load all locales, set current for quick access
            WowheadQuestInfo* qInfo = LoadWowheadQuestInfo(entry, expansion, locale, true);
            if (!qInfo)
            {
                msg_delay("> WH " + typeName(TYPE_QUEST) + " #" + std::to_string(entry) + "-" + localeName(locale) + ": Failed to load from Wowhead! \n");
                return 0;
            }
            // Load English in all cases
            if (qInfo->GetCurLocale() != 1)
                qInfo->LoadEntryData(expansion, 1);

            DatabaseQuestInfo* qDbInfo = LoadDatabaseQuestInfo(entry, expansion, locale, true);
            if (!qDbInfo)
            {
                msg_delay("> DB " + typeName(TYPE_QUEST) + " #" + std::to_string(entry) + "-" + localeName(locale) + ": Failed to load from Database! \n");
                return 0;
            }
            // Load English in all cases
            if (qDbInfo->GetCurLocale() != 1)
                qDbInfo->LoadEntryData(expansion, 1);

            //Table questCompare;
            //questCompare.add_row({qInfo->GetDetails()});
            //questCompare.add_row({"Text", "Wowhead Info", "Database Info"});
            //questCompare.add_row({"Title", qInfo->GetTitle(), qDbInfo->GetTitle()});
            //questCompare.add_row({"Objectives", qInfo->GetObjectives(), qDbInfo->GetObjectives()});
            //questCompare.add_row({"Details", qInfo->GetDetails(), qDbInfo->GetDetails()});
            //questCompare.add_row({"ReqItemText", qInfo->GetRequestItemText(), qDbInfo->GetRequestItemText()});
            //questCompare.add_row({"OfferRewardText", qInfo->GetOfferRewardText(), qDbInfo->GetOfferRewardText()});
            //questCompare.add_row({"EndText", qInfo->GetEndText(), qDbInfo->GetEndText()});
            //questCompare.add_row({"Objective 1", qInfo->GetObjective(0), qDbInfo->GetObjective(0)});
            //questCompare.add_row({"Objective 2", qInfo->GetObjective(1), qDbInfo->GetObjective(1)});
            //questCompare.add_row({"Objective 3", qInfo->GetObjective(2), qDbInfo->GetObjective(2)});
            //questCompare.add_row({"Objective 4", qInfo->GetObjective(3), qDbInfo->GetObjective(3)});*/

            /*questCompare[0].format()
                    .padding_top(1)
                    .padding_bottom(1)
                    .font_align(FontAlign::center);*/

            //questCompare[0].format().width(10);
            //questCompare[0].format().width(50);
            //questCompare[0].format().width(100);

            //questCompare.column(1).format()
            //        .multi_byte_characters(true);
            //questCompare.column(2).format()
            //        .multi_byte_characters(true);

            std::cout << "\n";
            //questCompare.print(std::cout);
            //std::cout << questCompare << std::endl;

            /*Table questCompare;
            questCompare.add_row({"Wowhead Info", "Database Info"});
            questCompare.add_row({qInfo->GetDetails(), qDbInfo->GetDetails()});
            questCompare[0].format().width(80);
            questCompare.print(std::cout);*/

            //printf("|%5s|%5s|%5s|%5s|", qInfo->GetTitle().c_str(), qInfo->GetObjectives().c_str(), qInfo->GetDetails().c_str(), qInfo->GetRequestItemText().c_str());

            msg_delay("\n");
            /*Table tb;
            tb.add_row({"WH Title"});
            tb.print(std::cout);*/

            msg_delay("\n> WH Title: " + qInfo->GetTitle() + "\n");
            if (qInfo->GetCurLocale() != 1) msg_delay("\n> WH Title EN: " + qInfo->GetTitle(expansion, 1) + "\n");
            if (qDbInfo) msg_delay("\n> DB Title: " + qDbInfo->GetTitle() + "\n");
            if (qDbInfo->GetCurLocale() != 1) msg_delay("\n> DB Title EN: " + qDbInfo->GetTitle(expansion, 1) + "\n");
            msg_delay("\n> WH Objectives: " + qInfo->GetObjectives() + "\n");
            if (qInfo->GetCurLocale() != 1) msg_delay("\n> WH Objectives EN: " + qInfo->GetObjectives(expansion, 1) + "\n");
            if (qDbInfo) msg_delay("\n> DB Objectives: " + qDbInfo->GetObjectives() + "\n");
            if (qDbInfo->GetCurLocale() != 1) msg_delay("\n> DB Objectives EN: " + qDbInfo->GetObjectives(expansion, 1) + "\n");
            msg_delay("\n> WH Details: " + qInfo->GetDetails() + "\n");
            if (qInfo->GetCurLocale() != 1) msg_delay("\n> WH Details EN: " + qInfo->GetDetails(expansion, 1) + "\n");
            if (qDbInfo) msg_delay("\n> DB Details: " + qDbInfo->GetDetails() + "\n");
            if (qDbInfo->GetCurLocale() != 1) msg_delay("\n> DB Details EN: " + qDbInfo->GetDetails(expansion, 1) + "\n");
            msg_delay("\n> WH ReqItemText: " + qInfo->GetRequestItemText() + "\n");
            if (qInfo->GetCurLocale() != 1) msg_delay("\n> WH ReqItemText EN: " + qInfo->GetRequestItemText(expansion, 1) + "\n");
            if (qDbInfo) msg_delay("\n> DB ReqItemText: " + qDbInfo->GetRequestItemText() + "\n");
            if (qDbInfo->GetCurLocale() != 1) msg_delay("\n> DB ReqItemText EN: " + qDbInfo->GetRequestItemText(expansion, 1) + "\n");
            msg_delay("\n> WH OfferRewardText: " + qInfo->GetOfferRewardText() + "\n");
            if (qInfo->GetCurLocale() != 1) msg_delay("\n> WH OfferRewardText EN: " + qInfo->GetOfferRewardText(expansion, 1) + "\n");
            if (qDbInfo) msg_delay("\n> DB OfferRewardText: " + qDbInfo->GetOfferRewardText() + "\n");
            if (qDbInfo->GetCurLocale() != 1) msg_delay("\n> DB OfferRewardText EN: " + qDbInfo->GetOfferRewardText(expansion, 1) + "\n");
            msg_delay("\n> WH EndText: " + qInfo->GetEndText() + "\n");
            if (qInfo->GetCurLocale() != 1) msg_delay("\n> WH EndText EN: " + qInfo->GetEndText(expansion, 1) + "\n");
            if (qDbInfo) msg_delay("\n> DB EndText: " + qDbInfo->GetEndText() + "\n");
            if (qDbInfo->GetCurLocale() != 1) msg_delay("\n> DB EndText EN: " + qDbInfo->GetEndText(expansion, 1) + "\n");
            msg_delay("\n");
            msg_delay("\n> WH Objective 1: " + qInfo->GetObjective(0)); if (qInfo->GetCurLocale() != 1) msg_delay(" > WH Objective 1 EN: " + qInfo->GetObjective(0, expansion , 1));
            msg_delay("\n> WH Objective 2: " + qInfo->GetObjective(1)); if (qInfo->GetCurLocale() != 1) msg_delay(" > WH Objective 2 EN: " + qInfo->GetObjective(1, expansion , 1));
            msg_delay("\n> WH Objective 3: " + qInfo->GetObjective(2)); if (qInfo->GetCurLocale() != 1) msg_delay(" > WH Objective 3 EN: " + qInfo->GetObjective(2, expansion , 1));
            msg_delay("\n> WH Objective 4: " + qInfo->GetObjective(3)); if (qInfo->GetCurLocale() != 1) msg_delay(" > WH Objective 4 EN: " + qInfo->GetObjective(3, expansion , 1));
            if (qDbInfo)
            {
                msg_delay("\n> DB Objective 1: " + qDbInfo->GetObjective(0)); if (qDbInfo->GetCurLocale() != 1) msg_delay(" > DB Objective 1 EN: " + qDbInfo->GetObjective(0, expansion , 1));
                msg_delay("\n> DB Objective 2: " + qDbInfo->GetObjective(1)); if (qDbInfo->GetCurLocale() != 1) msg_delay(" > DB Objective 2 EN: " + qDbInfo->GetObjective(1, expansion , 1));
                msg_delay("\n> DB Objective 3: " + qDbInfo->GetObjective(2)); if (qDbInfo->GetCurLocale() != 1) msg_delay(" > DB Objective 3 EN: " + qDbInfo->GetObjective(2, expansion , 1));
                msg_delay("\n> DB Objective 4: " + qDbInfo->GetObjective(3)); if (qDbInfo->GetCurLocale() != 1) msg_delay(" > DB Objective 4 EN: " + qDbInfo->GetObjective(3, expansion , 1));
            }
/*
            if (!wh_page.empty())
            {
                msg_delay("> quest #" + std::to_string(entry) + ": Parsing... \n");

                GumboOutput* parsed_response = gumbo_parse(wh_page.c_str());
                parse_quest(query_result, parsed_response->root, entryPart, locale);
                gumbo_destroy_output(&kGumboDefaultOptions, parsed_response);

                if (query_result.str().empty())
                    msg_delay("> quest #" + std::to_string(entry) + ": Failed! \n");
                else
                {
                    std::cout << "\n";
                    msg_delay("> Result: '" + query_result.str() + "'");
                    msg_delay("\n\n> quest #" + std::to_string(entry) + ": Done!");
                }
            }*/
        }
        // NPC
        if (type == 2)
        {
            std::string wh_page = loadPageOrCache(TYPE_NPC, entry, expansion, locale);

            if (!wh_page.empty())
            {
                msg_delay("> npc #" + std::to_string(entry) + ": Parsing... \n");

                GumboOutput* parsed_response = gumbo_parse(wh_page.c_str());
                parse_npc(query_result, parsed_response->root, entryPart, locale);
                gumbo_destroy_output(&kGumboDefaultOptions, parsed_response);

                if (query_result.str().empty())
                    msg_delay("> npc #" + std::to_string(entry) + ": Failed! \n");
                else
                {
                    std::cout << "\n";
                    msg_delay("> Result: '" + query_result.str() + "'");
                    msg_delay("\n\n> npc #" + std::to_string(entry) + ": Done!");

                    // test
                    writeToFile(query_result.str().c_str(), "result");
                }
            }
        }
    }

    std::cout << "\n\nFinished!\n";

    return 1;
}

QuestStrings LoadQuestDatabaseStrings(uint32 id, uint32 expansion = 1, uint32 locale = 1)
{
    QuestStrings qStrings;

    auto DbConnect = sDataMgr.GetCon(expansion);
    if (!DbConnect || !DbConnect->IsConnected() || !DbConnect->IsEntryExistInDb(TYPE_QUEST, id))
        return qStrings;

    qStrings = LoadQuestFull(DbConnect, id, locale);

    /*qStrings.title = LoadQuestText(DbConnect, id, "title", locale);
    qStrings.objectives = LoadQuestText(DbConnect, id, "objectives", locale);
    qStrings.details = LoadQuestText(DbConnect, id, "details", locale);
    qStrings.requestItemText = LoadQuestText(DbConnect, id, "requestItemText", locale);
    qStrings.offerRewardText = LoadQuestText(DbConnect, id, "offerRewardText", locale);
    qStrings.endText = LoadQuestText(DbConnect, id, "endText", locale);

    qStrings.objectiveList[0] = LoadQuestText(DbConnect, id, "objectiveText1", locale);
    qStrings.objectiveList[1] = LoadQuestText(DbConnect, id, "objectiveText2", locale);
    qStrings.objectiveList[2] = LoadQuestText(DbConnect, id, "objectiveText3", locale);
    qStrings.objectiveList[3] = LoadQuestText(DbConnect, id, "objectiveText4", locale);*/

    //delete DbConnect;
    return qStrings;
}

QuestStrings LoadQuestCacheStrings(const std::string &whPage, uint32 locale)
{
    QuestStrings qStrings;

    // parse page
    auto parsedPage = GumboParsePage(whPage);

    // parse details
    qStrings.title = ReadQuestText(parsedPage, "title", locale);
    qStrings.objectives = ReadQuestText(parsedPage, "objectives", locale);
    qStrings.details = ReadQuestText(parsedPage, "details", locale);
    qStrings.requestItemText = ReadQuestText(parsedPage, "requestItemText", locale);
    qStrings.offerRewardText = ReadQuestText(parsedPage, "offerRewardText", locale);
    qStrings.endText = ReadQuestText(parsedPage, "endText", locale);
    // read list of objectives;
    std::string objList = ReadQuestText(parsedPage, "objectiveList", locale);
    if (!objList.empty())
    {
        std::stringstream ss;
        ss << objList;
        std::vector<std::string> result;

        while( ss.good() )
        {
            std::string substr;
            getline( ss, substr, ';' );
            result.push_back( substr );
        }
        if (!result.empty())
        {
            for (auto i = 0; i < result.size(); i++)
            {
                qStrings.objectiveList[i] = result[i];
            }
        }
    }

    // clear memory
    GumboClosePage(parsedPage);

    return qStrings;
}

WowGameEntry::WowGameEntry(uint32 id, TypeId type, uint32 expansion, uint32 locale, EntrySource src)
{
    entry = id;
    typeId = type;
    curExpansion = expansion;
    curLocale = locale;
    for (auto i = 1; i <= MAX_EXPANSION; ++i)
        for (auto l = 1; l <= MAX_LOCALE; ++l)
            isLoaded[i][l] = false;

    source = src;
}


void WowheadQuestInfo::LoadEntryData(uint32 expansion, uint32 locale)
{
    std::string cachePage = loadPageOrCache(TYPE_QUEST, GetEntry(), expansion, locale, true);
    if (cachePage.empty())
        return;

    SetQuestTexts(expansion, locale, LoadQuestCacheStrings(cachePage, locale));
    SetLoaded(expansion, locale, true);
}

void DatabaseQuestInfo::LoadEntryData(uint32 expansion, uint32 locale)
{
    auto DbConnect = sDataMgr.GetCon(expansion);
    if (DbConnect && DbConnect->IsConnected() && !DbConnect->IsEntryExistInDb(TYPE_QUEST, GetEntry()))
        return;
    // TODO More elegant way to skip missing quests

    QuestStrings qstr = LoadQuestDatabaseStrings(GetEntry(), expansion, locale);
    /*if (!qstr.title.empty() || !qstr.details.empty())
    {
        SetQuestTexts(expansion, locale, qstr);
        SetLoaded(expansion, locale, true);
    }*/
    SetQuestTexts(expansion, locale, qstr);
    SetLoaded(expansion, locale, true);
}

std::string QuestInfo::GetTitle(uint32 expansion, uint32 locale)
{
    if (expansion == 0 || locale == 0)
        return questTexts[GetCurExpansion()][GetCurLocale()].title;
    else
        return questTexts[expansion][locale].title;
}

std::string QuestInfo::GetObjectives(uint32 expansion, uint32 locale)
{
    if (expansion == 0 || locale == 0)
        return questTexts[GetCurExpansion()][GetCurLocale()].objectives;
    else
        return questTexts[expansion][locale].objectives;
}

std::string QuestInfo::GetDetails(uint32 expansion, uint32 locale)
{
    if (expansion == 0 || locale == 0)
        return questTexts[GetCurExpansion()][GetCurLocale()].details;
    else
        return questTexts[expansion][locale].details;
}

std::string QuestInfo::GetRequestItemText(uint32 expansion, uint32 locale)
{
    if (expansion == 0 || locale == 0)
        return questTexts[GetCurExpansion()][GetCurLocale()].requestItemText;
    else
        return questTexts[expansion][locale].requestItemText;
}

std::string QuestInfo::GetOfferRewardText(uint32 expansion, uint32 locale)
{
    if (expansion == 0 || locale == 0)
        return questTexts[GetCurExpansion()][GetCurLocale()].offerRewardText;
    else
        return questTexts[expansion][locale].offerRewardText;
}

std::string QuestInfo::GetEndText(uint32 expansion, uint32 locale)
{
    if (expansion == 0 || locale == 0)
        return questTexts[GetCurExpansion()][GetCurLocale()].endText;
    else
        return questTexts[expansion][locale].endText;
}

std::string QuestInfo::GetObjective(uint32 index, uint32 expansion, uint32 locale)
{
    if (index < 0 || index > 3)
        return "";

    if (expansion == 0 || locale == 0)
        return questTexts[GetCurExpansion()][GetCurLocale()].objectiveList[index];
    else
        return questTexts[expansion][locale].objectiveList[index];
}

std::string QuestInfo::GetQuestPart(const std::string& qPart, uint32 expansion, uint32 locale)
{
    if (qPart == "title")
        return GetTitle(expansion, locale);
    if (qPart == "objectives")
        return GetObjective(expansion, locale);
    if (qPart == "details")
        return GetDetails(expansion, locale);
    if (qPart == "requestItemText")
        return GetRequestItemText(expansion, locale);
    if (qPart == "offerRewardText")
        return GetOfferRewardText(expansion, locale);
    if (qPart == "endText")
        return GetEndText(expansion, locale);

    if (qPart == "objectiveText1")
        return GetObjective(0, expansion, locale);
    if (qPart == "objectiveText2")
        return GetObjective(1, expansion, locale);
    if (qPart == "objectiveText3")
        return GetObjective(2, expansion, locale);
    if (qPart == "objectiveText4")
        return GetObjective(3, expansion, locale);

    return "";
}

DatabaseConnect::DatabaseConnect(const char *host, uint32 port, const char *user, const char *pass, const char *dbname)
{
    isConnected = false;
    mysql = new MYSQL;
    mysql_init(mysql);
    mysql_options(mysql, MYSQL_READ_DEFAULT_GROUP, "your_prog_name");
    mysql_options(mysql, MYSQL_SET_CHARSET_NAME, "utf8");
    mysql_options(mysql, MYSQL_INIT_COMMAND, "SET NAMES utf8");
    if (!mysql_real_connect(mysql, host, user, pass, dbname, port, nullptr, 0)) {
        fprintf(stderr, "Failed to connect to database: Error: %s\n",
                mysql_error(mysql));

        mysql_close(mysql);
    }

    isConnected = true;
}

DatabaseConnect::~DatabaseConnect()
{
    mysql_close(mysql);
}

void DataManager::Initialize()
{
    if (isInitialized)
        return;

    for (auto i = 0; i < MAX_EXPANSION; ++i)
    {
        auto dbCon = new DatabaseConnect("127.0.0.1", 3310, "root", "123456", dbName(i + 1));
        if (!dbCon->IsConnected())
            delete dbCon;
        else
        {
            isInitialized = true;
            mysqlCon[i] = dbCon;
        }
    }

    // fill quest database entries with nullptr
    for (auto i = 1; i < 30000; ++i)
    {
        questDatabaseInfoList[i] = nullptr;
        questWowheadInfoList[i] = nullptr;
    }
}