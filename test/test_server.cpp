#include "httplib.h"

void HelloWorld(const httplib::Request& req, httplib::Response& rsp)
{
  rsp.set_content("<html></h1>Hello World</h1></html>", "text/html");
}

int main()
{
  httplib::Server srv;
  //设定根目录
  srv.set_base_dir("./WWW");

  //处理客户端的GET
  srv.Get("/", HelloWorld);

  srv.listen("0.0.0.0", 9000);
  return 0;
}
