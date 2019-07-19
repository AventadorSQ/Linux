#include "httplib.h"
#include <boost/filesystem.hpp>
#include<fstream>

#define SERVER_BASE_DIR "www"
#define SERVER_ADDR "0.0.0.0"
#define SERVER_PORT 6666
#define SERVER_BACKUP_DIR SERVER_BASE_DIR"/list/"

using namespace httplib;
namespace bf = boost::filesystem;
class CloudServer
{
  private:
    Server srv;
  public:
    CloudServer()
    {
      //目录不存在则创建
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
      //设置相对根目录
      srv.set_base_dir(SERVER_BASE_DIR);
      //请求根目录
      srv.Get("/(list(/){0,1}){0,1}", GetFileList);
      srv.Get("/list/(.*)", GetFileData);
      //开始监听
      srv.listen(SERVER_ADDR, SERVER_PORT);
      return true;
    }
  private:
    //获取文件列表
    static void GetFileList(const Request& req, Response& rsp)
    {
      bf::path list(SERVER_BASE_DIR);
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
        //获取文件名
        std::string file = item_begin->path().filename().string();
        
        std::string uri = "/list/" + file;
        body += "<h3><li><a href='";
        body += uri;
        body += "'>";
        body += file;
        body += "</a></li></h3>";
        std::string file = item_begin->path().string();
        std::cerr << "file: " << file << std::endl;
      }
      body += "<hr/></ol></body></html>"
      rsp.set_content(&body[0], "text/html");
      return;
    }
    //文件下载
    static void GetFileData(const Request& req, Response& rsp)
    {
      //保存资源路径
      req.path = "/list/";
      std::string file = SERVER_BASE_DIR + req.path;
      if(!bf::exist(file))
      {
        rsp.status = 404;
        return;
      }
      //
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
      //good判断上一次操作是否正确
      if(!if_file.good())
      {
        std::cerr << "read file " << file << " body error" << std::endl;
        rsp.status = 500;
        return;
      }
      rsp.set_content(body, "text/plain");
    }
    static void BackupFile(const Request& req, Response& rsp)
    {}
};

int main()
{
  CloudServer srv;
  srv.Start();
  return 0;
}
