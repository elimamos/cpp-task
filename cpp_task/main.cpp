#include <iostream>
//#include <json/value.h>
#include <fstream>
#include "json.hpp"
#include <string>
#include <vector>
#include <list>

using namespace std;
using json = nlohmann::json;
using json_pointer=nlohmann::json::json_pointer;
  vector<string> string_comp_list={"re"};
    vector<string> math_comp_list={"eq","ge","le","lt","gt"};
    vector<string> set_comp_list={"in"};

int getIndex(vector<string> v, string K)
{
    auto it = find(v.begin(), v.end(), K);

    // If element was found
    if (it != v.end())
    {

        // calculating the index
        // of K
        int index = it - v.begin();
        return index;
    }
    else {
        // If the element is not
        // present in the vector
        return -1;
    }
}
class CityObject{
    public:
    string city_name;
    string country;
    string voivodeship;
    double area;
    long population;
    int population_density;

    CityObject(   string city_name,string country,string voivodeship,double area,long population,int population_density){
           this-> city_name=city_name;
           this-> country=country;
           this-> voivodeship=voivodeship;
           this-> area=area;
           this-> population=population;
           this-> population_density=population_density;
    };

};
class FilterObject{
    public:
    string what_to_filter;
    string comparator;
    FilterObject(string what_to_filter,string comparator){
        this->what_to_filter=what_to_filter;
        this->comparator=comparator;
    };
    virtual void toString()=0;
    virtual bool filter(json city)=0;
};
class StringFilterObject:public FilterObject{
    public:
    string filter_value;
    StringFilterObject(string what_to_filter,string comparator,string filter_value): FilterObject( what_to_filter, comparator){
        this->what_to_filter=what_to_filter;
        this->comparator=comparator;
        this->filter_value=filter_value;
    };
     void toString(){

        for (auto const& wtf : what_to_filter) {
                    cout<<"What to filter?:"<< wtf<<&endl;
            }
        cout << "What to do?: "<<comparator<<&endl;
        cout <<"Value is: " << filter_value<<&endl;
    };
     bool filter(json city){
         return false;
    }


};
class MathFilterObject:public FilterObject{
    public:
    int filter_value;
    MathFilterObject(string what_to_filter,string comparator,int filter_value): FilterObject( what_to_filter, comparator){
        this->what_to_filter=what_to_filter;
        this->comparator=comparator;
        this->filter_value=filter_value;
    };
     void toString(){

          cout<<"What to filter?:"<< what_to_filter<<&endl;

        cout << "What to do?: "<<comparator<<&endl;
        cout <<"Value is: " << filter_value<<&endl;
    };
    bool filter(json city){
        double city_content;
        json_pointer p1(what_to_filter);
        try {
                city_content=city[p1];
        }catch (...) {
            cout <<"Wrong path provided! " <<what_to_filter<< " No such field exists for " << city["city"]<<". Will return false.\n";
            return false;
            }

         cout <<"city content:"<<city_content<<"\n";
         int comparator_position = getIndex(math_comp_list,comparator);
         switch(comparator_position) {
          case 0:
                if(city_content==filter_value){
                    return true;
                }
            break;
          case 1:
                if(city_content>=filter_value){
                    return true;
                }
            break;
          case 2:
                if(city_content<=filter_value){
                    return true;
                }
            break;
          case 3:
              if(city_content<filter_value){
                    return true;
                }
            break;
          case 4:
              if(city_content>filter_value){
                    return true;
                }
            break;
          default:
              cout << "Wrong command provided in json. Please use one of the provided ''eq','ge','le','lt','gt'"<<&endl;

           return false;
            // code block
        }
       // cout << filer_content[]
        // cout << "HERE!!!: "<<filer_content;
        return false;
    }

};
typedef std::list<FilterObject*> ParsedFiltersList;

class SetFilterObject:public FilterObject{
    public:
    vector<int> filter_value;
    SetFilterObject(string what_to_filter,string comparator,vector<int> filter_value): FilterObject( what_to_filter, comparator){
        this->what_to_filter=what_to_filter;
        this->comparator=comparator;
        this->filter_value=filter_value;
    };
     void toString(){
         for (auto const& wtf : what_to_filter) {
                    cout<<"What to filter?:"<< wtf<<&endl;
            }
        cout << "What to do?: "<<comparator<<&endl;
        for(auto& value : filter_value){
             cout <<"Value is: " << value<<&endl;
        }

    };
     bool filter(json city){
         return false;
    }

};


string file_to_string(string filename){
    ifstream file_stream(filename);
    string line="";
    string file="";
    while (getline (file_stream, line)) {
        // Output the text from the file
        file+=line+"\n";
    }
    return file;
};
bool array_contains(string argument, vector<string> value_array){
        int arraySize=value_array.size();
        for(int i = 0; i < arraySize; i++){
                string value = value_array[i];
                int comparison= value.compare(argument);
            if(comparison == 0){
                return true;
            }
        }
        return false;
};
string PATH;
ParsedFiltersList parseFilters(json filters){


    int size= filters.size();
    ParsedFiltersList parsedFilters;

    for(int i =0; i<size;i++){
         //cout<<i;
         string path = filters[i]["path"];
         /* list<string> splitted_path;
         std::string delimiter = "/";
         size_t pos = 0;
         path.erase(0,1);
         ///cout << path<<"PATH \n\n";
         std::string token;
        while ((pos = path.find(delimiter)) != std::string::npos) {
            token = path.substr(0, pos);
            splitted_path.push_back(token);
            path.erase(0, pos + delimiter.length());
        }
         splitted_path.push_back(path);
                 //cout <<path;
          //cout<<i;
          */
         string comparator = filters[i]["op"];
         //cout <<comparator;
        string value ="";
         if(array_contains(comparator,math_comp_list)){
                int value =filters[i]["value"];
               cout <<value <<"\n\n";

               parsedFilters.push_back(new MathFilterObject(path,comparator,value));
          }
         /*else if (array_contains(comparator,string_comp_list)){
             string value = filters[i]["value"];
             cout <<value<<"\n\n" ;
         }else if(array_contains(comparator,set_comp_list)){

             vector <int> value;
             //cout << to_string(filters[i]["value"]);
             for (auto& current_value : filters[i]["value"]) {
                    value.push_back(current_value);
              }
         }
         else{
            cout << "'op' value is a not supported format! Please fix your filer file!";
            return 0;
         }
*/
    //     cout << path<<"|"<<comparator<<"|"<<to_string(value)<<";";
    };
    return parsedFilters;
};
list<json> parseCities(json cities){
        int size= cities.size();
        list<json> cityList;
        for(int i =0; i<size;i++){
            json city;
            city = cities[i];
            cityList.push_back(city);
          }
        return cityList;

};

int main()
{
    string filepath;


 //   cout << "Please provide the path to your filter json file:";
//    cin >> filepath;
    filepath="filter.json";

    string city_file = file_to_string("cities.json");
    json city_json_file = json::parse(city_file);
    json cities_all=city_json_file["cities"];
    list<json>cities = parseCities(cities_all);
    for (auto const& i : cities) {
        std::cout << to_string(i)<<"WOW CITY!!!\n\n";
    }


     string filter_file=file_to_string(filepath);
     //cout << filter_file<<&endl;
    json filter_json_file = json::parse(filter_file);
    json filters=filter_json_file["filters"];

    ParsedFiltersList parsedFilters=parseFilters(filters);
    list<json> match_cities;
    for(json city :cities){
        bool output =false;
        for(ParsedFiltersList::const_iterator f = parsedFilters.begin(),
        endF = parsedFilters.end();
        f != endF;
        ++f){
            cout <<city["city"]<<"\n";
            FilterObject *generic_filter = *f;
            generic_filter->toString();

            output = generic_filter->filter(city);
            cout <<to_string(output)<<&endl;
            if(!output){
                break;
            }

    }
    if(output){
        match_cities.push_back(city);
    }

    }
    cout << "SUCCESSFUL CITIES HERE WE GOoooooo:"<<"\n";
    for(json city :match_cities){
        cout<<city["city"]<<"\n";
    }
   //    // to_string()

    /*for (json::iterator it = filter1.begin(); it != filter1.end(); ++it) {
          cout<< "HI";
          std::cout << it.key() << " : " << it.value() << "\n\n\n";
    }*/





   // cout << filters.count()<<&endl;
    // std::cout << j_complete << std::endl;


    // Access the values
   //  json  cities = j_complete["cities"];
 //    string city_one=cities[1].value("city", "oops1");
     //std::cout << "city 1 is: " << city_one << std::endl;
     return 0;
}
