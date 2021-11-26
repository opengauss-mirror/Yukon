#!/bin/bash

function install() {

    echo "开始安装......"

    if [ "$GAUSSHOME" == "" ]; then
        echo "没有检测到openGauss环境，请加载相关环境变量"
        exit
    fi

    if [ ! -d /opt/yukon_dep ]; then
        mkdir -p /opt/yukon_dep
    fi

    cp -r yukon_dep/* /opt/yukon_dep

    #检测三方库路径是否加载

    if [ $(grep -c "yukon_dep" /etc/ld.so.conf) -eq '0' ]; then
        echo /opt/yukon_dep/gdal/lib >>/etc/ld.so.conf
        echo /opt/yukon_dep/geos/lib >>/etc/ld.so.conf
        echo /opt/yukon_dep/proj4/lib >>/etc/ld.so.conf
        echo /opt/yukon_dep/jsonc/lib >>/etc/ld.so.conf
        echo /opt/yukon_dep/libxml2/lib >>/etc/ld.so.conf
        echo /opt/yukon_dep/ugc >>/etc/ld.so.conf
        echo /opt/yukon_dep/sfcgal/lib64 >>/etc/ld.so.conf
        echo /opt/yukon_dep/cgal/lib >>/etc/ld.so.conf
    fi

    ldconfig

    chmod 755 -R /opt/yukon_dep

    #################
    #安装Yukon
    #################

    cp lib/* $GAUSSHOME/lib/postgresql
    cp extension/* $GAUSSHOME/share/postgresql/extension

    #################
    #修改权限
    #################

    sudo chmod a+r $GAUSSHOME/lib/postgresql/*
    sudo chmod a+r $GAUSSHOME/share/postgresql/extension/*

    echo "__     __      _                   _____   ____  "
    echo "\ \   / /     | |                 |  __ \ |  _ \ "
    echo " \ \_/ /_   _ | | __ ___   _ __   | |  | || |_) |"
    echo "  \   /| | | || |/ // _ \ | '_ \  | |  | ||  _ < "
    echo "   | | | |_| ||   <| (_) || | | | | |__| || |_) |"
    echo "   |_|  \__,_||_|\_\\\___/ |_| |_| |_____/ |____/ "

}

function uninstall()
{
    echo "开始卸载......"

    if [ "$GAUSSHOME" == "" ]; then
        echo "没有检测到openGauss环境，请加载相关环境变量"
        exit
    fi

    # 删除第三方库目录
    rm -rf /opt/yukon_dep

    # 删除动态库文件
    rm -rf $GAUSSHOME/lib/postgresql/postgis*
    rm -rf $GAUSSHOME/lib/postgresql/yukon*

    # 删除 sql 和 control 文件

    rm -rf  $GAUSSHOME/share/postgresql/extension/postgis*
    rm -rf  $GAUSSHOME/share/postgresql/extension/yukon*

    # 删除动态库路径
    sed -i '/yukon_dep/d' /etc/ld.so.conf

    echo "卸载 Yukon 完成"

}

function usage()
{
    echo "Usage:"
    echo "--------------------------------"
    echo "install.sh -i 安装 Yukon "
    echo "install.sh -r 卸载 Yukon "
    echo "--------------------------------"
    
}


if [ $# -lt 1 ];then
    usage
    exit
elif [ $1 == '-r' ];then
    uninstall
elif [ $1 == '-i' ];then
    install
else 
    usage
fi

