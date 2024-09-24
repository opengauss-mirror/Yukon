----create table----

CREATE TABLE geometry_point(id serial, geog geometry(geometry, 4326) );

with a as (select id, (random()*10)::float x, (random()*10)::float y from generate_series(1,10000) t(id)) insert into geometry_point select id, ST_MakePoint(165+x, 30+y) from a;

----create Pyramid-----
analyze geometry_point;
select ST_BuildPyramid('public','geometry_point',
                      'geog',
                       '[{  "level": 5,"attribute": ["id"] },{  "level": 6,"attribute": ["id"] }]');

--- default resolution for point ---
CREATE TABLE geometry_point_default(id serial, geog geometry(geometry, 4269) );

with a as (select id, (random()*10)::float x, (random()*10)::float y from generate_series(1,10000) t(id)) insert into geometry_point_default select id, ST_MakePoint(165+x, 30+y) from a;
analyze geometry_point_default;
select ST_BuildPyramid('public','geometry_point_default',
                      'geog',
                       '[{  "level": 5,"attribute": ["id"] },{  "level": 6,"attribute": ["id"] }]');
DROP TABLE geometry_point_default;

---default resolution for polygon and line ---
CREATE TABLE geometry_poly_default(id serial, geog geometry(polygon, 4269) );

with a as (select id, (random()*10)::float x, (random()*10)::float y from generate_series(1,10000) t(id)) insert into geometry_poly_default select id, st_buffer(ST_MakePoint(165+x, 30+y),1) from a;

select ST_BuildPyramid('public','geometry_poly_default',
                      'geog',
                       '[{  "level": 5,"attribute": ["id"] },{  "level": 6,"attribute": ["id"] }]');
DROP TABLE geometry_poly_default;

----create Tile----
select ST_BuildTile('public','geometry_point','geog',6);

----information----

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

CREATE TABLE geometry_line(id serial, geog geometry(geometry, 3857) );

with a as (select id, (random()*10)::float x, (random()*10)::float y from generate_series(1,10000) t(id)) insert into geometry_line select id, st_transform(st_makeenvelope(x, y, 10+x, 20+y, 4326), 3857) from a;

----create Pyramid-----
select ST_BuildPyramid('public','geometry_line',
                      'geog',
                       '[{  "level": 5,"attribute": ["id"] },{  "level": 6,"attribute": ["id"] }]');
----create Tile----
select ST_BuildTile('public','geometry_line','geog',6);

----information----

select ST_HasPyramid('public', 'geometry_line', 'geog');

select ST_ListPyramid('public', 'geometry_line', 'geog');

select count(*) from tile_geometry_line_geog;

----update----
with a as (select id, (random()*5)::float x, (random()*5)::float y from generate_series(1,70000) t(id)) insert into geometry_line select id, st_transform(st_makeenvelope(x,y,10+x, 20+y,4326),3857) from a;

select ST_UpdatePyramid('public', 'geometry_line', 'geog', 'BOX(165 30,170 35)'::box2d, 6);

select count(*) from tile_geometry_line_geog;

select ST_UpdatePyramid('public', 'geometry_line', 'geog', 'BOX(165 30,170 35)'::box2d, 7);

select count(*) from tile_geometry_line_geog;

----Tile----
select LENGTH(st_astile('public','geometry_line','geog',4,15,2));

----delete----
select ST_DeletePyramid('public', 'geometry_line', 'geog');
select ST_HasPyramid('public', 'geometry_line', 'geog');
DROP TABLE tile_geometry_line_geog;