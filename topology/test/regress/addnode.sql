set client_min_messages to WARNING;

-- Test with zero tolerance

SELECT public.CreateTopology('nodes') > 0;

-- Check that the same point geometry return the same node id

SELECT 'p1',  public.addNode('nodes', 'POINT(0 0)');
SELECT 'p1b', public.addNode('nodes', 'POINT(0 0)');

SELECT 'p2',  public.addNode('nodes', 'POINT(1 0)');
SELECT 'p2b', public.addNode('nodes', 'POINT(1 0)');

-- Check that adding a node in the middle of an existing edge is refused
-- While adding one on the endpoint is fine
INSERT INTO nodes.edge VALUES(nextval('nodes.edge_data_edge_id_seq'),1,1,1,-1,0,0,
  'LINESTRING(0 10,10 10)');
SELECT 'p3*1',  public.addNode('nodes', 'POINT(5 10)'); -- refused
SELECT 'p3',  public.addNode('nodes', 'POINT(0 10)'); -- good
SELECT 'p4',  public.addNode('nodes', 'POINT(10 10)'); -- good

-- Now allow edge splitting:
SELECT 'p5',  public.addNode('nodes', 'POINT(5 10)', true);
-- ... and verify the edge was split
SELECT 'post-p5', edge_id, ST_AsText(geom) FROM nodes.edge ORDER BY edge_id;

-- And same against a closed edge
SELECT NULL FROM public.TopoGeo_addPolygon('nodes', 'POLYGON((0 20, 10 20, 10 30, 0 30, 0 20))');
SELECT 'p6',  public.addNode('nodes', 'POINT(0 20)'); -- good

-- Now allow computing containing face:
SELECT 'p7',  public.addNode('nodes', 'POINT(5 25)', false, true);

-- Check all nodes
SELECT node_id, containing_face, st_astext(geom) from nodes.node
ORDER by node_id;

SELECT public.DropTopology('nodes');

-- Test topology with MixedCase
SELECT public.CreateTopology('Ul') > 0;
SELECT 'MiX',  public.addNode('Ul', 'POINT(0 0)');
SELECT public.DropTopology('Ul');
