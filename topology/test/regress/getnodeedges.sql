set client_min_messages to ERROR;

\i ../load_topology.sql

SELECT 'N'||node_id, (public.GetNodeEdges('city_data', node_id)).*
	FROM city_data.node ORDER BY node_id, sequence;

SELECT public.DropTopology('city_data');
