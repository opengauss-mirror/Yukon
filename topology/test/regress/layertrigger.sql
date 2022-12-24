\set VERBOSITY terse
set client_min_messages to ERROR;

select 'seq_reset', setval('public.topology_id_seq', 1, false);
select 't1', 'topology_id:' || public.CreateTopology('t1');

--insert into t1.node(geom, containing_face) values ('POINT( 0 0)', 0);
--create table t1.l1 (id serial);
--select 't1.l1',public.AddTopoGeometryColumn('t1', 't1', 'l1', 'g', 'POINT');
--insert INTO t1.l1(g) VALUES (public.CreateTopoGeom('t1', 1, 1, '{{1,1}}'));
--select 't1.rel',* from t1.relation;

select 't2', 'topology_id:' || public.CreateTopology('t2');

insert into t2.node(geom, containing_face) values ('POINT( 0 0)', 0);
insert into t2.node(geom, containing_face) values ('POINT( 1 1)', 0);

create table t2.l1 (id serial);

select 't2.l1', public.AddTopoGeometryColumn('t2', 't2', 'l1', 'g', 'POINT');

insert into t2.l1(g) VALUES (public.CreateTopoGeom('t2', 1, 1, '{{1,1}}'));
--insert into t2.l1(g) VALUES (public.CreateTopoGeom('t2', 1, 1, '{{2,1}}'));

select 't2.rel',* from t2.relation;

--always the trigger must reject an update
update public.layer set topology_id = 123;
select 'after update: public.layer ',* from public.layer;

-- the trigger must reject the delete because some referenced features still exists
BEGIN;
  delete from t1.relation;
  delete from t2.relation;
  select 't2.rel',* from t2.relation;
  delete from public.layer;
  select 'after delete n.1: public.layer count', count(*) from public.layer;
ROLLBACK;

-- the trigger must reject because the t2.relation is not empty.
-- This test failed due to #950
-- (there's no row in t2.relation where topogeo_id matches the topology id)
BEGIN;
  delete from t2.l1;
  delete from public.layer where topology_id=2;
  select 'after delete n.2: public.layer count', count(*) from public.layer;
ROLLBACK;

-- the trigger must accept the delete
delete from t2.l1;
delete from t2.relation;
delete from public.layer where topology_id=2;
select 'after delete n.3: public.layer count', count(*) from public.layer;

select public.DropTopology('t2');
select public.DropTopology('t1');
