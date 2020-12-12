#include "logger.h"
#include "utils.h"
#include "handler.h"
#include <errno.h>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/stat.h>
//#include <filesystem>
#include <fcntl.h>
//#include <sys/sendfile.h>


static void try_to_mkdir_REAL_PROC_dir()
{
    auto REAL_PROC_DIR_exist = []() -> bool {
        struct stat st;
        stat(REAL_PROC_ROOT, &st);
        return st.st_mode & S_IFDIR;
    };

    if (!REAL_PROC_DIR_exist())
    {
        mkdir(REAL_PROC_ROOT, 0744);
    }
}

static bool isWarpper(std::string &procname)
{
    return procname == "wrapper";
}

static bool isProcHasBeenTraced(std::string &procname)
{
    struct stat st;
    std::string path = map_to_real_proc_path((char *)procname.c_str());
    return (stat(path.c_str(), &st) == 0);
}

static void install_wrapper(char *current, char *targetProc, char *procname)
{
    auto cp = [](char *origin, char *target) {
	    // not work on wsl1 & wsl2, copied file will not be executable
        //std::ifstream in((const char *)origin, std::ios::binary);
        //std::ofstream out((const char *)target, std::ios::binary);
        //out << in.rdbuf();

        int source = open(origin, O_RDONLY, 0);
        int dest = open(target, O_WRONLY | O_CREAT, 0755);

        char buf[BUFSIZ];
        size_t size;
        while ((size = read(source, buf, BUFSIZ)) > 0) {
            write(dest, buf, size);
        }
        // Linux sendfile, data transfering is handled in kernel space
        //struct stat stat_source;
        //fstat(source, &stat_source);
        //sendfile(dest, source, 0, stat_source.st_size);
        close(source);
        close(dest);

	    //works, but require c++17
        //std::filesystem::copy((const char *)origin, (const char *)target);
    };

    std::string real_proc_path = map_to_real_proc_path(procname);
    cp(targetProc, (char *)real_proc_path.c_str());
    std::remove(targetProc);
    cp(current, targetProc);
}

int main(int argc, char *argv[])
{
    char **my_argv = argv;
    try_to_mkdir_REAL_PROC_dir();
    std::string procname = get_procname(argv[0]);
    if (isWarpper(procname))
    {
        std::string targetProc = get_procname(argv[1]);
        if (argc == 2 && !isProcHasBeenTraced(targetProc))
        {
            install_wrapper(argv[0], argv[1], (char *)targetProc.c_str());
            my_argv++;
        }
        else
        {
            LOG << "trace one proc a time\n";
            exit(-1);
        }
    }

    int ret = 0, stat;
    if ((ret = regist_handlers()))
    {
        LOG << "sigaction: register handler error errno: [" << errno << "]" << '\n';
    }
    child_pid = fork();
    if (!child_pid)
    {
        std::string new_proc = map_to_real_proc_path(my_argv[0]); // if we used symbolic link, it should be argv[0]
        LOG << "gonna to run: " << new_proc << '\n';
        my_argv[0] = (char *)new_proc.c_str();
	for (int i=0;i<(argc-1);i++)
		LOG << my_argv[i] << '\n';
	LOG << new_proc.c_str() << '\n';
        ret = execv(new_proc.c_str(), my_argv); // if we used symbolic link, it should be argv
        LOG << "what happend:" << errno << " ret: " << ret << '\n';
    }
    else
    {
        waitpid(child_pid, &stat, 0);
    }
}
