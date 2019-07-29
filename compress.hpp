#pragma once
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <string>
#include <thread>
#include <unistd.h>
#include <fcntl.h>
#include <zlib.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#define GZIPFILE_PATH    "www/zip/"
#define UNGZIPFILE_PATH  "www/list/"
#define RECORD_FILE      "record.list"
#define HEAT_TIME        5

namespace bf = boost::filesystem;

class CompressStore
{
    public:
        CompressStore()
        {
            pthread_rwlock_init(&_rwlock, NULL);
            if(!bf::exists(GZIPFILE_PATH))
            {
                bf::create_directory(GZIPFILE_PATH);
            }
        }
        ~CompressStore()
        {
            pthread_rwlock_destroy(&_rwlock);
        }
    private:
        pthread_rwlock_t _rwlock;
        //3.1S
        std::string _file_dir;
        //用于保存文件列表
        std::unordered_map<std::string, std::string> _file_list;
    private:
        //每次压缩存储线程启动就要从文件中读取列表信息
        bool GetListRecord()
        {
            bf::path name(RECORD_FILE);
            if(!bf::exists(name))
            {
                std::cerr << "record file is no exists\n";
                return false;
            }
            std::ifstream file(RECORD_FILE, std::ios::binary);
            if(!file.is_open())
            {
                std::cerr << "open record file error\n";
                return false;
            }
            int64_t fsize = bf::file_size(name);
            std::string body;
            body.resize(fsize);
            file.read(&body[0], fsize);
            if(!file.good())
            {
                std::cerr << "record file read error\n";
                file.close();
                return false;
            }
            file.close();
            std::vector<std::string> list;
            boost::split(list, body, boost::is_any_of("\n"));
            for(auto i : list)
            {
                size_t pos = i.find(" ");
                if(pos == std::string::npos)
                {
                    continue;
                }
                std::string key = i.substr(0, pos);
                std::string val = i.substr(pos + 1);
                _file_list[key] = val;
            }
            return true;
        }
        //每次压缩存储完，都要将列表信息存储到文件中
        bool SetListRecord()
        {
            std::stringstream tmp;
            for(auto i : _file_list)
            {
                tmp << i.first << " " << i.second << std::endl;
            }
            std::ofstream file(RECORD_FILE, std::ios::binary|std::ios::trunc);
            if(!file.is_open())
            {
                std::cerr << "record file open error" << std::endl;
                return false;
            }
            file.write(tmp.str().c_str(), tmp.str().size());
            if(!file.good())
            {
                std::cerr << "record file write body error" << std::endl;
                file.close();
                return false;
            }
            file.close();
            return true;
        }

        //3.1目录检测,获取目录中的文件名
        bool DirecotryCheck()
        {
            if(!bf::exists(UNGZIPFILE_PATH))
            {
                bf::create_directory(UNGZIPFILE_PATH);
            }
            bf::directory_iterator item_begin(UNGZIPFILE_PATH);
            bf::directory_iterator item_end;
            for(;item_begin != item_end; ++item_begin)
            {
                if(bf::is_directory(item_begin->status()))
                {
                    continue;
                }
                std::string name = item_begin->path().string();
                if(IsNeedCompress(name))
                {
                    std::string gzip = GZIPFILE_PATH + item_begin->path().filename().string() + ".gz";
                    CompressFile(name, gzip);
                    AddFileRecord(name,gzip);
                }
            }
            return true;
        }
        //3.2判断文件是否需要压缩存储
        bool IsNeedCompress(std::string& file)
        {
            struct stat st;
            if(stat(file.c_str(), &st) < 0)
            {
                std::cerr << "get file : [" << file <<"] stat error" << std::endl;
                return false;
            }
            time_t cur_time = time(NULL);
            time_t acc_time = st.st_atime;
            if((cur_time - acc_time) < HEAT_TIME)
            {
                return false;
            }
            return true;
        }
        //3.3对文件进行压缩存储
        bool CompressFile(std::string& file, std::string& gzip)
        {
            int fd = open(file.c_str(), O_RDONLY);
            if(fd < 0)
            {
                std::cerr << "com open file : [" << file << "] error" << std::endl;
                return false;
            }
            gzFile gf = gzopen(gzip.c_str(), "wb");
            if(gf == NULL)
            {
                close(fd);
                std::cerr << "com open gzip:[" << gzip << "] error" << std::endl;
                return false;
            }
            int ret;
            char buf[1024];
            flock(fd, LOCK_SH);
            while((ret = read(fd, buf, 1024)) > 0)
            {
                gzwrite(gf, buf, ret);
            }
            flock(fd, LOCK_UN);
            close(fd);
            gzclose(gf);
            //删除源文件
            unlink(file.c_str());

            return true;
        }

        //对文件进行解压缩
        bool UnCompressFile(const std::string& gzip, const std::string& file)
        {
            int fd = open(file.c_str(), O_CREAT|O_WRONLY, 0664);
            if(fd < 0)
            {
                std::cerr << "open file " << file << " failed\n";
                return false;
            }
            gzFile gf = gzopen(gzip.c_str(), "rb");
            if(gf == NULL)
            {
                std::cerr << "open gzip " << gzip << " failed\n";
                close(fd);
                return false;
            }
            int ret;
            char buf[1024] = {0};
            flock(fd, LOCK_EX);
            while((ret = gzread(gf, buf, 1024)) > 0)
            {
                int len = write(fd, buf, ret);
                if(len < 0)
                {
                    std::cerr << "get gzip data error\n";
                    gzclose(gf);
                    close(fd);
                    flock(fd, LOCK_UN);
                    return false;
                }
            }
            flock(fd, LOCK_UN);
            gzclose(gf);
            close(fd);
            //删除压缩包
            unlink(gzip.c_str());
            return true;
        }
        bool GetNormalFile(std::string& name, std::string& body)
        {
            int64_t fsize = bf::file_size(name);
            body.resize(fsize);

            int fd = open(name.c_str(), O_RDONLY);
            if(fd < 0)
            {
                std::cerr << "open file " << name << " failed\n";
                return false;
            }
            flock(fd, LOCK_SH);
            int ret = read(fd, &body[0], fsize);
            flock(fd, LOCK_UN);
            if(ret < 0)
            {
                std::cerr << " get file " << name << " body error\n";
                close(fd);
                return false;
            }
            close(fd);
            return true;
        }
    public:
        //1.向外提供文件列表的功能
        bool GetFileList(std::vector<std::string>& list)
        {
            pthread_rwlock_rdlock(&_rwlock);
            for(auto i : _file_list)
            {
                list.push_back(i.first);
            }
            pthread_rwlock_unlock(&_rwlock);
            return true;
        }
        //2.对外提供文件下载的功能
        bool GetFileData(std::string& file, std::string& body)
        {
            //2.1.非压缩文件下载
            if(bf::exists(file))
            {
                GetNormalFile(file, body);
            }
            //2.2.压缩文件下载
            else
            {
                std::string gzip;
                GetFileGzip(file, gzip);
                UnCompressFile(gzip, file);
                GetNormalFile(file, body);
            }
        }
        bool AddFileRecord(const std::string& file, const std::string& gzip)
        {
            pthread_rwlock_wrlock(&_rwlock);
            _file_list[file] = gzip;
            pthread_rwlock_unlock(&_rwlock);
        }
        //通过文件名，获取对应压缩包名
        bool GetFileGzip(const std::string& file, std::string& gzip)
        {
            pthread_rwlock_rdlock(&_rwlock);
            auto it = _file_list.find(file);
            if(it == _file_list.end())
            {
                pthread_rwlock_unlock(&_rwlock);
                return false;
            }
            gzip = it->second;
            pthread_rwlock_unlock(&_rwlock);
            return true;
        }
        //文件存储
        bool SetFileData(const std::string& file, const std::string& body, int64_t offset)
        {
            int fd = open(file.c_str(), O_CREAT|O_WRONLY, 0664);
            if(fd < 0)
            {
                std::cerr << "open file " << file << " error";
                return false;
            }
            lseek(fd, offset, SEEK_SET);
            flock(fd, LOCK_EX);
            int ret = write(fd, &body[0], body.size());
            if(ret < 0)
            {
                flock(fd, LOCK_UN);
                close(fd);
                std::cerr << "store file " << file << " data error\n";
                return false;
            }

            flock(fd, LOCK_UN);
            close(fd);
            AddFileRecord(file, "");
            return true;
        }
        //3.对低热度文件进行压缩存储
        bool LowHeatFileStore()
        {
            GetListRecord();
            while(1)
            {
                //3.1.获取目录下文件名称
                //3.2判断文件是否需要压缩存储
                //3.3对文件进行压缩存储
                DirecotryCheck();
                SetListRecord();
                sleep(5);
            }
        }
};
