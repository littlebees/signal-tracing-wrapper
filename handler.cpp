#include "handler.h"
#include "logger.h"
#include <vector>
#include <cerrno>
#include <sys/wait.h>
#include <unistd.h>

pid_t child_pid;

typedef void (*SigHandler)(int sig, siginfo_t *siginfo, void *context);

static int reg_handler(int sig, SigHandler f)
{
    struct sigaction siga;
    siga.sa_sigaction = f;
    siga.sa_flags |= SA_SIGINFO; // get detail info
    if (sigaction(sig, &siga, NULL) != 0)
    {
        LOG << "errno:" << errno << " error sigaction(): "
            << "sig[" << sig << "]" << '\n';
        return errno;
    }
    else
    {
        return 0;
    }
}

static void log_who_kill_me_handler(int sig, siginfo_t *siginfo, void *context)
{
    int stat;
    pid_t sender_pid = siginfo->si_pid;
    LOG << "sender_pid: " << sender_pid << " sig:" << sig << '\n';
    kill(child_pid, sig);
    waitpid(child_pid, &stat, 0);
    exit(0);
}

static void child_handler(int sig, siginfo_t *siginfo, void *context)
{
    LOG << child_pid << " is terminated" << '\n';
}

static void relay_handler(int sig, siginfo_t *siginfo, void *context)
{
    kill(child_pid, sig);
}

int regist_handlers()
{
    auto reg_handler = [](int sig, SigHandler f) -> int {
        int ret = 0;
        struct sigaction siga;
        siga.sa_sigaction = f;
        siga.sa_flags |= SA_SIGINFO; // get detail info
        if (sigaction(sig, &siga, NULL) != 0)
        {
            LOG << "errno:" << errno << " error sigaction(): "
                << "sig[" << sig << "]" << '\n';
            ret = errno;
        }
        return ret;

    };
    int ret = 0;
    static std::vector<SigHandler> sig_handler_mapping(32 + 1, NULL); // 1~32

    sig_handler_mapping[SIGINT] = log_who_kill_me_handler;
    sig_handler_mapping[SIGQUIT] = log_who_kill_me_handler;
    sig_handler_mapping[SIGTERM] = log_who_kill_me_handler;
    sig_handler_mapping[SIGABRT] = log_who_kill_me_handler;

    sig_handler_mapping[SIGCHLD] = child_handler;

    sig_handler_mapping[SIGUSR1] = relay_handler;
    sig_handler_mapping[SIGUSR2] = relay_handler;
    sig_handler_mapping[SIGHUP] = relay_handler;

    for (int i = 1; i < sig_handler_mapping.size(); i++)
        if (sig_handler_mapping[i] != NULL)
            if ((ret = reg_handler(i, sig_handler_mapping[i])))
                return ret;
    return ret;
}

