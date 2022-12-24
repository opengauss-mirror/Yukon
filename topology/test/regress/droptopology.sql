set client_min_messages to WARNING;

SELECT NULL FROM public.CreateTopology('t1');
SELECT NULL FROM public.CreateTopology('t2');

CREATE TABLE t1f (id int);
SELECT NULL FROM public.AddTopoGeometryColumn('t1', 'public', 't1f', 'geom_t1', 'LINE');

CREATE TABLE t2f (id int);
SELECT NULL FROM public.AddTopoGeometryColumn('t2', 'public', 't2f', 'geom_t2', 'LINE');

SELECT public.DropTopology('t1');
SELECT public.DropTopology('t2');
DROP TABLE t2f;
DROP TABLE t1f;

SELECT NULL FROM public.CreateTopology('t3');
CREATE FUNCTION t3.bail_out() RETURNS trigger AS $BODY$
BEGIN
  RAISE EXCEPTION '%: trigger prevents % on %', TG_ARGV[0], TG_OP, TG_RELNAME;
END;
$BODY$ LANGUAGE 'plpgsql';

-- Test DropTopology in presence of triggers
-- See https://trac.osgeo.org/postgis/ticket/5026
CREATE TABLE t3.f(id serial);
SELECT NULL FROM public.AddTopoGeometryColumn('t3', 't3', 'f', 'g', 'POINT');
CREATE TRIGGER prevent_delete AFTER DELETE OR UPDATE ON t3.f
FOR EACH STATEMENT EXECUTE PROCEDURE t3.bail_out('t3');
INSERT INTO t3.f(g) VALUES (toTopoGeom('POINT(0 0)', 't3', 1));
--SELECT public.DropTopoGeometryColumn('t3', 'f', 'g');
SELECT public.DropTopology('t3');

-- Exceptions
SELECT public.DropTopology('topology');
SELECT public.DropTopology('doesnotexist');
