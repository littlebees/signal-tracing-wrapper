#include <string>
#include <vector>

#define REAL_PROC_ROOT "/home/bee314/realProc/"

std::vector<std::string> &split_into_vector(const std::string &s, char delim, std::vector<std::string> &elems);
std::string get_procname(char *proc);
std::string map_to_real_proc_path(char *proc);