#include "Main.h"

#define STRING_SIZE 50
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

std::string typeName(unsigned int typeId)
{
    switch (typeId)
    {
    case 1:
        return "quest";
    case 2:
        return "npc";
    default:
        return "quest";
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
        if (row_count != 1)
        {
            fprintf(stderr, " MySQL failed to return all rows\n");
            exit(0);
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

std::string load_WH_page(CacheType type, unsigned int id, unsigned int& error_code, unsigned int locale = 0, unsigned int expansion = 0)
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

std::string loadPageOrCache(CacheType type, unsigned int id, unsigned int expansion, unsigned int locale, bool silent = false)
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

void cachePagesRange(CacheType type, unsigned int min_id, unsigned int max_id, unsigned int locale = 0, unsigned int expansion = 0)
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
                            query_result << "\n";
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
                                        GumboVector* rowChildren = &tableChild->v.element.children;
                                        for (unsigned int r = 0; r < rowChildren->length; ++r)
                                        {
                                            GumboNode* rowChild = static_cast<GumboNode*>(rowChildren->data[r]);
                                            // Filter Quest Objectives 1-4. They should not have <p style="height:26px">
                                            if (rowChild && preTag == GUMBO_TAG_A)
                                            {
                                                if (rowChild->v.element.tag == GUMBO_TAG_P)
                                                    return NULL;

                                                GumboVector* colChildren = &rowChild->v.element.children;
                                                for (unsigned int c = 0; c < colChildren->length; ++c)
                                                {
                                                    GumboNode* colChild = static_cast<GumboNode*>(colChildren->data[c]);
                                                    if (colChild && colChild->v.element.tag == GUMBO_TAG_P)
                                                        return NULL;

                                                    // filter Provided Item: <a class=q1>
                                                    if (colChild && colChild->v.element.tag == GUMBO_TAG_A)
                                                    {
                                                        GumboAttribute* s_attr = gumbo_get_attribute(&colChild->v.element.attributes, "class");
                                                        if (s_attr && (strcmp(s_attr->value, "q1") == 0))
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
                            if (childNode->v.element.tag == GUMBO_TAG_BR)
                                query_result << "\n";
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

GumboOutput* GumboParsePage(const std::string& page)
{
    return gumbo_parse(page.c_str());
}

void GumboClosePage(GumboOutput* output)
{
    gumbo_destroy_output(&kGumboDefaultOptions, output);
}

WowheadQuestInfo* LoadQuestCache(uint32 id, uint32 expansion = 0, uint32 locale = 0)
{
    if (!id)
        return nullptr;

    auto qInfo = new WowheadQuestInfo(id, expansion, locale);
    qInfo->LoadCacheDataAll();
    if (!qInfo->IsLoaded())
    {
        delete qInfo;
        return nullptr;
    }

    questWowheadInfoList[id] = qInfo;
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


    if (!isCommandLine) // no command line arguments
    {
        // Choose Activity
        std::cout << "Select option: \n";
        std::cout << "1) Test Page Info \n";
        std::cout << "2) Cache Pages \n";
        std::cout << "3) Test DB connect \n";
        std::cout << "4) Cache stats \n";
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

            cachePagesRange(CacheType(type), entry, max_entry, locale, expansion);
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
            msg_delay("> Cache: Collecting data...\n");
            msg_delay("> Cache: Quests cached: \n");
            for (auto e = 1; e < 4 + 1; ++e) // expansion
            {
                msg_delay("\n> %s \n", expansionName(e).c_str());
                for (auto i = 1; i < 7 + 1; ++i) // locales
                {
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
                        }
                    }
                    msg_delay(">  %s %d \n", localeName(i).c_str(), counter);
                }
            }

            unsigned int counter = 0;
            std::map<unsigned int, std::string> englishEndText;
            std::map<unsigned int, std::string> englishObjText;
            std::map<unsigned int, std::string> englishReqText;
            std::map<unsigned int, std::string> englishOfferText;
            unsigned int limit = 10000;
            msg_delay("\n");
            for (auto e = 1; e < 4 + 1; ++e) // expansion
            {
                //if (counter > limit)
                //    break;
                msg_delay("> %s \n", expansionName(e).c_str());

                for (auto i = 1; i < 7 + 1; ++i) // locales
                {
                    // test non eng
                    //if (!(i == 1 || i == 7))
                    //    continue;
                    msg_delay("\n>  %s \n", localeName(i).c_str());

                    unsigned int withClassTag = 0, withRaceTag = 0, hasEndText = 0, hasObjectives = 0, hasOfferText = 0, hasReqText = 0, hasBoth = 0;
                    std::string endObj;
                    unsigned int questBothId = 0;
                    unsigned int hasEnglishEnd = 0;
                    unsigned int hasEnglishObj = 0;
                    unsigned int hasEnglishOffer = 0;
                    unsigned int hasEnglishReq = 0;
                    std::ostringstream missingEndText, missingObjText, missingReqText, missingOfferText;
                    unsigned int missingEndTextNum = 0, missingObjTextNum = 0, missingReqTextNum = 0, missingOfferTextNum = 0;

                    if (counter > limit)
                    {
                        counter = 0;
                        continue;
                    }

                    std::string cacheLocation = "cache/" + expansionName(e) + "/" + localeName(i) + "/quest/";
                    if (!std::filesystem::is_directory(cacheLocation))
                        continue;

                    auto dirIter = std::filesystem::directory_iterator(cacheLocation);
                    for (auto& fl : dirIter)
                    {
                        if (counter > limit)
                        {
                            counter = 0;
                            break;
                        }

                        if (fl.is_regular_file())
                        {
                            std::string wh_page = readFromFile(fl.path().string());
                            if (wh_page.empty())
                                continue; // should not happen

                            const std::filesystem::path& p(fl.path());
                            GumboOutput* parsed_response = gumbo_parse(wh_page.c_str());
                            std::string reqTextStr = parse_quest(parsed_response->root, 4, i);
                            std::string offerTextStr = parse_quest(parsed_response->root, 5, i);
                            std::string endTextStr = parse_quest(parsed_response->root, 6, i);
                            std::string objectivesListStr = parse_quest(parsed_response->root, 7, i);
                            if (i == 1)
                            {
                                if (!endTextStr.empty())
                                    englishEndText[stoi(p.stem().string())] = endTextStr;
                                if (!objectivesListStr.empty())
                                    englishObjText[stoi(p.stem().string())] = objectivesListStr;
                                if (!reqTextStr.empty())
                                    englishReqText[stoi(p.stem().string())] = reqTextStr;
                                if (!offerTextStr.empty())
                                    englishOfferText[stoi(p.stem().string())] = offerTextStr;
                            }
                            else
                            {
                                auto testStr = englishEndText[stoi(p.stem().string())];
                                if (!testStr.empty() && strcmp(testStr.c_str(), endTextStr.c_str()) == 0)
                                {
                                    hasEnglishEnd++;
                                    msg_nodelay("\n   enEnd:" + p.stem().string() + ":");
                                    msg_nodelay(testStr);
                                }
                                if (!testStr.empty() && endTextStr.empty())
                                {
                                    missingEndText << stoi(p.stem().string()) <<  ", ";
                                    missingEndTextNum++;
                                }

                                auto testStr2 = englishObjText[stoi(p.stem().string())];
                                if (!testStr2.empty() && strcmp(testStr2.c_str(), objectivesListStr.c_str()) == 0)
                                {
                                    hasEnglishObj++;
                                    msg_nodelay("\n   enObj:" + p.stem().string() + ":");
                                    msg_nodelay(testStr2);
                                }
                                if (!testStr2.empty() && objectivesListStr.empty())
                                {
                                    missingObjText << stoi(p.stem().string()) << ", ";
                                    missingObjTextNum++;
                                }

                                auto testStr3 = englishReqText[stoi(p.stem().string())];
                                if (!testStr3.empty() && strcmp(testStr3.c_str(), reqTextStr.c_str()) == 0)
                                {
                                    hasEnglishReq++;
                                    msg_nodelay("\n   enReq:" + p.stem().string() + ":");
                                    msg_nodelay(testStr3);
                                }
                                if (!testStr3.empty() && reqTextStr.empty())
                                {
                                    missingReqText << stoi(p.stem().string()) << ", ";
                                    missingReqTextNum++;
                                }

                                auto testStr4 = englishOfferText[stoi(p.stem().string())];
                                if (!testStr4.empty() && strcmp(testStr4.c_str(), offerTextStr.c_str()) == 0)
                                {
                                    hasEnglishOffer++;
                                    msg_nodelay("\n   enOffer:" + p.stem().string() + ":");
                                    msg_nodelay(testStr4);
                                }
                                if (!testStr4.empty() && offerTextStr.empty())
                                {
                                    missingOfferText << stoi(p.stem().string()) << ", ";
                                    missingOfferTextNum++;
                                }
                            }
                            bool endText = !endTextStr.empty();
                            bool objectivesList = !objectivesListStr.empty();
                            bool bothObj = endText && objectivesList;
                            if (!reqTextStr.empty())
                                hasReqText++;
                            if (!offerTextStr.empty())
                                hasOfferText++;
                            if (endText)
                                hasEndText++;
                            if (objectivesList)
                                hasObjectives++;
                            if (bothObj)
                            {
                                hasBoth++;
                                endObj = fl.path().string();
                                msg_nodelay(endObj);
                                //questBothId = fl.path().string()
                            }

                            counter++;
                            gumbo_destroy_output(&kGumboDefaultOptions, parsed_response);
                            //msg_nodelay("> %s %s: %d \n", expansionName(e).c_str(), localeName(i).c_str(), counter);
                        }
                    }

                    msg_delay("\n");
                    msg_delay(">   Has ReqText: %d \n", hasReqText);
                    msg_delay(">   Has OfferText: %d \n", hasOfferText);
                    msg_delay(">   Has EndText: %d \n", hasEndText);
                    msg_delay(">   Has Objectives 1-4: %d \n", hasObjectives);
                    if (hasBoth)
                        msg_delay("> Has Both: %d \n", hasBoth);
                    if (i != 1)
                    {
                        msg_delay(">   Has Eng ReqText: %d \n", hasEnglishReq);
                        msg_delay(">   Has Eng OfferText: %d \n", hasEnglishOffer);
                        msg_delay(">   Has Eng EndText: %d \n", hasEnglishEnd);
                        msg_delay(">   Has Eng ObjText: %d \n", hasEnglishObj);
                        if (!missingReqText.str().empty())
                        {
                            msg_delay(">   Missing ReqText: %d \n", missingReqTextNum);
                            writeToFile(missingReqText.str().c_str(), expansionName(e) + "-" + localeName(i) + "-missingReqText");
                        }
                        if (!missingOfferText.str().empty())
                        {
                            msg_delay(">   Missing OfferText: %d \n", missingOfferTextNum);
                            writeToFile(missingOfferText.str().c_str(), expansionName(e) + "-" + localeName(i) + "-missingOfferText");
                        }
                        if (!missingEndText.str().empty())
                        {
                            msg_delay(">   Missing EndText: %d \n", missingEndTextNum);
                            writeToFile(missingEndText.str().c_str(), expansionName(e) + "-" + localeName(i) + "-missingEndText");
                        }
                        if (!missingObjText.str().empty())
                        {
                            msg_delay(">   Missing ObjectivesText: %d \n", missingObjTextNum);
                            writeToFile(missingObjText.str().c_str(), expansionName(e) + "-" + localeName(i) + "-missingObjText");
                        }
                    }
                    //msg_delay("\n");

                }
                msg_delay("\n");
            }

            return 1;
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
            WowheadQuestInfo* qInfo = LoadQuestCache(entry, expansion, locale);
            if (!qInfo)
            {
                msg_delay("> " + typeName(1) + " #" + std::to_string(entry) + ": Not Found! (Not Found) \n");
                return 0;
            }

            msg_delay("\n");
            msg_delay(">  Title: " + qInfo->GetTitle() + "\n\n");
            msg_delay(">  Objectives: " + qInfo->GetObjectives() + "\n\n");
            msg_delay(">  Details: " + qInfo->GetDetails() + "\n\n");
            msg_delay(">  ReqItemText: " + qInfo->GetRequestItemText() + "\n\n");
            msg_delay(">  OfferRewardText: " + qInfo->GetOfferRewardText() + "\n\n");
            msg_delay(">  EndText: " + qInfo->GetEndText() + "\n\n");
            msg_delay(">  Objective 1: " + qInfo->GetObjective(0) + "\n");
            msg_delay(">  Objective 2: " + qInfo->GetObjective(1) + "\n");
            msg_delay(">  Objective 3: " + qInfo->GetObjective(2) + "\n");
            msg_delay(">  Objective 4: " + qInfo->GetObjective(3) + "\n");
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
            std::string wh_page = loadPageOrCache(CACHE_NPC, entry, expansion, locale);

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

WowheadCacheEntry::WowheadCacheEntry(uint32 id, CacheType type, uint32 expansion, uint32 locale)
{
    entry = id;
    cacheType = type;
    curExpansion = expansion;
    curLocale = locale;
    isLoaded = false;
}


void WowheadQuestInfo::LoadCacheData(uint32 expansion, uint32 locale)
{
    std::string cachePage = loadPageOrCache(CACHE_QUEST, GetEntry(), expansion, locale, true);
    if (cachePage.empty())
        return;

    questTexts[expansion][locale] = LoadQuestCacheStrings(cachePage, locale);
    SetLoaded(true);
}

std::string WowheadQuestInfo::GetTitle(uint32 expansion, uint32 locale)
{
    if (expansion == 0 || locale == 0)
        return questTexts[GetCurExpansion()][GetCurLocale()].title;
    else
        return questTexts[expansion][locale].title;
}

std::string WowheadQuestInfo::GetObjectives(uint32 expansion, uint32 locale)
{
    if (expansion == 0 || locale == 0)
        return questTexts[GetCurExpansion()][GetCurLocale()].objectives;
    else
        return questTexts[expansion][locale].objectives;
}

std::string WowheadQuestInfo::GetDetails(uint32 expansion, uint32 locale)
{
    if (expansion == 0 || locale == 0)
        return questTexts[GetCurExpansion()][GetCurLocale()].details;
    else
        return questTexts[expansion][locale].details;
}

std::string WowheadQuestInfo::GetRequestItemText(uint32 expansion, uint32 locale)
{
    if (expansion == 0 || locale == 0)
        return questTexts[GetCurExpansion()][GetCurLocale()].requestItemText;
    else
        return questTexts[expansion][locale].requestItemText;
}

std::string WowheadQuestInfo::GetOfferRewardText(uint32 expansion, uint32 locale)
{
    if (expansion == 0 || locale == 0)
        return questTexts[GetCurExpansion()][GetCurLocale()].offerRewardText;
    else
        return questTexts[expansion][locale].offerRewardText;
}

std::string WowheadQuestInfo::GetEndText(uint32 expansion, uint32 locale)
{
    if (expansion == 0 || locale == 0)
        return questTexts[GetCurExpansion()][GetCurLocale()].endText;
    else
        return questTexts[expansion][locale].endText;
}

std::string WowheadQuestInfo::GetObjective(uint32 index, uint32 expansion, uint32 locale)
{
    if (index < 0 || index > 3)
        return "";

    if (expansion == 0 || locale == 0)
        return questTexts[GetCurExpansion()][GetCurLocale()].objectiveList[index];
    else
        return questTexts[expansion][locale].objectiveList[index];
}
