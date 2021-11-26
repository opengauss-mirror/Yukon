FAQ
===

-  为什么我无法卸载 yukon\_geomodel 插件?
    由于目前 openGauss 的白名单机制，除 PostGIS
    以外的插件，其余插件均无法卸载，但可以在 postgresql.conf 里设置参数
    "support\_extended\_features = on" 即可卸载插件。

-  为什么一直连接不到数据库？
    由于 openGauss 默认使用 sha256 加密，你需要修改为 md5
    加密后，重新创建用户名和密码。你也可以参考\ `连接问题 <https://www.modb.pro/db/27157>`__\ 。

-  为什么在创建扩展时显示没有权限访问动态库？
    请查看相关的库文件有没有除 root 用户之外的读取权限
