#include "request.hpp"

class CgiManager
{
    private:
        int _p_fd[2];
        std::map<std::string, std::string> _env;
    public:
        CgiManager();
        ~CgiManager();
};

