set client_min_messages to WARNING;

SELECT public.CreateTopology('tt') > 0;

SELECT 'e1',  public.addEdge('tt', 'LINESTRING(0 0, 8 0)');
-- Equal edge
SELECT 'e*1', public.addEdge('tt', 'LINESTRING(0 0, 8 0)');

-- Failing cases (should all raise exceptions) -------

-- Contained with endpoint contact
SELECT 'e*2', public.addEdge('tt', 'LINESTRING(1 0, 8 0)');
-- Contained with no endpoint contact
SELECT 'e*3', public.addEdge('tt', 'LINESTRING(1 0, 7 0)');
-- Overlapping
SELECT 'e*4', public.addEdge('tt', 'LINESTRING(1 0, 9 0)');
-- Contains with endpoint contact
SELECT 'e*5', public.addEdge('tt', 'LINESTRING(0 0, 9 0)');
-- Contains with no endpoint contact
SELECT 'e*6', public.addEdge('tt', 'LINESTRING(-1 0, 9 0)');
-- Touches middle with endpoint
SELECT 'e*7', public.addEdge('tt', 'LINESTRING(5 0, 5 10)');
-- Crosses
SELECT 'e*8', public.addEdge('tt', 'LINESTRING(5 -10, 5 10)');
-- Is touched on the middle by endpoint
SELECT 'e*9', public.addEdge('tt', 'LINESTRING(0 -10, 0 10)');
-- Touches middle with internal vertex
SELECT 'e*10', public.addEdge('tt', 'LINESTRING(0 10, 5 0, 5 10)');

-- Endpoint touching cases (should succeed) ------

SELECT 'e2',  public.addEdge('tt', 'LINESTRING(8 0, 8 10)');
SELECT 'e3',  public.addEdge('tt', 'LINESTRING(0 0, 0 10)');
-- this one connects e2-e3
SELECT 'e4',  public.addEdge('tt', 'LINESTRING(8 10, 0 10)');

-- Disjoint case (should succeed) ------
SELECT 'e5',  public.addEdge('tt', 'LINESTRING(8 -10, 0 -10)');

-- this one touches the same edge (e5) at both endpoints
SELECT 'e6',  public.addEdge('tt', 'LINESTRING(8 -10, 4 -20, 0 -10)');

--
-- See http://trac.osgeo.org/postgis/ticket/770
--
-- Closed edge shares endpoint with existing open edge
SELECT '#770-1', public.addEdge('tt', 'LINESTRING(8 10, 10 10, 10 12, 8 10)');
-- Closed edge shares endpoint with existing closed edge (and open one)
SELECT '#770-2', public.addEdge('tt', 'LINESTRING(8 10, 9 8, 10 9, 8 10)');
-- boundary has puntual intersection with "interior" of closed edge 770-1,
-- but not _only_ on endpoint !
SELECT '#770-*', public.addEdge('tt', 'LINESTRING(8 10, 8 12, 10 12)');
-- same as above, but this time the new edge is closed too
SELECT '#770-*', public.addEdge('tt', 'LINESTRING(8 10, 7 13, 10 12, 8 12, 10 12)');
-- once again, but the intersection is now at the new edge endpoint
-- (not the existing edge endpoint)
SELECT '#770-*', public.addEdge('tt', 'LINESTRING(10 12, 11 12, 10 13, 10 12)');

-- Another equals case, this time a closed edge
SELECT '#770-1*', public.addEdge('tt', 'LINESTRING(8 10, 10 10, 10 12, 8 10)');

SELECT edge_id, left_face, right_face,
	next_left_edge, next_right_edge,
	st_astext(geom) from tt.edge ORDER by edge_id;

SELECT public.DropTopology('tt');

-- Test topology with MixedCase
SELECT public.CreateTopology('Ul') > 0;
SELECT 'MiX',  public.addEdge('Ul', 'LINESTRING(0 0, 8 0)');
SELECT public.DropTopology('Ul');
