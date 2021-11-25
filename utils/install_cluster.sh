#!/bin/bash

#集群化自动部署脚本 filename(存放部署节点ip的文件) user_name(安装的用户名)

filename=""
user_name="omm"

while [ $# -gt 0 ]; do
    case $1 in
        -f|--filename )
        if [ "$2"X = X ]
        then
            echo "没有给出 $1"
            exit 1
        fi
        filename=$2
        shift 2
        ;;
        -U|--user_name )
        if [ "$2"X = X ]
        then
            echo "没有给出 $1"
            exit 1
        fi
        user_name=$2
        shift 2
        ;;

        * )
                echo "参数错误 输入-f [文件名] 执行脚本"
                exit 1
    esac
done

[ ! $filename ] && echo "缺少 hostfile; 输入 -f [filename]" && exit

if [ -a $filename ]
then
    cat $filename | while read line
    do
        echo "在$line 上部署Yukon..."
        ssh root@$line "mkdir -p /opt/software"
        scp Yukon-1.0.tar root@$line:/opt/software/
        ssh root@$line "cd /opt/software; tar -xf Yukon-1.0.tar"
        ssh root@$line "cd /opt/software/Yukon; sh install.sh $user_name"
        echo "$line 部署完成..."
    done
else
    echo "$filename文件不存在"
    exit
fi


