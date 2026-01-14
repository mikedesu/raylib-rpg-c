#pragma once

#include <cstddef>
#include <string>
#include <vector>

typedef enum
{
    OPTION_NONE,
    OPTION_GAMEPLAY,
    OPTION_SOUND,
    OPTION_CONTROLS,
} option_type;

using std::string;
using std::vector;


class option_menu
{
private:
    vector<option_type> options;
    size_t selection;



public:
    option_menu()
    {
        selection = 0;
        options.push_back(OPTION_GAMEPLAY);
        options.push_back(OPTION_CONTROLS);
        options.push_back(OPTION_SOUND);
    }



    size_t get_selection()
    {
        return selection;
    }



    void incr_selection()
    {
        if (selection < options.size() - 1)
        {
            selection++;
        }
    }



    void decr_selection()
    {
        if (selection > 0)
        {
            selection--;
        }
    }



    vector<option_type>& get_options()
    {
        return options;
    }



    size_t get_option_count()
    {
        return options.size();
    }



    option_type get_option(size_t i)
    {
        return options[i];
    }



    string get_option_str(option_type t)
    {
        switch (t)
        {
        case OPTION_NONE: return "none";
        case OPTION_GAMEPLAY: return "gameplay";
        case OPTION_CONTROLS: return "controls";
        case OPTION_SOUND: return "sound";
        default: break;
        }
        return "unknown option";
    }
};
