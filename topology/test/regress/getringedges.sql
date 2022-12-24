set client_min_messages to ERROR;

\i ../load_topology.sql

-- Test bogus calls
SELECT 'non-existent-topo', public.GetRingEdges('non-existent', 1);
SELECT 'non-existent-edge', public.GetRingEdges('city_data', 10000000);
SELECT 'non-existent-edge-negative', public.GetRingEdges('city_data', -10000000);

SELECT 'R'||edge_id, (public.GetRingEdges('city_data', edge_id)).*
	FROM city_data.edge;

SELECT 'R-'||edge_id, (public.GetRingEdges('city_data', -edge_id)).*
	FROM city_data.edge;

SELECT public.DropTopology('city_data');
