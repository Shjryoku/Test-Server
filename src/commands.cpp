#include "headers/commands.h"
#include <ctime>
#include <sstream>

CommandResult handle(
                    const std::string& cmd,
                    const ServerStats& stats,
                    std::string& response
                    )
{
    if(cmd == "time"){
        time_t t = time(nullptr);
        char buf[32];

        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S \n", localtime(&t));

        response = buf;
        response += "\n";
        return CommandResult::OK;
    }
    if(cmd == "stats"){
        std::ostringstream ss;
        ss << "total = " << stats.total_clients
            << "\ncurrent = " << stats.curr_clients
            << "\n";
        
        response = ss.str();
        return CommandResult::OK;
    }
    if(cmd == "shutdown"){
        response = "Server is shutting down...\n";
        return CommandResult::SHUTDOWN;
    }

    response = "Unknown command\n";
    return CommandResult::OK;
}