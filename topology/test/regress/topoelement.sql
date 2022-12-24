-- Good
SELECT '1','{101,1}'::public.TopoElement;
SELECT '2','{101,2}'::public.TopoElement;
SELECT '3','{101,3}'::public.TopoElement;
SELECT '4','{1,104}'::public.TopoElement; -- layer id has no higher limit
-- Invalid: has 3 elements
SELECT '[0:2]={1,2,3}'::public.TopoElement;
-- Invalid: 0 is both an invalid primitive element id and an invalid layer id
SELECT '{1,0}'::public.TopoElement;
