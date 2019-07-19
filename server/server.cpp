#include "httplib.h"
#include <fstream>
#include <boost/filesystem.hpp>

#define SERVER_BASE_DIR "www"
#define SERVER_ADDR "0.0.0.0"
#define SERVER_PORT 10000
#define SERVER_BACKUP_DIR SERVER_BASE_DIR"/list/"

namespace bf = boost::filesystem;

class CloudServer
{
    private:
        httplib::Server srv;
    public:
        CloudServer()
        {
            bf::path base_path(SERVER_BASE_DIR);

            if(!bf::exists(base_path))
            {
                bf::create_directory(base_path);
            }
            bf::path list_path(SERVER_BACKUP_DIR);
            if(!bf::exists(list_path))
            {
                bf::create_directory(list_path);
            }
        }
    public:
        bool Start()
        {
            srv.set_base_dir(SERVER_BASE_DIR);
            srv.Get("/(list(/){0,1}){0,1}", GetFileList);
            srv.Get("/list/(.*)", GetFileData);
            srv.listen(SERVER_ADDR, SERVER_PORT);
            return true;
        }
    private:
        static void GetFileList(const httplib::Request& req, httplib::Response& rsp)
        {
            bf::path list(SERVER_BACKUP_DIR);
            bf::directory_iterator item_begin(list);
            bf::directory_iterator item_end;

            std::string body;
            body = "<html><body><ol><hr/>";
            for(; item_begin != item_end; ++item_begin)
            {
                if(bf::is_directory(item_begin->status()))
                {
                    continue;
                }
                std::string file = item_begin->path().filename().string();

                std::string uri = "/list/" + file;
                body += "<h3><li><a href='";
                body += uri;
                body += "'>";
                body += file;
                body += "</a></li></h3>";
                std::cout << "[filename: " << file << " ]" << std::endl;
            }
            body += "<hr/></ol></body></html>";
            rsp.set_content(&body[0], "text/html");
            return;
        }
        static void GetFileData(const httplib::Request& req, httplib::Response& rsp)
        {
            std::string file = SERVER_BASE_DIR + req.path;
            if(!bf::exists(file))
            {
                rsp.status = 404;
                return;
            }
            std::ifstream if_file(file, std::ios::binary);
            if(!if_file.is_open())
            {
                std::cerr << "open file " << file << " error" << std::endl;
                rsp.status = 500;
                return;
            }

            std::string body;
            int64_t file_size = bf::file_size(file);
            body.resize(file_size);
            if_file.read(&body[0], file_size);
            if(!if_file.good())
            {
                std::cerr << "read file " << file << " body error" << std::endl;
                rsp.status = 500;
                return;
            }
            rsp.set_content(body, "txt/plain");
            std::cout << "download success" << std::endl;
        }
        static void BackupFile(const httplib::Request& req, httplib::Response& rsp)
        {}
};

int main()
{
    CloudServer srv;
    srv.Start();
    return 0;
}
