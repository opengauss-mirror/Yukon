set client_min_messages to WARNING;

\i ../invalid_topology.sql

-- Validate full topology, store invalidities in a table
CREATE TABLE invalid_public.invalidities AS
SELECT * FROM public.validatetopology('invalid_topology');
SELECT * FROM invalid_public.invalidities
ORDER BY 1,2,3;

-- Test bbox-limited checking
-- See https://trac.osgeo.org/postgis/ticket/4936
CREATE TABLE invalid_public.grid_invalidities AS
WITH
extents AS (
  SELECT ST_Envelope(ST_Extent(geom)) env
  FROM invalid_public.edge
    UNION
  SELECT ST_Envelope(ST_Extent(geom))
  FROM invalid_public.node
    UNION
  SELECT ST_Envelope(ST_Extent(mbr))
  FROM invalid_public.face
),
topo_envelope AS (
  SELECT ST_Envelope(ST_Union(env)) env
  FROM extents
),
-- roughly a 8x5 grid on whole topology extent
grid AS (
  SELECT *
  FROM ST_SquareGrid(
      ST_Length(
        ST_BoundingDiagonal(
          ( SELECT env FROM topo_envelope )
        )
      )/8.0,
      ( SELECT env FROM topo_envelope )
    )
)
SELECT g.i, g.j, vt.*
FROM grid g,
LATERAL public.validatetopology('invalid_topology', g.geom) vt ;

-- Check that all errors found by the catch-all validator
-- are also cought by the per-cell validator
CREATE TABLE invalid_public.missing_invalidities AS
  SELECT error, id1, id2 FROM invalid_public.invalidities
    EXCEPT
  SELECT error, id1, id2 FROM invalid_public.grid_invalidities
;

SELECT '#4936', 'missing_count', count(*)
FROM invalid_public.missing_invalidities ;

SELECT '#4936', 'missing', *
FROM invalid_public.missing_invalidities
ORDER BY 1, 2, 3 ASC;

-- clean up
SELECT public.DropTopology('invalid_topology');
