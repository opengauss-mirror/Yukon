set client_min_messages to WARNING;

--
-- ST_InitTopoGeo
--

SELECT regexp_replace(ST_InitTopoGeo('sqlmm_topology'), 'id:[0-9]*', 'id:x');

-------------------------------------------------------------
-- ST_AddIsoNode (1)
-------------------------------------------------------------

SELECT '-- ST_AddIsoNode ------------------------';

-- null input
SELECT public.ST_AddIsoNode('sqlmm_topology', NULL, NULL);
SELECT public.ST_AddIsoNode(NULL, NULL, 'POINT(0 0)');
SELECT public.ST_AddIsoNode(NULL, 1, NULL);

-- good nodes on the 'world' face
SELECT public.ST_AddIsoNode('sqlmm_topology', NULL, 'POINT(0 0)');
SELECT public.ST_AddIsoNode('sqlmm_topology', NULL, 'POINT(10 0)');
SELECT public.ST_AddIsoNode('sqlmm_topology', NULL, 'POINT(5 0)');
SELECT public.ST_AddIsoNode('sqlmm_topology', NULL, 'POINT(5 10)');
SELECT public.ST_AddIsoNode('sqlmm_topology', NULL, 'POINT(10 10)');
SELECT public.ST_AddIsoNode('sqlmm_topology', NULL, 'POINT(20 10)');

-- existing nodes
SELECT public.ST_AddIsoNode('sqlmm_topology', NULL, 'POINT(0 0)');
SELECT public.ST_AddIsoNode('sqlmm_topology', NULL, 'POINT(10 0)');

-- other good ones (add another 0 to be detected as coincident)
SELECT public.ST_AddIsoNode('sqlmm_topology', NULL, 'POINT(10.000000000000001 0)');
SELECT public.ST_AddIsoNode('sqlmm_topology', NULL, 'POINT(7 10)');

-- non-existent face specification
SELECT public.ST_AddIsoNode('sqlmm_topology', 1, 'POINT(20 0)');

-- using other then point
SELECT public.ST_AddIsoNode('sqlmm_topology', 1, 'MULTIPOINT(20 0)');

-- coincident nodes
SELECT public.ST_AddIsoNode('sqlmm_topology', NULL, 'POINT(10.000000000000001 0)');
SELECT public.ST_AddIsoNode('sqlmm_topology', NULL, 'POINT(0 0)');
SELECT public.ST_AddIsoNode('sqlmm_topology', NULL, 'POINT(10 0)');

-- ST_AddIsoNode not within face (TODO when ST_GetFaceGeometry is done)

------------------------------------------
-- ST_AddIsoEdge (1)
------------------------------------------

SELECT '-- ST_AddIsoEdge ------------------------';

-- null input
SELECT public.ST_AddIsoEdge('sqlmm_topology', 1, 2, NULL);
SELECT public.ST_AddIsoEdge(NULL, 1, 2, 'LINESTRING(0 0, 1 1)');
SELECT public.ST_AddIsoEdge('sqlmm_topology', 1, NULL, 'LINESTRING(0 0, 1 1)');
SELECT public.ST_AddIsoEdge('sqlmm_topology', NULL, 2, 'LINESTRING(0 0, 1 1)');

-- invalid curve
SELECT public.ST_AddIsoEdge('sqlmm_topology', 1, 2, 'POINT(0 0)');

-- non-simple curve
SELECT public.ST_AddIsoEdge('sqlmm_topology', 1, 2, 'LINESTRING(0 0, 10 0, 5 5, 5 -5)');

-- non-existing nodes
SELECT public.ST_AddIsoEdge('sqlmm_topology', 10000, 2, 'LINESTRING(0 0, 1 1)');

-- Curve endpoints mismatch
SELECT public.ST_AddIsoEdge('sqlmm_topology', 1, 2, 'LINESTRING(0 0, 1 1)');
SELECT public.ST_AddIsoEdge('sqlmm_topology', 1, 2, 'LINESTRING(0 1, 10 0)');

-- Node crossing
SELECT public.ST_AddIsoEdge('sqlmm_topology', 1, 2, 'LINESTRING(0 0, 10 0)');

-- Good ones
SELECT public.ST_AddIsoEdge('sqlmm_topology', 4, 5, 'LINESTRING(5 10, 5 9, 10 10)');
SELECT public.ST_AddIsoEdge('sqlmm_topology', 1, 2, 'LINESTRING(0 0, 2 1, 10 5, 10 0)');

-- Not isolated edge (shares endpoint with previous)
SELECT public.ST_AddIsoEdge('sqlmm_topology', 4, 6, 'LINESTRING(5 10, 10 9, 20 10)');
SELECT public.ST_AddIsoEdge('sqlmm_topology', 5, 6, 'LINESTRING(10 10, 20 10)');

-- Edge intersection (geometry intersects an edge)
SELECT public.ST_AddIsoEdge('sqlmm_topology', 1, 2, 'LINESTRING(0 0, 2 20, 10 0)');

-- on different faces (TODO req. nodes contained in face)

-------------------------------------------------------------
-- ST_AddIsoNode (2)
-------------------------------------------------------------

SELECT '-- ST_AddIsoNode(2) ------------------------';

-- ST_AddIsoNode edge-crossing node
SELECT public.ST_AddIsoNode('sqlmm_topology', NULL, 'POINT(5 9.5)');

-------------------------------------------------------------
-- ST_RemoveIsoEdge
-------------------------------------------------------------
SELECT '-- ST_RemoveIsoEdge ---------------------';

CREATE TEMP TABLE edge1_endnodes AS
  WITH edge AS (
    SELECT start_node,end_node
    FROM sqlmm_public.edge_data
    WHERE edge_id = 1
  )
  SELECT start_node id FROM edge UNION
  SELECT end_node FROM edge;
SELECT '#3351.1', node_id, containing_face
 FROM sqlmm_public.node where node_id in (
    SELECT id FROM edge1_endnodes
  )
 ORDER BY node_id;
SELECT public.ST_RemoveIsoEdge('sqlmm_topology', 1);
SELECT '#3351.2', node_id, containing_face
 FROM sqlmm_public.node where node_id in (
    SELECT id FROM edge1_endnodes
  )
 ORDER BY node_id;
DROP TABLE edge1_endnodes;

-------------------------------------------------------------
-- ST_NewEdgesSplit
-------------------------------------------------------------

SELECT '-- ST_NewEdgesSplit  ---------------------';
SELECT public.ST_NewEdgesSplit('sqlmm_topology', 2, 'POINT(10 2)');

SELECT public.DropTopology('sqlmm_topology');
