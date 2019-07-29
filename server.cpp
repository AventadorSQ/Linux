#include "httplib.h"
#include <fstream>
#include <boost/filesystem.hpp>
#include <unistd.h>
#include <fcntl.h>
#include "compress.hpp"

#define SERVER_BASE_DIR "www"
#define SERVER_ADDR "0.0.0.0"
#define SERVER_PORT 10000
#define SERVER_BACKUP_DIR SERVER_BASE_DIR"/list/"

namespace bf = boost::filesystem;

CompressStore cstor;

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
            srv.Get("/list/(.*)", DownLoadFile);
            srv.Put("/list/(.*)", BackupFile);
            srv.listen(SERVER_ADDR, SERVER_PORT);
            return true;
        }
    private:
        static void GetFileList(const httplib::Request& req, httplib::Response& rsp)
        {
            std::vector<std::string> list;
            cstor.GetFileList(list);
            std::cout << "文件列表：" << std::endl;
            std::string body;
            body = "<html><body><ol><hr/>";
            for(auto i : list)
            {
                bf::path path(i);
                std::string file = path.filename().string();

                std::string uri = "/list/" + file;
                body += "<h3><li><a href='";
                body += uri;
                body += "'>";
                body += file;
                body += "</a></li></h3>";
                std::cout << "(" << i << ")[ " << file << " ]" << std::endl;
            }
            body += "<hr/></ol></body></html>";
            rsp.set_content(&body[0], "text/html");
            return;
        }
        static void DownLoadFile(const httplib::Request& req, httplib::Response& rsp)
        {
            std::string file = SERVER_BASE_DIR + req.path;
            std::string body;

            cstor.GetFileData(file, body);
            rsp.set_content(body, "application/octet-stream");
            std::cout << "文件：[" << file <<"]下载成功！" << std::endl;
        }
        static void BackupFile(const httplib::Request& req, httplib::Response& rsp)
        {
            std::cout << "上传文件：[" << req.path << "]" << std::endl;
            if(!req.has_header("range"))
            {
                rsp.status = 400;
                return;
            }
            std::string range = req.get_header_value("Range");
            int64_t range_start;
            if(!RangeParse(range, range_start))
            {
                rsp.status = 400;
                return;
            }
            //std::cout << "上传文件名：[" << req.path << "] 块大小：[" << range << "] 数据：" << req.body << "\n";
            std::string realpath = SERVER_BASE_DIR + req.path;
            cstor.SetFileData(realpath, req.body, range_start);
            return;
        }

        static bool RangeParse(std::string& range, int64_t& start)
        {
            //range: byte=start-end
            size_t pos1 = range.find("=");
            size_t pos2 = range.find("-");
            if(pos1 == std::string::npos || pos2 == std::string::npos)
            {
                std::cerr << " range:[" << range << "] formate error" << std::endl;
                return false;
            }
            std::stringstream rs;
            rs << range.substr(pos1 + 1, pos2 - pos1 - 1);
            rs >> start;
            return true;
        }
};

void ThreadCreat()
{
    cstor.LowHeatFileStore();
}
int main()
{
    std::thread Thread(ThreadCreat);
    Thread.detach();
    CloudServer srv;
    srv.Start();
    return 0;
}
