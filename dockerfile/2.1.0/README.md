# 镜像制作

此镜像基于 [openGauss](https://gitee.com/opengauss/openGauss-server/tree/2.1.0/docker) 仓库内的 Dockerfile 制作。

## 文件准备

1. openGauss2.1.0 (openGauss-2.1.0-CentOS-64bit.tar.bz2)安装包请放置于根目录下
2. 将编译好的 yukon 文件放置于 yukon 文件夹内，其中：

    1. extension 目录放置 sql 文件和 control 文件
    2. lib 目录放置动态库文件
    3. yukon_dep 放置第三方依赖库文件,可以在 gitee 仓库的发行版中下载安装包。


## 编译镜像

使用  docker build 命令编译



