入门教程
========

准备工作
--------

在使用空间数据库之前，我们需要了解用户及其权限、数据库的字符集和表空间等数据库基础内容。如果您已具备相关基础，可以略过本节。

创建用户
~~~~~~~~

在安装 openGauss 数据库时，会自动创建一个初始用户，初始用户具有系统的最高权限，能够执行所有的操作。这里我们通过命令行登录到数据库，并创建一个带有
``SYSADMIN`` 属性的新用户。

.. code:: sh

    # 登录到数据库
    gsql -d postgres

    # 创建新用户 yukontest

    CREATE USER yukontest WITH SYSADMIN password "Bigdata@123";

如果新创建的用户不带有 ``SYSADMIN`` 属性，很多操作都会没有权限。关于用户和权限的相关问题可以参考 openGauss 的 `文档 <https://opengauss.org/zh/docs/2.1.0/docs/Developerguide/%E7%AE%A1%E7%90%86%E7%94%A8%E6%88%B7%E5%8F%8A%E6%9D%83%E9%99%90.html>`__;

创建表空间
~~~~~~~~~~

通过使用表空间，管理员可以控制一个数据库安装的磁盘布局。这样有以下优点：

-  如果初始化数据库所在的分区或者卷空间已满，又不能逻辑上扩展更多空间，可以在不同的分区上创建和使用表空间

-  表空间允许管理员根据数据库对象的使用模式安排数据位置，从而提高性能

   -  一个频繁使用的索引可以放在性能稳定且运算速度较快的磁盘上，比如一种固态设备
   -  一个存储归档的数据，很少使用的或者对性能要求不高的表可以存储在一个运算速度较慢的磁盘上

-  管理员通过表空间可以设置占用的磁盘空间。用以在和其他数据共用分区的时候，防止表空间占用相同分区上的其他空间

这里我们使用刚才创建的 yukontest 用户来创建一个表空间

.. code:: sh

    # 登录到数据库
    gsql -d postgres -U yukontest -W Bigdata@123

    # 创建表空间 yukonspace
    CREATE TABLESPACE yukonspace LOCATION '/home/omm/data';

注意：目录 ``/home/omm/data`` 必须已经存在且具有可访问权限。

创建数据库
~~~~~~~~~~

创建一个新的数据库。缺省情况下新的数据库将通过复制标准系统数据库 ``template0`` 来创建。且仅支持使用 ``template0`` 来创建


.. note::
   #. 只有拥有 CREATEDB 权限的用户才可以创建新数据库，系统管理员默认拥有此权限。 
   #. 不能在事务块中执行创建数据库语句。 
   #. 在创建数据库过程中，出现类似 ``Permission denied`` 的错误提示，可能是由于文件系统上数据目录的权限不足。出现类似 ``No space left on device`` 的错误提示，可能是由于磁盘满引起的。

如果您在数据库中存储中文字符，推荐选择 GBK 编码，当然您也可以选择 UTF8 编码。

这里我们创建一个 yukontutorial 数据库来进行后面的学习。

.. code:: sh

    # 使用 yukontest 用户连接到 postgres 数据库
    gsql -d postgres -U yukontest -W Bigdata@123

    # 创建数据库 yukontutorial ，并指定数据库编码为 'UTF8' 表空间为上面创建好的 yukonspace
    CREATE DATABASE yukontutorial ENCODING='UTF8' TABLESPACE=yukonspace;

有关更多创建数据库的操作可以参考 openGauss 的\ `文档 <https://opengauss.org/zh/docs/2.1.0/docs/Developerguide/CREATE-DATABASE.html>`__

使用矢量数据
------------

创建 postgis 扩展
~~~~~~~~~~~~~~~~~

在命令行使用 yukontest 用户连接到 ``yukontutorial`` 数据库：

.. code:: sh

    # 连接到数据库
    gsql -d yukontutorial -U yukontest -W Bigdata@123

.. code:: SQL

    --创建 postgis 扩展
    CREATE EXTENSION postgis;

显示如下信息，则表示创建成功：

CREATE EXTENSION

写入数据
~~~~~~~~

新建表格 global\_points，并写入点数据：

.. code:: SQL

    -- 创建带geometry列的表
    CREATE TABLE global_points (id SERIAL PRIMARY KEY,name VARCHAR(64),location geometry(POINT,3857));

    -- 写入10个点对象
    INSERT INTO global_points (name, location) VALUES ('Town1', ST_GeomFromEWKT('SRID=3857;POINT (12057531.405152922 4096788.3009192282)') );
    INSERT INTO global_points (name, location) VALUES ('Town2', ST_GeomFromEWKT('SRID=3857;POINT (12057879.323089447 4096794.06898925)') );
    INSERT INTO global_points (name, location) VALUES ('Town3', ST_GeomFromEWKT('SRID=3857;POINT (12057241.150712628 4096835.404659481)') );
    INSERT INTO global_points (name, location) VALUES ('Town4', ST_GeomFromEWKT('SRID=3857;POINT (12057396.37461059 4096885.372392862)') );
    INSERT INTO global_points (name, location) VALUES ('Town5', ST_GeomFromEWKT('SRID=3857;POINT (12056940.710538926 4096886.9123589676)') );
    INSERT INTO global_points (name, location) VALUES ('Town6', ST_GeomFromEWKT('SRID=3857;POINT (12058128.24460281 4096938.23117457)') );
    INSERT INTO global_points (name, location) VALUES ('Town7', ST_GeomFromEWKT('SRID=3857;POINT (12058378.134595742 4097081.4360874156)') );
    INSERT INTO global_points (name, location) VALUES ('Town8', ST_GeomFromEWKT('SRID=3857;POINT (12058636.607321415 4097235.1257181372)') );
    INSERT INTO global_points (name, location) VALUES ('Town9', ST_GeomFromEWKT('SRID=3857;POINT (12058917.488660585 4097395.7746241256)') );
    INSERT INTO global_points (name, location) VALUES ('Town10', ST_GeomFromEWKT('SRID=3857;POINT (12059180.102471316 4097548.7255362184)') );

空间查询
~~~~~~~~

查询距离指定点500米范围内的对象：

.. code:: SQL

    SELECT * FROM global_points WHERE ST_DWithin(location, ST_GeomFromEWKT('SRID=3857;POINT (12058378 4097081)'), 500);

输出结果：

.. code:: sh

     id | name  |                      location
    ----+-------+----------------------------------------------------
      6 | Town6 | 0101000020110F000046C9D307C2FF6641D920971DD5414F41
      7 | Town7 | 0101000020110F0000BB9B4E44E1FF664162B6D1B71C424F41
      8 | Town8 | 0101000020110F0000522D6F93010067412C88179069424F41
    (3 rows)

栅格数据
------------

创建 postgis_raster 扩展
~~~~~~~~~~~~~~~~~~~~~~~~~

在命令行使用 yukontest 用户连接到 ``yukontutorial`` 数据库：

.. code:: sh

    # 连接到数据库
    gsql -d yukontutorial -U yukontest -W Bigdata@123

.. code:: SQL

    --创建 postgis_raster 扩展
    CREATE EXTENSION postgis_raster;

显示如下信息，则表示创建成功：

CREATE EXTENSION

创建成功后，退出 gsql 控制台。

导入数据
~~~~~~~~

用 raster2pgsql 工具导入范例数据 hillshade.tif 数据：

.. code:: sh

    raster2pgsql -s 0 /SampleData/hillshade.tif -t 256x256 | gsql -d yukontutorial 

导入成功后，再次登录数据库，刷新元数据信息：

.. code:: sh

    # 连接到数据库
    gsql -d yukontutorial -U yukontest -W Bigdata@123
    # 刷新元数据信息
    select AddRasterConstraints('hillshade', 'rast');
    # 查看信息
    select * from raster_columns where r_table_name='hillshade';

使用栅格数据
~~~~~~~~~~~~

提取指定像素的值：

.. code:: SQL

    select ST_Value(rast, 1, 10, 10, true) from hillshade ;
