set client_min_messages to ERROR;

SELECT public.CreateTopology('tt') > 0;

SELECT public.ST_GetFaceEdges(null, null);
SELECT public.ST_GetFaceEdges('tt', null);
SELECT public.ST_GetFaceEdges(null, 1);
SELECT public.ST_GetFaceEdges('', 1);
SELECT public.ST_GetFaceEdges('NonExistent', 1);

SELECT 'E'||public.AddEdge('tt', 'LINESTRING(2 2, 2  8)');        -- 1
SELECT 'E'||public.AddEdge('tt', 'LINESTRING(2  8,  8  8)');      -- 2
SELECT 'E'||public.AddEdge('tt', 'LINESTRING(8  8,  8 2, 2 2)');  -- 3
SELECT 'E'||public.AddEdge('tt', 'LINESTRING(0 0, 0 10, 10 10)'); -- 4
SELECT 'E'||public.AddEdge('tt', 'LINESTRING(0 0, 10 0)');        -- 5
SELECT 'E'||public.AddEdge('tt', 'LINESTRING(10 10, 10 5)');      -- 6
SELECT 'E'||public.AddEdge('tt', 'LINESTRING(10 0, 10 5)');       -- 7

SELECT 'F' ||
  public.AddFace('tt',
    'POLYGON((0 0, 10 0, 10 10, 0 10, 0 0),(2 2, 2 8, 8 8, 8 2, 2 2))');
SELECT 'F' ||
  public.AddFace('tt',
    'POLYGON((2 2, 2 8, 8 8, 8 2, 2 2))');

SELECT 'F1', (public.ST_GetFaceEdges('tt', 1)).*;
SELECT 'F2', (public.ST_GetFaceEdges('tt', 2)).*;

SELECT 'F0', (public.ST_GetFaceEdges('tt', 0)).*;

SELECT public.DropTopology('tt');

-- See https://trac.osgeo.org/postgis/ticket/3265
SELECT public.CreateTopology('tt') > 0;
SELECT '#3265.1', 'E'||public.TopoGeo_addLinestring('tt',
  'LINESTRING(150 150, 180 150, 180 180)');
SELECT '#3265.2', 'E'||public.TopoGeo_addLinestring('tt',
  'LINESTRING(150 150, 180 180)');
SELECT '#3265.3', 'E'||public.TopoGeo_addLinestring('tt',
  'LINESTRING(178 170, 178 161, 170 161, 178 170)');
SELECT '#3265.4', 0, ST_GetFaceEdges('tt', 0);
SELECT '#3265.5', 1, ST_GetFaceEdges('tt', 1);
SELECT '#3265.6', 2, ST_GetFaceEdges('tt', 2);
SELECT public.DropTopology('tt');
