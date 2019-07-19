
mp_ptr->m_what += m_imp_ptr->m_path2.string();
                                                            ^
                                                            In file included from /usr/include/boost/filesystem.hpp:16:0,
                                                                             from server.cpp:2:
                                                                             /usr/include/boost/filesystem/path.hpp:364:12: note: candidate: template<class String> String httplib::boost::filesystem::path::string(const int&) const
                                                                                  String string(const codecvt_type& cvt) const;
                                                                                              ^
                                                                                              /usr/include/boost/filesystem/path.hpp:364:12: note:   template argument deduction/substitution failed:
                                                                                              In file included from /usr/include/boost/filesystem.hpp:17:0,
                                                                                                               from server.cpp:2:
                                                                                                               /usr/include/boost/filesystem/operations.hpp:1059:60: note:   candidate expects 1 argument, 0 provided
                                                                                                                            m_imp_ptr->m_what += m_imp_ptr->m_path2.string();
                                                                                                                                                                                        ^
                                                                                                                                                                                        /usr/include/boost/filesystem/operations.hpp:1060:24: error: ‘struct httplib::boost::filesystem::filesystem_error::m_imp’ has no member named ‘m_what’
                                                                                                                                                                                                     m_imp_ptr->m_what += "\"";
                                                                                                                                                                                                                             ^
                                                                                                                                                                                                                             /usr/include/boost/filesystem/operations.hpp:1063:27: error: ‘struct httplib::boost::filesystem::filesystem_error::m_imp’ has no member named ‘m_what’
                                                                                                                                                                                                                                      return m_imp_ptr->m_what.c_str();
                                                                                                                                                                                                                                                                 ^
                                                                                                                                                                                                                                                                 vinclud. <iostream>
#include <ifstream>
#include "httplib.h"
#include <boost/filesystem.hpp>

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

      for(; item_begin != item_end; ++item_begin)
      {
        if(bf::is_directory(item_begin->status()))
        {
          continue;
        }
        std::string file = item_begin->path().string();
        std::cerr << "file: " << file << std::endl;
      }
      return;
    }
    static void GetFileData(const Request& req, Response& rsp)
    {}
    static void BackupFile(const Request& req, Response& rsp)
    {}
};

int main()
{
  CloudServer srv;
  srv.Start();
  return 0;
}
