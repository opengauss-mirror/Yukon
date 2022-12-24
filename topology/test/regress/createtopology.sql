\set VERBOSITY terse
set client_min_messages to WARNING;

SELECT public.CreateTopology('2d') > 0;

SELECT public.CreateTopology('2dAgain', -1, 0, false) > 0;

SELECT public.CreateTopology('3d', -1, 0, true) > 0;

SELECT public.CreateTopology('3d'); -- already exists

SELECT name,srid,precision,hasz from public.topology
WHERE name in ('2d', '2dAgain', '3d' )
ORDER by name;

-- Only 3dZ accepted in 3d topo
SELECT public.AddNode('3d', 'POINT(0 0)');
SELECT public.AddNode('3d', 'POINTM(1 1 1)');
SELECT public.AddNode('3d', 'POINT(2 2 2)');

-- Only 2d accepted in 2d topo
SELECT public.AddNode('2d', 'POINTM(0 0 0)');
SELECT public.AddNode('2d', 'POINT(1 1 1)');
SELECT public.AddNode('2d', 'POINT(2 2)');

SELECT public.DropTopology('2d');
SELECT public.DropTopology('2dAgain');
SELECT public.DropTopology('3d');

-- Exceptions
SELECT public.CreateTopology('public');
SELECT public.CreateTopology('topology');
