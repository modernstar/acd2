#include <string>

std::string trim(std::string str)
{
    size_t pos = str.find_first_not_of(" ");
    str = str.substr( pos );
    pos = str.find_last_not_of("\r\n");
    str = str.substr(0, pos+1);
    pos = str.find_last_not_of(" ");
    str = str.substr(0, pos+1);

    return str;

}
