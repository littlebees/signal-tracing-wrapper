#include "utils.h"
#include <sstream>

std::vector<std::string> &split_into_vector(const std::string &s, char delim,
                                            std::vector<std::string> &elems)
{
    std::stringstream ss(s);
    std::string item;
    while (getline(ss, item, delim))
    {
        if (item.length())
            elems.push_back(item);
    }
    return elems;
}

std::string get_procname(char *proc)
{
    auto split_into_vector = [](const std::string &s, char delim,
                                std::vector<std::string> &elems)
        -> std::vector<std::string> & {
        std::stringstream ss(s);
        std::string item;
        while (getline(ss, item, delim))
        {
            if (item.length())
                elems.push_back(item);
        }
        return elems;
    };
    std::string new_proc(proc);
    std::vector<std::string> path;
    split_into_vector(proc, '/', path);
    return path[path.size() - 1];
}

std::string map_to_real_proc_path(char *proc)
{
    std::string new_proc(proc);
    // take the last item, filename
    // assume all proc which needs to be warped is in /usr/sbin
    new_proc = REAL_PROC_ROOT + get_procname(proc); // ex: /home/bee314/realProc/test
    return new_proc;
}