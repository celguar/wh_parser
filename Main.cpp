#include "Main.h"
#include <codecvt>
#include <set>

#define STRING_SIZE 100000
#define NOT_FOUND_TEXT "database-detail-page-not-found-message"
#define MSG_DELAY 100

namespace StringHelper
{
    bool ContainsString(const std::string& string, const std::string& substring)
    {
        return string.find(substring) != std::string::npos;
    }

    std::string GetFirstStringBetween(const std::string& string, const std::string& start, const std::string& end)
    {
        std::string substring;
        std::size_t startPos = string.find(start);
        if (startPos != std::string::npos)
        {
            std::size_t endPos = string.find(end, startPos);
            if (endPos != std::string::npos)
            {
                startPos += start.length();
                substring = string.substr(startPos, endPos - startPos);
            }
        }

        return substring;
    }

    std::vector<std::string> GetStringsBetween(const std::string& input, const std::string& start, const std::string& end) 
    {
        std::vector<std::string> strings;

        size_t pos = 0;
        while ((pos = input.find(start, pos)) != std::string::npos) 
        {
            pos += start.size();
            size_t end_pos = input.find(end, pos);
            if (end_pos == std::string::npos) 
            {
                break;
            }
            size_t length = end_pos - pos + end.size();
            strings.push_back(input.substr(pos, length));
            pos = end_pos + end.size();
        }

        return strings;
    }

    std::vector<std::string> SplitString(const std::string& str, const std::string& separator)
    {
        std::vector<std::string> substrings;
        std::size_t start = 0, end = 0;
        while ((end = str.find(separator, start)) != std::string::npos)
        {
            substrings.push_back(str.substr(start, end - start));
            start = end + separator.length();
        }

        substrings.push_back(str.substr(start));
        return substrings;
    }

    std::string ReplaceString(const std::string& string, const std::string& toReplace, const std::string& replaceWith)
    {
        std::string output = string;
        size_t found = output.find(toReplace);
        while (found != std::string::npos)
        {
            output.replace(found, toReplace.size(), replaceWith);
            found = output.find(toReplace, found + 1);
        }

        return output;
    }

    std::string DecodeUnicodeString(const std::string& input)
    {
        // Replace \u2018 tag for '
        std::string input2 = ReplaceString(input, "\\u2018", "'");

        // Replace \u2019 tag for '
        input2 = ReplaceString(input2, "\\u2019", "'");

        // Replace \u201c tag for "
        input2 = ReplaceString(input2, "\\u201c", "\"");

        // Replace \u201d tag for "
        const std::string input3 = ReplaceString(input2, "\\u201d", "\"");

        std::string output;
        for (std::string::const_iterator it = input3.begin(); it != input3.end(); ++it)
        {
            if (*it == '\\' && std::distance(it, input3.end()) >= 6 && *(it + 1) == 'u')
            {
                std::string hex_code(it + 2, it + 6);
                unsigned int unicode_code = std::stoul(hex_code, nullptr, 16);
                output += static_cast<char>(unicode_code);
                it += 5;
            }
            else
            {
                output += *it;
            }
        }

        return output;
    }

    std::string DecodeHTMLString(const std::string& input)
    {
        // Decode unicode string
        std::string output = DecodeUnicodeString(input);

        // Replace \x19 tag for '
        output = ReplaceString(output, "\x19", "'");

        // Replace <br /> tag for newline
        output = ReplaceString(output, "<br \\/>", "\n");

        // Remove \"
        output = ReplaceString(output, "\\""", "");

        // Replace &amp; tag with &
        output = ReplaceString(output, "&amp;", "&");

        // Replace &lt; tag with <
        output = ReplaceString(output, "&lt;", "<");

        // Replace &gt; tag with >
        output = ReplaceString(output, "&gt;", ">");

        // Replace &apos; tag with '
        output = ReplaceString(output, "&apos;", "'");

        // Replace &nsbp; tag with space
        output = ReplaceString(output, "&nsbp;", " ");

        return output;
    }

    std::string RemoveStartAndEndQuotes(const std::string& input)
    {
        std::string output = input;
        if (output.size() >= 2 && (output.front() == '"' || output.back() == '"'))
        {
            if (output.front() == '"')
            {
                output.erase(output.begin());
            }
            
            if (output.back() == '"')
            {
                output.pop_back();
            }
        }

        return output;
    }

    std::string Utf8ToIso(std::string utf8str)
    {
        std::string isostr;
        for (size_t i = 0; i < utf8str.size(); ) 
        {
            // Convert the next character to its Unicode code point
            unsigned int codepoint = utf8str[i] & 0xFF;
            int extra_bytes = 0;
            if ((codepoint & 0xE0) == 0xC0) {
                extra_bytes = 1;
                codepoint &= 0x1F;
            }
            else if ((codepoint & 0xF0) == 0xE0) 
            {
                extra_bytes = 2;
                codepoint &= 0x0F;
            }
            else if ((codepoint & 0xF8) == 0xF0) 
            {
                extra_bytes = 3;
                codepoint &= 0x07;
            }
            for (int j = 0; j < extra_bytes; j++) 
            {
                i++;
                codepoint = (codepoint << 6) | (utf8str[i] & 0x3F);
            }
            i++;

            // Convert the Unicode code point to an ISO-8859-1 code point, if possible
            if (codepoint <= 0xFF) 
            {
                isostr += static_cast<char>(codepoint);
            }
            else 
            {
                // Convert the Unicode code point back to UTF-8 encoding
                std::string utf8char;
                if (codepoint <= 0x7FF) 
                {
                    utf8char += static_cast<char>(0xC0 | (codepoint >> 6));
                    utf8char += static_cast<char>(0x80 | (codepoint & 0x3F));
                }
                else if (codepoint <= 0xFFFF) 
                {
                    utf8char += static_cast<char>(0xE0 | (codepoint >> 12));
                    utf8char += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
                    utf8char += static_cast<char>(0x80 | (codepoint & 0x3F));
                }
                else 
                {
                    utf8char += static_cast<char>(0xF0 | (codepoint >> 18));
                    utf8char += static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F));
                    utf8char += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
                    utf8char += static_cast<char>(0x80 | (codepoint & 0x3F));
                }

                isostr += utf8char;
            }
        }

        return isostr;
    }

    std::string IsoToUtf8(const std::string& isostr) 
    {
        std::string utf8str;

        for (size_t i = 0; i < isostr.size(); i++) 
        {
            unsigned int codepoint = isostr[i] & 0xFF;
            if (codepoint <= 0x7F) 
            {
                utf8str += static_cast<char>(codepoint);
            }
            else if (codepoint <= 0x7FF) 
            {
                utf8str += static_cast<char>(0xC0 | (codepoint >> 6));
                utf8str += static_cast<char>(0x80 | (codepoint & 0x3F));
            }
            else if (codepoint <= 0xFFFF) 
            {
                utf8str += static_cast<char>(0xE0 | (codepoint >> 12));
                utf8str += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
                utf8str += static_cast<char>(0x80 | (codepoint & 0x3F));
            }
            else 
            {
                utf8str += static_cast<char>(0xF0 | (codepoint >> 18));
                utf8str += static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F));
                utf8str += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
                utf8str += static_cast<char>(0x80 | (codepoint & 0x3F));
            }
        }

        return utf8str;
    }

    void ReplaceNewLine(std::string& theText)
    {
        const std::regex re("\\n");
        theText = std::regex_replace(theText, re, "$B");
    }

    void ReplaceNameTag(std::string& theText, uint32 expansion, uint32 locale)
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
            theText = std::regex_replace(theText, re, "$n");
        }

        // find buggy russian tags
        // classic RU does not support declination
        if (locale == 7)
        {
            // do not remove declination
            if (expansion == 10)
            {
                const std::regex re1(R"(\|\d-\d\(\$N\))");
                theText = std::regex_replace(theText, re1, "$N");
                const std::regex re2(R"(\|\d-\d\(\$n\))");
                theText = std::regex_replace(theText, re2, "$n");
                const std::regex re3(R"(\|\d-\d\(\$N(?!\)))");
                theText = std::regex_replace(theText, re3, "$N");
                const std::regex re4(R"(\|\d-\d\(\$n(?!\)))");
                theText = std::regex_replace(theText, re4, "$n");
            }
            else
            {
                const std::regex re3(R"(\|\d-\d\(\$N(?!\)))");
                theText = std::regex_replace(theText, re3, "|$1-$2($N)");
                const std::regex re4(R"(\|\d-\d\(\$n(?!\)))");
                theText = std::regex_replace(theText, re4, "|$1-$2($n)");
            }
        }
    }

    void ReplaceRaceTag(std::string& theText, uint32 expansion, uint32 locale)
    {
        std::vector<std::string> nameTagsUp;
        nameTagsUp.emplace_back("<Race>");
        nameTagsUp.emplace_back("<Raza>");
        nameTagsUp.emplace_back("<Раса>");
        nameTagsUp.emplace_back("<Volk>");

        std::vector<std::string> nameTagsLow;
        nameTagsLow.emplace_back("<race>");
        nameTagsLow.emplace_back("<raza>");
        nameTagsLow.emplace_back("<раса>");
        nameTagsLow.emplace_back("<volk>");

        for (const auto& name : nameTagsUp)
        {
            const std::regex re(name);
            theText = std::regex_replace(theText, re, "$R");
        }
        for (const auto& name : nameTagsLow)
        {
            const std::regex re(name);
            theText = std::regex_replace(theText, re, "$r");
        }

        if (locale == 7)
        {
            std::vector<std::string> posClassesLow;
            std::vector<std::string> posClassesUp;
            posClassesLow.emplace_back("человек");
            posClassesLow.emplace_back("орк");
            posClassesLow.emplace_back("нежить");
            posClassesLow.emplace_back("таурен");
            posClassesLow.emplace_back("гном");
            posClassesLow.emplace_back("дворф");
            posClassesLow.emplace_back("тролль");
            posClassesLow.emplace_back("ночной эльф");
            posClassesLow.emplace_back("дреней");
            posClassesLow.emplace_back("эльф крови");
            posClassesUp.emplace_back("Человек");
            posClassesUp.emplace_back("Орк");
            posClassesUp.emplace_back("Нежить");
            posClassesUp.emplace_back("Таурен");
            posClassesUp.emplace_back("Гном");
            posClassesUp.emplace_back("Дворф");
            posClassesUp.emplace_back("Тролль");
            posClassesUp.emplace_back("Ночной эльф");
            posClassesUp.emplace_back("Дреней");
            posClassesUp.emplace_back("Эльф крови");

            // classic RU does not support declination
            for (const auto& className : posClassesLow)
            {
                const std::regex re("\\|(\\d)-(\\d)\\(" + className + "\\)");
                if (expansion == 10) // do not remove declination
                    theText = std::regex_replace(theText, re, "$r");
                else
                    theText = std::regex_replace(theText, re, "|$1-$2($r)");
            }

            for (const auto& className : posClassesUp)
            {
                const std::regex re("\\|(\\d)-(\\d)\\(" + className + "\\)");
                if (expansion == 10) // do not remove declination
                    theText = std::regex_replace(theText, re, "$R");
                else
                    theText = std::regex_replace(theText, re, "|$1-$2($R)");
            }

            // do not remove declination
            if (expansion == 10)
            {
                const std::regex re1(R"(\|\d-\d\(\$R\))");
                theText = std::regex_replace(theText, re1, "$R");
                const std::regex re2(R"(\|\d-\d\(\$r\))");
                theText = std::regex_replace(theText, re2, "$r");
                const std::regex re3(R"(\|\d-\d\(\$R(?!\)))");
                theText = std::regex_replace(theText, re3, "$R");
                const std::regex re4(R"(\|\d-\d\(\$r(?!\)))");
                theText = std::regex_replace(theText, re4, "$r");
            }
            else
            {
                const std::regex re3(R"(\|(\d)-(\d)\(\$R(?!\)))");
                theText = std::regex_replace(theText, re3, "|$1-$2($R)");
                const std::regex re4(R"(\|(\d)-(\d)\(\$r(?!\)))");
                theText = std::regex_replace(theText, re4, "|$1-$2($r)");
            }
        }
    }

    void ReplaceClassTag(std::string& theText, uint32 expansion, uint32 locale)
    {
        std::vector<std::string> nameTagsUp;
        nameTagsUp.emplace_back("<Class>");
        nameTagsUp.emplace_back("<Clase>");
        nameTagsUp.emplace_back("<Classe>");
        nameTagsUp.emplace_back("<Класс>");
        nameTagsUp.emplace_back("<Klasse>");

        std::vector<std::string> nameTagsLow;
        nameTagsLow.emplace_back("<class>");
        nameTagsLow.emplace_back("<clase>");
        nameTagsLow.emplace_back("<classe>");
        nameTagsLow.emplace_back("<класс>");
        nameTagsUp.emplace_back("<Klasse>");

        for (const auto& name : nameTagsUp)
        {
            const std::regex re(name);
            theText = std::regex_replace(theText, re, "$C");
        }
        for (const auto& name : nameTagsLow)
        {
            const std::regex re(name);
            theText = std::regex_replace(theText, re, "$c");
        }

        if (locale == 7)
        {
            std::vector<std::string> posClassesLow;
            std::vector<std::string> posClassesUp;
            posClassesLow.emplace_back("чернокнижник");
            posClassesLow.emplace_back("жрец");
            posClassesLow.emplace_back("воин");
            posClassesLow.emplace_back("друид");
            posClassesLow.emplace_back("паладин");
            posClassesLow.emplace_back("разбойник");
            posClassesLow.emplace_back("шаман");
            posClassesLow.emplace_back("маг");
            posClassesLow.emplace_back("охотник");
            posClassesLow.emplace_back("рыцарь смерти");
            posClassesUp.emplace_back("Чернокнижник");
            posClassesUp.emplace_back("Жрец");
            posClassesUp.emplace_back("Воин");
            posClassesUp.emplace_back("Друид");
            posClassesUp.emplace_back("Паладин");
            posClassesUp.emplace_back("Разбойник");
            posClassesUp.emplace_back("Шаман");
            posClassesUp.emplace_back("Маг");
            posClassesUp.emplace_back("Охотник");
            posClassesUp.emplace_back("Рыцарь смерти");

            // classic RU does not support declination
            for (const auto& className : posClassesLow)
            {
                std::string rege = "\\|(\\d)-(\\d)\\(" + className + "\\)";
                const std::regex re(rege);
                if (expansion == 10) // do not remove declination
                    theText = std::regex_replace(theText, re, "$c");
                else
                    theText = std::regex_replace(theText, re, "|$1-$2($c)");
            }
            for (const auto& className : posClassesUp)
            {
                std::string rege = "\\|(\\d)-(\\d)\\(" + className + "\\)";
                const std::regex re(rege);
                if (expansion == 10) // do not remove declination
                    theText = std::regex_replace(theText, re, "$C");
                else
                    theText = std::regex_replace(theText, re, "|$1-$2($C)");
            }

            // do not remove declination
            if (expansion == 10)
            {
                const std::regex re1(R"(\|\d-\d\(\$C\))");
                theText = std::regex_replace(theText, re1, "$C");
                const std::regex re2(R"(\|\d-\d\(\$c\))");
                theText = std::regex_replace(theText, re2, "$c");
                const std::regex re3(R"(\|\d-\d\(\$C(?!\)))");
                theText = std::regex_replace(theText, re3, "$C");
                const std::regex re4(R"(\|\d-\d\(\$c(?!\)))");
                theText = std::regex_replace(theText, re4, "$c");
            }
            else
            {
                const std::regex re3(R"(\|(\d)-(\d)\(\$C(?!\)))");
                theText = std::regex_replace(theText, re3, "|$1-$2($C)");
                const std::regex re4(R"(\|(\d)-(\d)\(\$c(?!\)))");
                theText = std::regex_replace(theText, re4, "|$1-$2($c)");
            }
        }
    }

    void ReplaceGenderTag(std::string& theText, uint32 expansion, uint32 locale)
    {
        // TO DO: Add other locales
        // ...

        // Spanish
        if (locale == 6)
        {
            // Select all gender strings
            std::vector<std::string> genderTags;
            std::regex pattern(R"(<[^/<>]*\/[^<>]*>)");
            for (auto it = std::sregex_iterator(theText.begin(), theText.end(), pattern); it != std::sregex_iterator(); ++it)
            {
                if (!ContainsString(it->str(), "</") && !ContainsString(it->str(), "< /") && !StringHelper::ContainsString(it->str(), "/>") && !StringHelper::ContainsString(it->str(), "/ >"))
                {
                    genderTags.push_back(it->str());
                }
            }

            // Replace them with the correct syntax
            for (std::string& genderTag : genderTags)
            {
                std::vector<std::string> genderWords = SplitString(genderTag, "/");
                if (genderWords.size() == 2)
                {
                    // Remove the "<" and ">" from the gender words
                    genderWords[0].erase(genderWords[0].begin());
                    genderWords[1].pop_back();

                    theText = ReplaceString(theText, genderTag, "$G" + genderWords[0] + ":" + genderWords[1] + ";");
                }
            }
        }
        // Russian
        if (locale == 7)
        {
            // Select all gender strings
            std::vector<std::string> genderTags;
            std::regex pattern(R"(<[^/<>]*\/[^<>]*>)");
            for (auto it = std::sregex_iterator(theText.begin(), theText.end(), pattern); it != std::sregex_iterator(); ++it)
            {
                if (!ContainsString(it->str(), "</") && !ContainsString(it->str(), "< /") && !StringHelper::ContainsString(it->str(), "/>") && !StringHelper::ContainsString(it->str(), "/ >"))
                {
                    genderTags.push_back(it->str());
                }
            }

            // Replace them with the correct syntax
            for (std::string& genderTag : genderTags)
            {
                std::vector<std::string> genderWords = SplitString(genderTag, "/");
                if (genderWords.size() == 2)
                {
                    // Remove the "<" and ">" from the gender words
                    genderWords[0].erase(genderWords[0].begin());
                    genderWords[1].pop_back();

                    theText = ReplaceString(theText, genderTag, "$G" + genderWords[0] + ":" + genderWords[1] + ";");

                    // remove :c, not sure what it's for
                    //theText = ReplaceString(theText, ":c", "");
                }
            }
        }
    }

    std::string EncodeWoWString(const std::string& input, uint32 expansion = 0, uint32 locale = 0)
    {
        std::string output = input;
        ReplaceNewLine(output);
        ReplaceClassTag(output, expansion, locale);
        ReplaceRaceTag(output, expansion, locale);
        ReplaceNameTag(output, expansion, locale);
        ReplaceGenderTag(output, expansion, locale);
        return output;
    }
}

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

    std::wofstream fileOut(path + filename);
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
        case 8:
            return "tw";
        case 10:
            return "pt";
        case 11:
            return "it";
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
        case TYPE_ITEM:
            return "item";
        case TYPE_OBJECT:
            return "object";
        case TYPE_ACHIEVEMENT:
            return "achievement";
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
        case 8:
            return 5;
            break;
    }
}

uint32 itemTextId(const std::string& partName)
{
    if (partName == "name")
        return 1;
    else if (partName == "description")
        return 2;
    else if (partName == "pages")
        return 3;

    return 0;
}

uint32 gameObjectTextId(const std::string& partName)
{
    if (partName == "name")
        return 1;

    return 0;
}

uint32 achievementTextId(const std::string& partName)
{
    if (partName == "name")
        return 1;
    else if (partName == "description")
        return 2;

    return 0;
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


std::string itemColumnName(const std::string& partName, uint32 locale = 1)
{
    if (partName == "name")
        return locale == 1 ? "name" : ("name_loc" + std::to_string(localeRealId(locale)));
    if (partName == "description")
        return locale == 1 ? "description" : ("description_loc" + std::to_string(localeRealId(locale)));
    if (partName == "page")
        return locale == 1 ? "text" : ("Text_loc" + std::to_string(localeRealId(locale)));

    return "";
}

std::string itemAllColumns(uint32 locale = 1)
{
    if (locale == 1)
        return "name, description";

    return "name_loc" + std::to_string(localeRealId(locale)) +
    ", description_loc" + std::to_string(localeRealId(locale));
}

std::string gameObjectColumnName(const std::string& partName, uint32 locale = 1)
{
    if (partName == "name")
        return locale == 1 ? "name" : ("name_loc" + std::to_string(localeRealId(locale)));
    if (partName == "page")
        return locale == 1 ? "text" : ("Text_loc" + std::to_string(localeRealId(locale)));

    return "";
}

std::string gameObjectAllColumns(uint32 locale = 1)
{
    if (locale == 1)
        return "name";

    return "name_loc" + std::to_string(localeRealId(locale));
}

std::string getAchievementLocale(uint32 locale)
{
    std::string localeName;
    switch (locale)
    {
        case 1:
        {
            localeName = "enUS";
            break;
        }

        case 2:
        {
            localeName = "koKR";
            break;
        }

        case 3:
        {
            localeName = "frFR";
            break;
        }

        case 4:
        {
            localeName = "deDE";
            break;
        }

        case 5:
        {
            localeName = "zhCN";
            break;
        }

        case 6:
        {
            localeName = "esES";
            break;
        }

        case 7:
        {
            localeName = "ruRU";
            break;
        }

        case 8:
        {
            localeName = "zhTW";
            break;
        }

        default: break;
    }

    return localeName;
}

std::string achievementColumnName(const std::string& partName, uint32 locale = 1)
{
    const std::string localeName = getAchievementLocale(locale);
    if (partName == "name")
        return "Title_Lang_" + localeName;
    if (partName == "description")
        return "Description_Lang_" + localeName;

    return "";
}

std::string achievementAllColumns(uint32 locale = 1)
{
    const std::string localeName = getAchievementLocale(locale);
    return "Title_Lang_" + localeName +
        ", Description_Lang_" + localeName;
}

const char* dbName(uint32 expansion, const std::string& projectName = "cmangos")
{
    if (expansion == 1 && projectName == "vmangos")
        return "mangos";

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

bool hasGenderTag(std::string& theText, bool dbOnly = false, uint32 expansion = 0, uint32 locale = 0)
{
    if (theText.empty())
        return false;

    std::vector<std::string> genderTags = StringHelper::GetStringsBetween(theText, "$G", ";");
    
    if (!dbOnly)
    {
        std::regex pattern(R"(<[^/<>]*\/[^<>]*>)");

        for (auto it = std::sregex_iterator(theText.begin(), theText.end(), pattern); it != std::sregex_iterator(); ++it)
        {
            if (!StringHelper::ContainsString(it->str(), "</") && !StringHelper::ContainsString(it->str(), "< /") && !StringHelper::ContainsString(it->str(), "/>") && !StringHelper::ContainsString(it->str(), "/ >"))
            {
                genderTags.push_back(it->str());
            }
        }
    }

    return !genderTags.empty();
}

bool hasNameTag(std::string& theText, bool dbOnly = false, uint32 expansion = 0, uint32 locale = 0)
{
    if (theText.empty())
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
    return std::any_of(nameTags.begin(), nameTags.end(), [theText](const std::string& str){return theText.find(str) != std::string::npos;});
}

bool hasRaceTag(std::string& theText, bool dbOnly = false, uint32 expansion = 0, uint32 locale = 0)
{
    if (theText.empty())
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
        nameTags.emplace_back("<volk>");
        nameTags.emplace_back("<Volk>");
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
            if (std::regex_search(theText, re))
                return true;
        }
    }

    return std::any_of(nameTags.begin(), nameTags.end(), [theText](const std::string& str){return theText.find(str) != std::string::npos;});
}

bool hasClassTag(std::string& theText, bool dbOnly = false, uint32 expansion = 0, uint32 locale = 0)
{
    if (theText.empty())
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
        nameTags.emplace_back("<klasse>");
        nameTags.emplace_back("<Klasse>");
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
            if (std::regex_search(theText, re))
                return true;
        }
    }

    return std::any_of(nameTags.begin(), nameTags.end(), [theText](const std::string& str){return theText.find(str) != std::string::npos;});
}

bool IsSameString(const std::string& text1, const std::string& text2, uint32 locale = 1, bool replaceTags = false)
{
    // do not compare empty strings here
    if (text1.empty() && text2.empty())
        return false;

    if (text1 == text2)
        return true;

    // expensive
    if (replaceTags)
    {
        std::string text1Encoded = StringHelper::EncodeWoWString(text1, 1, locale);
        std::string text2Encoded = StringHelper::EncodeWoWString(text2, 1, locale);
        return text1Encoded == text2Encoded;
    }

    return false;
}

bool ShouldOverrideDifference(const std::string& name, const std::string& wowhead, const std::string& local)
{
    unsigned int choice;
    std::cout << "\nA different version has been found for the " << name << ":\n\n";

    std::string localVersion = StringHelper::ReplaceString(local, "$B", "\n");
    std::string wowheadVersion = StringHelper::EncodeWoWString(wowhead);
    wowheadVersion = StringHelper::ReplaceString(wowhead, "$B", "\n");

    std::cout << "Wowhead Version: \n";
    std::cout << wowheadVersion << "\n\n";

    std::cout << "Local Version: \n";
    std::cout << localVersion << "\n\n";

    std::cout << "Select Option: \n";
    std::cout << "1) Use Wowhead version \n";
    std::cout << "2) Keep local version \n";
    std::cin >> choice;

    return choice == 1;
}

std::string updateQuestFromWhQuery(WowheadQuestInfo* whInfo, DatabaseQuestInfo* dbInfo, const std::string& partName, uint32 expansion, uint32 locale)
{
    if (!whInfo || !dbInfo || !whInfo->IsLoaded(whInfo->GetCurExpansion(), whInfo->GetCurLocale()) || !dbInfo->IsLoaded(dbInfo->GetCurExpansion(), dbInfo->GetCurLocale()))
        return "";

    std::string tableName = locale == 1 ? "quest_template" : "locales_quest";
    std::string colName = questColumnName(partName, locale);
    // fix tw => cn
    std::string whText = whInfo->GetQuestPart(partName, expansion, locale == 8 ? 5 : locale);
    if (whText.empty())
        return "";
    //std::string dbText = dbInfo->GetQuestPart(partName, expansion, locale);

    // replace nameTags
    whText = StringHelper::EncodeWoWString(whText, expansion, locale);

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
    replaceText = StringHelper::EncodeWoWString(replaceText, expansion, locale);

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

std::string updateAchievementFromWhQuery(WowheadAchievementInfo* whInfo, DatabaseAchievementInfo* dbInfo, const std::string& partName, uint32 expansion, uint32 locale)
{
    if (!whInfo || !dbInfo || !whInfo->IsLoaded(whInfo->GetCurExpansion(), whInfo->GetCurLocale()) || !dbInfo->IsLoaded(dbInfo->GetCurExpansion(), dbInfo->GetCurLocale()))
        return "";

    std::string tableName = "achievement_dbc";
    std::string colName = achievementColumnName(partName, locale);
    // fix tw => cn
    std::string whText = whInfo->GetAchievementPart(partName, expansion, locale == 8 ? 5 : locale);
    if (whText.empty())
        return "";

    // replace nameTags
    whText = StringHelper::EncodeWoWString(whText, expansion, locale);

    char* tStr = new char[strlen(whText.c_str()) * 2 + 1];
    mysql_real_escape_string(sDataMgr.GetCon(expansion)->GetMysql(), tStr, whText.c_str(), strlen(whText.c_str()));
    std::string replaceString(tStr);
    delete[] tStr;

    std::string command = "UPDATE " + tableName + " SET " + colName + " = '" + replaceString + "' WHERE ID = '" + std::to_string(dbInfo->GetEntry()) + "';";
    return command;
}

std::string updateItemFromWhQuery(WowheadItemInfo* whInfo, DatabaseItemInfo* dbInfo, const std::string& partName, uint32 expansion, uint32 locale)
{
    if (!whInfo || !dbInfo || !whInfo->IsLoaded(whInfo->GetCurExpansion(), whInfo->GetCurLocale()) || !dbInfo->IsLoaded(dbInfo->GetCurExpansion(), dbInfo->GetCurLocale()))
        return "";

    std::string tableName = locale == 1 ? "item_template" : "locales_item";
    std::string colName = itemColumnName(partName, locale);
    // fix tw => cn
    std::string whText = whInfo->GetItemPart(partName, expansion, locale == 8 ? 5 : locale);
    if (whText.empty())
        return "";

    // replace nameTags
    whText = StringHelper::EncodeWoWString(whText, expansion, locale);

    char* tStr = new char[strlen(whText.c_str()) * 2 + 1];
    mysql_real_escape_string(sDataMgr.GetCon(expansion)->GetMysql(), tStr, whText.c_str(), strlen(whText.c_str()));
    std::string replaceString(tStr);
    delete[] tStr;

    std::string command = "UPDATE " + tableName + " SET " + colName + " = '" + replaceString + "' WHERE entry = '" + std::to_string(dbInfo->GetEntry()) + "';";
    return command;
}

std::string updateItemPageFromWhQuery(const std::string& replace, uint32 pageEntry, WowheadItemInfo* whInfo, uint32 expansion, uint32 locale)
{
    if (!whInfo || replace.empty() || !whInfo->IsLoaded(whInfo->GetCurExpansion(), whInfo->GetCurLocale()))
        return "";

    std::string tableName = locale == 1 ? "page_text" : "locales_page_text";
    std::string colName = itemColumnName("page", locale);

    // replace nameTags
    std::string whText = StringHelper::EncodeWoWString(replace, expansion, locale);

    char* tStr = new char[strlen(whText.c_str()) * 2 + 1];
    mysql_real_escape_string(sDataMgr.GetCon(expansion)->GetMysql(), tStr, whText.c_str(), strlen(whText.c_str()));
    std::string replaceString(tStr);
    delete[] tStr;

    std::string command = "UPDATE " + tableName + " SET " + colName + " = '" + replaceString + "' WHERE entry = '" + std::to_string(pageEntry) + "';";
    return command;
}

std::string updateItemFromTextQueryNoTags(const std::string& replace, DatabaseItemInfo* dbInfo, const std::string& partName, uint32 expansion, uint32 locale)
{
    if (!dbInfo || replace.empty() || !dbInfo->IsLoaded(dbInfo->GetCurExpansion(), dbInfo->GetCurLocale()))
        return "";

    std::string tableName = locale == 1 ? "item_template" : "locales_item";
    std::string colName = itemColumnName(partName, locale);

    char* tStr = new char[strlen(replace.c_str()) * 2 + 1];
    mysql_real_escape_string(sDataMgr.GetCon(expansion)->GetMysql(), tStr, replace.c_str(), strlen(replace.c_str()));
    std::string replaceString(tStr);
    delete[] tStr;

    std::string command = "UPDATE " + tableName + " SET " + colName + " = '" + replaceString + "' WHERE entry = '" + std::to_string(dbInfo->GetEntry()) + "';";
    return command;
}

std::string updateItemPageFromTextQueryNoTags(const std::string& replace, uint32 pageEntry, DatabaseItemInfo* dbInfo, uint32 expansion, uint32 locale)
{
    if (!dbInfo || replace.empty() || !dbInfo->IsLoaded(dbInfo->GetCurExpansion(), dbInfo->GetCurLocale()))
        return "";

    std::string tableName = locale == 1 ? "page_text" : "locales_page_text";
    std::string colName = itemColumnName("page", locale);

    char* tStr = new char[strlen(replace.c_str()) * 2 + 1];
    mysql_real_escape_string(sDataMgr.GetCon(expansion)->GetMysql(), tStr, replace.c_str(), strlen(replace.c_str()));
    std::string replaceString(tStr);
    delete[] tStr;

    std::string command = "UPDATE " + tableName + " SET " + colName + " = '" + replaceString + "' WHERE entry = '" + std::to_string(pageEntry) + "';";
    return command;
}

std::string updateAchievementCriteriaFromTextQueryNoTags(const std::string& replace, uint32 criteriaEntry, DatabaseAchievementInfo* dbInfo, uint32 expansion, uint32 locale)
{
    if (!dbInfo || replace.empty() || !dbInfo->IsLoaded(dbInfo->GetCurExpansion(), dbInfo->GetCurLocale()))
        return "";

    std::string tableName = "achievement_criteria_dbc";
    std::string colName = "Description_Lang_" + getAchievementLocale(locale);

    char* tStr = new char[strlen(replace.c_str()) * 2 + 1];
    mysql_real_escape_string(sDataMgr.GetCon(expansion)->GetMysql(), tStr, replace.c_str(), strlen(replace.c_str()));
    std::string replaceString(tStr);
    delete[] tStr;

    std::string command = "UPDATE " + tableName + " SET " + colName + " = '" + replaceString + "' WHERE ID = '" + std::to_string(criteriaEntry) + "';";
    return command;
}

std::string updateGameObjectFromWhQuery(WowheadGameObjectInfo* whInfo, DatabaseGameObjectInfo* dbInfo, const std::string& partName, uint32 expansion, uint32 locale)
{
    if (!whInfo || !dbInfo || !whInfo->IsLoaded(whInfo->GetCurExpansion(), whInfo->GetCurLocale()) || !dbInfo->IsLoaded(dbInfo->GetCurExpansion(), dbInfo->GetCurLocale()))
        return "";

    std::string tableName = locale == 1 ? "gameobject_template" : "locales_gameobject";
    std::string colName = gameObjectColumnName(partName, locale);
    // fix tw => cn
    std::string whText = whInfo->GetGameObjectPart(partName, expansion, locale == 8 ? 5 : locale);
    if (whText.empty())
        return "";

    // replace nameTags
    whText = StringHelper::EncodeWoWString(whText, expansion, locale);

    char* tStr = new char[strlen(whText.c_str()) * 2 + 1];
    mysql_real_escape_string(sDataMgr.GetCon(expansion)->GetMysql(), tStr, whText.c_str(), strlen(whText.c_str()));
    std::string replaceString(tStr);
    delete[] tStr;

    std::string command = "UPDATE " + tableName + " SET " + colName + " = '" + replaceString + "' WHERE entry = '" + std::to_string(dbInfo->GetEntry()) + "';";
    return command;
}

std::string updateGameObjectPageFromWhQuery(const std::string& replace, uint32 pageEntry, WowheadGameObjectInfo* whInfo, uint32 expansion, uint32 locale)
{
    if (!whInfo || replace.empty() || !whInfo->IsLoaded(whInfo->GetCurExpansion(), whInfo->GetCurLocale()))
        return "";

    std::string tableName = locale == 1 ? "page_text" : "locales_page_text";
    std::string colName = gameObjectColumnName("page", locale);

    // replace nameTags
    std::string whText = StringHelper::EncodeWoWString(replace, expansion, locale);

    char* tStr = new char[strlen(whText.c_str()) * 2 + 1];
    mysql_real_escape_string(sDataMgr.GetCon(expansion)->GetMysql(), tStr, whText.c_str(), strlen(whText.c_str()));
    std::string replaceString(tStr);
    delete[] tStr;

    std::string command = "UPDATE " + tableName + " SET " + colName + " = '" + replaceString + "' WHERE entry = '" + std::to_string(pageEntry) + "';";
    return command;
}

std::string updateGameObjectFromTextQueryNoTags(const std::string& replace, DatabaseGameObjectInfo* dbInfo, const std::string& partName, uint32 expansion, uint32 locale)
{
    if (!dbInfo || replace.empty() || !dbInfo->IsLoaded(dbInfo->GetCurExpansion(), dbInfo->GetCurLocale()))
        return "";

    std::string tableName = locale == 1 ? "gameobject_template" : "locales_gameobject";
    std::string colName = gameObjectColumnName(partName, locale);

    char* tStr = new char[strlen(replace.c_str()) * 2 + 1];
    mysql_real_escape_string(sDataMgr.GetCon(expansion)->GetMysql(), tStr, replace.c_str(), strlen(replace.c_str()));
    std::string replaceString(tStr);
    delete[] tStr;

    std::string command = "UPDATE " + tableName + " SET " + colName + " = '" + replaceString + "' WHERE entry = '" + std::to_string(dbInfo->GetEntry()) + "';";
    return command;
}

std::string updateGameObjectPageFromTextQueryNoTags(const std::string& replace, uint32 pageEntry, DatabaseGameObjectInfo* dbInfo, uint32 expansion, uint32 locale)
{
    if (!dbInfo || replace.empty() || !dbInfo->IsLoaded(dbInfo->GetCurExpansion(), dbInfo->GetCurLocale()))
        return "";

    std::string tableName = locale == 1 ? "page_text" : "locales_page_text";
    std::string colName = gameObjectColumnName("page", locale);

    char* tStr = new char[strlen(replace.c_str()) * 2 + 1];
    mysql_real_escape_string(sDataMgr.GetCon(expansion)->GetMysql(), tStr, replace.c_str(), strlen(replace.c_str()));
    std::string replaceString(tStr);
    delete[] tStr;

    std::string command = "UPDATE " + tableName + " SET " + colName + " = '" + replaceString + "' WHERE entry = '" + std::to_string(pageEntry) + "';";
    return command;
}

std::string FindAltText(DatabaseQuestInfo* dbInfo, const std::string& partName, uint32 expansion, uint32 locale, bool skipDiff = true)
{
    if (!dbInfo)
        return "";

    uint32 localeId = locale == 8 ? 5 : locale;

    std::string origText = dbInfo->GetQuestPart(partName, expansion, localeId);
    std::string origTextEng = dbInfo->GetQuestPart(partName, expansion, 1);

    // if engText empty, skip
    if (/*origText.empty() || */origTextEng.empty())
        return "";

    // find alt text in other expansion
    std::string altText;
    for (auto ex = 1; ex <= MAX_EXPANSION; ++ex) // expansion
    {
        if (ex == expansion)
            continue;

        // load expansion data here (should be loaded already)
        /*if (!dbInfo->IsLoaded(ex, localeId))
            dbInfo->LoadEntryData(ex, localeId);*/

        // skip if no data (should not happen even if empty quest)
        if (!dbInfo->IsLoaded(ex, localeId))
            continue;

        // skip if no locales to take
        std::string expText = dbInfo->GetQuestPart(partName, ex, localeId);
        if (expText.empty())
            continue;

        // skip if EN texts are different (quest was changed in expansion)
        std::string expTextEng = dbInfo->GetQuestPart(partName, ex, 1);
        if (skipDiff && expTextEng != origTextEng)
            continue;

        // skip if locales are same as EN text
        if(IsSameString(expText, expTextEng, locale, true))
            continue;

        // not needed if skipDiff is true
        if(!skipDiff && IsSameString(expText, origTextEng, locale,true))
            continue;

        altText = expText;
    }

    return altText;
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

bool DatabaseConnect::RunIntStmt(const std::string &command, std::vector<int> &result, int32 numStrings, bool silent)
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
                result.emplace_back(int_data);
            }
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

bool DatabaseConnect::GetDbInt(const std::string& command, int& result, bool silent)
{
    std::vector<int> strVec;
    RunIntStmt(command, strVec, 1, silent);
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
            command = "SELECT entry FROM quest_template WHERE entry = " + std::to_string(id);

        if (type == TYPE_NPC)
            command = "SELECT Entry FROM creature_template WHERE Entry = " + std::to_string(id);

        if (type == TYPE_ITEM)
            command = "SELECT entry FROM item_template WHERE Entry = " + std::to_string(id);

        if (type == TYPE_OBJECT)
            command = "SELECT entry FROM gameobject_template WHERE entry = " + std::to_string(id);

        if (type == TYPE_ACHIEVEMENT)
            command = "SELECT ID FROM achievement_dbc WHERE ID = " + std::to_string(id);

        int result;
        return GetDbInt(command, result, true);
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
    case 10:
        localePrefix = "/pt";
        break;
    case 11:
        localePrefix = "/it";
        break;
    }

    // Achievements are only available from wotlk+
    if (type == TYPE_ACHIEVEMENT && expansion < 3)
        expansion = 3;

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

    std::string pageText = response.text;

    // Fix for achievement pages
    if (type == TYPE_ACHIEVEMENT)
    {
        pageText = StringHelper::ReplaceString(pageText, "<h1 class=\"heading-size-1 h1-icon\">", "<h1 class=\"heading-size-1\">");
    }

    return pageText;
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
                msg_delay("> " + typeName(type) + " #" + std::to_string(id) + " " + expansionName(expansion) + "-" + localeName(locale) + " Caching... \n");
            }
            writeToFile(wh_page.c_str(), std::to_string(id) + ".txt", cacheLocation);
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
            for (auto i = 1; i <= MAX_LOCALE; ++i) // locales
            {
                if (locale != 0 && i != locale)
                    continue;

                if (localeName(i).empty() || localeName(i) == "tw" || localeName(i) == "pt" || localeName(i) == "it")
                    continue;

                if (noEnglishPage)
                {
                    noEnglishPage = false;
                    break;
                }

                std::string cacheLocation = "cache/" + expansionName(e) + "/" + localeName(i) + "/" + typeName(type) + "/";
                std::string wh_page = readFromFile(cacheLocation + std::to_string(min_id) + ".txt");
                if (!wh_page.empty())
                    msg_nodelay("\n> " + typeName(type) + " #" + std::to_string(min_id) + " " + expansionName(e) + "-" + localeName(i) + ": Already cached, skipping...");
                else
                {
                    std::string page = loadPageOrCache(type, min_id, e, i, false);
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
            for (auto e = 1; e < 4 + 1; ++e) // expansion
            {
                if (expansion != 0 && e != expansion)
                    continue;

#ifdef SKIP_NONDB
                // skip loading pages not in DB
                if ((type == TYPE_OBJECT || type == TYPE_ITEM || type == TYPE_NPC || type == TYPE_ACHIEVEMENT) && sDataMgr.IsDbOn(e))
                {
                    auto DbCon = sDataMgr.GetCon(e);
                    if (DbCon)
                    {
                        if (!DbCon->IsEntryExistInDb(type, id))
                        {
                            msg_nodelay("\n> " + typeName(type) + " # (" + std::to_string(id) + "/" + std::to_string(max_id) + ") " + expansionName(e) + ": Not in DB, skipping...");
                            continue;
                        }
                    }
                }
#endif

                bool noEnglishPage = false; // speed up missing quests process
                for (auto i = 1; i <= MAX_LOCALE; ++i) // locales
                {
                    if (locale != 0 && i != locale)
                        continue;

                    if (localeName(i).empty() || localeName(i) == "tw" || localeName(i) == "pt" || localeName(i) == "it")
                        continue;

                    if (noEnglishPage)
                    {
                        noEnglishPage = false;
                        break;
                    }

                    std::string cacheLocation = "cache/" + expansionName(e) + "/" + localeName(i) + "/" + typeName(type) + "/";
                    std::string wh_page = readFromFile(cacheLocation + std::to_string(id) + ".txt");
                    if (!wh_page.empty())
                        msg_nodelay("\n> " + typeName(type) + " # (" + std::to_string(id) + "/" + std::to_string(max_id) + ") " + expansionName(e) + "-" + localeName(i) + ": Already cached, skipping...");
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
            if (id > min_id)
                msg_nodelay("\n");
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
                            // BR tag on div...
                            if (childNode->v.text.text && childNode->type == GUMBO_NODE_TEXT)
                                query_result << childNode->v.text.text;
                            else
                                query_result << "\n";
                            //query_result << "\n";
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

void parse_quest(std::ostringstream& query_result, GumboNode* node, unsigned int questPartID, unsigned int locale = 0, uint32 hotfix = 0)
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
        if (hotfix == 1)
        {
            attr_name = "classes";
            attr_value = "first";
            between = true;
            attr_second_name = "id";
            attr_second_value = "data.mapper.objectiveTerms";
        }
        else
        {
            attr_name = "id";
            attr_value = "lknlksndgg-progress";
        }
        break;
    case 5: // OfferRewardText
        if (hotfix == 1)
        {
            attr_name = "classes";
            attr_value = "first";
            between = true;
            attr_second_name = "id";
            attr_second_value = "data.mapper.objectiveTerms";
        }
        else
        {
            attr_name = "id";
            attr_value = "lknlksndgg-completion";
        }
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

std::string parse_quest(GumboNode* node, unsigned int questPartID, unsigned int locale = 1, uint32 hotfix = 0)
{
    std::ostringstream query_result;
    parse_quest(query_result, node, questPartID, locale, hotfix);
    return query_result.str();
}

std::string ReadQuestText(GumboOutput* parsedPage, const std::string& questPart, uint32 locale, uint32 hotfix = 0)
{
    std::ostringstream questText;
    uint32 questPartId = questTextId(questPart);
    if (!questPartId)
        return "";

    return parse_quest(parsedPage->root, questPartId, locale, hotfix);
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
    if (locale == 1 && sDataMgr.getProjectName() == "vmangos")
        command << " ORDER by patch DESC LIMIT 1";
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

void parse_item(std::ostringstream& query_result, GumboNode* node, unsigned int itemPartID, unsigned int locale = 0)
{
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

    switch (itemPartID)
    {
    default:
    case 1: // Name
        attr_name = "class";
        attr_value = "heading-size-1";
        break;
    case 2: // Description
        attr_name = "class";
        attr_value = "q";
        break;
    }

    parse_details(query_result, node, tag, attr_name, attr_value, contents, between, attr_second_name, attr_second_value, attr_second_alt_value, steps_back, unique, hasChildren, preTag, preType, searchChildren, childIndex, fromEnd, ignoreContent);

    
    if (itemPartID == 2)
    {
        // Get the text between quotes
        query_result.str(StringHelper::GetFirstStringBetween(query_result.str(), "\"", "\""));
        
        // Remove quotes from description
        query_result.str(StringHelper::RemoveStartAndEndQuotes(query_result.str()));
    }
}

std::string parse_item(GumboNode* node, unsigned int itemPartID, unsigned int locale = 1)
{
    std::ostringstream query_result;
    parse_item(query_result, node, itemPartID, locale);
    return query_result.str();
}

std::string ReadItemText(GumboOutput* parsedPage, const std::string& itemPart, uint32 locale)
{
    uint32 itemPartId = itemTextId(itemPart);
    if (!itemPartId)
        return "";

    return parse_item(parsedPage->root, itemPartId, locale);
}

void parse_game_object(std::ostringstream& query_result, GumboNode* node, unsigned int gameObjectPartID, unsigned int locale = 0)
{
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

    switch (gameObjectPartID)
    {
    default:
    case 1: // Name
        attr_name = "class";
        attr_value = "heading-size-1";
        break;
    }

    parse_details(query_result, node, tag, attr_name, attr_value, contents, between, attr_second_name, attr_second_value, attr_second_alt_value, steps_back, unique, hasChildren, preTag, preType, searchChildren, childIndex, fromEnd, ignoreContent);
}

std::string parse_game_object(GumboNode* node, unsigned int gameObjectPartID, unsigned int locale = 1)
{
    std::ostringstream query_result;
    parse_game_object(query_result, node, gameObjectPartID, locale);
    return query_result.str();
}

std::string ReadGameObjectText(GumboOutput* parsedPage, const std::string& itemPart, uint32 locale)
{
    uint32 gameObjectPartId = gameObjectTextId(itemPart);
    if (!gameObjectPartId)
        return "";

    return parse_game_object(parsedPage->root, gameObjectPartId, locale);
}

void parse_achievement(std::ostringstream& query_result, GumboNode* node, unsigned int achievementPartID, unsigned int locale = 0)
{
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

    switch (achievementPartID)
    {
    default:
    case 1: // Name
        attr_name = "class";
        attr_value = "heading-size-1";
        break;
    }

    parse_details(query_result, node, tag, attr_name, attr_value, contents, between, attr_second_name, attr_second_value, attr_second_alt_value, steps_back, unique, hasChildren, preTag, preType, searchChildren, childIndex, fromEnd, ignoreContent);
}

std::string parse_achievement(GumboNode* node, unsigned int achievementPartID, unsigned int locale = 1)
{
    std::ostringstream query_result;
    parse_achievement(query_result, node, achievementPartID, locale);
    return query_result.str();
}

std::string ReadAchievementText(GumboOutput* parsedPage, const std::string& itemPart, uint32 locale)
{
    uint32 achievementPartID = achievementTextId(itemPart);
    if (!achievementPartID)
        return "";

    return parse_achievement(parsedPage->root, achievementPartID, locale);
}

GumboOutput* GumboParsePage(const std::string& page)
{
    return gumbo_parse(page.c_str());
}

void GumboClosePage(GumboOutput* output)
{
    gumbo_destroy_output(&kGumboDefaultOptions, output);
}

ItemStrings LoadItemFull(DatabaseConnect* dbCon, uint32 id, uint32 locale)
{
    ItemStrings itemStrings;
    std::ostringstream command;
    std::vector<std::string> itemText;
    std::string itemColumns = itemAllColumns(locale);
    std::string tableName = locale == 1 ? "item_template" : "locales_item";
    command << "SELECT " << itemColumns << " FROM " << tableName << " WHERE entry = " << std::to_string(id);

    if (locale == 1 && sDataMgr.getProjectName() == "vmangos")
    {
        command << " ORDER by patch DESC LIMIT 1";
    }
    if (!dbCon->GetDbStrings(command.str(), itemText, 2, true))
    {
        return itemStrings;
    }

    itemStrings.name = itemText[0];
    itemStrings.description = itemText[1];

    // Select pages
    std::ostringstream command2;
    if (sDataMgr.getProjectName() == "vmangos")
        command2 << "SELECT page_text FROM item_template WHERE entry = " << std::to_string(id);
    else
        command2 << "SELECT PageText FROM item_template WHERE entry = " << std::to_string(id);

    int pageEntry;
    if (!dbCon->GetDbInt(command2.str(), pageEntry, true))
    {
        return itemStrings;
    }

    const std::string itemPageTableName = locale == 1 ? "page_text" : "locales_page_text";
    const std::string itemPageColumnName = locale == 1 ? "text" : "Text_loc" + std::to_string(localeRealId(locale));
    bool nextPage = pageEntry > 0;
    while (nextPage)
    {
        // Get the page text
        std::string page;
        std::ostringstream command3;
        command3 << "SELECT " << itemPageColumnName << " FROM " << itemPageTableName << " WHERE entry = " << std::to_string(pageEntry);
        if (!dbCon->GetDbString(command3.str(), page, true))
        {
            break;
        }

        PageEntry itemPage;
        itemPage.entry = pageEntry;
        itemPage.text = page;
        itemStrings.pages.push_back(std::move(itemPage));

        // Get the next page
        std::ostringstream command4;
        command4 << "SELECT next_page FROM page_text WHERE entry = " << std::to_string(pageEntry);
        if (!dbCon->GetDbInt(command4.str(), pageEntry, true))
        {
            break;
        }

        nextPage = pageEntry > 0;
    }

    return itemStrings;
}

GameObjectStrings LoadGameObjectFull(DatabaseConnect* dbCon, uint32 id, uint32 locale)
{
    GameObjectStrings gameObjectStrings;

    std::string gameObjectName;
    std::ostringstream command;
    std::string gameObjectColumns = gameObjectAllColumns(locale);
    std::string tableName = locale == 1 ? "gameobject_template" : "locales_gameobject";
    command << "SELECT " << gameObjectColumns << " FROM " << tableName << " WHERE entry = " << std::to_string(id);

    if (!dbCon->GetDbString(command.str(), gameObjectName, true))
    {
        return gameObjectStrings;
    }

    gameObjectStrings.name = gameObjectName;

    int gameObjectType;
    std::ostringstream command2;
    gameObjectColumns = "type";
    command2 << "SELECT " << gameObjectColumns << " FROM gameobject_template WHERE entry = " << std::to_string(id);

    if (!dbCon->GetDbInt(command2.str(), gameObjectType, true))
    {
        return gameObjectStrings;
    }

    if (gameObjectType == 9)
    {
        int pageEntry = 0;
        std::ostringstream command3;
        gameObjectColumns = "data0";
        command3 << "SELECT " << gameObjectColumns << " FROM gameobject_template WHERE entry = " << std::to_string(id);

        if (!dbCon->GetDbInt(command3.str(), pageEntry, true))
        {
            return gameObjectStrings;
        }

        const std::string pageTableName = locale == 1 ? "page_text" : "locales_page_text";
        const std::string pageColumnName = locale == 1 ? "text" : "Text_loc" + std::to_string(localeRealId(locale));
        bool nextPage = pageEntry > 0;
        while (nextPage)
        {
            // Get the page text
            std::string page;
            std::ostringstream command3;
            command3 << "SELECT " << pageColumnName << " FROM " << pageTableName << " WHERE entry = " << std::to_string(pageEntry);
            if (!dbCon->GetDbString(command3.str(), page, true))
            {
                break;
            }

            PageEntry gameObjectPage;
            gameObjectPage.entry = pageEntry;
            gameObjectPage.text = page;
            gameObjectStrings.pages.push_back(std::move(gameObjectPage));

            // Get the next page
            std::ostringstream command4;
            command4 << "SELECT next_page FROM page_text WHERE entry = " << std::to_string(pageEntry);
            if (!dbCon->GetDbInt(command4.str(), pageEntry, true))
            {
                break;
            }

            nextPage = pageEntry > 0;
        }
    }

    return gameObjectStrings;
}

AchievementStrings LoadAchievementFull(DatabaseConnect* dbCon, uint32 id, uint32 locale)
{
    AchievementStrings achievementStrings;

    std::vector<std::string> achievementTexts;
    std::ostringstream command;
    std::string achievementColumns = achievementAllColumns(locale);
    std::string tableName = "achievement_dbc";
    command << "SELECT " << achievementColumns << " FROM " << tableName << " WHERE ID = " << std::to_string(id);

    if (!dbCon->GetDbStrings(command.str(), achievementTexts, 2, true))
    {
        return achievementStrings;
    }

    achievementStrings.name = achievementTexts[0];
    achievementStrings.description = achievementTexts[1];

    int criteriasCount = 0;
    std::string criteriaTableName = "achievement_criteria_dbc";
    std::string criteriaColumns = "ID, Description_Lang_" + getAchievementLocale(locale);
    std::ostringstream command2;
    command2 << "SELECT COUNT(*) FROM " << criteriaTableName << " WHERE Achievement_Id = " << std::to_string(id);
    if (dbCon->GetDbInt(command2.str(), criteriasCount) && criteriasCount > 0)
    {
        for (uint32 i = 0; i < criteriasCount; i++)
        {
            std::vector<std::string> criteriasData;
            std::ostringstream command3;
            command3 << "SELECT " << criteriaColumns << " FROM " << criteriaTableName << " WHERE Achievement_Id = " << std::to_string(id) << " LIMIT " << i << ", 1";
            if (dbCon->GetDbStrings(command3.str(), criteriasData, 2, true))
            {
                AchievementCriteria criteria;
                criteria.entry = std::stoi(criteriasData[0]);
                criteria.description = criteriasData[1];
                achievementStrings.criterias.emplace_back(std::move(criteria));
            }
        }
    }

    return achievementStrings;
}

std::vector<PageEntry> ReadPages(const std::string& whPage, uint32 expansion, uint32 locale)
{
    std::vector<PageEntry> pages;

    std::string pagesString = StringHelper::GetFirstStringBetween(whPage, "new Book({", "})");
    if (!pagesString.empty())
    {
        pagesString = StringHelper::GetFirstStringBetween(pagesString, "pages: [", "]");
        if (!pagesString.empty())
        {
            pagesString = StringHelper::RemoveStartAndEndQuotes(pagesString);
            pagesString = StringHelper::DecodeHTMLString(pagesString);
            pagesString = StringHelper::IsoToUtf8(pagesString);
            pagesString = StringHelper::EncodeWoWString(pagesString, expansion, locale);

            std::vector<std::string> pagesStr = StringHelper::SplitString(pagesString, "\",\"");
            for (std::string& page : pagesStr)
            {
                const bool isHTML = StringHelper::ContainsString(page, "</h1>") || StringHelper::ContainsString(page, "</p>") || StringHelper::ContainsString(page, "<img");
                if (isHTML)
                {
                    // Insert html and body tags
                    page = "<HTML><BODY>" + page + "</BODY></HTML>";

                    // Replace $B tag for <br/>
                    page = StringHelper::ReplaceString(page, "$B", "<BR/>");

                    // Convert tags to upper case
                    page = StringHelper::ReplaceString(page, "<h1", "<H1");
                    page = StringHelper::ReplaceString(page, "h1>", "H1>");
                    page = StringHelper::ReplaceString(page, "<p", "<P");
                    page = StringHelper::ReplaceString(page, "p>", "P>");
                    page = StringHelper::ReplaceString(page, "<br", "<BR");
                    page = StringHelper::ReplaceString(page, "<img", "<IMG");
                    page = StringHelper::ReplaceString(page, "p>", "P>");

                    // Fix the image file paths
                    // Replace "/" with "\"
                    const std::string searchStr = "src=\"";
                    const char replaceChar = '\\';
                    size_t pos = page.find(searchStr);
                    while (pos != std::string::npos)
                    {
                        size_t endPos = page.find("\"", pos + searchStr.length());
                        std::string substr = page.substr(pos + searchStr.length(), endPos - pos - searchStr.length());
                        size_t slashPos = 0;
                        while ((slashPos = substr.find("/", slashPos)) != std::string::npos)
                        {
                            substr.replace(slashPos, 1, &replaceChar, 1);
                            slashPos += 1;
                        }

                        page.replace(pos + searchStr.length(), endPos - pos - searchStr.length(), substr);
                        pos = page.find(searchStr, pos + searchStr.length());
                    }

                    // Fix image path point to game image asset
                    page = StringHelper::ReplaceString(page, "\\\\wow.zamimg.com\\images\\wow\\interface", "Interface");
                    page = StringHelper::ReplaceString(page, ".jpg", "");
                    page = StringHelper::ReplaceString(page, "\"[<IMG", "<IMG");
                }

                PageEntry itemPage;
                itemPage.text = page;
                pages.push_back(std::move(itemPage));
            }
        }
    }

    return pages;
}

WowheadGameObjectInfo* LoadWowheadGameObjectInfo(uint32 id, uint32 expansion, uint32 locale, bool onlyOneVersion = false)
{
    if (!id)
        return nullptr;

    if (locale == 8)
        locale = 5;

    // check if already saved
    if (expansion && locale && sDataMgr.gameObjectWowheadInfoList[id])
    {
        WowheadGameObjectInfo* savedItemInfo = sDataMgr.gameObjectWowheadInfoList[id];
        if (savedItemInfo->IsLoaded(expansion, locale))
        {
            savedItemInfo->SetCurExpansion(expansion);
            savedItemInfo->SetCurLocale(locale);
            return savedItemInfo;
        }
        else
        {
            savedItemInfo->LoadEntryData(expansion, locale);
            if (savedItemInfo->IsLoaded(expansion, locale))
            {
                savedItemInfo->SetCurExpansion(expansion);
                savedItemInfo->SetCurLocale(locale);
                return savedItemInfo;
            }
            else
                return nullptr;
        }
    }

    WowheadGameObjectInfo* gameObjectInfo = nullptr;
    bool isLoaded = false;
    if (sDataMgr.gameObjectWowheadInfoList[id])
    {
        isLoaded = true;
        gameObjectInfo = sDataMgr.gameObjectWowheadInfoList[id];
    }
    else
        gameObjectInfo = new WowheadGameObjectInfo(id, expansion ? expansion : 1, locale ? locale : 1);

    if (onlyOneVersion && expansion && locale)
        gameObjectInfo->LoadEntryData(expansion, locale);
    else if (expansion && !locale)
    {
        for (auto i = 1; i <= MAX_LOCALE; ++i)
            gameObjectInfo->LoadEntryData(expansion, i);
    }
    else if (!expansion && locale)
    {
        for (auto i = 1; i <= MAX_EXPANSION; ++i)
            gameObjectInfo->LoadEntryData(i, locale);
    }
    else
        gameObjectInfo->LoadEntryDataAllVersions();

    // if not loaded english classic
    if (!gameObjectInfo->IsLoaded(expansion ? expansion : 1, locale ? locale : 1))
    {
        bool silent = false;
        if (!silent)
        {
            if (locale != 1)
                msg_delay("> WH " + typeName(TYPE_OBJECT) + " #" + std::to_string(id) + "-" + localeName(locale ? locale : 1) + ": Translation Not Found! \n");
            else
                msg_delay("> WH " + typeName(TYPE_OBJECT) + " #" + std::to_string(id) + "-" + localeName(locale ? locale : 1) + ": Game Object Not Found! \n");
        }

        // erase after checking above
        if (!isLoaded)
        {
            delete gameObjectInfo;
        }

        return nullptr;
    }

    if (!isLoaded) sDataMgr.gameObjectWowheadInfoList[id] = gameObjectInfo;
    return gameObjectInfo;
}

WowheadItemInfo* LoadWowheadItemInfo(uint32 id, uint32 expansion, uint32 locale, bool onlyOneVersion = false)
{
    if (!id)
        return nullptr;

    if (locale == 8)
        locale = 5;

    // check if already saved
    if (expansion && locale && sDataMgr.itemWowheadInfoList[id])
    {
        WowheadItemInfo* savedItemInfo = sDataMgr.itemWowheadInfoList[id];
        if (savedItemInfo->IsLoaded(expansion, locale))
        {
            savedItemInfo->SetCurExpansion(expansion);
            savedItemInfo->SetCurLocale(locale);
            return savedItemInfo;
        }
        else
        {
            savedItemInfo->LoadEntryData(expansion, locale);
            if (savedItemInfo->IsLoaded(expansion, locale))
            {
                savedItemInfo->SetCurExpansion(expansion);
                savedItemInfo->SetCurLocale(locale);
                return savedItemInfo;
            }
            else
                return nullptr;
        }
    }

    WowheadItemInfo* itemInfo = nullptr;
    bool isLoaded = false;
    if (sDataMgr.itemWowheadInfoList[id])
    {
        isLoaded = true;
        itemInfo = sDataMgr.itemWowheadInfoList[id];
    }
    else
        itemInfo = new WowheadItemInfo(id, expansion ? expansion : 1, locale ? locale : 1);

    if (onlyOneVersion && expansion && locale)
        itemInfo->LoadEntryData(expansion, locale);
    else if (expansion && !locale)
    {
        for (auto i = 1; i <= MAX_LOCALE; ++i)
            itemInfo->LoadEntryData(expansion, i);
    }
    else if (!expansion && locale)
    {
        for (auto i = 1; i <= MAX_EXPANSION; ++i)
            itemInfo->LoadEntryData(i, locale);
    }
    else
        itemInfo->LoadEntryDataAllVersions();

    // if not loaded english classic
    if (!itemInfo->IsLoaded(expansion ? expansion : 1, locale ? locale : 1))
    {
        bool silent = false;
        if (!silent)
        {
            if (locale != 1)
                msg_delay("> WH " + typeName(TYPE_ITEM) + " #" + std::to_string(id) + "-" + localeName(locale ? locale : 1) + ": Translation Not Found! \n");
            else
                msg_delay("> WH " + typeName(TYPE_ITEM) + " #" + std::to_string(id) + "-" + localeName(locale ? locale : 1) + ": Item Not Found! \n");
        }

        // erase after checking above
        if (!isLoaded)
        {
            delete itemInfo;
        }

        return nullptr;
    }

    if (!isLoaded) sDataMgr.itemWowheadInfoList[id] = itemInfo;
    return itemInfo;
}

DatabaseItemInfo* LoadDatabaseItemInfo(uint32 id, uint32 expansion, uint32 locale, bool onlyOneVersion = false, bool silent = false)
{
    if (!id)
        return nullptr;

    bool allLocales = false;
    if (!expansion && !locale)
        allLocales = true;

    //DatabaseQuestInfo* questInfo = nullptr;

    // check if already saved
    if (expansion && locale && sDataMgr.itemDatabaseInfoList[id])
    {
        DatabaseItemInfo* savedItemInfo = sDataMgr.itemDatabaseInfoList[id];
        if (savedItemInfo->IsLoaded(expansion, locale))
        {
            savedItemInfo->SetCurExpansion(expansion);
            savedItemInfo->SetCurLocale(locale);
            return savedItemInfo;
        }
        else
        {
            savedItemInfo->LoadEntryData(expansion, locale);
            if (savedItemInfo->IsLoaded(expansion, locale))
            {
                savedItemInfo->SetCurExpansion(expansion);
                savedItemInfo->SetCurLocale(locale);
                return savedItemInfo;
            }
            else
                return nullptr;
        }
    }

    DatabaseItemInfo* itemInfo = nullptr;
    bool isLoaded = false;
    if (sDataMgr.itemDatabaseInfoList[id])
    {
        isLoaded = true;
        itemInfo = sDataMgr.itemDatabaseInfoList[id];
    }
    else
        itemInfo = new DatabaseItemInfo(id, expansion ? expansion : 1, locale ? locale : 1);

    if (onlyOneVersion && expansion && locale)
        itemInfo->LoadEntryData(expansion, locale);
    else if (expansion && !locale)
    {
        for (auto i = 1; i <= MAX_LOCALE; ++i)
            itemInfo->LoadEntryData(expansion, i);
    }
    else if (!expansion && locale)
    {
        for (auto i = 1; i <= MAX_EXPANSION; ++i)
            itemInfo->LoadEntryData(i, locale);
    }
    else
        itemInfo->LoadEntryDataAllVersions();

    if (!itemInfo->IsLoaded(expansion ? expansion : 1, locale ? locale : 1))
    {
        if (!silent)
        {
            if (locale > 1)
                msg_delay("> DB " + typeName(TYPE_ITEM) + " #" + std::to_string(id) + "-" + localeName(locale ? locale : 1) + ": Translation Not Found! \n");
            else
                msg_delay("> DB " + typeName(TYPE_ITEM) + " #" + std::to_string(id) + "-" + localeName(locale ? locale : 1) + ": Item Not Found! \n");
        }

        delete itemInfo;
        return nullptr;
    }

    if (!isLoaded) sDataMgr.itemDatabaseInfoList[id] = itemInfo;
    return itemInfo;
}

WowheadAchievementInfo* LoadWowheadAchievementInfo(uint32 id, uint32 expansion, uint32 locale, bool onlyOneVersion = false)
{
    if (!id)
        return nullptr;

    if (locale == 8)
        locale = 5;

    // check if already saved
    if (expansion && locale && sDataMgr.achievementWowheadInfoList[id])
    {
        WowheadAchievementInfo* savedAchievementInfo = sDataMgr.achievementWowheadInfoList[id];
        if (savedAchievementInfo->IsLoaded(expansion, locale))
        {
            savedAchievementInfo->SetCurExpansion(expansion);
            savedAchievementInfo->SetCurLocale(locale);
            return savedAchievementInfo;
        }
        else
        {
            savedAchievementInfo->LoadEntryData(expansion, locale);
            if (savedAchievementInfo->IsLoaded(expansion, locale))
            {
                savedAchievementInfo->SetCurExpansion(expansion);
                savedAchievementInfo->SetCurLocale(locale);
                return savedAchievementInfo;
            }
            else
                return nullptr;
        }
    }

    WowheadAchievementInfo* achievementInfo = nullptr;
    bool isLoaded = false;
    if (sDataMgr.achievementWowheadInfoList[id])
    {
        isLoaded = true;
        achievementInfo = sDataMgr.achievementWowheadInfoList[id];
    }
    else
        achievementInfo = new WowheadAchievementInfo(id, expansion ? expansion : 1, locale ? locale : 1);

    if (onlyOneVersion && expansion && locale)
        achievementInfo->LoadEntryData(expansion, locale);
    else if (expansion && !locale)
    {
        for (auto i = 1; i <= MAX_LOCALE; ++i)
            achievementInfo->LoadEntryData(expansion, i);
    }
    else if (!expansion && locale)
    {
        for (auto i = 1; i <= MAX_EXPANSION; ++i)
            achievementInfo->LoadEntryData(i, locale);
    }
    else
        achievementInfo->LoadEntryDataAllVersions();

    // if not loaded english classic
    if (!achievementInfo->IsLoaded(expansion ? expansion : 1, locale ? locale : 1))
    {
        bool silent = false;
        if (!silent)
        {
            if (locale != 1)
                msg_delay("> WH " + typeName(TYPE_ACHIEVEMENT) + " #" + std::to_string(id) + "-" + localeName(locale ? locale : 1) + ": Translation Not Found! \n");
            else
                msg_delay("> WH " + typeName(TYPE_ACHIEVEMENT) + " #" + std::to_string(id) + "-" + localeName(locale ? locale : 1) + ": Achievement Not Found! \n");
        }

        // erase after checking above
        if (!isLoaded)
        {
            delete achievementInfo;
        }

        return nullptr;
    }

    if (!isLoaded) sDataMgr.achievementWowheadInfoList[id] = achievementInfo;
    return achievementInfo;
}

DatabaseAchievementInfo* LoadDatabaseAchievementInfo(uint32 id, uint32 expansion, uint32 locale, bool onlyOneVersion = false, bool silent = false)
{
    if (!id)
        return nullptr;

    bool allLocales = false;
    if (!expansion && !locale)
        allLocales = true;

    // check if already saved
    if (expansion && locale && sDataMgr.achievementDatabaseInfoList[id])
    {
        DatabaseAchievementInfo* savedAchievementInfo = sDataMgr.achievementDatabaseInfoList[id];
        if (savedAchievementInfo->IsLoaded(expansion, locale))
        {
            savedAchievementInfo->SetCurExpansion(expansion);
            savedAchievementInfo->SetCurLocale(locale);
            return savedAchievementInfo;
        }
        else
        {
            savedAchievementInfo->LoadEntryData(expansion, locale);
            if (savedAchievementInfo->IsLoaded(expansion, locale))
            {
                savedAchievementInfo->SetCurExpansion(expansion);
                savedAchievementInfo->SetCurLocale(locale);
                return savedAchievementInfo;
            }
            else
                return nullptr;
        }
    }

    DatabaseAchievementInfo* achievementInfo = nullptr;
    bool isLoaded = false;
    if (sDataMgr.achievementDatabaseInfoList[id])
    {
        isLoaded = true;
        achievementInfo = sDataMgr.achievementDatabaseInfoList[id];
    }
    else
        achievementInfo = new DatabaseAchievementInfo(id, expansion ? expansion : 1, locale ? locale : 1);

    if (onlyOneVersion && expansion && locale)
        achievementInfo->LoadEntryData(expansion, locale);
    else if (expansion && !locale)
    {
        for (auto i = 1; i <= MAX_LOCALE; ++i)
            achievementInfo->LoadEntryData(expansion, i);
    }
    else if (!expansion && locale)
    {
        for (auto i = 1; i <= MAX_EXPANSION; ++i)
            achievementInfo->LoadEntryData(i, locale);
    }
    else
        achievementInfo->LoadEntryDataAllVersions();

    if (!achievementInfo->IsLoaded(expansion ? expansion : 1, locale ? locale : 1))
    {
        if (!silent)
        {
            if (locale > 1)
                msg_delay("> DB " + typeName(TYPE_ACHIEVEMENT) + " #" + std::to_string(id) + "-" + localeName(locale ? locale : 1) + ": Translation Not Found! \n");
            else
                msg_delay("> DB " + typeName(TYPE_ACHIEVEMENT) + " #" + std::to_string(id) + "-" + localeName(locale ? locale : 1) + ": Achievement Not Found! \n");
        }

        delete achievementInfo;
        return nullptr;
    }

    if (!isLoaded) sDataMgr.achievementDatabaseInfoList[id] = achievementInfo;
    return achievementInfo;
}

DatabaseGameObjectInfo* LoadDatabaseGameObjectInfo(uint32 id, uint32 expansion, uint32 locale, bool onlyOneVersion = false, bool silent = false)
{
    if (!id)
        return nullptr;

    bool allLocales = false;
    if (!expansion && !locale)
        allLocales = true;

    //DatabaseQuestInfo* questInfo = nullptr;

    // check if already saved
    if (expansion && locale && sDataMgr.gameObjectDatabaseInfoList[id])
    {
        DatabaseGameObjectInfo* savedGameObjectInfo = sDataMgr.gameObjectDatabaseInfoList[id];
        if (savedGameObjectInfo->IsLoaded(expansion, locale))
        {
            savedGameObjectInfo->SetCurExpansion(expansion);
            savedGameObjectInfo->SetCurLocale(locale);
            return savedGameObjectInfo;
        }
        else
        {
            savedGameObjectInfo->LoadEntryData(expansion, locale);
            if (savedGameObjectInfo->IsLoaded(expansion, locale))
            {
                savedGameObjectInfo->SetCurExpansion(expansion);
                savedGameObjectInfo->SetCurLocale(locale);
                return savedGameObjectInfo;
            }
            else
                return nullptr;
        }
    }

    DatabaseGameObjectInfo* gameObjectInfo = nullptr;
    bool isLoaded = false;
    if (sDataMgr.gameObjectDatabaseInfoList[id])
    {
        isLoaded = true;
        gameObjectInfo = sDataMgr.gameObjectDatabaseInfoList[id];
    }
    else
        gameObjectInfo = new DatabaseGameObjectInfo(id, expansion ? expansion : 1, locale ? locale : 1);

    if (onlyOneVersion && expansion && locale)
        gameObjectInfo->LoadEntryData(expansion, locale);
    else if (expansion && !locale)
    {
        for (auto i = 1; i <= MAX_LOCALE; ++i)
            gameObjectInfo->LoadEntryData(expansion, i);
    }
    else if (!expansion && locale)
    {
        for (auto i = 1; i <= MAX_EXPANSION; ++i)
            gameObjectInfo->LoadEntryData(i, locale);
    }
    else
        gameObjectInfo->LoadEntryDataAllVersions();

    if (!gameObjectInfo->IsLoaded(expansion ? expansion : 1, locale ? locale : 1))
    {
        if (!silent)
        {
            if (locale > 1)
                msg_delay("> DB " + typeName(TYPE_OBJECT) + " #" + std::to_string(id) + "-" + localeName(locale ? locale : 1) + ": Translation Not Found! \n");
            else
                msg_delay("> DB " + typeName(TYPE_OBJECT) + " #" + std::to_string(id) + "-" + localeName(locale ? locale : 1) + ": Quest Not Found! \n");
        }

        delete gameObjectInfo;
        return nullptr;
    }

    if (!isLoaded) sDataMgr.gameObjectDatabaseInfoList[id] = gameObjectInfo;
    return gameObjectInfo;
}

WowheadQuestInfo* LoadWowheadQuestInfo(uint32 id, uint32 expansion, uint32 locale, bool onlyOneVersion = false)
{
    if (!id)
        return nullptr;

    if (locale == 8)
        locale = 5;

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
    if (sDataMgr.questWowheadInfoList[id])
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

std::vector<PageEntry> ReadItemPages(const std::string& whPage, uint32 locale, uint32 expansion, uint32 itemId)
{
    std::vector<PageEntry> pages = ReadPages(whPage, expansion, locale);
    if (!pages.empty())
    {
        // Get the page entries from the database
        auto DbConnect = sDataMgr.GetCon(expansion);
        if (DbConnect && DbConnect->IsConnected() && DbConnect->IsEntryExistInDb(TYPE_ITEM, itemId))
        {
            DatabaseItemInfo* dbItem = LoadDatabaseItemInfo(itemId, expansion, 1, true);
            if (dbItem)
            {
                const auto& dbItemPages = dbItem->GetPages();
                if (pages.size() == dbItemPages.size())
                {
                    for (uint8 i = 0; i < pages.size(); i++)
                    {
                        pages[i].entry = dbItemPages[i].entry;
                    }
                }
            }
        }
    }

    return pages;
}

std::vector<PageEntry> ReadGameObjectPages(const std::string& whPage, uint32 locale, uint32 expansion, uint32 gameObjectId)
{
    std::vector<PageEntry> pages = ReadPages(whPage, expansion, locale);
    if (!pages.empty())
    {
        // Get the page entries from the database
        auto DbConnect = sDataMgr.GetCon(expansion);
        if (DbConnect && DbConnect->IsConnected() && DbConnect->IsEntryExistInDb(TYPE_OBJECT, gameObjectId))
        {
            DatabaseGameObjectInfo* dbGameObject = LoadDatabaseGameObjectInfo(gameObjectId, expansion, 1, true);
            if (dbGameObject)
            {
                const auto& dbGameObjectPages = dbGameObject->GetPages();
                if (pages.size() == dbGameObjectPages.size())
                {
                    for (uint8 i = 0; i < pages.size(); i++)
                    {
                        pages[i].entry = dbGameObjectPages[i].entry;
                    }
                }
            }
        }
    }

    return pages;
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
            std::cout << "3) Item \n";
            std::cout << "4) GameObject \n";
            std::cout << "5) Achievement \n";
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
                msg_delay("\n> WH Objective 1: " + qInfo->GetObjective(0)); if (qInfo->GetCurLocale() != 1) msg_delay(" > WH Objective 1 EN: " + qInfo->GetObjective(0, expansion, 1));
                msg_delay("\n> WH Objective 2: " + qInfo->GetObjective(1)); if (qInfo->GetCurLocale() != 1) msg_delay(" > WH Objective 2 EN: " + qInfo->GetObjective(1, expansion, 1));
                msg_delay("\n> WH Objective 3: " + qInfo->GetObjective(2)); if (qInfo->GetCurLocale() != 1) msg_delay(" > WH Objective 3 EN: " + qInfo->GetObjective(2, expansion, 1));
                msg_delay("\n> WH Objective 4: " + qInfo->GetObjective(3)); if (qInfo->GetCurLocale() != 1) msg_delay(" > WH Objective 4 EN: " + qInfo->GetObjective(3, expansion, 1));
                if (qDbInfo)
                {
                    msg_delay("\n> DB Objective 1: " + qDbInfo->GetObjective(0)); if (qDbInfo->GetCurLocale() != 1) msg_delay(" > DB Objective 1 EN: " + qDbInfo->GetObjective(0, expansion, 1));
                    msg_delay("\n> DB Objective 2: " + qDbInfo->GetObjective(1)); if (qDbInfo->GetCurLocale() != 1) msg_delay(" > DB Objective 2 EN: " + qDbInfo->GetObjective(1, expansion, 1));
                    msg_delay("\n> DB Objective 3: " + qDbInfo->GetObjective(2)); if (qDbInfo->GetCurLocale() != 1) msg_delay(" > DB Objective 3 EN: " + qDbInfo->GetObjective(2, expansion, 1));
                    msg_delay("\n> DB Objective 4: " + qDbInfo->GetObjective(3)); if (qDbInfo->GetCurLocale() != 1) msg_delay(" > DB Objective 4 EN: " + qDbInfo->GetObjective(3, expansion, 1));
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
            // Item
            if (type == 3)
            {
                // load all locales, set current for quick access
                WowheadItemInfo* itemInfo = LoadWowheadItemInfo(entry, expansion, locale, true);
                if (!itemInfo)
                {
                    msg_delay("> WH " + typeName(TYPE_ITEM) + " #" + std::to_string(entry) + "-" + localeName(locale) + ": Failed to load from Wowhead! \n");
                    return 0;
                }

                // Load English in all cases
                if (itemInfo->GetCurLocale() != 1)
                    itemInfo->LoadEntryData(expansion, 1);

                DatabaseItemInfo* itemDbInfo = LoadDatabaseItemInfo(entry, expansion, locale, true);
                if (!itemDbInfo)
                {
                    msg_delay("> DB " + typeName(TYPE_ITEM) + " #" + std::to_string(entry) + "-" + localeName(locale) + ": Failed to load from Database! \n");
                    return 0;
                }

                // Load English in all cases
                if (itemDbInfo->GetCurLocale() != 1)
                    itemDbInfo->LoadEntryData(expansion, 1);

                std::cout << "\n";
                msg_delay("\n");

                msg_delay("\n> WH Name: " + itemInfo->GetName() + "\n");
                if (itemInfo->GetCurLocale() != 1) msg_delay("\n> WH Name EN: " + itemInfo->GetName(expansion, 1) + "\n");
                if (itemDbInfo) msg_delay("\n> DB Name: " + itemDbInfo->GetName() + "\n");
                if (itemDbInfo->GetCurLocale() != 1) msg_delay("\n> DB Name EN: " + itemDbInfo->GetName(expansion, 1) + "\n");
                msg_delay("\n> WH Description: " + itemInfo->GetDescription() + "\n");
                if (itemInfo->GetCurLocale() != 1) msg_delay("\n> WH Description EN: " + itemInfo->GetDescription(expansion, 1) + "\n");
                if (itemDbInfo) msg_delay("\n> DB Description: " + itemDbInfo->GetDescription() + "\n");
                if (itemDbInfo->GetCurLocale() != 1) msg_delay("\n> DB Description EN: " + itemDbInfo->GetDescription(expansion, 1) + "\n");
            
                msg_delay("\n> WH Pages:");
                const std::vector<PageEntry>& itemPages = itemInfo->GetPages();
                for (uint8 i = 0; i < itemPages.size(); i++)
                {
                    std::stringstream pageStr;
                    pageStr << "\n> Page " << i + 1 << ": " << itemPages[i].text << "\n";
                    msg_delay(pageStr.str());
                }

                if (itemInfo->GetCurLocale() != 1)
                {
                    msg_delay("\n> WH Pages EN:");
                    const std::vector<PageEntry>& itemEnPages = itemInfo->GetPages(expansion, 1);
                    for (uint8 i = 0; i < itemEnPages.size(); i++)
                    {
                        std::stringstream pageStr;
                        pageStr << "\n> Page " << i + 1 << ": " << itemEnPages[i].text << "\n";
                        msg_delay(pageStr.str());
                    }
                }

                msg_delay("\n> DB Pages:");
                const std::vector<PageEntry>& itemDbPages = itemDbInfo->GetPages();
                for (uint8 i = 0; i < itemDbPages.size(); i++)
                {
                    std::stringstream pageStr;
                    pageStr << "\n> Page " << i + 1 << ": " << itemDbPages[i].text << "\n";
                    msg_delay(pageStr.str());
                }

                if (itemInfo->GetCurLocale() != 1)
                {
                    msg_delay("\n> DB Pages EN:");
                    const std::vector<PageEntry>& itemDbEnPages = itemDbInfo->GetPages(expansion, 1);
                    for (uint8 i = 0; i < itemDbEnPages.size(); i++)
                    {
                        std::stringstream pageStr;
                        pageStr << "\n> Page " << i + 1 << ": " << itemDbEnPages[i].text << "\n";
                        msg_delay(pageStr.str());
                    }
                }
            }
            // Game Object
            if (type == 4)
            {
                // load all locales, set current for quick access
                WowheadGameObjectInfo* gameObjectInfo = LoadWowheadGameObjectInfo(entry, expansion, locale, true);
                if (!gameObjectInfo)
                {
                    msg_delay("> WH " + typeName(TYPE_OBJECT) + " #" + std::to_string(entry) + "-" + localeName(locale) + ": Failed to load from Wowhead! \n");
                    return 0;
                }

                // Load English in all cases
                if (gameObjectInfo->GetCurLocale() != 1)
                    gameObjectInfo->LoadEntryData(expansion, 1);

                DatabaseGameObjectInfo* gameObjectDbInfo = LoadDatabaseGameObjectInfo(entry, expansion, locale, true);
                if (!gameObjectDbInfo)
                {
                    msg_delay("> DB " + typeName(TYPE_OBJECT) + " #" + std::to_string(entry) + "-" + localeName(locale) + ": Failed to load from Database! \n");
                    return 0;
                }

                // Load English in all cases
                if (gameObjectDbInfo->GetCurLocale() != 1)
                    gameObjectDbInfo->LoadEntryData(expansion, 1);

                std::cout << "\n";
                msg_delay("\n");

                msg_delay("\n> WH Name: " + gameObjectInfo->GetName() + "\n");
                if (gameObjectInfo->GetCurLocale() != 1) msg_delay("\n> WH Name EN: " + gameObjectInfo->GetName(expansion, 1) + "\n");
                if (gameObjectDbInfo) msg_delay("\n> DB Name: " + gameObjectDbInfo->GetName() + "\n");
                if (gameObjectDbInfo->GetCurLocale() != 1) msg_delay("\n> DB Name EN: " + gameObjectDbInfo->GetName(expansion, 1) + "\n");

                msg_delay("\n> WH Pages:");
                const std::vector<PageEntry>& itemPages = gameObjectInfo->GetPages();
                for (uint8 i = 0; i < itemPages.size(); i++)
                {
                    std::stringstream pageStr;
                    pageStr << "\n> Page " << i + 1 << ": " << itemPages[i].text << "\n";
                    msg_delay(pageStr.str());
                }

                if (gameObjectInfo->GetCurLocale() != 1)
                {
                    msg_delay("\n> WH Pages EN:");
                    const std::vector<PageEntry>& itemEnPages = gameObjectInfo->GetPages(expansion, 1);
                    for (uint8 i = 0; i < itemEnPages.size(); i++)
                    {
                        std::stringstream pageStr;
                        pageStr << "\n> Page " << i + 1 << ": " << itemEnPages[i].text << "\n";
                        msg_delay(pageStr.str());
                    }
                }

                msg_delay("\n> DB Pages:");
                const std::vector<PageEntry>& itemDbPages = gameObjectDbInfo->GetPages();
                for (uint8 i = 0; i < itemDbPages.size(); i++)
                {
                    std::stringstream pageStr;
                    pageStr << "\n> Page " << i + 1 << ": " << itemDbPages[i].text << "\n";
                    msg_delay(pageStr.str());
                }

                if (gameObjectInfo->GetCurLocale() != 1)
                {
                    msg_delay("\n> DB Pages EN:");
                    const std::vector<PageEntry>& itemDbEnPages = gameObjectDbInfo->GetPages(expansion, 1);
                    for (uint8 i = 0; i < itemDbEnPages.size(); i++)
                    {
                        std::stringstream pageStr;
                        pageStr << "\n> Page " << i + 1 << ": " << itemDbEnPages[i].text << "\n";
                        msg_delay(pageStr.str());
                    }
                }
            }
            // Achievement
            if (type == 5)
            {
                // load all locales, set current for quick access
                WowheadAchievementInfo* achievementInfo = LoadWowheadAchievementInfo(entry, expansion, locale, true);
                if (!achievementInfo)
                {
                    msg_delay("> WH " + typeName(TYPE_ACHIEVEMENT) + " #" + std::to_string(entry) + "-" + localeName(locale) + ": Failed to load from Wowhead! \n");
                    return 0;
                }
                // Load English in all cases
                if (achievementInfo->GetCurLocale() != 1)
                    achievementInfo->LoadEntryData(expansion, 1);

                DatabaseAchievementInfo* achievementDbInfo = LoadDatabaseAchievementInfo(entry, expansion, locale, true);
                if (!achievementDbInfo)
                {
                    msg_delay("> DB " + typeName(TYPE_ACHIEVEMENT) + " #" + std::to_string(entry) + "-" + localeName(locale) + ": Failed to load from Database! \n");
                    return 0;
                }
                // Load English in all cases
                if (achievementDbInfo->GetCurLocale() != 1)
                    achievementDbInfo->LoadEntryData(expansion, 1);

                msg_delay("\n> WH Name: " + achievementInfo->GetName() + "\n");
                if (achievementInfo->GetCurLocale() != 1) msg_delay("\n> WH Name EN: " + achievementInfo->GetName(expansion, 1) + "\n");
                if (achievementDbInfo) msg_delay("\n> DB Name: " + achievementDbInfo->GetName() + "\n");
                if (achievementDbInfo->GetCurLocale() != 1) msg_delay("\n> DB Name EN: " + achievementDbInfo->GetName(expansion, 1) + "\n");
                msg_delay("\n> WH Description: " + achievementInfo->GetDescription() + "\n");
                if (achievementInfo->GetCurLocale() != 1) msg_delay("\n> WH Description EN: " + achievementInfo->GetDescription(expansion, 1) + "\n");
                if (achievementDbInfo) msg_delay("\n> DB Description: " + achievementDbInfo->GetDescription() + "\n");
                if (achievementDbInfo->GetCurLocale() != 1) msg_delay("\n> DB Description EN: " + achievementDbInfo->GetDescription(expansion, 1) + "\n");

            }
        }
        if (activity == 2) // cache multiple pages
        {
            // Choose parse type
            std::cout << "\nSelect parse type: \n";
            std::cout << "1) Quest \n";
            std::cout << "2) NPC \n";
            std::cout << "3) Item \n";
            std::cout << "4) Gameobject \n";
            std::cout << "5) Achievement \n";
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
            std::cout << "10) Portugues (LOC 8)\n";
            std::cout << "11) Italian (LOC 8)\n";
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

                    // skip tw as it does not exist on wh
                    if (i == 8)
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

                    // skip tw as it does not exist on wh
                    if (i == 8)
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
                    uint32 missingGenderTag = 0;
                    uint32 missingClassTag = 0;
                    uint32 missingRaceTag = 0;
                    uint32 missingNameTag = 0;

                    // write statistics details
                    std::string filesLocation = "stats/db/" + sDataMgr.getProjectName() + "/" + expansionName(e) + "/" + localeName(i) + "/" + typeName(TYPE_QUEST) + "/";
                    //std::string updateStmt = readFromFile(filesLocation + "missingEngTexts.txt");
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

                    // do not check english or tw here
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
                        if (IsSameString(qWhInfo->GetTitle(e, 1), qWhInfo->GetTitle(), i, true))
                        {
                            hasEngTitle++;
                            engQuestText += "title:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                        }

                        if (!qWhInfo->GetEndText(e, 1).empty() && qWhInfo->GetEndText().empty())
                        {
                            missingEndText++;
                            missingQuestText += "endText:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                        }
                        if (IsSameString(qWhInfo->GetEndText(e, 1), qWhInfo->GetEndText(), i,true))
                        {
                            hasEngEndText++;
                            engQuestText += "endText:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                        }
                        if (!qWhInfo->GetEndText(e, 1).empty() && !qWhInfo->GetEndText().empty())
                        {
                            std::string endText = qWhInfo->GetEndText();
                            std::string endTextExp = qWhInfo->GetEndText(e, 1);
                            if (hasNameTag(endTextExp, false, e, 1) && !hasNameTag(endText))
                            {
                                missingNameTag++;
                                missingTags += "name:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                            }
                            if (hasRaceTag(endTextExp, false, e, 1) && !hasRaceTag(endText))
                            {
                                missingRaceTag++;
                                missingTags += "race:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                            }
                            if (hasClassTag(endTextExp, false, e, 1) && !hasClassTag(endText))
                            {
                                missingClassTag++;
                                missingTags += "class:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                            }
                            if (hasGenderTag(endTextExp, false, e, 1) && !hasGenderTag(endText))
                            {
                                missingGenderTag++;
                                missingTags += "gender:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                            }
                        }

                        if (!qWhInfo->GetObjectives(e, 1).empty() && qWhInfo->GetObjectives().empty())
                        {
                            missingObjText++;
                            missingQuestText += "objective:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                        }
                        if (IsSameString(qWhInfo->GetObjectives(e, 1), qWhInfo->GetObjectives(), i, true))
                        {
                            hasEngObjText++;
                            engQuestText += "objective:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                        }
                        if (!qWhInfo->GetObjectives(e, 1).empty() && !qWhInfo->GetObjectives().empty())
                        {
                            std::string objectives = qWhInfo->GetObjectives();
                            std::string objectivesExp = qWhInfo->GetObjectives(e, 1);
                            if (hasNameTag(objectivesExp, false, e, 1) && !hasNameTag(objectives))
                            {
                                missingNameTag++;
                                missingTags += "name:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                            }
                            if (hasRaceTag(objectivesExp, false, e, 1) && !hasRaceTag(objectives))
                            {
                                missingRaceTag++;
                                missingTags += "race:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                            }
                            if (hasClassTag(objectivesExp, false, e, 1) && !hasClassTag(objectives))
                            {
                                missingClassTag++;
                                missingTags += "class:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                            }
                            if (hasGenderTag(objectivesExp, false, e, 1) && !hasClassTag(objectives))
                            {
                                missingGenderTag++;
                                missingTags += "gender:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                            }
                        }

                        if (!qWhInfo->GetRequestItemText(e, 1).empty() && qWhInfo->GetRequestItemText().empty())
                        {
                            missingReqText++;
                            missingQuestText += "reqItem:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                        }
                        if (IsSameString(qWhInfo->GetRequestItemText(e, 1), qWhInfo->GetRequestItemText(), i, true))
                        {
                            hasEngReqText++;
                            engQuestText += "reqItem:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                        }
                        if (!qWhInfo->GetRequestItemText(e, 1).empty() && !qWhInfo->GetRequestItemText().empty())
                        {
                            std::string requestItemText = qWhInfo->GetRequestItemText();
                            std::string requestItemTextExp = qWhInfo->GetRequestItemText(e, 1);
                            if (hasNameTag(requestItemTextExp, false, e, 1) && !hasNameTag(requestItemText))
                            {
                                missingNameTag++;
                                missingTags += "name:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                            }
                            if (hasRaceTag(requestItemTextExp, false, e, 1) && !hasRaceTag(requestItemText))
                            {
                                missingRaceTag++;
                                missingTags += "race:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                            }
                            if (hasClassTag(requestItemTextExp, false, e, 1) && !hasClassTag(requestItemText))
                            {
                                missingClassTag++;
                                missingTags += "class:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                            }
                            if (hasGenderTag(requestItemTextExp, false, e, 1) && !hasGenderTag(requestItemText))
                            {
                                missingGenderTag++;
                                missingTags += "gender:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                            }
                        }

                        if (!qWhInfo->GetOfferRewardText(e, 1).empty() && qWhInfo->GetOfferRewardText().empty())
                        {
                            missingOfferText++;
                            missingQuestText += "offer:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                        }
                        if (IsSameString(qWhInfo->GetOfferRewardText(e, 1), qWhInfo->GetOfferRewardText(), i, true))
                        {
                            hasEngOfferText++;
                            engQuestText += "offer:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                        }
                        if (!qWhInfo->GetOfferRewardText(e, 1).empty() && !qWhInfo->GetOfferRewardText().empty())
                        {
                            std::string offerRewardText = qWhInfo->GetOfferRewardText();
                            std::string offerRewardTextExp = qWhInfo->GetOfferRewardText(e, 1);
                            if (hasNameTag(offerRewardTextExp, false, e, 1) && !hasNameTag(offerRewardText))
                            {
                                missingNameTag++;
                                missingTags += "name:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                            }
                            if (hasRaceTag(offerRewardTextExp, false, e, 1) && !hasRaceTag(offerRewardText))
                            {
                                missingRaceTag++;
                                missingTags += "race:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                            }
                            if (hasClassTag(offerRewardTextExp, false, e, 1) && !hasClassTag(offerRewardText))
                            {
                                missingClassTag++;
                                missingTags += "class:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                            }
                            if (hasGenderTag(offerRewardTextExp, false, e, 1) && !hasGenderTag(offerRewardText))
                            {
                                missingGenderTag++;
                                missingTags += "gender:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                            }
                        }

                        if (!qWhInfo->GetDetails(e, 1).empty() && qWhInfo->GetDetails().empty())
                        {
                            missingDetails++;
                            missingQuestText += "details:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                        }
                        if (IsSameString(qWhInfo->GetDetails(e, 1), qWhInfo->GetDetails(), i, true))
                        {
                            hasEngDetails++;
                            engQuestText += "details:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                        }
                        if (!qWhInfo->GetDetails(e, 1).empty() && !qWhInfo->GetDetails().empty())
                        {
                            std::string details = qWhInfo->GetDetails();
                            std::string detailsExp = qWhInfo->GetDetails(e, 1);
                            if (hasNameTag(detailsExp, false, e, 1) && !hasNameTag(details))
                            {
                                missingNameTag++;
                                missingTags += "name:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                            }
                            if (hasRaceTag(detailsExp, false, e, 1) && !hasRaceTag(details))
                            {
                                missingRaceTag++;
                                missingTags += "race:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                            }
                            if (hasClassTag(detailsExp, false, e, 1) && !hasClassTag(details))
                            {
                                missingClassTag++;
                                missingTags += "class:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                            }
                            if (hasGenderTag(detailsExp, false, e, 1) && !hasGenderTag(details))
                            {
                                missingGenderTag++;
                                missingTags += "gender:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                            }
                        }

                        // Objective 1-4
                        if (!qWhInfo->GetObjective(0, e, 1).empty() && qWhInfo->GetObjective(0).empty())
                        {
                            missingObjective1++;
                            missingQuestText += "obj1:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                        }
                        if (IsSameString(qWhInfo->GetObjective(0, e, 1), qWhInfo->GetObjective(0), i, true))
                        {
                            hasEngObjective1++;
                            engQuestText += "obj1:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                        }

                        if (!qWhInfo->GetObjective(1, e, 1).empty() && qWhInfo->GetObjective(1).empty())
                        {
                            missingObjective2++;
                            missingQuestText += "obj2:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                        }
                        if (IsSameString(qWhInfo->GetObjective(1, e, 1), qWhInfo->GetObjective(1), i, true))
                        {
                            hasEngObjective2++;
                            engQuestText += "obj2:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                        }

                        if (!qWhInfo->GetObjective(2, e, 1).empty() && qWhInfo->GetObjective(2).empty())
                        {
                            missingObjective3++;
                            missingQuestText += "obj3:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                        }
                        if (IsSameString(qWhInfo->GetObjective(2, e, 1), qWhInfo->GetObjective(2), i, true))
                        {
                            hasEngObjective3++;
                            engQuestText += "obj3:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                        }

                        if (!qWhInfo->GetObjective(3, e, 1).empty() && qWhInfo->GetObjective(3).empty())
                        {
                            missingObjective4++;
                            missingQuestText += "obj4:" + std::to_string(qWhInfo->GetEntry()) + "\n";
                        }
                        if (IsSameString(qWhInfo->GetObjective(3, e, 1), qWhInfo->GetObjective(3), i, true))
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

                        if (missingNameTag)
                            msg_delay(">   Missing Name Tag: %d \n", missingNameTag);
                        if (missingRaceTag)
                            msg_delay(">   Missing Race Tag: %d \n", missingRaceTag);
                        if (missingClassTag)
                            msg_delay(">   Missing Class Tag: %d \n", missingClassTag);
                        if (missingGenderTag)
                            msg_delay(">   Missing Gender Tag: %d \n", missingGenderTag);

                        // write stats to file
                        if (!missingQuestText.empty())
                            writeToFile(missingQuestText.c_str(), "missingQuestText.txt", filesLocation);
                        if (!engQuestText.empty())
                            writeToFile(engQuestText.c_str(), "engQuestText.txt", filesLocation);
                        if (!missingTags.empty())
                            writeToFile(missingTags.c_str(), "missingTags.txt", filesLocation);
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
                    uint32 missingGenderTag = 0;
                    uint32 missingRaceTag = 0;
                    uint32 missingNameTag = 0;

                    // write statistics details
                    std::string filesLocation = "stats/db/" + sDataMgr.getProjectName() + "/" + expansionName(e) + "/" + localeName(i) + "/" + typeName(TYPE_QUEST) + "/";
                    //std::string updateStmt = readFromFile(filesLocation + "missingEngTexts.txt");
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
                        if (IsSameString(qDbInfo->GetTitle(e, 1), qDbInfo->GetTitle()))
                        {
                            hasEngTitle++;
                            engQuestText += "title:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                        }

                        if (!qDbInfo->GetEndText(e, 1).empty() && qDbInfo->GetEndText().empty())
                        {
                            missingEndText++;
                            missingQuestText += "endText:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                        }
                        if (IsSameString(qDbInfo->GetEndText(e, 1), qDbInfo->GetEndText()))
                        {
                            hasEngEndText++;
                            engQuestText += "endText:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                        }
                        if (!qDbInfo->GetEndText(e, 1).empty() && !qDbInfo->GetEndText().empty())
                        {
                            std::string endText = qDbInfo->GetEndText();
                            std::string endTextExp = qDbInfo->GetEndText(e, 1);
                            if (hasNameTag(endTextExp, false, e, 1) && !hasNameTag(endText))
                            {
                                missingNameTag++;
                                missingTags += "name:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                            }
                            if (hasRaceTag(endTextExp, false, e, 1) && !hasRaceTag(endText))
                            {
                                missingRaceTag++;
                                missingTags += "race:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                            }
                            if (hasClassTag(endTextExp, false, e, 1) && !hasClassTag(endText))
                            {
                                missingClassTag++;
                                missingTags += "class:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                            }
                            if (hasGenderTag(endTextExp, false, e, 1) && !hasGenderTag(endText))
                            {
                                missingGenderTag++;
                                missingTags += "gender:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                            }
                        }

                        if (!qDbInfo->GetObjectives(e, 1).empty() && qDbInfo->GetObjectives().empty())
                        {
                            missingObjText++;
                            missingQuestText += "objective:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                        }

                        if (IsSameString(qDbInfo->GetObjectives(e, 1), qDbInfo->GetObjectives()))
                        {
                            hasEngObjText++;
                            engQuestText += "objective:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                        }
                        if (!qDbInfo->GetObjectives(e, 1).empty() && !qDbInfo->GetObjectives().empty())
                        {
                            std::string objectives = qDbInfo->GetObjectives();
                            std::string objectivesExp = qDbInfo->GetObjectives(e, 1);
                            if (hasNameTag(objectivesExp, false, e, 1) && !hasNameTag(objectives))
                            {
                                missingNameTag++;
                                missingTags += "name:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                            }
                            if (hasRaceTag(objectivesExp, false, e, 1) && !hasRaceTag(objectives))
                            {
                                missingRaceTag++;
                                missingTags += "race:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                            }
                            if (hasClassTag(objectivesExp, false, e, 1) && !hasClassTag(objectives))
                            {
                                missingClassTag++;
                                missingTags += "class:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                            }
                            if (hasGenderTag(objectivesExp, false, e, 1) && !hasClassTag(objectives))
                            {
                                missingGenderTag++;
                                missingTags += "gender:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                            }
                        }

                        if (!qDbInfo->GetRequestItemText(e, 1).empty() && qDbInfo->GetRequestItemText().empty())
                        {
                            missingReqText++;
                            missingQuestText += "reqItem:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                        }
                        if (IsSameString(qDbInfo->GetRequestItemText(e, 1), qDbInfo->GetRequestItemText()))
                        {
                            hasEngReqText++;
                            engQuestText += "reqItem:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                        }
                        if (!qDbInfo->GetRequestItemText(e, 1).empty() && !qDbInfo->GetRequestItemText().empty())
                        {
                            std::string requestItemText = qDbInfo->GetRequestItemText();
                            std::string requestItemTextExp = qDbInfo->GetRequestItemText(e, 1);
                            if (hasNameTag(requestItemTextExp, false, e, 1) && !hasNameTag(requestItemText))
                            {
                                missingNameTag++;
                                missingTags += "name:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                            }
                            if (hasRaceTag(requestItemTextExp, false, e, 1) && !hasRaceTag(requestItemText))
                            {
                                missingRaceTag++;
                                missingTags += "race:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                            }
                            if (hasClassTag(requestItemTextExp, false, e, 1) && !hasClassTag(requestItemText))
                            {
                                missingClassTag++;
                                missingTags += "class:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                            }
                            if (hasGenderTag(requestItemTextExp, false, e, 1) && !hasGenderTag(requestItemText))
                            {
                                missingGenderTag++;
                                missingTags += "gender:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                            }
                        }

                        if (!qDbInfo->GetOfferRewardText(e, 1).empty() && qDbInfo->GetOfferRewardText().empty())
                        {
                            missingOfferText++;
                            missingQuestText += "offer:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                        }
                        if (IsSameString(qDbInfo->GetOfferRewardText(e, 1), qDbInfo->GetOfferRewardText()))
                        {
                            hasEngOfferText++;
                            engQuestText += "offer:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                        }
                        if (!qDbInfo->GetOfferRewardText(e, 1).empty() && !qDbInfo->GetOfferRewardText().empty())
                        {
                            std::string offerRewardText = qDbInfo->GetOfferRewardText();
                            std::string offerRewardTextExp = qDbInfo->GetOfferRewardText(e, 1);
                            if (hasNameTag(offerRewardTextExp, false, e, 1) && !hasNameTag(offerRewardText))
                            {
                                missingNameTag++;
                                missingTags += "name:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                            }
                            if (hasRaceTag(offerRewardTextExp, false, e, 1) && !hasRaceTag(offerRewardText))
                            {
                                missingRaceTag++;
                                missingTags += "race:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                            }
                            if (hasClassTag(offerRewardTextExp, false, e, 1) && !hasClassTag(offerRewardText))
                            {
                                missingClassTag++;
                                missingTags += "class:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                            }
                            if (hasGenderTag(offerRewardTextExp, false, e, 1) && !hasGenderTag(offerRewardText))
                            {
                                missingGenderTag++;
                                missingTags += "gender:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                            }
                        }

                        if (!qDbInfo->GetDetails(e, 1).empty() && qDbInfo->GetDetails().empty())
                        {
                            missingDetails++;
                            missingQuestText += "details:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                        }
                        if (IsSameString(qDbInfo->GetDetails(e, 1), qDbInfo->GetDetails()))
                        {
                            hasEngDetails++;
                            engQuestText += "details:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                        }
                        if (!qDbInfo->GetDetails(e, 1).empty() && !qDbInfo->GetDetails().empty())
                        {
                            std::string details = qDbInfo->GetDetails();
                            std::string detailsExp = qDbInfo->GetDetails(e, 1);
                            if (hasNameTag(detailsExp, false, e, 1) && !hasNameTag(details))
                            {
                                missingNameTag++;
                                missingTags += "name:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                            }
                            if (hasRaceTag(detailsExp, false, e, 1) && !hasRaceTag(details))
                            {
                                missingRaceTag++;
                                missingTags += "race:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                            }
                            if (hasClassTag(detailsExp, false, e, 1) && !hasClassTag(details))
                            {
                                missingClassTag++;
                                missingTags += "class:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                            }
                            if (hasGenderTag(detailsExp, false, e, 1) && !hasGenderTag(details))
                            {
                                missingGenderTag++;
                                missingTags += "gender:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                            }
                        }

                        // Objective 1-4
                        if (!qDbInfo->GetObjective(0, e, 1).empty() && qDbInfo->GetObjective(0).empty())
                        {
                            missingObjective1++;
                            missingQuestText += "obj1:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                        }
                        if (IsSameString(qDbInfo->GetObjective(0, e, 1), qDbInfo->GetObjective(0)))
                        {
                            hasEngObjective1++;
                            engQuestText += "obj1:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                        }

                        if (!qDbInfo->GetObjective(1, e, 1).empty() && qDbInfo->GetObjective(1).empty())
                        {
                            missingObjective2++;
                            missingQuestText += "obj2:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                        }
                        if (IsSameString(qDbInfo->GetObjective(1, e, 1), qDbInfo->GetObjective(1)))
                        {
                            hasEngObjective2++;
                            engQuestText += "obj2:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                        }

                        if (!qDbInfo->GetObjective(2, e, 1).empty() && qDbInfo->GetObjective(2).empty())
                        {
                            missingObjective3++;
                            missingQuestText += "obj3:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                        }
                        if (IsSameString(qDbInfo->GetObjective(2, e, 1), qDbInfo->GetObjective(2)))
                        {
                            hasEngObjective3++;
                            engQuestText += "obj3:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                        }

                        if (!qDbInfo->GetObjective(3, e, 1).empty() && qDbInfo->GetObjective(3).empty())
                        {
                            missingObjective4++;
                            missingQuestText += "obj4:" + std::to_string(qDbInfo->GetEntry()) + "\n";
                        }
                        if (IsSameString(qDbInfo->GetObjective(3, e, 1), qDbInfo->GetObjective(3)))
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

                        if (missingNameTag)
                            msg_delay(">   Missing Name Tag: %d \n", missingNameTag);
                        if (missingRaceTag)
                            msg_delay(">   Missing Race Tag: %d \n", missingRaceTag);
                        if (missingClassTag)
                            msg_delay(">   Missing Class Tag: %d \n", missingClassTag);
                        if (missingGenderTag)
                            msg_delay(">   Missing Gender Tag: %d \n", missingGenderTag);

                        if (missingQuests)
                            msg_delay("\n>   Missing Quests Locales: %d \n", missingQuests);

                        // write stats to file
                        if (!missingQuestText.empty())
                            writeToFile(missingQuestText.c_str(), "missingQuestText.txt", filesLocation);
                        if (!engQuestText.empty())
                            writeToFile(engQuestText.c_str(), "engQuestText.txt", filesLocation);
                        if (!missingTags.empty())
                            writeToFile(missingTags.c_str(), "missingTags.txt", filesLocation);
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
            std::cout << "2) Add and/or Update locales from WowHead (Skip english texts) \n";
            std::cout << "3) Replace <name>, <class>, <race> and <gender> with $n, $c, $r and $g. Also |3-X() in ru classic \n";
            std::cout << "4) Auto add missing locales from other expansions (if English is same) \n";
            std::cout << "5) TODO \n";
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
                    std::string filesLocation = "work/" + sDataMgr.getProjectName() + "/" + expansionName(expansion) + "/" + typeName(TYPE_QUEST) + "/";
                    //std::string updateStmt = readFromFile(filesLocation + "missingEngTexts.txt");
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
                                msg_nodelay(std::to_string(counter));
                            else if ((counter % 100) == 0)
                                msg_nodelay(".");

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
                    if (!updateQueries.empty())
                        writeToFile(updateQueries.c_str(), "missingEngTexts.sql", filesLocation);
                }
                // ACTION 1 END
                return 1;
            }
            // Auto add missing translations and update different
            if (action == 2)
            {
                uint32 updateVersionOption = 0;
                std::cout << "\nSelect what to do when Wowhead version is different than local: \n";
                std::cout << "1) Use Wowhead version \n";
                std::cout << "2) Ask for every difference found \n";
                std::cout << "3) Keep local version \n";
                std::cin >> updateVersionOption;

                const bool ignoreDifferentVersion = updateVersionOption >= 3;
                const bool autoUpdateWowheadVersion = updateVersionOption == 1;

                uint32 checkLocalesOption = 0;
                std::cout << "\nSelect what locales to check: \n";
                std::cout << "1) All \n";
                std::cout << "2) Quests \n";
                std::cout << "3) Items \n";
                std::cout << "4) Game Objects \n";
                std::cout << "5) Achievements \n";
                std::cin >> checkLocalesOption;

                const bool checkQuests = (checkLocalesOption == 1) || (checkLocalesOption == 2);
                const bool checkItems = (checkLocalesOption == 1) || (checkLocalesOption == 3);
                const bool checkGameObjects = (checkLocalesOption == 1) || (checkLocalesOption == 4);
                const bool checkAchievements = (checkLocalesOption == 1) || (checkLocalesOption == 5);

                // print wh cache stats
                msg_delay("\n");
                msg_delay("> WH: \n");

                int maxQuestId[MAX_EXPANSION];
                maxQuestId[0] = 0;
                maxQuestId[1] = 0;
                maxQuestId[2] = 0;

                int maxItemId[MAX_EXPANSION];
                maxItemId[0] = 0;
                maxItemId[1] = 0;
                maxItemId[2] = 0;

                int maxGameObjectId[MAX_EXPANSION];
                maxGameObjectId[0] = 0;
                maxGameObjectId[1] = 0;
                maxGameObjectId[2] = 0;

                int maxAchievementId[MAX_EXPANSION];
                maxAchievementId[0] = 0;
                maxAchievementId[1] = 0;
                maxAchievementId[2] = 0;

                uint32 iterations = 0;
                uint32 totalIterations = 0;

                for (auto e = 1; e <= MAX_EXPANSION; ++e) // expansion
                {
                    if (expansion && expansion != e)
                        continue;

                    msg_delay("\n> %s \n", expansionName(e).c_str());
                    for (auto i = 1; i <= MAX_LOCALE; ++i) // locales
                    {
                        // use CN for TW from wowhead
                        uint32 localeId = locale;
                        if (localeId == 8)
                            localeId = 5;

                        if (localeId && localeId != i && i != 1)
                            continue;

                        if(checkQuests)
                        {
                            std::string cacheQuestLocation = "cache/" + expansionName(e) + "/" + localeName(i) + "/quest/";
                            if (std::filesystem::is_directory(cacheQuestLocation))
                            {
                                auto questCounter = 0;
                                auto questDirIter = std::filesystem::directory_iterator(cacheQuestLocation);
                                for (auto& fl : questDirIter)
                                {
                                    if (fl.is_regular_file())
                                    {
                                        ++questCounter;
                                        const std::filesystem::path& p(fl.path());
                                        uint32 qID = stoi(p.stem().string());
                                        if ((int)qID > maxQuestId[e - 1])
                                            maxQuestId[e - 1] = (int)qID;
                                    }
                                }
                                msg_delay(">  Quests %s total: %d, max id: %d \n", localeName(i).c_str(), questCounter, maxQuestId[e - 1]);
                                
                                if (localeId == i)
                                {
                                    totalIterations += questCounter;
                                }
                            }
                        }

                        if(checkItems)
                        {
                            std::string cacheItemLocation = "cache/" + expansionName(e) + "/" + localeName(i) + "/item/";
                            if (std::filesystem::is_directory(cacheItemLocation))
                            {
                                auto itemCounter = 0;
                                auto itemDirIter = std::filesystem::directory_iterator(cacheItemLocation);
                                for (auto& fl : itemDirIter)
                                {
                                    if (fl.is_regular_file())
                                    {
                                        ++itemCounter;
                                        const std::filesystem::path& p(fl.path());
                                        uint32 itemID = stoi(p.stem().string());
                                        if ((int)itemID > maxItemId[e - 1])
                                            maxItemId[e - 1] = (int)itemID;
                                    }
                                }
                                msg_delay(">  Items %s total: %d, max id: %d \n", localeName(i).c_str(), itemCounter, maxItemId[e - 1]);
                            
                                if (localeId == i)
                                {
                                    totalIterations += itemCounter;
                                }
                            }
                        }

                        if (checkGameObjects)
                        {
                            std::string cacheGameObjectLocation = "cache/" + expansionName(e) + "/" + localeName(i) + "/object/";
                            if (std::filesystem::is_directory(cacheGameObjectLocation))
                            {
                                auto gameObjectCounter = 0;
                                auto itemDirIter = std::filesystem::directory_iterator(cacheGameObjectLocation);
                                for (auto& fl : itemDirIter)
                                {
                                    if (fl.is_regular_file())
                                    {
                                        ++gameObjectCounter;
                                        const std::filesystem::path& p(fl.path());
                                        uint32 gameObjectID = stoi(p.stem().string());
                                        if ((int)gameObjectID > maxItemId[e - 1])
                                            maxGameObjectId[e - 1] = (int)gameObjectID;
                                    }
                                }
                                msg_delay(">  Game Objects %s total: %d, max id: %d \n", localeName(i).c_str(), gameObjectCounter, maxItemId[e - 1]);

                                if (localeId == i)
                                {
                                    totalIterations += gameObjectCounter;
                                }
                            }
                        }

                        if (checkAchievements)
                        {
                            std::string cacheAchievementLocation = "cache/" + expansionName(e) + "/" + localeName(i) + "/achievement/";
                            if (std::filesystem::is_directory(cacheAchievementLocation))
                            {
                                auto achievementCounter = 0;
                                auto itemDirIter = std::filesystem::directory_iterator(cacheAchievementLocation);
                                for (auto& fl : itemDirIter)
                                {
                                    if (fl.is_regular_file())
                                    {
                                        ++achievementCounter;
                                        const std::filesystem::path& p(fl.path());
                                        uint32 achievementID = stoi(p.stem().string());
                                        if ((int)achievementID > maxAchievementId[e - 1])
                                            maxAchievementId[e - 1] = (int)achievementID;
                                    }
                                }
                                msg_delay(">  Achievements %s total: %d, max id: %d \n", localeName(i).c_str(), achievementCounter, maxAchievementId[e - 1]);

                                if (localeId == i)
                                {
                                    totalIterations += achievementCounter;
                                }
                            }
                        }
                    }
                }

                maxQuestId[0] = 0;
                maxQuestId[1] = 0;
                maxQuestId[2] = 0;

                maxItemId[0] = 0;
                maxItemId[1] = 0;
                maxItemId[2] = 0;

                maxGameObjectId[0] = 0;
                maxGameObjectId[1] = 0;
                maxGameObjectId[2] = 0;

                maxAchievementId[0] = 0;
                maxAchievementId[1] = 0;
                maxAchievementId[2] = 0;

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

                    if(checkQuests)
                    {
                        int questCounter = 0;
                        DbConnect->GetDbInt("SELECT COUNT(*) FROM quest_template", questCounter);
                        DbConnect->GetDbInt("SELECT MAX(entry) FROM quest_template", maxQuestId[e - 1]);
                        msg_delay(">  Quests %s total: %d, max id: %d \n", localeName(1).c_str(), questCounter, maxQuestId[e - 1]);

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

                    if(checkItems)
                    {
                        int itemCounter = 0;
                        DbConnect->GetDbInt("SELECT COUNT(*) FROM item_template", itemCounter);
                        DbConnect->GetDbInt("SELECT MAX(entry) FROM item_template", maxItemId[e - 1]);
                        msg_delay(">  Items %s total: %d, max id: %d \n", localeName(1).c_str(), itemCounter, maxItemId[e - 1]);
                    }

                    if (checkGameObjects)
                    {
                        int gameObjectCounter = 0;
                        DbConnect->GetDbInt("SELECT COUNT(*) FROM gameobject_template", gameObjectCounter);
                        DbConnect->GetDbInt("SELECT MAX(entry) FROM gameobject_template", maxGameObjectId[e - 1]);
                        msg_delay(">  Game Objects %s total: %d, max id: %d \n", localeName(1).c_str(), gameObjectCounter, maxGameObjectId[e - 1]);
                    }

                    if (checkAchievements)
                    {
                        int achievementsCounter = 0;
                        DbConnect->GetDbInt("SELECT COUNT(*) FROM achievement_dbc", achievementsCounter);
                        DbConnect->GetDbInt("SELECT MAX(ID) FROM achievement_dbc", maxAchievementId[e - 1]);
                        msg_delay(">  Achievements %s total: %d, max id: %d \n", localeName(1).c_str(), achievementsCounter, maxAchievementId[e - 1]);
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

                    msg_delay("\n>  %s\n", localeName(locale).c_str());

                    uint32 missingQuestTitle = 0;
                    uint32 missingQuestEndText = 0;
                    uint32 missingQuestObjText = 0;
                    uint32 missingQuestReqText = 0;
                    uint32 missingQuestOfferText = 0;
                    uint32 missingQuestDetails = 0;
                    uint32 missingQuestObjective1 = 0;
                    uint32 missingQuestObjective2 = 0;
                    uint32 missingQuestObjective3 = 0;
                    uint32 missingQuestObjective4 = 0;

                    uint32 updatedQuestTitle = 0;
                    uint32 updatedQuestEndText = 0;
                    uint32 updatedQuestObjText = 0;
                    uint32 updatedQuestReqText = 0;
                    uint32 updatedQuestOfferText = 0;
                    uint32 updatedQuestDetails = 0;
                    uint32 updatedQuestObjective1 = 0;
                    uint32 updatedQuestObjective2 = 0;
                    uint32 updatedQuestObjective3 = 0;
                    uint32 updatedQuestObjective4 = 0;

                    uint32 missingDbQuest = 0; // wh exist, db not exist

                    uint32 missingItemName = 0;
                    uint32 missingItemDescription = 0;
                    uint32 missingItemPageText = 0;

                    uint32 updatedItemName = 0;
                    uint32 updatedItemDescription = 0;
                    uint32 updatedItemPageText = 0;

                    uint32 missingDbItem = 0; // wh exist, db not exist

                    uint32 missingGameObjectName = 0;
                    uint32 missingGameObjectPageText = 0;

                    uint32 updatedGameObjectName = 0;
                    uint32 updatedGameObjectPageText = 0;

                    uint32 missingDbGameObject = 0; // wh exist, db not exist

                    uint32 missingAchievementName = 0;
                    uint32 missingAchievementDescription = 0;
                    uint32 missingAchievementCriteriaDescription = 0;

                    uint32 updatedAchievementName = 0;
                    uint32 updatedAchievementDescription = 0;
                    uint32 updatedAchievementCriteriaDescription = 0;

                    uint32 missingDbAchievement = 0; // wh exist, db not exist

                    // Quests
                    if(checkQuests)
                    {
                        // use CN for TW from wowhead
                        uint32 localeId = locale;
                        if (localeId == 8)
                            localeId = 5;

                        std::string cacheEngLocation = "cache/" + expansionName(e) + "/" + localeName(1) + "/" + typeName(TYPE_QUEST) + "/";
                        std::string cacheLocLocation = "cache/" + expansionName(e) + "/" + localeName(localeId) + "/" + typeName(TYPE_QUEST) + "/";
                        if (!std::filesystem::is_directory(cacheEngLocation))
                            continue;
                        if (!std::filesystem::is_directory(cacheLocLocation))
                            continue;

                        // read existing update file
                        std::string filesLocation = "work/" + sDataMgr.getProjectName() + "/" + expansionName(expansion) + "/" + typeName(TYPE_QUEST) + "/";
                        //std::string updateStmt = readFromFile(filesLocation + "missingEngTexts.txt");
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

                                // load both eng and locale (later)
                                WowheadQuestInfo* qWhEngInfo = nullptr;//LoadWowheadQuestInfo(qID, e, 1, true);
                                WowheadQuestInfo* qWhLocInfo = nullptr;//LoadWowheadQuestInfo(qID, e, localeId, true);

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

                                iterations++;
                                if ((iterations % 10) == 0)
                                {
                                    std::stringstream message;
                                    uint32 iterationPct = (iterations * 100) / totalIterations;
                                    message << "   " << std::to_string(iterationPct) << "%c (" << std::to_string(iterations) << "/" + std::to_string(totalIterations) << ")\n";
                                    msg_nodelay(message.str(), '\u0025');
                                }

                                // use database english as proof that text should exist
                                // TODO fix wowhead parsing of quests that have no details

                                if (!qDbLocInfo->GetTitle(e, 1).empty() && (qDbLocInfo->GetTitle().empty() || !ignoreDifferentVersion))
                                {
                                    if (qWhEngInfo == nullptr)
                                    {
                                        qWhEngInfo = LoadWowheadQuestInfo(qID, e, 1, true);
                                        if (!qWhEngInfo)
                                            continue;
                                    }
                                    if (!qWhLocInfo)
                                    {
                                        qWhLocInfo = LoadWowheadQuestInfo(qID, e, localeId, true);
                                        if (!qWhLocInfo)
                                            continue;
                                    }

                                    if (!qWhLocInfo->GetTitle().empty())
                                    {
                                        if (qDbLocInfo->GetTitle().empty())
                                        {
                                            if (!IsSameString(qWhLocInfo->GetTitle(e, 1), qWhLocInfo->GetTitle(), locale, true))
                                            {
                                                missingQuestTitle++;
                                                updateQueries += updateQuestFromWhQuery(qWhLocInfo, qDbLocInfo, "title", e, locale) + "\n";
                                            }
                                        }
                                        else if (!ignoreDifferentVersion)
                                        {
                                            if (!IsSameString(qWhLocInfo->GetTitle(), qDbLocInfo->GetTitle(), locale, true))
                                            {
                                                const std::string message = "Quest (" + std::to_string(qWhLocInfo->GetEntry()) + ") Title";
                                                if (autoUpdateWowheadVersion || ShouldOverrideDifference(message, qWhLocInfo->GetTitle(), qDbLocInfo->GetTitle()))
                                                {
                                                    updatedQuestTitle++;
                                                    updateQueries += updateQuestFromWhQuery(qWhLocInfo, qDbLocInfo, "title", e, locale) + "\n";
                                                }
                                            }
                                        }
                                    }
                                }

                                if (!qDbLocInfo->GetEndText(e, 1).empty() && (qDbLocInfo->GetEndText().empty() || !ignoreDifferentVersion))
                                {
                                    if (!qWhEngInfo)
                                    {
                                        qWhEngInfo = LoadWowheadQuestInfo(qID, e, 1, true);
                                        if (!qWhEngInfo)
                                            continue;
                                    }
                                    if (!qWhLocInfo)
                                    {
                                        qWhLocInfo = LoadWowheadQuestInfo(qID, e, localeId, true);
                                        if (!qWhLocInfo)
                                            continue;
                                    }

                                    if (!qWhLocInfo->GetEndText().empty())
                                    {
                                        if (qDbLocInfo->GetEndText().empty())
                                        {
                                            if (!IsSameString(qWhLocInfo->GetEndText(e, 1), qWhLocInfo->GetEndText(), locale, true))
                                            {
                                                missingQuestEndText++;
                                                updateQueries += updateQuestFromWhQuery(qWhLocInfo, qDbLocInfo, "endText", e, locale) + "\n";
                                            }
                                        }
                                        else if (!ignoreDifferentVersion)
                                        {
                                            if (!IsSameString(qWhLocInfo->GetEndText(), qDbLocInfo->GetEndText(), locale, true))
                                            {
                                                const std::string message = "Quest (" + std::to_string(qWhLocInfo->GetEntry()) + ") End Text";
                                                if (autoUpdateWowheadVersion || ShouldOverrideDifference(message, qWhLocInfo->GetEndText(), qDbLocInfo->GetEndText()))
                                                {
                                                    updatedQuestEndText++;
                                                    updateQueries += updateQuestFromWhQuery(qWhLocInfo, qDbLocInfo, "endText", e, locale) + "\n";
                                                }
                                            }
                                        }
                                    }
                                }

                                if (!qDbLocInfo->GetObjectives(e, 1).empty() && (qDbLocInfo->GetObjectives().empty() || !ignoreDifferentVersion))
                                {
                                    if (!qWhEngInfo)
                                    {
                                        qWhEngInfo = LoadWowheadQuestInfo(qID, e, 1, true);
                                        if (!qWhEngInfo)
                                            continue;
                                    }
                                    if (!qWhLocInfo)
                                    {
                                        qWhLocInfo = LoadWowheadQuestInfo(qID, e, localeId, true);
                                        if (!qWhLocInfo)
                                            continue;
                                    }

                                    if (!qWhLocInfo->GetObjectives().empty())
                                    {
                                        if (qDbLocInfo->GetObjectives().empty())
                                        {
                                            if (!IsSameString(qWhLocInfo->GetObjectives(e, 1), qWhLocInfo->GetObjectives(), locale, true))
                                            {
                                                missingQuestObjText++;
                                                updateQueries += updateQuestFromWhQuery(qWhLocInfo, qDbLocInfo, "objectives", e, locale) + "\n";
                                            }
                                        }
                                        else if (!ignoreDifferentVersion)
                                        {
                                            if (!IsSameString(qWhLocInfo->GetObjectives(), qDbLocInfo->GetObjectives(), locale, true))
                                            {
                                                const std::string message = "Quest (" + std::to_string(qWhLocInfo->GetEntry()) + ") Objectives";
                                                if (autoUpdateWowheadVersion || ShouldOverrideDifference(message, qWhLocInfo->GetObjectives(), qDbLocInfo->GetObjectives()))
                                                {
                                                    updatedQuestObjText++;
                                                    updateQueries += updateQuestFromWhQuery(qWhLocInfo, qDbLocInfo, "objectives", e, locale) + "\n";
                                                }
                                            }
                                        }
                                    }
                                }

                                if (!qDbLocInfo->GetRequestItemText(e, 1).empty() && (qDbLocInfo->GetRequestItemText().empty() || !ignoreDifferentVersion))
                                {
                                    if (!qWhEngInfo)
                                    {
                                        qWhEngInfo = LoadWowheadQuestInfo(qID, e, 1, true);
                                        if (!qWhEngInfo)
                                            continue;
                                    }
                                    if (!qWhLocInfo)
                                    {
                                        qWhLocInfo = LoadWowheadQuestInfo(qID, e, localeId, true);
                                        if (!qWhLocInfo)
                                            continue;
                                    }

                                    if (!qWhLocInfo->GetRequestItemText().empty())
                                    {
                                        if (qDbLocInfo->GetRequestItemText().empty())
                                        {
                                            if (!IsSameString(qWhLocInfo->GetRequestItemText(e, 1), qWhLocInfo->GetRequestItemText(), locale, true))
                                            {
                                                missingQuestReqText++;
                                                updateQueries += updateQuestFromWhQuery(qWhLocInfo, qDbLocInfo, "requestItemText", e, locale) + "\n";
                                            }
                                        }
                                        else if (!ignoreDifferentVersion)
                                        {
                                            if (!IsSameString(qWhLocInfo->GetRequestItemText(), qDbLocInfo->GetRequestItemText(), locale, true))
                                            {
                                                const std::string message = "Quest (" + std::to_string(qWhLocInfo->GetEntry()) + ") requestItemText";
                                                if (autoUpdateWowheadVersion || ShouldOverrideDifference(message, qWhLocInfo->GetRequestItemText(), qDbLocInfo->GetRequestItemText()))
                                                {
                                                    updatedQuestReqText++;
                                                    updateQueries += updateQuestFromWhQuery(qWhLocInfo, qDbLocInfo, "requestItemText", e, locale) + "\n";
                                                }
                                            }
                                        }
                                    }
                                }

                                if (!qDbLocInfo->GetOfferRewardText(e, 1).empty() && (qDbLocInfo->GetOfferRewardText().empty() || !ignoreDifferentVersion))
                                {
                                    if (!qWhEngInfo)
                                    {
                                        qWhEngInfo = LoadWowheadQuestInfo(qID, e, 1, true);
                                        if (!qWhEngInfo)
                                            continue;
                                    }
                                    if (!qWhLocInfo)
                                    {
                                        qWhLocInfo = LoadWowheadQuestInfo(qID, e, localeId, true);
                                        if (!qWhLocInfo)
                                            continue;
                                    }

                                    if (!qWhLocInfo->GetOfferRewardText().empty())
                                    {
                                        if (qDbLocInfo->GetOfferRewardText().empty())
                                        {
                                            if (!IsSameString(qWhLocInfo->GetOfferRewardText(e, 1), qWhLocInfo->GetOfferRewardText(), locale, true))
                                            {
                                                missingQuestOfferText++;
                                                updateQueries += updateQuestFromWhQuery(qWhLocInfo, qDbLocInfo, "offerRewardText", e, locale) + "\n";
                                            }
                                        }
                                        else if (!ignoreDifferentVersion)
                                        {
                                            if (!IsSameString(qWhLocInfo->GetOfferRewardText(), qDbLocInfo->GetOfferRewardText(), locale, true))
                                            {
                                                const std::string message = "Quest (" + std::to_string(qWhLocInfo->GetEntry()) + ") offerRewardText";
                                                if (autoUpdateWowheadVersion || ShouldOverrideDifference(message, qWhLocInfo->GetOfferRewardText(), qDbLocInfo->GetOfferRewardText()))
                                                {
                                                    updatedQuestOfferText++;
                                                    updateQueries += updateQuestFromWhQuery(qWhLocInfo, qDbLocInfo, "offerRewardText", e, locale) + "\n";
                                                }
                                            }
                                        }
                                    }
                                }

                                if (!qDbLocInfo->GetDetails(e, 1).empty() && (qDbLocInfo->GetDetails().empty() || !ignoreDifferentVersion))
                                {
                                    if (!qWhEngInfo)
                                    {
                                        qWhEngInfo = LoadWowheadQuestInfo(qID, e, 1, true);
                                        if (!qWhEngInfo)
                                            continue;
                                    }
                                    if (!qWhLocInfo)
                                    {
                                        qWhLocInfo = LoadWowheadQuestInfo(qID, e, localeId, true);
                                        if (!qWhLocInfo)
                                            continue;
                                    }

                                    if (!qWhLocInfo->GetDetails().empty())
                                    {
                                        if (qDbLocInfo->GetDetails().empty())
                                        {
                                            if (!IsSameString(qWhLocInfo->GetDetails(e, 1), qWhLocInfo->GetDetails(), locale, true))
                                            {
                                                missingQuestDetails++;
                                                updateQueries += updateQuestFromWhQuery(qWhLocInfo, qDbLocInfo, "details", e, locale) + "\n";
                                            }
                                        }
                                        else if (!ignoreDifferentVersion)
                                        {
                                            if (!IsSameString(qWhLocInfo->GetDetails(), qDbLocInfo->GetDetails(), locale, true))
                                            {
                                                const std::string message = "Quest (" + std::to_string(qWhLocInfo->GetEntry()) + ") details";
                                                if (autoUpdateWowheadVersion || ShouldOverrideDifference(message, qWhLocInfo->GetDetails(), qDbLocInfo->GetDetails()))
                                                {
                                                    updatedQuestDetails++;
                                                    updateQueries += updateQuestFromWhQuery(qWhLocInfo, qDbLocInfo, "details", e, locale) + "\n";
                                                }
                                            }
                                        }
                                    }
                                }

                                // Objective 1-4
                                if (!qDbLocInfo->GetObjective(0, e, 1).empty() && (qDbLocInfo->GetObjective(0).empty() || !ignoreDifferentVersion))
                                {
                                    if (!qWhEngInfo)
                                    {
                                        qWhEngInfo = LoadWowheadQuestInfo(qID, e, 1, true);
                                        if (!qWhEngInfo)
                                            continue;
                                    }
                                    if (!qWhLocInfo)
                                    {
                                        qWhLocInfo = LoadWowheadQuestInfo(qID, e, localeId, true);
                                        if (!qWhLocInfo)
                                            continue;
                                    }

                                    if (!qWhLocInfo->GetObjective(0).empty())
                                    {
                                        if (qDbLocInfo->GetObjective(0).empty())
                                        {
                                            if (!IsSameString(qWhLocInfo->GetObjective(0, e, 1), qWhLocInfo->GetObjective(0), locale, true))
                                            {
                                                missingQuestObjective1++;
                                                updateQueries += updateQuestFromWhQuery(qWhLocInfo, qDbLocInfo, "objectiveText1", e, locale) + "\n";
                                            }
                                        }
                                        else if (!ignoreDifferentVersion)
                                        {
                                            if (!IsSameString(qWhLocInfo->GetObjective(0), qDbLocInfo->GetObjective(0), locale, true))
                                            {
                                                const std::string message = "Quest (" + std::to_string(qWhLocInfo->GetEntry()) + ") objectiveText1";
                                                if (autoUpdateWowheadVersion || ShouldOverrideDifference(message, qWhLocInfo->GetObjective(0), qDbLocInfo->GetObjective(0)))
                                                {
                                                    updatedQuestObjective1++;
                                                    updateQueries += updateQuestFromWhQuery(qWhLocInfo, qDbLocInfo, "objectiveText1", e, locale) + "\n";
                                                }
                                            }
                                        }
                                    }
                                }

                                if (!qDbLocInfo->GetObjective(1, e, 1).empty() && (qDbLocInfo->GetObjective(1).empty() || !ignoreDifferentVersion))
                                {
                                    if (!qWhEngInfo)
                                    {
                                        qWhEngInfo = LoadWowheadQuestInfo(qID, e, 1, true);
                                        if (!qWhEngInfo)
                                            continue;
                                    }
                                    if (!qWhLocInfo)
                                    {
                                        qWhLocInfo = LoadWowheadQuestInfo(qID, e, localeId, true);
                                        if (!qWhLocInfo)
                                            continue;
                                    }

                                    if (!qWhLocInfo->GetObjective(1).empty())
                                    {
                                        if (qDbLocInfo->GetObjective(1).empty())
                                        {
                                            if (!IsSameString(qWhLocInfo->GetObjective(1, e, 1), qWhLocInfo->GetObjective(1), locale, true))
                                            {
                                                missingQuestObjective2++;
                                                updateQueries += updateQuestFromWhQuery(qWhLocInfo, qDbLocInfo, "objectiveText2", e, locale) + "\n";
                                            }
                                        }
                                        else if (!ignoreDifferentVersion)
                                        {
                                            if (!IsSameString(qWhLocInfo->GetObjective(1), qDbLocInfo->GetObjective(1), locale, true))
                                            {
                                                const std::string message = "Quest (" + std::to_string(qWhLocInfo->GetEntry()) + ") objectiveText2";
                                                if (autoUpdateWowheadVersion || ShouldOverrideDifference(message, qWhLocInfo->GetObjective(1), qDbLocInfo->GetObjective(1)))
                                                {
                                                    updatedQuestObjective2++;
                                                    updateQueries += updateQuestFromWhQuery(qWhLocInfo, qDbLocInfo, "objectiveText2", e, locale) + "\n";
                                                }
                                            }
                                        }
                                    }
                                }

                                if (!qDbLocInfo->GetObjective(2, e, 1).empty() && (qDbLocInfo->GetObjective(2).empty() || !ignoreDifferentVersion))
                                {
                                    if (!qWhEngInfo)
                                    {
                                        qWhEngInfo = LoadWowheadQuestInfo(qID, e, 1, true);
                                        if (!qWhEngInfo)
                                            continue;
                                    }
                                    if (!qWhLocInfo)
                                    {
                                        qWhLocInfo = LoadWowheadQuestInfo(qID, e, localeId, true);
                                        if (!qWhLocInfo)
                                            continue;
                                    }

                                    if (!qWhLocInfo->GetObjective(2).empty())
                                    {
                                        if (qDbLocInfo->GetObjective(2).empty())
                                        {
                                            if (!IsSameString(qWhLocInfo->GetObjective(2, e, 1), qWhLocInfo->GetObjective(2), locale, true))
                                            {
                                                missingQuestObjective3++;
                                                updateQueries += updateQuestFromWhQuery(qWhLocInfo, qDbLocInfo, "objectiveText3", e, locale) + "\n";
                                            }
                                        }
                                        else if (!ignoreDifferentVersion)
                                        {
                                            if (!IsSameString(qWhLocInfo->GetObjective(2), qDbLocInfo->GetObjective(2), locale, true))
                                            {
                                                const std::string message = "Quest (" + std::to_string(qWhLocInfo->GetEntry()) + ") objectiveText3";
                                                if (autoUpdateWowheadVersion || ShouldOverrideDifference(message, qWhLocInfo->GetObjective(2), qDbLocInfo->GetObjective(2)))
                                                {
                                                    updatedQuestObjective3++;
                                                    updateQueries += updateQuestFromWhQuery(qWhLocInfo, qDbLocInfo, "objectiveText3", e, locale) + "\n";
                                                }
                                            }
                                        }
                                    }
                                }

                                if (!qDbLocInfo->GetObjective(3, e, 1).empty() && (qDbLocInfo->GetObjective(3).empty() || !ignoreDifferentVersion))
                                {
                                    if (!qWhEngInfo)
                                    {
                                        qWhEngInfo = LoadWowheadQuestInfo(qID, e, 1, true);
                                        if (!qWhEngInfo)
                                            continue;
                                    }
                                    if (!qWhLocInfo)
                                    {
                                        qWhLocInfo = LoadWowheadQuestInfo(qID, e, localeId, true);
                                        if (!qWhLocInfo)
                                            continue;
                                    }

                                    if (!qWhLocInfo->GetObjective(3).empty())
                                    {
                                        if (qDbLocInfo->GetObjective(3).empty())
                                        {
                                            if (!IsSameString(qWhLocInfo->GetObjective(3, e, 1), qWhLocInfo->GetObjective(3), locale, true))
                                            {
                                                missingQuestObjective4++;
                                                updateQueries += updateQuestFromWhQuery(qWhLocInfo, qDbLocInfo, "objectiveText4", e, locale) + "\n";
                                            }
                                        }
                                        else if (!ignoreDifferentVersion)
                                        {
                                            if (!IsSameString(qWhLocInfo->GetObjective(3), qDbLocInfo->GetObjective(3), locale, true))
                                            {
                                                const std::string message = "Quest (" + std::to_string(qWhLocInfo->GetEntry()) + ") objectiveText4";
                                                if (autoUpdateWowheadVersion || ShouldOverrideDifference(message, qWhLocInfo->GetObjective(3), qDbLocInfo->GetObjective(3)))
                                                {
                                                    updatedQuestObjective4++;
                                                    updateQueries += updateQuestFromWhQuery(qWhLocInfo, qDbLocInfo, "objectiveText4", e, locale) + "\n";
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        // write queries to file
                        if (!updateQueries.empty())
                        {
                            std::string fileName = (!ignoreDifferentVersion ? "update_quest_" : "missing_quest_") + localeName(locale) + ".sql";
                            writeToFile(updateQueries.c_str(), fileName, filesLocation);
                        }
                    }

                    // Items
                    if(checkItems)
                    {
                        // List of items and pages that are badly translated in wowhead
                        std::set<uint32> ignoreItems;
                        std::set<uint32> ignoreItemPages;

                        // Spanish
                        if (locale == 6)
                        {
                            ignoreItems = { 1318, 6766, 12627, 15293, 18565, 18566, 19054, 19055, 22822, 2450, 5417, 5418 };

                            ignoreItemPages = { 54, 62, 2819, 2888, 308, 316, 696, 697, 812, 891, 1191, 2465, 2435, 2432, 2434, 2464, 2453, 2447, 2448, 2513, 22822 };
                        }

                        // use CN for TW from wowhead
                        uint32 localeId = locale;
                        if (localeId == 8)
                            localeId = 5;

                        std::string cacheEngLocation = "cache/" + expansionName(e) + "/" + localeName(1) + "/" + typeName(TYPE_ITEM) + "/";
                        std::string cacheLocLocation = "cache/" + expansionName(e) + "/" + localeName(localeId) + "/" + typeName(TYPE_ITEM) + "/";
                        if (!std::filesystem::is_directory(cacheEngLocation))
                            continue;
                        if (!std::filesystem::is_directory(cacheLocLocation))
                            continue;

                        // read existing update file
                        std::string filesLocation = "work/" + sDataMgr.getProjectName() + "/" + expansionName(expansion) + "/" + typeName(TYPE_ITEM) + "/";
                        std::string updateQueries;
                        
                        auto dirIter = std::filesystem::directory_iterator(cacheLocLocation);
                        for (auto& fl : dirIter)
                        {
                            if (fl.is_regular_file())
                            {
                                const std::filesystem::path& p(fl.path());
                                uint32 itemID = stoi(p.stem().string());

                                iterations++;
                                if ((iterations % 10) == 0)
                                {
                                    std::stringstream message;
                                    uint32 iterationPct = (iterations * 100) / totalIterations;
                                    message << "   " << std::to_string(iterationPct) << "%c (" << std::to_string(iterations) << "/" + std::to_string(totalIterations) << ")\n";
                                    msg_nodelay(message.str(), '\u0025');
                                }

                                // load both eng and locale (later)
                                WowheadItemInfo* itemWhEngInfo = nullptr;//LoadWowheadItemInfo(itemID, e, 1, true);
                                WowheadItemInfo* itemWhLocInfo = nullptr;//LoadWowheadItemInfo(itemID, e, localeId, true);

                                // load english first
                                DatabaseItemInfo* itemDbLocInfo = LoadDatabaseItemInfo(itemID, e, 1, true, true);
                                if (!itemDbLocInfo)
                                {
                                    missingDbItem++;
                                    continue;
                                }

                                itemDbLocInfo = LoadDatabaseItemInfo(itemID, e, locale, true, true);
                                if (!itemDbLocInfo)
                                    continue;

                                if (ignoreItems.find(itemID) == ignoreItems.end())
                                {
                                    // Item name
                                    if (!itemDbLocInfo->GetName(e, 1).empty() && (itemDbLocInfo->GetName().empty() || !ignoreDifferentVersion))
                                    {
                                        if (!itemWhEngInfo)
                                        {
                                            itemWhEngInfo = LoadWowheadItemInfo(itemID, e, 1, true);
                                            if (!itemWhEngInfo)
                                                continue;
                                        }
                                        if (!itemWhLocInfo)
                                        {
                                            itemWhLocInfo = LoadWowheadItemInfo(itemID, e, localeId, true);
                                            if (!itemWhLocInfo)
                                                continue;
                                        }

                                        if (!itemWhLocInfo->GetName().empty())
                                        {
                                            if (itemDbLocInfo->GetName().empty())
                                            {
                                                if (!IsSameString(itemWhLocInfo->GetName(e, 1), itemWhLocInfo->GetName(), locale, true))
                                                {
                                                    missingItemName++;
                                                    updateQueries += updateItemFromWhQuery(itemWhLocInfo, itemDbLocInfo, "name", e, locale) + "\n";
                                                }
                                            }
                                            else if (!ignoreDifferentVersion)
                                            {
                                                if (!IsSameString(itemWhLocInfo->GetName(), itemDbLocInfo->GetName(), locale, true))
                                                {
                                                    const std::string message = "Item (" + std::to_string(itemWhLocInfo->GetEntry()) + ") name";
                                                    if (autoUpdateWowheadVersion || ShouldOverrideDifference(message, itemWhLocInfo->GetName(), itemDbLocInfo->GetName()))
                                                    {
                                                        updatedItemName++;
                                                        updateQueries += updateItemFromWhQuery(itemWhLocInfo, itemDbLocInfo, "name", e, locale) + "\n";
                                                    }
                                                }
                                            }
                                        }
                                    }
                                
                                    // Item description
                                    if (!itemDbLocInfo->GetDescription(e, 1).empty() && (itemDbLocInfo->GetDescription().empty() || !ignoreDifferentVersion))
                                    {
                                        if (!itemWhEngInfo)
                                        {
                                            itemWhEngInfo = LoadWowheadItemInfo(itemID, e, 1, true);
                                            if (!itemWhEngInfo)
                                                continue;
                                        }
                                        if (!itemWhLocInfo)
                                        {
                                            itemWhLocInfo = LoadWowheadItemInfo(itemID, e, localeId, true);
                                            if (!itemWhLocInfo)
                                                continue;
                                        }

                                        if (!itemWhLocInfo->GetDescription().empty())
                                        {
                                            if (itemDbLocInfo->GetDescription().empty())
                                            {
                                                if (!IsSameString(itemWhLocInfo->GetDescription(e, 1), itemWhLocInfo->GetDescription(), locale, true))
                                                {
                                                    missingItemDescription++;
                                                    updateQueries += updateItemFromWhQuery(itemWhLocInfo, itemDbLocInfo, "description", e, locale) + "\n";
                                                }
                                            }
                                            else if (!ignoreDifferentVersion)
                                            {
                                                if (!IsSameString(itemWhLocInfo->GetDescription(), itemDbLocInfo->GetDescription(), locale, true))
                                                {
                                                    const std::string message = "Item (" + std::to_string(itemWhLocInfo->GetEntry()) + ") description";
                                                    if (autoUpdateWowheadVersion || ShouldOverrideDifference(message, itemWhLocInfo->GetDescription(), itemDbLocInfo->GetDescription()))
                                                    {
                                                        updatedItemDescription++;
                                                        updateQueries += updateItemFromWhQuery(itemWhLocInfo, itemDbLocInfo, "description", e, locale) + "\n";
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }

                                // Item pages
                                if (!itemDbLocInfo->GetPages(e, 1).empty() && (itemDbLocInfo->GetPages().empty() || !ignoreDifferentVersion))
                                {
                                    if (!itemWhEngInfo)
                                    {
                                        itemWhEngInfo = LoadWowheadItemInfo(itemID, e, 1, true);
                                        if (!itemWhEngInfo)
                                            continue;
                                    }
                                    if (!itemWhLocInfo)
                                    {
                                        itemWhLocInfo = LoadWowheadItemInfo(itemID, e, localeId, true);
                                        if (!itemWhLocInfo)
                                            continue;
                                    }

                                    if (!itemWhLocInfo->GetPages().empty())
                                    {
                                        if (itemDbLocInfo->GetPages().empty())
                                        {
                                            if (!HasSamePages(itemWhLocInfo->GetPages(e, 1), itemWhLocInfo->GetPages()))
                                            {
                                                for (const PageEntry& page : itemWhLocInfo->GetPages(e, locale))
                                                {
                                                    if (ignoreItemPages.find(page.entry) == ignoreItemPages.end())
                                                    {
                                                        missingItemPageText++;
                                                        updateQueries += updateItemPageFromTextQueryNoTags(page.text, page.entry, itemDbLocInfo, e, locale) + "\n";
                                                    }
                                                }
                                            }
                                        }
                                        else if (!ignoreDifferentVersion)
                                        {
                                            std::vector<std::pair<PageEntry, PageEntry>> differentPages;
                                            if (!HasSamePages(itemWhLocInfo->GetPages(), itemDbLocInfo->GetPages(), &differentPages))
                                            {
                                                for (const std::pair<PageEntry, PageEntry>& pageDifference : differentPages)
                                                {
                                                    if (ignoreItemPages.find(pageDifference.first.entry) == ignoreItemPages.end())
                                                    {
                                                        const std::string message = "Item Page (" + std::to_string(pageDifference.first.entry) + ")";
                                                        if (autoUpdateWowheadVersion || ShouldOverrideDifference(message, pageDifference.first.text, pageDifference.second.text))
                                                        {
                                                            updatedItemPageText++;
                                                            updateQueries += updateItemPageFromTextQueryNoTags(pageDifference.first.text, pageDifference.first.entry, itemDbLocInfo, e, locale) + "\n";
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        // write queries to file
                        if (!updateQueries.empty())
                        {
                            std::string fileName = (!ignoreDifferentVersion ? "update_item_" : "missing_item_") + localeName(locale) + ".sql";
                            writeToFile(updateQueries.c_str(), fileName, filesLocation);
                        }
                    }

                    // Game Objects
                    if (checkGameObjects)
                    {
                        // List of game objects and pages that are badly translated in wowhead
                        std::set<uint32> ignoreGameObjects;
                        std::set<uint32> ignoreGameObjectPages;

                        // Spanish
                        if (locale == 6)
                        {
                            ignoreGameObjects =
                            {
                                148842, 153462, 166872, 174616, 175789, 176086, 176544, 176545, 178229, 178427, 178545, 178584, 178605,
                                178665, 178724, 178725, 178726, 179985, 180024, 180055, 180570, 180601, 180619, 180652, 180671, 180691,
                                180699, 180717, 180743, 180746, 180747, 180748, 180754, 180793, 180795, 180851, 180855, 180857, 180858,
                                180860, 180862, 180863, 180864, 180865, 180870, 180871, 180872, 180873, 180905, 181083, 181335, 181336,
                                181337, 181346, 181348, 181349, 181366, 181576, 181578, 181886, 20808, 21631, 249, 337, 3710, 4095, 73939,
                                73940, 141853, 103774, 179706, 21004
                            };

                            ignoreGameObjectPages = { 1432, 1751, 1760, 1753, 1755, 2175, 2177, 2311, 287, 288, 289, 2891, 2731, 2905, 2925 };
                        }

                        // use CN for TW from wowhead
                        uint32 localeId = locale;
                        if (localeId == 8)
                            localeId = 5;

                        std::string cacheEngLocation = "cache/" + expansionName(e) + "/" + localeName(1) + "/" + typeName(TYPE_OBJECT) + "/";
                        std::string cacheLocLocation = "cache/" + expansionName(e) + "/" + localeName(localeId) + "/" + typeName(TYPE_OBJECT) + "/";
                        if (!std::filesystem::is_directory(cacheEngLocation))
                            continue;
                        if (!std::filesystem::is_directory(cacheLocLocation))
                            continue;

                        // read existing update file
                        std::string filesLocation = "work/" + sDataMgr.getProjectName() + "/" + expansionName(expansion) + "/" + typeName(TYPE_OBJECT) + "/";
                        std::string updateQueries;

                        auto dirIter = std::filesystem::directory_iterator(cacheLocLocation);
                        for (auto& fl : dirIter)
                        {
                            if (fl.is_regular_file())
                            {
                                const std::filesystem::path& p(fl.path());
                                uint32 gameObjectID = stoi(p.stem().string());

                                iterations++;
                                if ((iterations % 10) == 0)
                                {
                                    std::stringstream message;
                                    uint32 iterationPct = (iterations * 100) / totalIterations;
                                    message << "   " << std::to_string(iterationPct) << "%c (" << std::to_string(iterations) << "/" + std::to_string(totalIterations) << ")\n";
                                    msg_nodelay(message.str(), '\u0025');
                                }

                                // load both eng and locale
                                WowheadGameObjectInfo* gameObjectWhEngInfo = nullptr; //LoadWowheadGameObjectInfo(gameObjectID, e, 1, true);
                                //if (!gameObjectWhEngInfo)
                                //    continue;

                                WowheadGameObjectInfo* gameObjectWhLocInfo = nullptr; //LoadWowheadGameObjectInfo(gameObjectID, e, localeId, true);
                                //if (!gameObjectWhLocInfo)
                                //    continue;

                                // load english first
                                DatabaseGameObjectInfo* gameObjectDbLocInfo = LoadDatabaseGameObjectInfo(gameObjectID, e, 1, true, true);
                                if (!gameObjectDbLocInfo)
                                {
                                    missingDbItem++;
                                    continue;
                                }

                                gameObjectDbLocInfo = LoadDatabaseGameObjectInfo(gameObjectID, e, locale, true, true);
                                if (!gameObjectDbLocInfo)
                                    continue;

                                if (ignoreGameObjects.find(gameObjectID) == ignoreGameObjects.end())
                                {
                                    // Game Object name
                                    if (!gameObjectDbLocInfo->GetName(e, 1).empty() && (gameObjectDbLocInfo->GetName().empty() || !ignoreDifferentVersion))
                                    {
                                        if (!gameObjectWhEngInfo)
                                        {
                                            gameObjectWhEngInfo = LoadWowheadGameObjectInfo(gameObjectID, e, 1, true);
                                            if (!gameObjectWhEngInfo)
                                                continue;
                                        }
                                        if (!gameObjectWhLocInfo)
                                        {
                                            gameObjectWhLocInfo = LoadWowheadGameObjectInfo(gameObjectID, e, localeId, true);
                                            if (!gameObjectWhLocInfo)
                                                continue;
                                        }

                                        if (!gameObjectWhLocInfo->GetName().empty())
                                        {
                                            if (gameObjectDbLocInfo->GetName().empty())
                                            {
                                                if (!IsSameString(gameObjectWhLocInfo->GetName(e, 1), gameObjectWhLocInfo->GetName(), localeId, true))
                                                {
                                                    missingGameObjectName++;
                                                    updateQueries += updateGameObjectFromWhQuery(gameObjectWhLocInfo, gameObjectDbLocInfo, "name", e, locale) + "\n";
                                                }
                                            }
                                            else if (!ignoreDifferentVersion)
                                            {
                                                if (!IsSameString(gameObjectWhLocInfo->GetName(), gameObjectDbLocInfo->GetName(), localeId, true))
                                                {
                                                    const std::string message = "Game Object (" + std::to_string(gameObjectWhLocInfo->GetEntry()) + ") name";
                                                    if (autoUpdateWowheadVersion || ShouldOverrideDifference(message, gameObjectWhLocInfo->GetName(), gameObjectDbLocInfo->GetName()))
                                                    {
                                                        updatedGameObjectName++;
                                                        updateQueries += updateGameObjectFromWhQuery(gameObjectWhLocInfo, gameObjectDbLocInfo, "name", e, locale) + "\n";
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }

                                // Game Object pages
                                if (!gameObjectDbLocInfo->GetPages(e, 1).empty() && (gameObjectDbLocInfo->GetPages().empty() || !ignoreDifferentVersion))
                                {
                                    if (!gameObjectWhEngInfo)
                                    {
                                        gameObjectWhEngInfo = LoadWowheadGameObjectInfo(gameObjectID, e, 1, true);
                                        if (!gameObjectWhEngInfo)
                                            continue;
                                    }
                                    if (!gameObjectWhLocInfo)
                                    {
                                        gameObjectWhLocInfo = LoadWowheadGameObjectInfo(gameObjectID, e, localeId, true);
                                        if (!gameObjectWhLocInfo)
                                            continue;
                                    }

                                    if (!gameObjectWhLocInfo->GetPages().empty())
                                    {
                                        if (gameObjectDbLocInfo->GetPages().empty())
                                        {
                                            if (!HasSamePages(gameObjectWhLocInfo->GetPages(e, 1), gameObjectWhLocInfo->GetPages()))
                                            {
                                                for (const PageEntry& page : gameObjectWhLocInfo->GetPages(e, locale))
                                                {
                                                    if (ignoreGameObjectPages.find(page.entry) == ignoreGameObjectPages.end())
                                                    {
                                                        missingGameObjectPageText++;
                                                        updateQueries += updateGameObjectPageFromTextQueryNoTags(page.text, page.entry, gameObjectDbLocInfo, e, locale) + "\n";
                                                    }
                                                }
                                            }
                                        }
                                        else if (!ignoreDifferentVersion)
                                        {
                                            std::vector<std::pair<PageEntry, PageEntry>> differentPages;
                                            if (!HasSamePages(gameObjectWhLocInfo->GetPages(), gameObjectDbLocInfo->GetPages(), &differentPages))
                                            {
                                                for (const std::pair<PageEntry, PageEntry>& pageDifference : differentPages)
                                                {
                                                    if (ignoreGameObjectPages.find(pageDifference.first.entry) == ignoreGameObjectPages.end())
                                                    {
                                                        const std::string message = "Game Object Page (" + std::to_string(pageDifference.first.entry) + ")";
                                                        if (autoUpdateWowheadVersion || ShouldOverrideDifference(message, pageDifference.first.text, pageDifference.second.text))
                                                        {
                                                            updatedGameObjectPageText++;
                                                            updateQueries += updateGameObjectPageFromTextQueryNoTags(pageDifference.first.text, pageDifference.first.entry, gameObjectDbLocInfo, e, locale) + "\n";
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        // write queries to file
                        if (!updateQueries.empty())
                        {
                            std::string fileName = (!ignoreDifferentVersion ? "update_object_" : "missing_object_") + localeName(locale) + ".sql";
                            writeToFile(updateQueries.c_str(), fileName, filesLocation);
                        }
                    }

                    // Achievements
                    if (checkAchievements)
                    {
                        // List of achievements that are badly translated in wowhead
                        std::set<uint32> ignoreAchievements;

                        // use CN for TW from wowhead
                        uint32 localeId = locale;
                        if (localeId == 8)
                            localeId = 5;

                        std::string cacheEngLocation = "cache/" + expansionName(e) + "/" + localeName(1) + "/" + typeName(TYPE_ACHIEVEMENT) + "/";
                        std::string cacheLocLocation = "cache/" + expansionName(e) + "/" + localeName(localeId) + "/" + typeName(TYPE_ACHIEVEMENT) + "/";
                        if (!std::filesystem::is_directory(cacheEngLocation))
                            continue;
                        if (!std::filesystem::is_directory(cacheLocLocation))
                            continue;

                        // read existing update file
                        std::string filesLocation = "work/" + sDataMgr.getProjectName() + "/" + expansionName(expansion) + "/" + typeName(TYPE_ACHIEVEMENT) + "/";
                        std::string updateQueries;

                        auto dirIter = std::filesystem::directory_iterator(cacheLocLocation);
                        for (auto& fl : dirIter)
                        {
                            if (fl.is_regular_file())
                            {
                                const std::filesystem::path& p(fl.path());
                                uint32 achievementID = stoi(p.stem().string());

                                iterations++;
                                if ((iterations % 10) == 0)
                                {
                                    std::stringstream message;
                                    uint32 iterationPct = (iterations * 100) / totalIterations;
                                    message << "   " << std::to_string(iterationPct) << "%c (" << std::to_string(iterations) << "/" + std::to_string(totalIterations) << ")\n";
                                    msg_nodelay(message.str(), '\u0025');
                                }

                                // load both eng and locale
                                WowheadAchievementInfo* achievementWhEngInfo = LoadWowheadAchievementInfo(achievementID, e, 1, true);
                                if (!achievementWhEngInfo)
                                    continue;

                                WowheadAchievementInfo* achievementWhLocInfo = LoadWowheadAchievementInfo(achievementID, e, localeId, true);
                                if (!achievementWhLocInfo)
                                    continue;

                                // load english first
                                DatabaseAchievementInfo* achievementDbLocInfo = LoadDatabaseAchievementInfo(achievementID, e, 1, true, true);
                                if (!achievementDbLocInfo)
                                {
                                    missingDbAchievement++;
                                    continue;
                                }

                                achievementDbLocInfo = LoadDatabaseAchievementInfo(achievementID, e, locale, true, true);
                                if (!achievementDbLocInfo)
                                    continue;

                                if (ignoreAchievements.find(achievementID) == ignoreAchievements.end())
                                {
                                    // Achievement name
                                    if (!achievementDbLocInfo->GetName(e, 1).empty())
                                    {
                                        if (!achievementWhLocInfo->GetName().empty())
                                        {
                                            if (achievementDbLocInfo->GetName().empty())
                                            {
                                                if (!IsSameString(achievementWhLocInfo->GetName(e, 1), achievementWhLocInfo->GetName(), locale, true))
                                                {
                                                    missingAchievementName++;
                                                    updateQueries += updateAchievementFromWhQuery(achievementWhLocInfo, achievementDbLocInfo, "name", e, locale) + "\n";
                                                }
                                            }
                                            else if (!ignoreDifferentVersion)
                                            {
                                                if (!IsSameString(achievementWhLocInfo->GetName(), achievementDbLocInfo->GetName(), locale, true))
                                                {
                                                    const std::string message = "Achievement (" + std::to_string(achievementWhLocInfo->GetEntry()) + ") name";
                                                    if (autoUpdateWowheadVersion || ShouldOverrideDifference(message, achievementWhLocInfo->GetName(), achievementDbLocInfo->GetName()))
                                                    {
                                                        updatedAchievementName++;
                                                        updateQueries += updateAchievementFromWhQuery(achievementWhLocInfo, achievementDbLocInfo, "name", e, locale) + "\n";
                                                    }
                                                }
                                            }
                                        }
                                    }

                                    // Achievement description
                                    if (!achievementDbLocInfo->GetDescription(e, 1).empty())
                                    {
                                        if (!achievementWhLocInfo->GetDescription().empty())
                                        {
                                            if (achievementDbLocInfo->GetDescription().empty())
                                            {
                                                if (!IsSameString(achievementWhLocInfo->GetDescription(e, 1), achievementWhLocInfo->GetDescription(), locale, true))
                                                {
                                                    missingAchievementDescription++;
                                                    updateQueries += updateAchievementFromWhQuery(achievementWhLocInfo, achievementDbLocInfo, "description", e, locale) + "\n";
                                                }
                                            }
                                            else if (!ignoreDifferentVersion)
                                            {
                                                if (!IsSameString(achievementWhLocInfo->GetDescription(), achievementDbLocInfo->GetDescription(), locale, true))
                                                {
                                                    const std::string message = "Achievement (" + std::to_string(achievementWhLocInfo->GetEntry()) + ") description";
                                                    if (autoUpdateWowheadVersion || ShouldOverrideDifference(message, achievementWhLocInfo->GetDescription(), achievementDbLocInfo->GetDescription()))
                                                    {
                                                        updatedAchievementDescription++;
                                                        updateQueries += updateAchievementFromWhQuery(achievementWhLocInfo, achievementDbLocInfo, "description", e, locale) + "\n";
                                                    }
                                                }
                                            }
                                        }
                                    }

                                    // Achievement criterias
                                    if (!achievementDbLocInfo->GetCriterias(e, 1).empty())
                                    {
                                        if (!achievementWhLocInfo->GetCriterias().empty())
                                        {
                                            if (achievementDbLocInfo->GetCriterias().empty())
                                            {
                                                if (!HasSameCriterias(achievementWhLocInfo->GetCriterias(e, 1), achievementWhLocInfo->GetCriterias()))
                                                {
                                                    for (const AchievementCriteria& criteria : achievementWhLocInfo->GetCriterias(e, locale))
                                                    {
                                                        missingAchievementCriteriaDescription++;
                                                        updateQueries += updateAchievementCriteriaFromTextQueryNoTags(criteria.description, criteria.entry, achievementDbLocInfo, e, locale) + "\n";
                                                    }
                                                }
                                            }
                                            else if (!ignoreDifferentVersion)
                                            {
                                                if (achievementDbLocInfo->GetCriterias().size() == 1)
                                                {
                                                    const AchievementCriteria& criteria = achievementDbLocInfo->GetCriterias()[0];
                                                    if (criteria.description != achievementWhLocInfo->GetDescription())
                                                    {
                                                        const std::string message = "Achievement Criteria (" + std::to_string(criteria.entry) + ")";
                                                        if (autoUpdateWowheadVersion || ShouldOverrideDifference(message, achievementWhLocInfo->GetDescription(), criteria.description))
                                                        {
                                                            updatedAchievementCriteriaDescription++;
                                                            updateQueries += updateAchievementCriteriaFromTextQueryNoTags(achievementWhLocInfo->GetDescription(), criteria.entry, achievementDbLocInfo, e, locale) + "\n";
                                                        }
                                                    }
                                                }
                                                else
                                                {
                                                    /* TO DO: Parse and cross reference multiple criteria from wowhead
                                                    std::vector<std::pair<AchievementCriteria, AchievementCriteria>> differentCriterias;
                                                    if (!HasSameCriterias(achievementWhLocInfo->GetCriterias(), achievementDbLocInfo->GetCriterias(), &differentCriterias))
                                                    {
                                                        for (const std::pair<AchievementCriteria, AchievementCriteria>& pageDifference : differentCriterias)
                                                        {
                                                            const std::string message = "Achievement Criteria (" + std::to_string(pageDifference.first.entry) + ")";
                                                            if (autoUpdateWowheadVersion || ShouldOverrideDifference(message, pageDifference.first.description, pageDifference.second.description))
                                                            {
                                                                updatedGameObjectPageText++;
                                                                updateQueries += updateAchievementCriteriaFromTextQueryNoTags(pageDifference.first.description, pageDifference.first.entry, achievementDbLocInfo, e, locale) + "\n";
                                                            }
                                                        }
                                                    }
                                                    */
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        // write queries to file
                        if (!updateQueries.empty())
                        {
                            std::string fileName = (!ignoreDifferentVersion ? "update_achievement_" : "missing_achievement_") + localeName(locale) + ".sql";
                            writeToFile(updateQueries.c_str(), fileName, filesLocation);
                        }
                    }

                    if (checkQuests)
                    {
                        // print results
                        msg_delay("\n\n");

                        msg_delay(">   Quest Stats: " + localeName(locale) + "\n");

                        if (missingDbQuest)
                            msg_delay(">   Missing Quests in DB: %d \n\n", missingDbQuest);

                        msg_delay(">   Added Title: %d \n", missingQuestTitle);
                        msg_delay(">   Added Objectives: %d \n", missingQuestObjText);
                        msg_delay(">   Added Details: %d \n", missingQuestDetails);
                        msg_delay(">   Added RequestItemText: %d \n", missingQuestReqText);
                        msg_delay(">   Added OfferRewardText: %d \n", missingQuestOfferText);
                        msg_delay(">   Added EndText: %d \n", missingQuestEndText);
                        msg_delay(">   Added Objective1-4: %d - %d - %d - %d \n", missingQuestObjective1, missingQuestObjective2, missingQuestObjective3, missingQuestObjective4);

                        msg_delay(">   Updated Title: %d \n", updatedQuestTitle);
                        msg_delay(">   Updated Objectives: %d \n", updatedQuestObjText);
                        msg_delay(">   Updated Details: %d \n", updatedQuestDetails);
                        msg_delay(">   Updated RequestItemText: %d \n", updatedQuestReqText);
                        msg_delay(">   Updated OfferRewardText: %d \n", updatedQuestOfferText);
                        msg_delay(">   Updated EndText: %d \n", updatedQuestEndText);
                        msg_delay(">   Updated Objective1-4: %d - %d - %d - %d \n", updatedQuestObjective1, updatedQuestObjective2, updatedQuestObjective3, updatedQuestObjective4);
                    }

                    if (checkItems)
                    {
                        // print results
                        msg_delay("\n\n");

                        msg_delay(">   Item Stats: " + localeName(locale) + "\n");

                        if (missingDbItem)
                            msg_delay(">   Missing Items in DB: %d \n\n", missingDbItem);

                        msg_delay(">   Added Name: %d \n", missingItemName);
                        msg_delay(">   Added Description: %d \n", missingItemDescription);
                        msg_delay(">   Added Page Texts: %d \n", missingItemPageText);

                        msg_delay(">   Updated Name: %d \n", updatedItemName);
                        msg_delay(">   Updated Description: %d \n", updatedItemDescription);
                        msg_delay(">   Updated Page Texts: %d \n", updatedItemPageText);
                    }

                    if (checkGameObjects)
                    {
                        // print results
                        msg_delay("\n\n");

                        msg_delay(">   Game Objects Stats: " + localeName(locale) + "\n");

                        if (missingDbItem)
                            msg_delay(">   Missing Game Objects in DB: %d \n\n", missingDbGameObject);

                        msg_delay(">   Added Name: %d \n", missingGameObjectName);
                        msg_delay(">   Added Page Texts: %d \n", missingGameObjectPageText);

                        msg_delay(">   Updated Name: %d \n", updatedGameObjectName);
                        msg_delay(">   Updated Page Texts: %d \n", updatedGameObjectPageText);
                    }

                    if (checkAchievements)
                    {
                        // print results
                        msg_delay("\n\n");

                        msg_delay(">   Achievements Stats: " + localeName(locale) + "\n");

                        if (missingDbItem)
                            msg_delay(">   Missing Achievements in DB: %d \n\n", missingDbAchievement);

                        msg_delay(">   Added Name: %d \n", missingAchievementName);
                        msg_delay(">   Added Description: %d \n", missingAchievementDescription);
                        msg_delay(">   Added Criteria Description: %d \n", missingAchievementCriteriaDescription);

                        msg_delay(">   Updated Name: %d \n", updatedAchievementName);
                        msg_delay(">   Updated Description: %d \n", updatedAchievementDescription);
                        msg_delay(">   Updated Criteria Description: %d \n", updatedAchievementCriteriaDescription);
                    }
                }
                // ACTION 2 END
                return 1;
            }
            // Auto replace <name> <class> <race> etc
            if (action == 3)
            {
                int maxQuestId[MAX_EXPANSION];
                maxQuestId[0] = 0;
                maxQuestId[1] = 0;
                maxQuestId[2] = 0;

                int maxItemId[MAX_EXPANSION];
                maxItemId[0] = 0;
                maxItemId[1] = 0;
                maxItemId[2] = 0;

                int maxGameObjectId[MAX_EXPANSION];
                maxGameObjectId[0] = 0;
                maxGameObjectId[1] = 0;
                maxGameObjectId[2] = 0;

                uint32 checkLocalesOption = 0;
                std::cout << "\nSelect what entries to fix: \n";
                std::cout << "1) All \n";
                std::cout << "2) Quests \n";
                std::cout << "3) Items \n";
                std::cout << "4) Game Objects \n";
                std::cin >> checkLocalesOption;

                // print db data
                msg_delay("\n");
                msg_delay("> DB: \n");

                int questCounter = 0;
                int itemCounter = 0;
                int gameObjectCounter = 0;

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

                    /*int questCounter = 0;
                    DbConnect->GetDbInt("SELECT COUNT(*) FROM quest_template", questCounter);
                    DbConnect->GetDbInt("SELECT MAX(entry) FROM quest_template", maxQuestId[e - 1]);
                    msg_delay("> DB: Quests %s locale, total: %d, max id: %d \n", localeName(1).c_str(), questCounter, maxQuestId[e - 1]);*/
                    //msg_delay("> DB: Quests translated (Title OR Details != NULL): \n");

                    for (auto i = 2; i <= MAX_LOCALE; ++i) // locales
                    {
                        if (locale && locale != i)
                            continue;

                        int trCount = 0;
                        DbConnect->GetDbInt("SELECT COUNT(*) FROM locales_quest WHERE Title_loc" + std::to_string(localeRealId(i)) + " IS NOT NULL"
                                                                                                                                     " OR Details_loc" + std::to_string(localeRealId(i)) + " IS NOT NULL"
                                                                                                                                     " OR Objectives_loc" + std::to_string(localeRealId(i)) + " IS NOT NULL", trCount);
                        //msg_delay(">  %s %d \n", localeName(i).c_str(), trCount);
                        /*if (locale && trCount == 0)
                        {
                            msg_delay("> DB: no %s translations found in database! \n", localeName(i).c_str());
                            return 0;
                        }*/
                    }

                    questCounter = 0;
                    if (checkLocalesOption == 1 || checkLocalesOption == 2)
                    {
                        DbConnect->GetDbInt("SELECT COUNT(*) FROM quest_template", questCounter);
                        DbConnect->GetDbInt("SELECT MAX(entry) FROM quest_template", maxQuestId[e - 1]);
                        msg_delay("> DB: Quests %s locale, total: %d, max id: %d \n", localeName(1).c_str(), questCounter, maxQuestId[e - 1]);
                    }

                    itemCounter = 0;
                    if (checkLocalesOption == 1 || checkLocalesOption == 3)
                    {
                        DbConnect->GetDbInt("SELECT COUNT(*) FROM item_template", itemCounter);
                        DbConnect->GetDbInt("SELECT MAX(entry) FROM item_template", maxItemId[e - 1]);
                        msg_delay("> DB: Items %s locale, total: %d, max id: %d \n", localeName(1).c_str(), itemCounter, maxItemId[e - 1]);
                    }

                    gameObjectCounter = 0;
                    if (checkLocalesOption == 1 || checkLocalesOption == 4)
                    {
                        DbConnect->GetDbInt("SELECT COUNT(*) FROM gameobject_template", gameObjectCounter);
                        DbConnect->GetDbInt("SELECT MAX(entry) FROM gameobject_template", maxGameObjectId[e - 1]);
                        msg_delay("> DB: Game Objects %s locale, total: %d, max id: %d \n", localeName(1).c_str(), gameObjectCounter, maxGameObjectId[e - 1]);
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

                    //msg_delay("> DB: Fixing wildcards... \n");

                    // read existing update file
                    std::string filesLocation = "work/" + sDataMgr.getProjectName() + "/" + expansionName(expansion) + "/" + typeName(TYPE_QUEST) + "/";
                    //std::string updateStmt = readFromFile(filesLocation + "missingEngTexts.txt");
                    //writeToFile("-- QUERIES GO BELOW\n", "missingTags", filesLocation);
                    std::string updateQueries;
                    uint32 addedNameTags = 0;
                    uint32 addedClassTags = 0;
                    uint32 addedGenderTags = 0;
                    uint32 addedRaceTags = 0;

                    uint32 counter = 0;

                    // Quests
                    if (checkLocalesOption == 1 || checkLocalesOption == 2)
                        msg_delay("> DB: Fixing Quest Tags... \n");

                    for (auto d = 1; d <= maxQuestId[e - 1]; ++d)
                    {
                        if (!(checkLocalesOption == 1 || checkLocalesOption == 2))
                            break;

                        DatabaseQuestInfo* qDbInfo = LoadDatabaseQuestInfo(d, e, locale, true, true);
                        if (!qDbInfo || !qDbInfo->IsLoaded(e, locale))
                        {
                            //msg_delay("> DB " + typeName(TYPE_QUEST) + " #" + std::to_string(entry) + "-" + localeName(locale) + ": Failed to load from Database! \n");
                            continue;
                        }
                        else
                        {
                            /*counter++;
                            //msg_nodelay(".");// msg_delay_set(".", 50);
                            if ((counter % 1000) == 0)
                                msg_nodelay(std::to_string(counter));
                            else if ((counter % 100) == 0)
                                msg_nodelay(".");*/

                            counter++;
                            if ((counter % 10) == 0)
                            {
                                std::stringstream message;
                                uint32 iterationPct = (counter * 100) / questCounter;
                                message << "   " << std::to_string(iterationPct) << "%c (" << std::to_string(counter) << "/" + std::to_string(questCounter) << ")\n";
                                msg_nodelay(message.str(), '\u0025');
                            }

                            // logic goes here

                            // Name
                            std::string details = qDbInfo->GetDetails(e, locale);
                            std::string offer = qDbInfo->GetOfferRewardText(e, locale);
                            std::string req = qDbInfo->GetRequestItemText(e, locale);
                            if (hasNameTag(details, false, e, locale))
                            {
                                std::string replace = details;
                                StringHelper::ReplaceNameTag(details, e, locale);
                                if (replace != details)
                                    addedNameTags++;
                            }
                            if (hasNameTag(offer, false, e, locale))
                            {
                                std::string replace = offer;
                                StringHelper::ReplaceNameTag(offer, e, locale);
                                if (replace != offer)
                                    addedNameTags++;
                            }
                            if (hasNameTag(req, false, e, locale))
                            {
                                std::string replace = req;
                                StringHelper::ReplaceNameTag(req, e, locale);
                                if (replace != req)
                                    addedNameTags++;
                            }

                            // Race
                            if (hasRaceTag(details, false, e, locale))
                            {
                                std::string replace = details;
                                StringHelper::ReplaceRaceTag(details, e, locale);
                                if (replace != details)
                                    addedRaceTags++;
                            }
                            if (hasRaceTag(offer, false, e, locale))
                            {
                                std::string replace = offer;
                                StringHelper::ReplaceRaceTag(offer, e, locale);
                                if (replace != offer)
                                    addedRaceTags++;
                            }
                            if (hasRaceTag(req, false, e, locale))
                            {
                                std::string replace = req;
                                StringHelper::ReplaceRaceTag(req, e, locale);
                                if (replace != req)
                                    addedRaceTags++;
                            }

                            // Class
                            if (hasClassTag(details, false, e, locale))
                            {
                                std::string replace = details;
                                StringHelper::ReplaceClassTag(details, e, locale);
                                if (replace != details)
                                    addedClassTags++;
                            }
                            if (hasClassTag(offer, false, e, locale))
                            {
                                std::string replace = offer;
                                StringHelper::ReplaceClassTag(offer, e, locale);
                                if (replace != offer)
                                    addedClassTags++;
                            }
                            if (hasClassTag(req, false, e, locale))
                            {
                                std::string replace = req;
                                StringHelper::ReplaceClassTag(req, e, locale);
                                if (replace != req)
                                    addedClassTags++;
                            }

                            // Gender
                            if (hasGenderTag(details, false, e, locale))
                            {
                                std::string replace = details;
                                StringHelper::ReplaceGenderTag(details, e, locale);
                                if (replace != details)
                                    addedGenderTags++;
                            }
                            if (hasGenderTag(offer, false, e, locale))
                            {
                                std::string replace = offer;
                                StringHelper::ReplaceGenderTag(offer, e, locale);
                                if (replace != offer)
                                    addedGenderTags++;
                            }
                            if (hasGenderTag(req, false, e, locale))
                            {
                                std::string replace = req;
                                StringHelper::ReplaceGenderTag(req, e, locale);
                                if (replace != req)
                                    addedGenderTags++;
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

                    // Items
                    if (checkLocalesOption == 1 || checkLocalesOption == 3)
                        msg_delay("> DB: Fixing Items Tags... \n");

                    counter = 0;
                    for (auto d = 1; d <= maxItemId[e - 1]; ++d)
                    {
                        if (!(checkLocalesOption == 1 || checkLocalesOption == 3))
                            break;

                        DatabaseItemInfo* itemDbInfo = LoadDatabaseItemInfo(d, e, locale, true, true);
                        if (!itemDbInfo || !itemDbInfo->IsLoaded(e, locale))
                        {
                            continue;
                        }
                        else
                        {
                            /*counter++;
                            //msg_nodelay(".");// msg_delay_set(".", 50);
                            if ((counter % 1000) == 0)
                                msg_nodelay(std::to_string(counter));
                            else if ((counter % 100) == 0)
                                msg_nodelay(".");*/

                            counter++;
                            if ((counter % 10) == 0)
                            {
                                std::stringstream message;
                                uint32 iterationPct = (counter * 100) / itemCounter;
                                message << "   " << std::to_string(iterationPct) << "%c (" << std::to_string(counter) << "/" + std::to_string(itemCounter) << ")\n";
                                msg_nodelay(message.str(), '\u0025');
                            }

                            // logic goes here
                            std::string name = itemDbInfo->GetName();
                            std::string description = itemDbInfo->GetDescription();
                            std::vector<PageEntry> pages = itemDbInfo->GetPages();

                            // Name
                            if (hasNameTag(name, false, e, locale))
                            {
                                std::string replace = name;
                                StringHelper::ReplaceNameTag(name, e, locale);
                                if (replace != name)
                                    addedNameTags++;
                            }
                            if (hasNameTag(description, false, e, locale))
                            {
                                std::string replace = description;
                                StringHelper::ReplaceNameTag(description, e, locale);
                                if (replace != description)
                                    addedNameTags++;
                            }

                            for (PageEntry& page : pages)
                            {
                                if (hasNameTag(page.text, false, e, locale))
                                {
                                    std::string replace = page.text;
                                    StringHelper::ReplaceNameTag(page.text, e, locale);
                                    if (replace != page.text)
                                        addedNameTags++;
                                }
                            }

                            // Race
                            if (hasRaceTag(name, false, e, locale))
                            {
                                std::string replace = name;
                                StringHelper::ReplaceRaceTag(name, e, locale);
                                if (replace != name)
                                    addedRaceTags++;
                            }
                            if (hasRaceTag(description, false, e, locale))
                            {
                                std::string replace = description;
                                StringHelper::ReplaceRaceTag(description, e, locale);
                                if (replace != description)
                                    addedRaceTags++;
                            }

                            for (PageEntry& page : pages)
                            {
                                if (hasRaceTag(page.text, false, e, locale))
                                {
                                    std::string replace = page.text;
                                    StringHelper::ReplaceRaceTag(page.text, e, locale);
                                    if (replace != page.text)
                                        addedRaceTags++;
                                }
                            }

                            // Class
                            if (hasClassTag(name, false, e, locale))
                            {
                                std::string replace = name;
                                StringHelper::ReplaceClassTag(name, e, locale);
                                if (replace != name)
                                    addedClassTags++;
                            }
                            if (hasClassTag(description, false, e, locale))
                            {
                                std::string replace = description;
                                StringHelper::ReplaceClassTag(description, e, locale);
                                if (replace != description)
                                    addedClassTags++;
                            }

                            for (PageEntry& page : pages)
                            {
                                if (hasClassTag(page.text, false, e, locale))
                                {
                                    std::string replace = page.text;
                                    StringHelper::ReplaceClassTag(page.text, e, locale);
                                    if (replace != page.text)
                                        addedClassTags++;
                                }
                            }

                            // Gender
                            if (hasGenderTag(name, false, e, locale))
                            {
                                std::string replace = name;
                                StringHelper::ReplaceGenderTag(name, e, locale);
                                if (replace != name)
                                    addedGenderTags++;
                            }
                            if (hasGenderTag(description, false, e, locale))
                            {
                                std::string replace = description;
                                StringHelper::ReplaceGenderTag(description, e, locale);
                                if (replace != description)
                                    addedGenderTags++;
                            }

                            for (PageEntry& page : pages)
                            {
                                if (hasGenderTag(page.text, false, e, locale))
                                {
                                    std::string replace = page.text;
                                    StringHelper::ReplaceGenderTag(page.text, e, locale);
                                    if (replace != page.text)
                                        addedGenderTags++;
                                }
                            }

                            // print queries if needed
                            if (name != itemDbInfo->GetName())
                                updateQueries += updateItemFromTextQueryNoTags(name, itemDbInfo, "name", e, locale) + "\n";
                            if (description != itemDbInfo->GetDescription())
                                updateQueries += updateItemFromTextQueryNoTags(description, itemDbInfo, "description", e, locale) + "\n";
                        
                            const std::vector<PageEntry>& originalPages = itemDbInfo->GetPages();
                            for (uint8 i = 0; i < originalPages.size(); i++)
                            {
                                if (pages[i].text != originalPages[i].text)
                                {
                                    updateQueries += updateItemPageFromTextQueryNoTags(pages[i].text, pages[i].entry, itemDbInfo, e, locale) + "\n";
                                }
                            }
                        }
                    }

                    // Game Objects
                    if (checkLocalesOption == 1 || checkLocalesOption == 4)
                        msg_delay("> DB: Fixing Game Object Tags... \n");

                    counter = 0;
                    for (auto d = 1; d <= maxItemId[e - 1]; ++d)
                    {
                        if (!(checkLocalesOption == 1 || checkLocalesOption == 4))
                            break;

                        DatabaseGameObjectInfo* gameObjectDbInfo = LoadDatabaseGameObjectInfo(d, e, locale, true, true);
                        if (!gameObjectDbInfo || !gameObjectDbInfo->IsLoaded(e, locale))
                        {
                            continue;
                        }
                        else
                        {
                            /*counter++;
                            //msg_nodelay(".");// msg_delay_set(".", 50);
                            if ((counter % 1000) == 0)
                                msg_nodelay(std::to_string(counter));
                            else if ((counter % 100) == 0)
                                msg_nodelay(".");*/

                            counter++;
                            if ((counter % 10) == 0)
                            {
                                std::stringstream message;
                                uint32 iterationPct = (counter * 100) / gameObjectCounter;
                                message << "   " << std::to_string(iterationPct) << "%c (" << std::to_string(counter) << "/" + std::to_string(gameObjectCounter) << ")\n";
                                msg_nodelay(message.str(), '\u0025');
                            }

                            // logic goes here
                            std::string name = gameObjectDbInfo->GetName();
                            std::vector<PageEntry> pages = gameObjectDbInfo->GetPages();

                            // Name
                            if (hasNameTag(name, false, e, locale))
                            {
                                std::string replace = name;
                                StringHelper::ReplaceNameTag(name, e, locale);
                                if (replace != name)
                                    addedNameTags++;
                            }

                            for (PageEntry& page : pages)
                            {
                                if (hasNameTag(page.text, false, e, locale))
                                {
                                    std::string replace = page.text;
                                    StringHelper::ReplaceNameTag(page.text, e, locale);
                                    if (replace != page.text)
                                        addedNameTags++;
                                }
                            }

                            // Race
                            if (hasRaceTag(name, false, e, locale))
                            {
                                std::string replace = name;
                                StringHelper::ReplaceRaceTag(name, e, locale);
                                if (replace != name)
                                    addedRaceTags++;
                            }

                            for (PageEntry& page : pages)
                            {
                                if (hasRaceTag(page.text, false, e, locale))
                                {
                                    std::string replace = page.text;
                                    StringHelper::ReplaceRaceTag(page.text, e, locale);
                                    if (replace != page.text)
                                        addedRaceTags++;
                                }
                            }

                            // Class
                            if (hasClassTag(name, false, e, locale))
                            {
                                std::string replace = name;
                                StringHelper::ReplaceClassTag(name, e, locale);
                                if (replace != name)
                                    addedClassTags++;
                            }

                            for (PageEntry& page : pages)
                            {
                                if (hasClassTag(page.text, false, e, locale))
                                {
                                    std::string replace = page.text;
                                    StringHelper::ReplaceClassTag(page.text, e, locale);
                                    if (replace != page.text)
                                        addedClassTags++;
                                }
                            }

                            // Gender
                            if (hasGenderTag(name, false, e, locale))
                            {
                                std::string replace = name;
                                StringHelper::ReplaceGenderTag(name, e, locale);
                                if (replace != name)
                                    addedGenderTags++;
                            }

                            for (PageEntry& page : pages)
                            {
                                if (hasGenderTag(page.text, false, e, locale))
                                {
                                    std::string replace = page.text;
                                    StringHelper::ReplaceGenderTag(page.text, e, locale);
                                    if (replace != page.text)
                                        addedGenderTags++;
                                }
                            }

                            // print queries if needed
                            if (name != gameObjectDbInfo->GetName())
                                updateQueries += updateGameObjectFromTextQueryNoTags(name, gameObjectDbInfo, "name", e, locale) + "\n";

                            const std::vector<PageEntry>& originalPages = gameObjectDbInfo->GetPages();
                            for (uint8 i = 0; i < originalPages.size(); i++)
                            {
                                if (pages[i].text != originalPages[i].text)
                                {
                                    updateQueries += updateGameObjectPageFromTextQueryNoTags(pages[i].text, pages[i].entry, gameObjectDbInfo, e, locale) + "\n";
                                }
                            }
                        }
                    }

                    // print results
                    msg_delay("\n\n");

                    msg_delay(">   Added $N: %d \n", addedNameTags);
                    msg_delay(">   Added $R: %d \n", addedRaceTags);
                    msg_delay(">   Added $C: %d \n", addedClassTags);
                    msg_delay(">   Added $G: %d \n", addedGenderTags);

                    // write queries to file
                    if (!updateQueries.empty())
                        writeToFile(updateQueries.c_str(), "missingTags_" + localeName(locale) + ".sql", filesLocation);
                }
                // ACTION 3 END
                return 1;
            }
            // Auto add missing translations from other expansions
            if (action == 4)
            {
                // Choose locale
                bool checkDiff = true;
                uint32 inputCheck = 0;
                std::cout << "\nCheck for English text equality?: \n";
                std::cout << "1) Yes \n";
                std::cout << "2) No \n";
                std::cin >> inputCheck;
                if (!inputCheck || inputCheck == 1)
                    checkDiff = true;
                else
                    checkDiff = false;

                int maxQuestId[MAX_EXPANSION];
                maxQuestId[0] = 0;
                maxQuestId[1] = 0;
                maxQuestId[2] = 0;

                // print db data
                msg_delay("\n");
                msg_delay("> DB: \n");

                for (auto e = 1; e <= MAX_EXPANSION; ++e) // expansion
                {
                    //if (expansion && expansion != e)
                    //    continue;

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

                    msg_delay("\n> %s \n", expansionName(e).c_str());

                    msg_delay("> DB: Checking other expansions... \n");

                    // read existing update file
                    std::string filesLocation = "work/" + sDataMgr.getProjectName() + "/" + expansionName(expansion) + "/" + typeName(TYPE_QUEST) + "/";
                    //std::string updateStmt = readFromFile(filesLocation + "missingEngTexts.txt");
                    //writeToFile("-- QUERIES GO BELOW\n", "missingTags", filesLocation);
                    std::string updateQueries;
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

                    uint32 counter = 0;
                    for (auto d = 1; d <= maxQuestId[e - 1]; ++d)
                    {
                        // initialize all 3 expansions
                        DatabaseQuestInfo* qDbLocInfo = LoadDatabaseQuestInfo(d, e, locale, true, true);
                        if (!qDbLocInfo || !qDbLocInfo->IsLoaded(e, locale))
                        {
                            //msg_delay("> DB " + typeName(TYPE_QUEST) + " #" + std::to_string(entry) + "-" + localeName(locale) + ": Failed to load from Database! \n");
                            continue;

                        }
                        else
                        {
                            // load EN
                            qDbLocInfo->LoadEntryData(1, 1);
                            qDbLocInfo->LoadEntryData(2, 1);
                            qDbLocInfo->LoadEntryData(3, 1);
                            qDbLocInfo->LoadEntryData(1, locale);
                            qDbLocInfo->LoadEntryData(2, locale);
                            qDbLocInfo->LoadEntryData(3, locale);
                            // set here othwerwise is 1
                            qDbLocInfo->SetCurExpansion(e);
                            qDbLocInfo->SetCurLocale(locale);

                            counter++;
                            //msg_nodelay(".");// msg_delay_set(".", 50);
                            if ((counter % 1000) == 0)
                                msg_nodelay(std::to_string(counter) + "\n");
                            else if ((counter % 100) == 0)
                                msg_nodelay(".");

                            // logic goes here

                            if (!qDbLocInfo->GetTitle(e, 1).empty() && qDbLocInfo->GetTitle().empty())
                            {
                                std::string replace = FindAltText(qDbLocInfo, "title", e, locale, checkDiff);
                                if (replace.empty())
                                {
                                    //msg_nodelay("x");
                                }
                                else
                                {
                                    msg_nodelay("v");
                                    missingTitle++;
                                    updateQueries += updateQuestFromTextQuery(replace,qDbLocInfo, "title", e, locale) + "\n";
                                }
                            }

                            if (!qDbLocInfo->GetEndText(e, 1).empty() && qDbLocInfo->GetEndText().empty())
                            {
                                std::string replace = FindAltText(qDbLocInfo, "endText", e, locale, checkDiff);
                                if (replace.empty())
                                {
                                    //msg_nodelay("x");
                                }
                                else
                                {
                                    msg_nodelay("v");
                                    missingEndText++;
                                    updateQueries += updateQuestFromTextQuery(replace,qDbLocInfo, "endText", e, locale) + "\n";
                                }
                            }

                            if (!qDbLocInfo->GetObjectives(e, 1).empty() && qDbLocInfo->GetObjectives().empty())
                            {
                                std::string replace = FindAltText(qDbLocInfo, "objectives", e, locale, checkDiff);
                                if (replace.empty())
                                {
                                    //msg_nodelay("x");
                                }
                                else
                                {
                                    msg_nodelay("v");
                                    missingObjText++;
                                    updateQueries += updateQuestFromTextQuery(replace,qDbLocInfo, "objectives", e, locale) + "\n";
                                }
                            }

                            if (!qDbLocInfo->GetRequestItemText(e, 1).empty() && qDbLocInfo->GetRequestItemText().empty())
                            {
                                std::string replace = FindAltText(qDbLocInfo, "requestItemText", e, locale, checkDiff);
                                if (replace.empty())
                                {
                                    //msg_nodelay("x");
                                }
                                else
                                {
                                    msg_nodelay("v");
                                    missingReqText++;
                                    updateQueries += updateQuestFromTextQuery(replace,qDbLocInfo, "requestItemText", e, locale) + "\n";
                                }
                            }

                            if (!qDbLocInfo->GetOfferRewardText(e, 1).empty() && qDbLocInfo->GetOfferRewardText().empty())
                            {
                                std::string replace = FindAltText(qDbLocInfo, "offerRewardText", e, locale, checkDiff);
                                if (replace.empty())
                                {
                                    //msg_nodelay("x");
                                }
                                else
                                {
                                    msg_nodelay("v");
                                    missingOfferText++;
                                    updateQueries += updateQuestFromTextQuery(replace,qDbLocInfo, "offerRewardText", e, locale) + "\n";
                                }
                            }

                            if (!qDbLocInfo->GetDetails(e, 1).empty() && qDbLocInfo->GetDetails().empty())
                            {
                                std::string replace = FindAltText(qDbLocInfo, "details", e, locale, checkDiff);
                                if (replace.empty())
                                {
                                    //msg_nodelay("x");
                                }
                                else
                                {
                                    msg_nodelay("v");
                                    missingDetails++;
                                    updateQueries += updateQuestFromTextQuery(replace, qDbLocInfo, "details", e, locale) + "\n";
                                }
                            }

                            // Objective 1-4
                            if (!qDbLocInfo->GetObjective(0, e, 1).empty() && qDbLocInfo->GetObjective(0).empty())
                            {
                                std::string replace = FindAltText(qDbLocInfo, "objectiveText1", e, locale, checkDiff);
                                if (replace.empty())
                                {
                                    //msg_nodelay("x");
                                }
                                else
                                {
                                    msg_nodelay("v");
                                    missingObjective1++;
                                    updateQueries += updateQuestFromTextQuery(replace, qDbLocInfo, "objectiveText1", e, locale) + "\n";
                                }
                            }

                            if (!qDbLocInfo->GetObjective(1, e, 1).empty() && qDbLocInfo->GetObjective(1).empty())
                            {
                                std::string replace = FindAltText(qDbLocInfo, "objectiveText2", e, locale, checkDiff);
                                if (replace.empty())
                                {
                                    //msg_nodelay("x");
                                }
                                else
                                {
                                    msg_nodelay("v");
                                    missingObjective2++;
                                    updateQueries += updateQuestFromTextQuery(replace, qDbLocInfo, "objectiveText2", e, locale) + "\n";
                                }
                            }

                            if (!qDbLocInfo->GetObjective(2, e, 1).empty() && qDbLocInfo->GetObjective(2).empty())
                            {
                                std::string replace = FindAltText(qDbLocInfo, "objectiveText3", e, locale, checkDiff);
                                if (replace.empty())
                                {
                                    //msg_nodelay("x");
                                }
                                else
                                {
                                    msg_nodelay("v");
                                    missingObjective3++;
                                    updateQueries += updateQuestFromTextQuery(replace, qDbLocInfo, "objectiveText3", e, locale) + "\n";
                                }
                            }

                            if (!qDbLocInfo->GetObjective(3, e, 1).empty() && qDbLocInfo->GetObjective(3).empty())
                            {
                                std::string replace = FindAltText(qDbLocInfo, "objectiveText4", e, locale, checkDiff);
                                if (replace.empty())
                                {
                                    //msg_nodelay("x");
                                }
                                else
                                {
                                    msg_nodelay("v");
                                    missingObjective4++;
                                    updateQueries += updateQuestFromTextQuery(replace, qDbLocInfo, "objectiveText4", e, locale) + "\n";
                                }
                            }
                        }


                    }

                    // print results
                    msg_delay("\n\n");


                    msg_delay(">   Added Title: %d \n", missingTitle);
                    msg_delay(">   Added Objectives: %d \n", missingObjText);
                    msg_delay(">   Added Details: %d \n", missingDetails);
                    msg_delay(">   Added RequestItemText: %d \n", missingReqText);
                    msg_delay(">   Added OfferRewardText: %d \n", missingOfferText);
                    msg_delay(">   Added EndText: %d \n", missingEndText);
                    msg_delay(">   Added Objective1-4: %d - %d - %d - %d \n", missingObjective1, missingObjective2, missingObjective3, missingObjective4);

                    // write queries to file
                    if (!updateQueries.empty())
                        writeToFile(updateQueries.c_str(), checkDiff ? "missingFromOtherExp_" + localeName(locale) + ".sql" : "missingFromOtherExpIncludeDiffEng_" + localeName(locale) + ".sql", filesLocation);
                }
                // ACTION 4 END
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

    std::cout << "\n\nFinished!\n";

    return 1;
}

QuestStrings LoadQuestCacheStrings(const std::string& whPage, uint32 locale)
{
    QuestStrings qStrings;

    // parse page
    auto parsedPage = GumboParsePage(whPage);

    // hotfix for reqItemText or progress (works for both but is filtered on English DB check)
    uint32 hotFixId = 0;
    if (whPage.find("classes=\"first\"") != std::string::npos)
        hotFixId = 1;

    // parse details
    qStrings.title = ReadQuestText(parsedPage, "title", locale);
    qStrings.objectives = ReadQuestText(parsedPage, "objectives", locale);
    qStrings.details = ReadQuestText(parsedPage, "details", locale);
    qStrings.requestItemText = ReadQuestText(parsedPage, "requestItemText", locale, hotFixId);
    qStrings.offerRewardText = ReadQuestText(parsedPage, "offerRewardText", locale, hotFixId);
    qStrings.endText = ReadQuestText(parsedPage, "endText", locale);
    // read list of objectives;
    std::string objList = ReadQuestText(parsedPage, "objectiveList", locale);
    if (!objList.empty())
    {
        std::stringstream ss;
        ss << objList;
        std::vector<std::string> result;

        while (ss.good())
        {
            std::string substr;
            getline(ss, substr, ';');
            result.push_back(substr);
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

GameObjectStrings LoadGameObjectCacheStrings(uint32 id, const std::string& whPage, uint32 locale, uint32 expansion)
{
    GameObjectStrings gameObjectStrings;

    // parse page
    auto parsedPage = GumboParsePage(whPage);

    // parse details
    gameObjectStrings.name = ReadGameObjectText(parsedPage, "name", locale);
    gameObjectStrings.pages = ReadGameObjectPages(whPage, locale, expansion, id);

    // clear memory
    GumboClosePage(parsedPage);

    return gameObjectStrings;
}

GameObjectStrings LoadGameObjectDatabaseStrings(uint32 id, uint32 expansion = 1, uint32 locale = 1)
{
    GameObjectStrings gameObjectStrings;

    auto DbConnect = sDataMgr.GetCon(expansion);
    if (!DbConnect || !DbConnect->IsConnected() || !DbConnect->IsEntryExistInDb(TYPE_OBJECT, id))
        return gameObjectStrings;

    return LoadGameObjectFull(DbConnect, id, locale);
}

AchievementStrings LoadAchievementCacheStrings(uint32 id, const std::string& whPage, uint32 locale, uint32 expansion)
{
    AchievementStrings achievementStrings;

    // parse page
    auto parsedPage = GumboParsePage(whPage);

    // parse details
    achievementStrings.name = ReadAchievementText(parsedPage, "name", locale);

    std::string description = StringHelper::GetFirstStringBetween(whPage, "heading-size-1", "heading-size-3");
    description = StringHelper::GetFirstStringBetween(description, "</script>", "<script");
    description = StringHelper::ReplaceString(description, "\n", "");
    achievementStrings.description = StringHelper::ReplaceString(description, "\n", "");

    DatabaseAchievementInfo* achievementDb = LoadDatabaseAchievementInfo(id, expansion, locale, true);
    if (achievementDb)
    {
        achievementStrings.criterias = achievementDb->GetCriterias(expansion, locale);
    }

    // clear memory
    GumboClosePage(parsedPage);

    return achievementStrings;
}

AchievementStrings LoadAchievementDatabaseStrings(uint32 id, uint32 expansion = 1, uint32 locale = 1)
{
    AchievementStrings achievementStrings;

    auto DbConnect = sDataMgr.GetCon(expansion);
    if (!DbConnect || !DbConnect->IsConnected() || !DbConnect->IsEntryExistInDb(TYPE_ACHIEVEMENT, id))
        return achievementStrings;

    return LoadAchievementFull(DbConnect, id, locale);
}

ItemStrings LoadItemCacheStrings(uint32 id, const std::string& whPage, uint32 locale, uint32 expansion)
{
    ItemStrings itemStrings;

    // parse page
    auto parsedPage = GumboParsePage(whPage);

    // parse details
    itemStrings.name = ReadItemText(parsedPage, "name", locale);
    itemStrings.description = ReadItemText(parsedPage, "description", locale);
    itemStrings.pages = ReadItemPages(whPage, locale, expansion, id);

    // clear memory
    GumboClosePage(parsedPage);

    return itemStrings;
}

ItemStrings LoadItemDatabaseStrings(uint32 id, uint32 expansion = 1, uint32 locale = 1)
{
    ItemStrings itemStrings;

    auto DbConnect = sDataMgr.GetCon(expansion);
    if (!DbConnect || !DbConnect->IsConnected() || !DbConnect->IsEntryExistInDb(TYPE_ITEM, id))
        return itemStrings;

    return LoadItemFull(DbConnect, id, locale);
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
        return GetObjectives(expansion, locale);
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
    else
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

    std::string host = "127.0.0.1";
    std::string port = "3310";
    std::string user = "root";
    std::string password = "123456";
    std::string projectName = "cmangos";

    std::string confStr = readFromFile("wh_parse.conf");
    if (!confStr.empty())
        msg_delay("\nConfig file found, reading parameters...\n");
    //const char config = confStr;

    std::istringstream is_file(confStr);

    std::string line;
    while( std::getline(is_file, line) )
    {
        std::istringstream is_line(line);
        std::string key;
        if( std::getline(is_line, key, '=') )
        {
            std::string value;
            if( std::getline(is_line, value) )
            {
                //store_line(key, value);
                if (key == "project")
                    projectInfo.projectName = value;
                if (key == "host")
                    projectInfo.host = value;
                if (key == "port")
                    projectInfo.port = value;
                if (key == "user")
                    projectInfo.user = value;
                if (key == "password")
                    projectInfo.password = value;
            }
        }
    }

    msg_delay("\n> DataManager: Initializing...");
    msg_delay("\n>   Project: %s", projectInfo.projectName.c_str());
    msg_delay("\n>   DB Host: %s", projectInfo.host.c_str());
    msg_delay("\n>   DB Port: %s", projectInfo.port.c_str());
    msg_delay("\n>   DB User: %s", projectInfo.user.c_str());
    msg_delay("\n>   DB Pass: %s", projectInfo.password.c_str());
    msg_delay("\n");

    for (auto i = 0; i < MAX_EXPANSION; ++i)
    {
        msg_delay("\n>   DB Connect: %s - ", dbName(i + 1, projectInfo.projectName));
        auto dbCon = new DatabaseConnect(projectInfo.host.c_str(), stoi(projectInfo.port), projectInfo.user.c_str(), projectInfo.password.c_str(), dbName(i + 1, projectInfo.projectName));
        if (!dbCon->IsConnected())
        {
            delete dbCon;
        }
        else
        {
            msg_delay("Success!\n", dbName(i + 1, projectInfo.projectName));
            isInitialized = true;
            mysqlCon[i] = dbCon;
        }
    }
    msg_delay("\n");

    // fill quest database entries with nullptr
    for (auto i = 1; i < 30000; ++i)
    {
        questDatabaseInfoList[i] = nullptr;
        questWowheadInfoList[i] = nullptr;
    }
}

std::string ItemInfo::GetName(uint32 expansion /*= 0*/, uint32 locale /*= 0*/)
{
    if (expansion == 0 || locale == 0)
        return itemTexts[GetCurExpansion()][GetCurLocale()].name;
    else
        return itemTexts[expansion][locale].name;
}

std::string ItemInfo::GetDescription(uint32 expansion /*= 0*/, uint32 locale /*= 0*/)
{
    if (expansion == 0 || locale == 0)
        return itemTexts[GetCurExpansion()][GetCurLocale()].description;
    else
        return itemTexts[expansion][locale].description;
}

const std::vector<PageEntry>& ItemInfo::GetPages(uint32 expansion /*= 0*/, uint32 locale /*= 0*/)
{
    if (expansion == 0 || locale == 0)
        return itemTexts[GetCurExpansion()][GetCurLocale()].pages;
    else
        return itemTexts[expansion][locale].pages;
}

std::string ItemInfo::GetItemPart(const std::string& iPart, uint32 expansion /*= 0*/, uint32 locale /*= 0*/)
{
    if (iPart == "name")
        return GetName(expansion, locale);
    if (iPart == "description")
        return GetDescription(expansion, locale);

    return "";
}

void WowheadItemInfo::LoadEntryData(uint32 expansion, uint32 locale)
{
    std::string cachePage = loadPageOrCache(TYPE_ITEM, GetEntry(), expansion, locale, true);
    if (cachePage.empty())
        return;

    SetItemTexts(expansion, locale, LoadItemCacheStrings(GetEntry(), cachePage, locale, expansion));
    SetLoaded(expansion, locale, true);
}

void DatabaseItemInfo::LoadEntryData(uint32 expansion, uint32 locale)
{
    auto DbConnect = sDataMgr.GetCon(expansion);
    if (DbConnect && DbConnect->IsConnected() && !DbConnect->IsEntryExistInDb(TYPE_ITEM, GetEntry()))
        return;

    ItemStrings itemStrings = LoadItemDatabaseStrings(GetEntry(), expansion, locale);
    SetItemTexts(expansion, locale, itemStrings);
    SetLoaded(expansion, locale, true);
}

bool IsMissingPages(const std::vector<PageEntry>& version1, const std::vector<PageEntry>& version2, std::vector<PageEntry>* version1MissingPages /*= nullptr*/, std::vector<PageEntry>* version2MissingPages /*= nullptr*/)
{
    bool missingPages = true;

    // Check for the missing pages on version2
    for (const PageEntry& pageVersion1 : version1)
    {
        bool found = false;
        for (const PageEntry& pageVersion2 : version2)
        {
            if (pageVersion1.entry == pageVersion2.entry)
            {
                found = true;
            }
        }

        if (!found && version2MissingPages)
        {
            version2MissingPages->push_back(pageVersion1);
        }
    }

    // Check for the missing pages on version1
    for (const PageEntry& pageVersion2 : version2)
    {
        bool found = false;
        for (const PageEntry& pageVersion1 : version1)
        {
            if (pageVersion1.entry == pageVersion2.entry)
            {
                found = true;
            }
        }

        if (!found && version1MissingPages)
        {
            version1MissingPages->push_back(pageVersion2);
        }
    }

    return missingPages;
}

bool HasSamePages(const std::vector<PageEntry>& version1, const std::vector<PageEntry>& version2, std::vector<std::pair<PageEntry, PageEntry>>* differentPages /*= nullptr*/)
{
    bool samePages = version1.size() == version2.size();
    if (samePages)
    {
        for (uint8 i = 0; i < version1.size(); i++)
        {
            if (!IsSameString(version1[i].text, version2[i].text))
            {
                samePages = false;
                if (differentPages)
                {
                    differentPages->push_back(std::make_pair(version1[i], version2[i]));
                }
            }
        }
    }

    return samePages;
}

bool HasSameCriterias(const std::vector<AchievementCriteria>& version1, const std::vector<AchievementCriteria>& version2, std::vector<std::pair<AchievementCriteria, AchievementCriteria>>* differentCriteria /*= nullptr*/)
{
    bool sameCriterias = version1.size() == version2.size();
    if (sameCriterias)
    {
        for (uint8 i = 0; i < version1.size(); i++)
        {
            if (!IsSameString(version1[i].description, version2[i].description))
            {
                sameCriterias = false;
                if (differentCriteria)
                {
                    differentCriteria->push_back(std::make_pair(version1[i], version2[i]));
                }
            }
        }
    }

    return sameCriterias;
}

std::string GameObjectInfo::GetName(uint32 expansion /*= 0*/, uint32 locale /*= 0*/)
{
    if (expansion == 0 || locale == 0)
        return gameObjectTexts[GetCurExpansion()][GetCurLocale()].name;
    else
        return gameObjectTexts[expansion][locale].name;
}

const std::vector<PageEntry>& GameObjectInfo::GetPages(uint32 expansion /*= 0*/, uint32 locale /*= 0*/)
{
    if (expansion == 0 || locale == 0)
        return gameObjectTexts[GetCurExpansion()][GetCurLocale()].pages;
    else
        return gameObjectTexts[expansion][locale].pages;
}

std::string GameObjectInfo::GetGameObjectPart(const std::string& iPart, uint32 expansion /*= 0*/, uint32 locale /*= 0*/)
{
    if (iPart == "name")
        return GetName(expansion, locale);

    return "";
}

void WowheadGameObjectInfo::LoadEntryData(uint32 expansion, uint32 locale)
{
    std::string cachePage = loadPageOrCache(TYPE_OBJECT, GetEntry(), expansion, locale, true);
    if (cachePage.empty())
        return;

    SetGameObjectTexts(expansion, locale, LoadGameObjectCacheStrings(GetEntry(), cachePage, locale, expansion));
    SetLoaded(expansion, locale, true);
}

void DatabaseGameObjectInfo::LoadEntryData(uint32 expansion, uint32 locale)
{
    auto DbConnect = sDataMgr.GetCon(expansion);
    if (DbConnect && DbConnect->IsConnected() && !DbConnect->IsEntryExistInDb(TYPE_OBJECT, GetEntry()))
        return;

    GameObjectStrings gameObjectStrings = LoadGameObjectDatabaseStrings(GetEntry(), expansion, locale);
    SetGameObjectTexts(expansion, locale, gameObjectStrings);
    SetLoaded(expansion, locale, true);
}

std::string AchievementInfo::GetName(uint32 expansion /*= 0*/, uint32 locale /*= 0*/)
{
    if (expansion == 0 || locale == 0)
        return achievementTexts[GetCurExpansion()][GetCurLocale()].name;
    else
        return achievementTexts[expansion][locale].name;
}

std::string AchievementInfo::GetDescription(uint32 expansion /*= 0*/, uint32 locale /*= 0*/)
{
    if (expansion == 0 || locale == 0)
        return achievementTexts[GetCurExpansion()][GetCurLocale()].description;
    else
        return achievementTexts[expansion][locale].description;
}

const std::vector<AchievementCriteria>& AchievementInfo::GetCriterias(uint32 expansion /*= 0*/, uint32 locale /*= 0*/)
{
    if (expansion == 0 || locale == 0)
        return achievementTexts[GetCurExpansion()][GetCurLocale()].criterias;
    else
        return achievementTexts[expansion][locale].criterias;
}

std::string AchievementInfo::GetAchievementPart(const std::string& iPart, uint32 expansion /*= 0*/, uint32 locale /*= 0*/)
{
    if (iPart == "name")
        return GetName(expansion, locale);
    if (iPart == "description")
        return GetDescription(expansion, locale);

    return "";
}

void WowheadAchievementInfo::LoadEntryData(uint32 expansion, uint32 locale)
{
    std::string cachePage = loadPageOrCache(TYPE_ACHIEVEMENT, GetEntry(), expansion, locale, true);
    if (cachePage.empty())
        return;

    SetAchievementTexts(expansion, locale, LoadAchievementCacheStrings(GetEntry(), cachePage, locale, expansion));
    SetLoaded(expansion, locale, true);
}

void DatabaseAchievementInfo::LoadEntryData(uint32 expansion, uint32 locale)
{
    auto DbConnect = sDataMgr.GetCon(expansion);
    if (DbConnect && DbConnect->IsConnected() && !DbConnect->IsEntryExistInDb(TYPE_ACHIEVEMENT, GetEntry()))
        return;

    AchievementStrings gameObjectStrings = LoadAchievementDatabaseStrings(GetEntry(), expansion, locale);
    SetAchievementTexts(expansion, locale, gameObjectStrings);
    SetLoaded(expansion, locale, true);
}
