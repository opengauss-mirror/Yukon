矢量对象自相交检查
-----------------------

postgis 提供的 ST_IsValid 函数即可检测对象是否自相交。

.. code:: SQL

    SELECT ST_IsValid(ST_GeomFromText('LINESTRING(0 0, 1 1)')) As good_line,
    ST_IsValid(ST_GeomFromText('POLYGON((0 0, 1 1, 1 2, 1 1, 0 0))')) As bad_poly
    -- results
    NOTICE: Self-intersection at or near point 0 0
    good_line|bad_poly|
    ---------+--------+
    true     |false   |

相关函数还有 ST_IsValidReason，给出非法的原因；ST_IsValidDetail 给出非法原因的同时，定位到具体的非法空间位置。


