数据库备份与恢复
======================

这里我们使用 openGauss 内置的备份与恢复工具： ``gs_dump`` 和 ``gs_restore`` 。

备份
----

``gs_dump`` 用法：

.. code:: sh

    gs_dump [OPTION]... [DBNAME]

以下是一些常用的选项：

-  ``-f``: 输出的文件名或者目录名
-  ``-F``: 输出的文件格式 ``c``: 自定义格式  ``d``:目录格式 ``t``: tar 格式  ``p``:简单文本模式（默认格式）
-  ``-Z``: 压缩文件的压缩等级，范围为 0-9
-  ``-t``: 只备份数据库中的某一张表
-  ``-T``: 备份除这个表之外的其余表

在 yukontutorial 数据库中创建一个 backtest 的表，然后进行备份：

.. code:: SQL

    -- 创建 backtest 表
    CREATE TABLE backtest(geom geometry);
    -- 插入数据
    INSERT INTO backtest(geom) values('Point(1 2)');
    -- 查看插入的数据
    SELECT ST_ASTEXT(geom) from backtest;

创建好数据就可以来备份数据了，退出数据库然后在命令行执行：

.. code:: sh

    # 这里只备份 yukontutorial 数据库中的 backtest 表，输出格式为自定义格式，输出文件名为 testdump.bak
    gs_dump  yukontutorial -t backtest -Fc -f testdump.bak

显示如下信息，则表示备份成功：

.. code:: text

    gs_dump[port='5432'][yukontutorial][2021-10-19 04:05:43]: The total objects number is 379.
    gs_dump[port='5432'][yukontutorial][2021-10-19 04:05:43]: [100.00%] 379 objects have been dumped.
    gs_dump[port='5432'][yukontutorial][2021-10-19 04:05:43]: dump database yukontutorial successfully
    gs_dump[port='5432'][yukontutorial][2021-10-19 04:05:43]: total time: 517  ms

恢复
----

``gs_restore`` 用法：

.. code:: sh

    gs_restore [OPTION]... FILE

以下是一些常用的选项：

-  ``-d``: 要还原到的数据库
-  ``-j``: 多线程恢复

使用上面备份的数据表进行还原：

.. code:: SQL

    -- 先创建一个新的数据库
    CREATE DATABASE restoretest;

    -- 切换到这个数据库 restoretest
    -- 创建 postgis 扩展
    CREATE EXTENSION postgis;

断开连接进行还原

.. code:: sh

    gs_restore -d restoretest testdump.bak

显示如下信息，则表示备份成功：

.. code:: text

    start restore operation ...
    table backtest complete data imported !
    Finish reading 4 SQL statements!
    end restore operation ...
    restore operation successful
    total time: 427  ms

.. warning::
    因为备份的数据库中含有 geometry 类型的数据，因此在还原的时候要先创建 postgis 扩展，才能恢复数据库，否则将会报错。
