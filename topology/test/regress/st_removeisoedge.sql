\set VERBOSITY terse
set client_min_messages to ERROR;

-- Import city_data
\i ../load_topology.sql

CREATE TABLE city_data.f_lin(id serial primary key);
SELECT NULL FROM AddTopoGeometryColumn('city_data', 'city_data', 'f_lin', 'tg', 'LINE');
CREATE TABLE city_data.f_mix(id serial primary key);
SELECT NULL FROM AddTopoGeometryColumn('city_data', 'city_data', 'f_mix', 'tg', 'COLLECTION');

BEGIN;
INSERT INTO city_data.f_lin(tg) VALUES(
  public.CreateTopoGeom(
    'city_data', -- Topology name
    2, -- Topology geometry type (line)
    1, -- TG_LAYER_ID for this topology (from public.layer)
    '{{25,2}}'
  )
);
SELECT '#3248.line', public.ST_RemoveIsoEdge('city_data', 25);
ROLLBACK;

BEGIN;
INSERT INTO city_data.f_mix(tg) VALUES(
  public.CreateTopoGeom(
    'city_data', -- Topology name
    4, -- Topology geometry type (collection)
    2, -- TG_LAYER_ID for this topology (from public.layer)
    '{{25,2}}'
  )
);
SELECT '#3248.collection', public.ST_RemoveIsoEdge('city_data', 25);
ROLLBACK;

SELECT 'non-iso', public.ST_RemoveIsoEdge('city_data', 1);
SELECT 'iso', public.ST_RemoveIsoEdge('city_data', 25);

SELECT NULL FROM public.DropTopology('city_data');

