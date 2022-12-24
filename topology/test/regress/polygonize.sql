set client_min_messages to ERROR;

SELECT public.CreateTopology('tt') > 0;

SELECT 'e1',  public.addEdge('tt', 'LINESTRING(0 0, 10 0)');
SELECT 'e2',  public.addEdge('tt', 'LINESTRING(10 0, 10 10)');
SELECT 'e3',  public.addEdge('tt', 'LINESTRING(0 10, 10 10)');
SELECT 'e4',  public.addEdge('tt', 'LINESTRING(0 0, 0 10)');
SELECT 'e5',  public.addEdge('tt', 'LINESTRING(0 0, 0 -10)');
SELECT 'e6',  public.addEdge('tt', 'LINESTRING(10 10, 20 10)');
SELECT 'e7',  public.addEdge('tt', 'LINESTRING(20 10, 20 0)');
SELECT 'e8',  public.addEdge('tt', 'LINESTRING(20 0, 10 0)');
SELECT 'e9',  public.addEdge('tt', 'LINESTRING(10 0, 0 -10)');
SELECT 'e10',  public.addEdge('tt', 'LINESTRING(2 2, 5 2, 2 5)');
SELECT 'e11',  public.addEdge('tt', 'LINESTRING(2 2, 2 5)');

-- Call, check linking
SELECT public.polygonize('tt');
SELECT face_id, Box2d(mbr) from tt.face ORDER by face_id;
SELECT edge_id, left_face, right_face from tt.edge ORDER by edge_id;

-- Call again and recheck linking (shouldn't change anything)
SELECT public.polygonize('tt');
SELECT face_id, Box2d(mbr) from tt.face ORDER by face_id;
SELECT edge_id, left_face, right_face from tt.edge ORDER by edge_id;

SELECT public.DropTopology('tt');

