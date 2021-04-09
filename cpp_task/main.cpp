#include <iostream>
#include <fstream>
#include "json.hpp"
#include <string>
#include <vector>
#include <list>
#include <regex>
#include <algorithm>
#include <cctype>
#include <locale.h>
#include <fcntl.h>
#include <io.h>
#include <string>
#include <stdio.h>

using namespace std;
using json = nlohmann::json;
using json_pointer=nlohmann::json::json_pointer;
//Possible types of filters sorted into 3 groups string operations, mathematical operations and operations on sets
vector<string> string_comp_list= {"re"};
vector<string> math_comp_list= {"eq","ge","le","lt","gt"};
vector<string> set_comp_list= {"in"};
//list of filter object can contain string, math and set typo of filters
typedef std::list<FilterObject*> ParsedFiltersList;

//translate the given list value into the list index to be used in the switch
int getIndex(vector<string> v, string K)
{
    auto it = find(v.begin(), v.end(), K);
    if (it != v.end())
    {
        int index = it - v.begin();
        return index;
    }
    else
    {
        return -1;
    }
}
//Generic filter object class used as interface
class FilterObject
{
public:
    string what_to_filter;
    string comparator;
    FilterObject(string what_to_filter,string comparator)
    {
        this->what_to_filter=what_to_filter;
        this->comparator=comparator;
    };
    virtual void toString()=0;
    virtual bool filter(json city)=0;
};

//Filter object to perform string operations
class StringFilterObject:public FilterObject
{
public:
    string filter_value;
    StringFilterObject(string what_to_filter,string comparator,string filter_value): FilterObject( what_to_filter, comparator)
    {
        this->what_to_filter=what_to_filter;
        this->comparator=comparator;
        this->filter_value=filter_value;
    };
    void toString()
    {

        cout<<"What to filter?:"<< what_to_filter<<&endl;
        cout << "What to do?: "<<comparator<<&endl;
        cout <<"Value is: " << filter_value<<&endl;
    };
    bool filter(json city)
    {
        regex reg (filter_value);
        json_pointer p1(what_to_filter);
        string value_to_check;
        try
        {
            json v = city[p1];
            value_to_check=to_string(v);
           }
        catch (...)
        {
            cout <<"Wrong path provided! " <<what_to_filter<< " No such field exists for " << city["city"]<<". Will return false.\n";
            return false;
        }

        return regex_match(value_to_check, reg);
    }
};

//Filter object to perform mathematical operations
class MathFilterObject:public FilterObject
{
public:
    int filter_value;
    MathFilterObject(string what_to_filter,string comparator,int filter_value): FilterObject( what_to_filter, comparator)
    {
        this->what_to_filter=what_to_filter;
        this->comparator=comparator;
        this->filter_value=filter_value;
    };
    void toString()
    {
        cout<<"What to filter?:"<< what_to_filter<<&endl;
        cout << "What to do?: "<<comparator<<&endl;
        cout <<"Value is: " << filter_value<<&endl;
    };
    bool filter(json city)
    {
        double city_content;
        json_pointer p1(what_to_filter);
        try
        {
            city_content=city[p1];
        }
        catch (...)
        {
            cout <<"Wrong path provided! " <<what_to_filter<< " No such field exists for " << city["city"]<<". Will return false.\n";
            return false;
        }

        int comparator_position = getIndex(math_comp_list,comparator);
        switch(comparator_position)
        {
        case 0:
            if(city_content==filter_value)
            {
                return true;
            }
            break;
        case 1:
            if(city_content>=filter_value)
            {
                return true;
            }
            break;
        case 2:
            if(city_content<=filter_value)
            {
                return true;
            }
            break;
        case 3:
            if(city_content<filter_value)
            {
                return true;
            }
            break;
        case 4:
            if(city_content>filter_value)
            {
                return true;
            }
            break;
        default:
            cout << "Wrong command provided in json. Please use one of the provided ''eq','ge','le','lt','gt'"<<&endl;

            return false;
            // code block
        }
        return false;
    }

};


//Filter object to perform string operations
class SetFilterObject:public FilterObject
{
public:
    vector<int> filter_value;
    SetFilterObject(string what_to_filter,string comparator,vector<int> filter_value): FilterObject( what_to_filter, comparator)
    {
        this->what_to_filter=what_to_filter;
        this->comparator=comparator;
        this->filter_value=filter_value;
    };
    void toString()
    {
        for (auto const& wtf : what_to_filter)
        {
            cout<<"What to filter?:"<< wtf<<&endl;
        }
        cout << "What to do?: "<<comparator<<&endl;
        for(auto& value : filter_value)
        {
            cout <<"Value is: " << value<<&endl;
        }

    };
    bool filter(json city)
    {
        double city_content;
        json_pointer p1(what_to_filter);
        try
        {
            city_content=city[p1];
        }
        catch (...)
        {
            cout <<"Wrong path provided! " <<what_to_filter<< " No such field exists for " << city["city"]<<". Will return false.\n";
            return false;
        }
        int comparator_position = getIndex(set_comp_list,comparator);
        switch(comparator_position)
        {
        case 0:
            if(city_content>=filter_value[0])
            {
                if(city_content<=filter_value[1])
                {
                    return true;
                }
            }
            break;
        default:
            cout << "Wrong command provided in json. Please use one of the provided: 'in'"<<&endl;
        }
        return false;
    }
};

//Reads file and saves it's content into a string
string file_to_string(string filename)
{
    string file="";

    ifstream file_stream(filename);
    if(file_stream.fail())
    {
        throw("An exception happened when reading the file.\n");
    };
    string line="";
    while (getline (file_stream, line))
    {

        file+=line+"\n";
    }
    file_stream.close();
    return file;
};
//check if array contains specific string
bool array_contains(string argument, vector<string> value_array)
{
    int arraySize=value_array.size();
    for(int i = 0; i < arraySize; i++)
    {
        string value = value_array[i];
        int comparison= value.compare(argument);
        if(comparison == 0)
        {
            return true;
        }
    }
    return false;
};
//creates proper filter object based on "op" value and lists defined on top of this file
ParsedFiltersList parseFilters(json filters)
{
    int size= filters.size();
    ParsedFiltersList parsedFilters;

    for(int i =0; i<size; i++)
    {
        string path = filters[i]["path"];
        string comparator = filters[i]["op"];

        string value ="";
        if(array_contains(comparator,math_comp_list))
        {
            int value =filters[i]["value"];
            parsedFilters.push_back(new MathFilterObject(path,comparator,value));
        }
        else if (array_contains(comparator,string_comp_list))
        {
            string value = filters[i]["value"];
            parsedFilters.push_back(new StringFilterObject(path,comparator,value));
        }
        else if(array_contains(comparator,set_comp_list))
        {

            vector <int> value;

            for (auto& current_value : filters[i]["value"])
            {
                value.push_back(current_value);
            }
            parsedFilters.push_back(new SetFilterObject(path,comparator,value));
        }
        else
        {
            cout << "'op' value is a not supported format! Please fix your filer file!";
            throw new exception();
        }
    };
    return parsedFilters;
};
// cuts the city json into seperate jsons with individual cities
list<json> parseCities(json cities)
{
    int size= cities.size();
    list<json> cityList;
    for(int i =0; i<size; i++)
    {
        json city;
        city = cities[i];
        cityList.push_back(city);
    }
    return cityList;
};
//perform filtration on each city all filters and return list of cities that meet all requirements
list<json> filterCities(json ct, json ft)
{
    list<json>cities = parseCities(ct);
    ParsedFiltersList parsedFilters=parseFilters(ft);
    list<json> match_cities;
    for(json city :cities)
    {
        bool output =false;
        for(ParsedFiltersList::const_iterator f = parsedFilters.begin(),
                endF = parsedFilters.end();
                f != endF;
                ++f)
        {

            FilterObject *generic_filter = *f;
            // will filter differently depending on what type of filter object we're using
            output = generic_filter->filter(city);
            if(!output)
            {
                break;
            }
        }
        if(output)
        {
            match_cities.push_back(city);
        }
    }

    return match_cities;
}
//use validator json and check if the values are logical. Set of filters can be extended and changed freely
bool validateCities(json ct, json ft)
{
    list<json>cities = parseCities(ct);
    ParsedFiltersList parsedFilters=parseFilters(ft);

    for(json city :cities)
    {
        bool output =false;
        for(ParsedFiltersList::const_iterator f = parsedFilters.begin(),
                endF = parsedFilters.end();
                f != endF;
                ++f)
        {

            FilterObject *generic_filter = *f;
            output = generic_filter->filter(city);
            if(!output)
            {
                cout<<"ERROR! Input data is incorrect!\n"<<to_string(city)<<"\n";
                generic_filter->toString();
                return false;
            }
        }
    }
    cout <<"Valid Json!\n";
    return true;

}

//some trimming for input content
static inline void ltrim(std::string &s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch)
    {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
static inline void rtrim(std::string &s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch)
    {
        return !std::isspace(ch);
    }).base(), s.end());
}

static inline void trim(std::string  &s)
{
    ltrim(s);
    rtrim(s);
}

//Read user input in a standarised way
string readUserInputFile(string defaultValue, bool isValidator)
{
    string filepath;
    bool success_read=false;
    string file;
    cout<<">";
    while(!success_read)
    {
        getline(cin,filepath);
        trim(filepath);
        if(filepath.empty())
        {
            if(isValidator)
            {
                cout <<"Validator path was empty. Will not perform pre-validation.\n";
                return "";
            }
            cout<<"Using default value -> "<<defaultValue<<"\n";
            filepath=defaultValue;
        }
        try
        {

            file = file_to_string(filepath);
            success_read=true;
        }
        catch(...)
        {
            cout << "Oh! Something went wrong! Could not read file! Please try again.\n";
            cout<<">";
        }
    }
    return file;
}

int main()
{
    string filepath;
    string citypath;
    bool success_read=false;
    string city_file;
    string filter_file;
    string validator_file ;
    cout <<"Welcome to the *city* filter!\nPlease your city JSON filename which should be places in the root dir of this project. \nLeaving this empty will look for the default value which is set to 'cities.json' in the same directory.\n";
    city_file=readUserInputFile("cities.json",false);
    cout << "Please your *filter* JSON filename which should be places in the root dir of this project.\nLeaving this empty will look for the default value which is set to 'filter.json' in the same directory.\n";
    filter_file=readUserInputFile("filter.json",false);
    cout << "Please your *validator* JSON filename which should be places in the root dir of this project.\nLeaving this empty will skip the json validation of cities.\n";
    validator_file=readUserInputFile("validator.json",true);

    json cities_all;
    json city_json_file;
    json validator_json_file;
    json validator_all;
    json filter_json_file;
    json filters;
    try
    {
        city_json_file = json::parse(city_file);
        cities_all=city_json_file["cities"];
    }
    catch (exception& e)
    {
        cout <<"\nCity Json seems to be invalid!"<<"\n";
        cout << e.what() << '\n';
    }
    try
    {
        filter_json_file = json::parse(filter_file);
        filters=filter_json_file["filters"];
    }
    catch (exception& e)
    {
        cout <<"\nFitler Json seems to be invalid!"<<"\n";
        cout << e.what() << '\n';
    }
    if(!validator_file.empty())
    {
        cout << "Cities content validation.\n";
        try
        {
            validator_json_file = json::parse(validator_file);
            validator_all=validator_json_file["validator"];
        }
        catch (exception& e)
        {
            cout <<"\nValidator Json seems to be invalid!"<<"\n";
            cout << e.what() << '\n';
        }

        if(!validateCities(cities_all,validator_all))
        {
            return -1;
        }
    }

    list<json> foundCities= filterCities(cities_all,filters);
    cout << "\n";
    cout << foundCities.size()<<" cities were found with this filter set.\n";
    cout <<"\nFiltered cities to be found under 'found_cities.txt'\n";
    ofstream outputFile("found_cities.txt", std::ios_base::out);

    for (json city: foundCities)
    {
        outputFile << city;
        outputFile <<"\n";
    }
    outputFile.close();
    return 0;
}
