#include "cmdline.h"
#include <cstddef>
#include <errno.h>
#include <iostream>
#include <regex>
#include <sstream>
#include <fstream>
#include <string>
#include <sys/stat.h>
#include <unistd.h>

#include "../postgis_config.h"

#include "Toolkit/YkLicenseRSA.h"

#define YUKONDEPFILE "yukondepfile.txt"
#define YUKONPKGFILE "yukonpkgfile.txt"
#define YUKONSQLFILE "yukonsqlfile.txt"

#define YUKONVERSION "Yukon 2.0(Community Edition)"

enum class InfoLevel
{
  VERSION,
  INFO,
  WARNING,
  ERROR
};

enum class ErrorCode : int
{
  E0000, // no error
  E0001, // pg_config 命令未找到，或运行不正确
  E0002, // pg_config --pkglibdir 命令错误，一般情况下不会出错
  E0003, // pg_config --sharedir 命令错误，一般情况下不会出错
  E0004, // pg_config --bindir 命令错误，一般情况下不会出错
  E0005, // pg_config --libdir 命令错误，一般情况下不会出错
  E0006, // 拷贝文件到 pkglibdir 错误
  E0007, // 拷贝文件到 sharedir 错误
  E0008, // 拷贝第三方依赖库到 libdir 错误
  E0009, // 将文件写入到 YKDEPFILE 错误
  E0010, // 拷贝文件到 bindir 错误
};

std::string
codeGetString(ErrorCode code)
{
  switch (code)
  {
  case ErrorCode::E0001:
    return std::string("E0001");
  case ErrorCode::E0002:
    return std::string("E0002");
  case ErrorCode::E0003:
    return std::string("E0003");
  case ErrorCode::E0004:
    return std::string("E0004");
  case ErrorCode::E0005:
    return std::string("E0005");
  case ErrorCode::E0006:
    return std::string("E0006");
  case ErrorCode::E0007:
    return std::string("E0007");
  case ErrorCode::E0008:
    return std::string("E0008");
  case ErrorCode::E0009:
    return std::string("E0009");
  case ErrorCode::E0010:
    return std::string("E0010");
  default:
    return std::string();
  }

  return std::string();
}

std::string
codeGetString(InfoLevel level)
{
  switch (level)
  {
  case InfoLevel::VERSION:
    return std::string("VERSION");
  case InfoLevel::INFO:
    return std::string("INFO");
  case InfoLevel::WARNING:
    return std::string("WARNING");
  case InfoLevel::ERROR:
    return std::string("ERROR");
  default:
    return std::string();
  }
  return std::string();
}

void log(InfoLevel level, std::string msg, ErrorCode code = ErrorCode::E0000)
{
  // 输出消息类型
  if (level == InfoLevel::INFO)
  {
    std::cout << "\033[32m" << codeGetString(level) << "\033[0m"
              << ":";
  }
  else if (level == InfoLevel::WARNING)
  {
    std::cout << "\033[33m" << codeGetString(level) << "\033[0m"
              << ":";
  }
  else if (level == InfoLevel::ERROR)
  {
    std::cout << "\033[31m" << codeGetString(level) << "\033[0m"
              << ":";
  }

  // 如果错误码不是 E0000,则输出
  if (code != ErrorCode::E0000)
  {
    std::cout << codeGetString(code) << ":";
  }

  // 输出最多 100 个字符的信息
  if (msg.length() > 100)
  {
    msg = msg.substr(0, 100);
  }

  std::cout << msg << std::endl;
}

std::string
exec(const char *cmd)
{
  char buffer[128];
  std::string result = "";
  FILE *pipe = popen((std::string(cmd) + " 2>&1").c_str(), "r");
  if (!pipe)
    throw std::runtime_error("popen() failed!");
  try
  {
    while (fgets(buffer, sizeof buffer, pipe) != NULL)
    {
      result += buffer;
    }
  }
  catch (...)
  {
    pclose(pipe);
    std::cout << "command error" << std::endl;
  }
  pclose(pipe);
  return result;
}

int install(std::string pkg)
{

  std::string databasetype;
  std::string databasever;
  std::string pkglibdir;
  std::string sharedir;
  std::string bindir;
  std::string libdir;

  // 获取当前系统环境
  // getosenv();
  log(InfoLevel::INFO, "install precheck...");
  std::string pgconfigpath;
  // 执行 pg_config 命令，获取
  if (!pkg.empty())
  {
    pgconfigpath = pkg;
    log(InfoLevel::INFO, "user specified pg_config path: " + pgconfigpath);
  }
  else
  {
    pgconfigpath = "pg_config";
  }

  // 1. 数据库版本
  std::string ret;
  ret = exec((pgconfigpath + std::string(" --version")).c_str());

  // 命令错误
  if (ret.find("sh") != std::string::npos)
  {
    log(InfoLevel::ERROR, "pg_config command is not valid," + ret, ErrorCode::E0001);
    return -1;
  }
  else
  {
    // 否则解析数据库类型和版本
    // 首先检查是否是 openGauss/GaussDB 数据库
    std::regex reggs("(opengauss|gaussdb).*", std::regex::icase);
    std::regex regpg("PostgreSQL.*", std::regex::icase);
    std::smatch results;
    if (std::regex_search(ret, results, reggs))
    {
      std::string db = results[0];
      if (db.at(db.size() - 1) == '\n')
      {
        db.erase(db.size() - 1);
      }
      int pos = db.find(' ');
      databasetype = db.substr(0, pos);
      databasever = db.substr(pos + 1, db.size() - 1);

    } // 然后检查是否是 postgresql 数据库
    else if (std::regex_search(ret, results, regpg))
    {
      std::string db = results[0];
      if (db.at(db.size() - 1) == '\n')
      {
        db.erase(db.size() - 1);
      }
      int pos = db.find(' ');
      databasetype = db.substr(0, pos);
      databasever = db.substr(pos + 1, db.size() - 1);
    }
    else
    {
      std::cerr << "Not Supported Database" << std::endl;
      return -1;
    }
    // 然后检查是否是 postgres 数据库
    log(InfoLevel::INFO, "database:" + databasetype);
    log(InfoLevel::INFO, "version:" + databasever);
  }

  // pkglibdir
  ret = exec((pgconfigpath + std::string(" --pkglibdir")).c_str());
  if (ret.empty())
  {
    log(InfoLevel::ERROR, "pkglibdir not find!", ErrorCode::E0002);
    return -1;
  }
  else
  {
    pkglibdir = ret;
    if (pkglibdir.at(pkglibdir.size() - 1) == '\n')
    {
      pkglibdir.erase(pkglibdir.size() - 1);
    }
    log(InfoLevel::INFO, "pkglibdir:" + pkglibdir);
  }

  // sharedir
  ret = exec((pgconfigpath + std::string(" --sharedir")).c_str());
  if (ret.empty())
  {
    log(InfoLevel::ERROR, "shareddir not find!", ErrorCode::E0003);
    return -1;
  }
  else
  {
    sharedir = ret;
    if (sharedir.at(sharedir.size() - 1) == '\n')
    {
      sharedir.erase(sharedir.size() - 1);
    }
    log(InfoLevel::INFO, "shareddir:" + sharedir);
  }

  // bindir
  ret = exec((pgconfigpath + std::string(" --bindir")).c_str());
  if (ret.empty())
  {
    log(InfoLevel::ERROR, "bindir not find!", ErrorCode::E0004);
    return -1;
  }
  else
  {
    bindir = ret;
    if (bindir.at(bindir.size() - 1) == '\n')
    {
      bindir.erase(bindir.size() - 1);
    }
    log(InfoLevel::INFO, "bindir:" + bindir);
  }

  // libdir
  ret = exec((pgconfigpath + std::string(" --libdir")).c_str());
  if (ret.empty())
  {
    log(InfoLevel::ERROR, "libdir not find!", ErrorCode::E0005);
    return -1;
  }
  else
  {
    libdir = ret;
    if (libdir.at(libdir.size() - 1) == '\n')
    {
      libdir.erase(libdir.size() - 1);
    }
    log(InfoLevel::INFO, "libdir:" + libdir);
  }

  // 开始安装,拷贝文件
  log(InfoLevel::INFO, "start install...");

  log(InfoLevel::INFO, "install lib file...");
  ret = exec((std::string("cp lib/* ") + pkglibdir + "/").c_str());
  if (!ret.empty())
  {
    log(InfoLevel::ERROR, "install lib file failed." + ret, ErrorCode::E0006);
    return -1;
  }

  log(InfoLevel::INFO, "install extension file...");
  ret = exec((std::string("cp extension/* ") + sharedir + "/extension").c_str());

  if (!ret.empty())
  {
    log(InfoLevel::ERROR, "install extension file failed." + ret, ErrorCode::E0007);
    return -1;
  }

  // 拷贝三方依赖库文件
  log(InfoLevel::INFO, "install third dependency file...");
  ret = exec("ls yukon_dep");
  std::istringstream inlibsname(ret);
  std::ostringstream outlibsname;
  std::string libname;
  while (inlibsname >> libname)
  {
    struct stat sb;
    int retcode = stat((libdir + "/" + libname).c_str(), &sb);
    if (retcode != 0 && errno == ENOENT)
    {
      // 文件不存在，则拷贝到 libdir 目录
      std::string cmd = (std::string("cp yukon_dep/") + libname + " " + libdir + "/");
      ret = exec(cmd.c_str());

      if (!ret.empty())
      {
        log(InfoLevel::ERROR, "install third dependency file failed" + ret, ErrorCode::E0008);
        return -1;
      }

      outlibsname << libname << std::endl;
    }
  }

  // 将安装文件记录到 yukondepfile
  std::string path = libdir + "/" + YUKONDEPFILE;
  std::ofstream outfile(path, std::ios::out | std::ios::trunc);
  if (outfile.is_open())
  {
    outfile << outlibsname.str();
  }
  else
  {
    log(InfoLevel::ERROR, "write depenency file failed.", ErrorCode::E0009);
    return -1;
  }
  outfile.close();

  // 拷贝可执行文件
  ret = exec((std::string("cp ./bin/* ") + bindir + "/").c_str());

  if (!ret.empty())
  {
    log(InfoLevel::ERROR, "copy binary file failed." + ret, ErrorCode::E0010);
    return -1;
  }

  log(InfoLevel::INFO, "install finished");
  return 0;
}
int uninstall(std::string pkg)
{
  std::string databasetype;
  std::string databasever;
  std::string pkglibdir;
  std::string sharedir;
  std::string bindir;
  std::string libdir;
  log(InfoLevel::INFO, "uninstall precheck...");
  std::string pgconfigpath;
  // 执行 pg_config 命令，获取
  if (!pkg.empty())
  {
    pgconfigpath = pkg;
    log(InfoLevel::INFO, "user specified pg_config path: " + pgconfigpath);
  }
  else
  {
    pgconfigpath = "pg_config";
  }

  // 1. 数据库版本
  std::string ret;
  ret = exec((pgconfigpath + std::string(" --version")).c_str());

  // 命令错误
  if (ret.find("sh") != std::string::npos)
  {
    log(InfoLevel::ERROR, "pg_config command not valid," + ret, ErrorCode::E0001);
    return -1;
  }
  else
  {
    // 否则解析数据库类型和版本
    // 首先检查是否是 openGauss/GaussDB 数据库
    std::regex reggs("(opengauss|gaussdb).*", std::regex::icase);
    std::regex regpg("PostgreSQL.*", std::regex::icase);
    std::smatch results;
    if (std::regex_search(ret, results, reggs))
    {
      std::string db = results[0];
      if (db.at(db.size() - 1) == '\n')
      {
        db.erase(db.size() - 1);
      }
      int pos = db.find(' ');
      databasetype = db.substr(0, pos);
      databasever = db.substr(pos + 1, db.size() - 1);

    } // 然后检查是否是 postgresql 数据库
    else if (std::regex_search(ret, results, regpg))
    {
      std::string db = results[0];
      if (db.at(db.size() - 1) == '\n')
      {
        db.erase(db.size() - 1);
      }
      int pos = db.find(' ');
      databasetype = db.substr(0, pos);
      databasever = db.substr(pos + 1, db.size() - 1);
    }
    else
    {
      std::cerr << "Not Supported Database" << std::endl;
      return -1;
    }
    // 然后检查是否是 postgres 数据库
    log(InfoLevel::INFO, "database:" + databasetype);
    log(InfoLevel::INFO, "version:" + databasever);
  }

  // pkglibdir
  ret = exec((pgconfigpath + std::string(" --pkglibdir")).c_str());
  if (ret.empty())
  {
    log(InfoLevel::ERROR, "pkglibdir not find!", ErrorCode::E0002);
    return -1;
  }
  else
  {
    pkglibdir = ret;
    if (pkglibdir.at(pkglibdir.size() - 1) == '\n')
    {
      pkglibdir.erase(pkglibdir.size() - 1);
    }
    log(InfoLevel::INFO, "pkglibdir:" + pkglibdir);
  }

  // sharedir
  ret = exec((pgconfigpath + std::string(" --sharedir")).c_str());
  if (ret.empty())
  {
    log(InfoLevel::ERROR, "shareddir not find!", ErrorCode::E0003);
    return -1;
  }
  else
  {
    sharedir = ret;
    if (sharedir.at(sharedir.size() - 1) == '\n')
    {
      sharedir.erase(sharedir.size() - 1);
    }
    log(InfoLevel::INFO, "shareddir:" + sharedir);
  }

  // bindir
  ret = exec((pgconfigpath + std::string(" --bindir")).c_str());
  if (ret.empty())
  {
    log(InfoLevel::ERROR, "bindir not find!", ErrorCode::E0004);
    return -1;
  }
  else
  {
    bindir = ret;
    if (bindir.at(bindir.size() - 1) == '\n')
    {
      bindir.erase(bindir.size() - 1);
    }
    log(InfoLevel::INFO, "bindir:" + bindir);
  }

  // libdir
  ret = exec((pgconfigpath + std::string(" --libdir")).c_str());
  if (ret.empty())
  {
    log(InfoLevel::ERROR, "libdir not find!", ErrorCode::E0005);
    return -1;
  }
  else
  {
    libdir = ret;
    if (libdir.at(libdir.size() - 1) == '\n')
    {
      libdir.erase(libdir.size() - 1);
    }
    log(InfoLevel::INFO, "libdir:" + libdir);
  }

  log(InfoLevel::INFO, "start uninstall...");

  log(InfoLevel::INFO, "uninstall extension file ...");
  {
    std::string path = sharedir + "/extension/" + YUKONSQLFILE;
    std::ifstream infile(path, std::ios::in);
    if (infile.is_open())
    {
      std::string libname;
      while (infile >> libname)
      {
        exec(("rm -rf " + libdir + "/extension/" + libname).c_str());
      }
      // 关闭文件，删除第三方库记录文件
      infile.close();
      exec((std::string("rm -rf ") + path).c_str());
    }
    else
    {
      // 如果找不到，则提示用户
      log(InfoLevel::WARNING,
          "yukonsqlfile file not found,delete sql files manually");
    }

    log(InfoLevel::INFO, "uninstall lib file ...");
    {
      std::string path = pkglibdir + "/" + YUKONPKGFILE;
      std::ifstream infile(path, std::ios::in);
      if (infile.is_open())
      {
        std::string libname;
        while (infile >> libname)
        {
          exec(("rm -rf " + pkglibdir + "/" + libname).c_str());
        }
        // 关闭文件，删除第三方库记录文件
        infile.close();
        exec((std::string("rm -rf ") + path).c_str());
      }
      else
      {
        // 如果找不到，则提示用户
        log(InfoLevel::WARNING,
            "yukonpkgfile file not found,delete pkg files manually");
      }
    }

    log(InfoLevel::INFO, "uninstall binary file ...");
    exec(("rm -rf " + bindir + "/raster2pgsql").c_str());
    exec(("rm -rf " + bindir + "/shp2pgsql").c_str());
    exec(("rm -rf " + bindir + "/pgsql2shp").c_str());
    exec(("rm -rf " + bindir + "/yk_tool").c_str());

    log(InfoLevel::INFO, "uninstall dependendy file ...");

    {
      std::string path = libdir + "/" + YUKONDEPFILE;
      std::ifstream infile(path, std::ios::in);
      if (infile.is_open())
      {
        std::string libname;
        while (infile >> libname)
        {
          exec(("rm -rf " + libdir + "/" + libname).c_str());
        }
        // 关闭文件，删除第三方库记录文件
        infile.close();
        exec((std::string("rm -rf ") + path).c_str());
      }
      else
      {
        // 如果找不到，则提示用户
        log(InfoLevel::WARNING,
            "yukondepfile file not found,delete third-party dependent library files manually");
      }
    }
  }

  log(InfoLevel::INFO, "uninstall finished");

  return 0;
}

int version()
{
  std::ostringstream version;
  version << YUKONVERSION;
  version << COMPILEINFO;
  log(InfoLevel::VERSION, version.str());
  return 0;
}

int license()
{
  std::string licstr = "";
  YkLicenseRSA rsa;
  int ret = rsa.Connect(20100);
  YkLicFeatureInfo licinfo = rsa.GetFeatureInfo();
  struct tm starttm;
  struct tm endtm;
  licinfo.m_startTime.GetDateTime(starttm);
  licinfo.m_expiredTime.GetDateTime(endtm);
  if (0 == ret)
  {
    licstr += "License validity: ";
    licstr += to_string(starttm.tm_year);
    licstr += "/";
    licstr += to_string(starttm.tm_mon);
    licstr += "/";
    licstr += to_string(starttm.tm_mday);
    licstr += "-";
    licstr += to_string(endtm.tm_year);
    licstr += "/";
    licstr += to_string(endtm.tm_mon);
    licstr += "/";
    licstr += to_string(endtm.tm_mday);
  }
  // 5001-过期 5002-未开始
  else if (5001 == ret || 5002 == ret)
  {
    licstr = "Not within the validity period of the license";
  }
  //-1-无法使用许可 31-没有许可文件
  else if (-1 == ret || 31 == ret)
  {
    licstr = "no license file";
  }
  // 5006-计算机名不对 5007-特征码不对
  else
  {
    licstr = "license file is invalid";
  }

  std::cout << licstr << std::endl;

  return 0;
}

int main(int argc, char *argv[])
{

  // 禁止使用  root 用户执行本程序

  std::string ret = exec("echo `id -u`");
  if (!ret.empty() && ret.at(0) == '0')
  {
    log(InfoLevel::ERROR, "Do not use root user to run this program");
    exit(-1);
  }

  cmdline::parser toolparser;
  toolparser.add("version", 'v', "Yukon Version Infomation");
  toolparser.add("license", 'l', "Yukon License Information");
  toolparser.add("remove", 'r', "Remove Yukon");
  toolparser.add("install", 'i', "Install Yukon");
  toolparser.add<std::string>("pkgpath", 0, "pg_config path", false, "");

  toolparser.parse_check(argc, argv);

  if (argc == 1)
  {
    std::cout << toolparser.usage();
    return 0;
  }

  std::string pgconfigPath = toolparser.get<std::string>("pkgpath");

  // boolean flags are referred by calling exist() method.
  if (toolparser.exist("version"))
  {
    version();
  }
  if (toolparser.exist("install"))
  {
    if (install(pgconfigPath) != 0)
    {
      std::cerr << "Install Failed" << std::endl;
    }
  }

  if (toolparser.exist("remove"))
  {
    uninstall(pgconfigPath);
  }
  if (toolparser.exist("license"))
  {
    license();
  }
}