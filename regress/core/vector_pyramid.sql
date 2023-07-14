----create table----

CREATE TABLE geometry_point(id serial, geog geometry(geometry, 4326) );

with a as (select id, (random()*10)::float x, (random()*10)::float y from generate_series(1,10000) t(id)) insert into geometry_point select id, ST_MakePoint(165+x, 30+y) from a;

----create Pyramid-----
analyze geometry_point;
select ST_BuildPyramid('public','geometry_point',
                      'geog',
                       '[{  "level": 5,"attribute": ["id"] },{  "level": 6,"attribute": ["id"] }]');
----create Tile----
select ST_BuildTile('public','geometry_point','geog',6);

----information----
select yukon_pyramid_version();

select ST_HasPyramid('public', 'geometry_point', 'geog');

select ST_ListPyramid('public', 'geometry_point', 'geog');

select count(*) from tile_geometry_point_geog;

----update----
with a as (select id, (random()*5)::float x, (random()*5)::float y from generate_series(1,70000) t(id)) insert into geometry_point select id, ST_MakePoint(165+x, 30+y) from a;

select ST_UpdatePyramid('public', 'geometry_point', 'geog', 'BOX(165 30,170 35)'::box2d, 6);

select count(*) from tile_geometry_point_geog;

select ST_UpdatePyramid('public', 'geometry_point', 'geog', 'BOX(165 30,170 35)'::box2d, 7);

select count(*) from tile_geometry_point_geog;

----Tile----
select LENGTH(st_astile('public','geometry_point','geog',4,15,2));

----delete----
select ST_DeletePyramid('public', 'geometry_point', 'geog');
select ST_HasPyramid('public', 'geometry_point', 'geog');