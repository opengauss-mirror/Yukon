安装与卸载
==========

在安装 Yukon 之前必须先安装 openGauss 数据库。如果还没有安装 openGauss 数据库，请先按照 `openGauss <https://opengauss.org/zh/docs/2.0.1/docs/installation/installation.html>`__ 安装指导进行安装。

.. note::
    openGauss 支持的操作系统有： centos_x86_64、openeuler_aarch64、openeuler_x86_64，请优先在这些操作系统进行安装。

openGauss安装成功后，开始 Yukon 产品的安装。

安装包安装
----------

我们提供了一个 ``Yukon-1.0-Alpha.tar.gz`` 压缩包，可以直接安装，里面包含了Yukon相关的帮助文档以及安装包。

下载并解压后进入到 Yukon-1.0-Alpha 目录中，首先加载 openGauss 环境变量，使用脚本一键安装的单机版环境变量一般为 ``source /home/user/env_single`` ，手动安装集群式安装环境变量一般为
``source /home/user/.bashrc``\ ，完成后进入 Yukon-1.0 目录下执行 ``sh install_yukon.sh -i``
进行安装，看到如下输出则说明安装成功。

.. code:: text


     __     __      _                   
     \ \   / /     | |                 
      \ \_/ /_   _ | | __ ___   _ __   
       \   /| | | || |/ // _ \ |  _ \  
        | | | |_| ||   <| (_) || | | | 
        |_|  \____||_|\_\\___/ |_| |_| 

编译安装
--------

1. 获取 Yukon 源码

如果您已经安装了 git 工具，那么可以在 Yukon 的 Gitee
仓库获取我们最新的源码安装包。

.. code:: bash

    git clone https://gitee.com/opengauss/Yukon.git 

2. 准备编译环境

   安装下列软件依赖包

   -  `gcc/g++ <https://centos.pkgs.org/7/centos-sclo-rh-x86_64/devtoolset-7-gcc-7.3.1-5.4.el7.x86_64.rpm.html>`__
      >= 7.3
   -  `gdal-devel <https://gdal.org/download.html>`__ >= 1.11.0
   -  `geos-devel <https://trac.osgeo.org/geos/>`__ >=3.6.0
   -  `json-c-devel <https://github.com/json-c/json-c>`__
   -  `proj-devel <https://proj.org/>`__
   -  `libxml2-devel <http://www.xmlsoft.org/>`__
   -  `sfcgal <http://www.sfcgal.org/>`__

   以上安装包可以通过源码安装，也可以直接使用 yum 工具直接安装。如果还缺少其他依赖，可自行安装。
    
3. 开始编译
   
   准备好编译环境以后，就可以开始进行编译安装：

   .. code:: bash

       # 如果您想使用 SFCGAL 作为 PostGIS 的处理后端，那么在准备安装环境时，需要预先安装 SFCGAL

       进入Yukon目录后运行配置文件：
       cd Yukon  
       ./configure --with-pgconfig=/pg_config --without-topology  --without-address-standardizer \
                   --without-interrupt-tests CFLAGS='-O2 -fpermissive -DPGXC  -pthread ' CC=g++    
       make
       make install 

   .. note::
       安装时可能会提示 openGauss 目录下缺少部分头文件，此时需要从 `openGauss源码库 <https://gitee.com/opengauss/openGauss-server/tree/2.0.0>`__ 中的 src/include 目录
       下拷贝对应文件到提示的目录下，出于方便也可以直接全部覆盖。

   如果在安装 gdal、 geos、 json-c、 libxml2、porj、 sfcgal时自定义了安装路径，那么运行配置文件时需要添加以下的配置选项：

   .. code:: bash

       --with-gdalconfig=/path/gdal-config --with-geosconfig=/path/geos-config \
       --with-xml2config=/path/xml2-config --with-jsondir=/path/json-c \
       --with-projdir=/path/proj4 --with-sfcgal=/path/sfcgal-config 

   同时，需要将这些依赖的动态库的路径添加到 /etc/ld.so.conf 文件中，然后使用 ``ldconfig`` 命令加载路径。

4. 开始使用
   
   以上安装完成后，可以进入数据库操作。

   .. code:: bash

       --- 切换到 omm (数据库安装用户)
       su omm
       --- 连接到 postgres 数据库
       gsql -d postgres

   .. code:: sql

       --- 创建 postgis 扩展
       CREATE EXTENSION postgis;
       --- 创建 postgis_raster 扩展
       CREATE EXTENSION postgis_raster;
       --- 创建 yukon_geomodel 扩展
       CREATE EXTENSION yukon_geomodel;
       -- 如果在编译时支持了 SFCGAL 则可以创建 SFCGAL 扩展
       CREATE EXTENSION postgis_sfcgal;

容器安装
--------

环境变量
~~~~~~~~

-  ``GS_PASSWORD`` ：【\ **必须设置**\ 】，该参数设置了 openGauss
   数据库的超级用户 omm 以及外部连接用户 gaussdb 的密码。openGauss
   安装时默认会创建 omm 超级用户，测试用户 gaussdb 是在
   `entrypoint.sh <https://gitee.com/opengauss/openGauss-server/blob/master/docker/dockerfiles/1.1.0/entrypoint.sh>`__
   中自定义创建的用户，openGauss 设置的密码要符合复杂度要求：

   -  最少包含8个字符。
   -  不能和用户名、当前密码（ALTER）、或当前密码反序相同。
   -  至少包含大写字母（A-Z），小写字母（a-z），数字，非字母数字字符（限定为~!@#$%^&\*()-\_=+\|[{}];:,<.>/?）四类字符中的三类字符。
  
-  ``GS_NODENAME``\ ：【可选设置】，该参数设置了 openGauss
   数据库的节点名称，默认为 gaussdb，可以在 ``entrypoint.sh`` 中进行查看
-  ``GS_USERNAME``\ ：【可选设置】，该参数设置了 openGauss
   数据库的外部连接用户名，默认为 gaussdb，可以在 ``entrypoint.sh``
   中进行查看
-  ``GS_PORT``:【可选设置】，该参数设置了 openGauss
   数据库的连接端口，默认为 5432，可以在 ``entrypoint.sh`` 中进行查看。

体验 Yukon
~~~~~~~~~~

.. code:: bash

    docker run --name Yukon --privileged=true -d -e GS_PASSWORD=Bigdata@123 supermap/Yukon:latest

稍等片刻，等 Yukon 启动后，您可以使用如下命令来体验 Yukon：

.. code:: bash

    sudo docker exec -it Yukon  /bin/bash
    su omm
    gsql -d postgres

连接成功后，可以看到类似如下的字符：

.. code:: bash

    [omm@367dd3bb5d10 /]$ gsql -d postgres 
    gsql ((openGauss 2.0.0 build 78689da9) compiled at 2021-03-31 21:04:03 commit 0 last mr  )
    Non-SSL connection (SSL connection is recommended when requiring high-security)
    Type "help" for help.

    postgres=#

在这里创建 postgis 和 postgis\_raster 扩展。

.. code:: text

    create extension postgis;
    create extension postgis_raster;
    # 查询 postgis 库版本
    select postgis_lib_version();

外部连接数据库
~~~~~~~~~~~~~~~~~~~~

openGauss 的默认监听端口为 5432
，如果想要从容器外部访问数据库，则需要在\ ``docker run``\ 的时候指定\ ``-p``
进行端口映射：

.. code:: bash

    docker run --name Yukon --privileged=true -d -e GS_PASSWORD=Bigdata@123 -p 5432:5432 supermap/Yukon:latest

现在就可以使用 openGauss 的数据库管理工具 DataStudio，或者使用开源的
DBeaver
连接数据库（第一次连接时间可能较长，需要等待数据库安装初始化完成）。

持久化存储数据
~~~~~~~~~~~~~~

容器一旦被删除，容器内的所有数据和配置也均会丢失，而从镜像重新运行一个容器的话，则所有数据又都是呈现在初始化状态，因此对于数据库容器来说，为了防止因为容器的消亡或者损坏导致的数据丢失，需要进行持久化存储数据的操作。通过在
``docker run``\ 的时候指定 ``-v``\ 参数来实现，或者可以指定已经建立好的
Volume 。使用以下命令将 openGauss 的所有数据文件存储在宿主机的
/Yukon/opengauss 下。

.. code:: bash

    mkdir -p /Yukon/opengauss
    docker run --name Yukon --privileged=true -d -e GS_PASSWORD=Bigdata@123 \
        -v /Yukon/opengauss:/var/lib/opengauss  -p 5432:5432 \
        supermap/Yukon:latest

主备节点安装
------------

如果是在 openGauss 集群下安装 Yukon，需要在主机间执行命令，传送文件等操作。
在主机之间建立互信的基础上，只需执行安装包里的脚本文件 ``sh install_cluster.sh -f hostfile -U username`` 即可（openGauss安装集群时，会在主机间建立互信，如果此时没有互信，可以再重新手动建立互信）。

-  ``hostfile`` ：存放有openGauss中所有主机IP的配置文件。

例： /opt/software/openGauss> vim hostfile

.. code::text
    192.168.0.1
    192.168.0.2
    192.168.0.3

-  ``username`` ：openGauss集群所使用的安装用户，默认为 ``omm`` 用户。

卸载
----

如果使用了我们的压缩包进行安装，执行 ``sh install_yukon.sh -r``
命令即可卸载。

如果使用源码进行编译安装，在源码根目录下，执行命令 ``make uninstall``
进行卸载。

如果使用 Docker 进行安装，直接删除 Docker 容器和镜像即可。

您也可以手动检查以下三个地方查看是否有卸载残留：

-  ``$GAUSSHOME/lib/postgresql/`` 目录下与 postgis 和 yukon 相关的文件
-  ``$GAUSSHOME/share/postgresql/extension/`` 目录下与 postgis 和 yukon
   相关的文件
-  ``/opt`` 目录下是否有 ``yukon_dep`` 文件夹
