#include <iostream>
#include <fstream>
#include "json.hpp"
#include <string>
#include <vector>
#include <list>
#include <regex>

using namespace std;
using json = nlohmann::json;
using json_pointer=nlohmann::json::json_pointer;
vector<string> string_comp_list= {"re"};
vector<string> math_comp_list= {"eq","ge","le","lt","gt"};
vector<string> set_comp_list= {"in"};

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
class CityObject
{
public:
    string city_name;
    string country;
    string voivodeship;
    double area;
    long population;
    int population_density;

    CityObject(   string city_name,string country,string voivodeship,double area,long population,int population_density)
    {
        this-> city_name=city_name;
        this-> country=country;
        this-> voivodeship=voivodeship;
        this-> area=area;
        this-> population=population;
        this-> population_density=population_density;
    };

};
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
        {    json v = city[p1];

            value_to_check=to_string(v);
            cout<< value_to_check<<&endl;
        }
        catch (...)
        {
            cout <<"Wrong path provided! " <<what_to_filter<< " No such field exists for " << city["city"]<<". Will return false.\n";
            return false;
        }

        return regex_match(value_to_check, reg);
    }
};
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

        cout <<"city content:"<<city_content<<"\n";
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
typedef std::list<FilterObject*> ParsedFiltersList;

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


string file_to_string(string filename)
{
    ifstream file_stream(filename);
    string line="";
    string file="";
    while (getline (file_stream, line))
    {
        // Output the text from the file
        file+=line+"\n";
    }
    return file;
};
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
            // cout <<value <<"\n\n";

            parsedFilters.push_back(new MathFilterObject(path,comparator,value));
        }
        else if (array_contains(comparator,string_comp_list))
        {
            string value = filters[i]["value"];
            // cout <<value<<"\n\n" ;
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
            cout<<city["city"]<<&endl;
            // cout <<city["city"]<<"\n";
            FilterObject *generic_filter = *f;
            //    generic_filter->toString();

            output = generic_filter->filter(city);
            cout <<to_string(output)<<&endl;
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

            //    cout <<city["city"]<<"\n";
            FilterObject *generic_filter = *f;


            output = generic_filter->filter(city);
            // cout <<to_string(output)<<&endl;
            if(!output)
            {
                cout<<"ERROR! Input data is incorrect!\n"<<to_string(city);
                generic_filter->toString();
                return false;
            }

        }

    }
    cout <<"Valid Json!";
    return true;

}
int main()
{
    string filepath;

    filepath="filter.json";

    string city_file = file_to_string("cities.json");
    json cities_all;
    json city_json_file;
    try
    {
        city_json_file = json::parse(city_file);
        cities_all=city_json_file["cities"];
    }
    catch (...)
    {
        cout <<"WOW! WRONG JSON MAN"<<"\n";
    }

    string validator_path="validator.json";
    string validator_file = file_to_string(validator_path);
    json validator_json_file = json::parse(validator_file);
    json validator_all=validator_json_file["validator"];


    string filter_file=file_to_string(filepath);
    json filter_json_file = json::parse(filter_file);
    json filters=filter_json_file["filters"];
    if(!validateCities(cities_all,validator_all))
    {
        return -1;
    }
    list<json> foundCities= filterCities(cities_all,filters);

    cout <<"\nFiltered cities: \n";
    for (json city: foundCities)
    {
        cout <<city["city"]<<"\n";
    }

    return 0;
}
