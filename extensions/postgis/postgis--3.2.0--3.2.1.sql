\echo Use "CREATE EXTENSION postgis" to load this file. \quit

CREATE OR REPLACE FUNCTION ST_AsLatLonText(geom geometry, tmpl text DEFAULT '')
	RETURNS text
	AS '$libdir/postgis-3','LWGEOM_to_latlon'
	LANGUAGE 'c' IMMUTABLE 
	COST 1;


CREATE OR REPLACE FUNCTION DropGeometryColumn(catalog_name varchar, schema_name varchar,table_name varchar,column_name varchar)
	RETURNS text
	AS
$$
DECLARE
	myrec RECORD;
	okay boolean;
	real_schema name;

BEGIN

	-- Find, check or fix schema_name
	IF ( schema_name IS NOT NULL ) THEN
		okay = false;

		FOR myrec IN SELECT nspname FROM pg_namespace WHERE text(nspname) = schema_name LOOP
			okay := true;
		END LOOP;

		IF ( okay <>  true ) THEN
			RAISE NOTICE 'Invalid schema name - using current_schema()';
			SELECT current_schema() into real_schema;
		ELSE
			real_schema = schema_name;
		END IF;
	ELSE
		SELECT current_schema() into real_schema;
	END IF;

	-- Find out if the column is in the geometry_columns table
	okay = false;
	FOR myrec IN SELECT * from @extschema@.geometry_columns where f_table_schema = text(real_schema) and f_table_name = table_name and f_geometry_column = column_name LOOP
		okay := true;
	END LOOP;
	IF (okay <> true) THEN
		RAISE EXCEPTION 'column not found in geometry_columns table';
		RETURN false;
	END IF;

	-- Remove table column
	EXECUTE 'ALTER TABLE ' || quote_ident(real_schema) || '.' ||
		quote_ident(table_name) || ' DROP COLUMN ' ||
		quote_ident(column_name);

	RETURN real_schema || '.' || table_name || '.' || column_name ||' effectively removed.';

END;
$$
LANGUAGE 'plpgsql' VOLATILE;

CREATE OR REPLACE FUNCTION UpdateGeometrySRID(catalogn_name varchar,schema_name varchar,table_name varchar,column_name varchar,new_srid_in integer)
	RETURNS text
	AS
$$
DECLARE
	myrec RECORD;
	okay boolean;
	cname varchar;
	real_schema name;
	unknown_srid integer;
	new_srid integer := new_srid_in;
	gtype varchar;
	gcoord_dimension  integer;
	sql text;
	count integer;

BEGIN

	-- Find, check or fix schema_name
	IF ( schema_name IS NOT NULL ) THEN
		okay = false;

		FOR myrec IN SELECT nspname FROM pg_namespace WHERE text(nspname) = schema_name LOOP
			okay := true;
		END LOOP;

		IF ( okay <> true ) THEN
			RAISE EXCEPTION 'Invalid schema name';
		ELSE
			real_schema = schema_name;
		END IF;
	ELSE
		SELECT INTO real_schema current_schema()::text;
	END IF;

	-- Ensure that column_name is in geometry_columns
	okay = false;
	
	EXECUTE 'SELECT count(*) FROM geometry_columns WHERE f_table_schema ='|| quote_literal(real_schema) ||
	' and f_table_name = '|| quote_literal(table_name) ||'  and f_geometry_column = '|| quote_literal(column_name)||';'
	INTO count;
	
	if(count) then
		EXECUTE 'SELECT type FROM geometry_columns WHERE f_table_schema ='|| quote_literal(real_schema) ||
		' and f_table_name = '|| quote_literal(table_name) ||'  and f_geometry_column = '|| quote_literal(column_name)||';'
		INTO gtype;
		
		EXECUTE 'SELECT coord_dimension FROM geometry_columns WHERE f_table_schema ='|| quote_literal(real_schema) ||
		' and f_table_name = '|| quote_literal(table_name) ||'  and f_geometry_column = '|| quote_literal(column_name)||';'
		INTO gcoord_dimension;
		okay := true;
	else
		okay := false;
	END IF;
	

	IF (NOT okay) THEN
		RAISE EXCEPTION 'column not found in geometry_columns table';
		RETURN false;
	END IF;

	-- Ensure that new_srid is valid
	IF ( new_srid > 0 ) THEN
		IF ( SELECT count(*) = 0 from spatial_ref_sys where srid = new_srid ) THEN
			RAISE EXCEPTION 'invalid SRID: % not found in spatial_ref_sys', new_srid;
			RETURN false;
		END IF;
	ELSE
		unknown_srid := @extschema@.ST_SRID('POINT EMPTY'::@extschema@.geometry);
		IF ( new_srid != unknown_srid ) THEN
			new_srid := unknown_srid;
			RAISE NOTICE 'SRID value % converted to the officially unknown SRID value %', new_srid_in, new_srid;
		END IF;
	END IF;

	IF postgis_constraint_srid(real_schema, table_name, column_name) IS NOT NULL THEN
	-- srid was enforced with constraints before, keep it that way.
        -- Make up constraint name
        cname = 'enforce_srid_'  || column_name;

        -- Drop enforce_srid constraint
        EXECUTE 'ALTER TABLE ' || quote_ident(real_schema) ||
            '.' || quote_ident(table_name) ||
            ' DROP constraint ' || quote_ident(cname);

        -- Update geometries SRID
        EXECUTE 'UPDATE ' || quote_ident(real_schema) ||
            '.' || quote_ident(table_name) ||
            ' SET ' || quote_ident(column_name) ||
            ' = @extschema@.ST_SetSRID(' || quote_ident(column_name) ||
            ', ' || new_srid::text || ')';

        -- Reset enforce_srid constraint
        EXECUTE 'ALTER TABLE ' || quote_ident(real_schema) ||
            '.' || quote_ident(table_name) ||
            ' ADD constraint ' || quote_ident(cname) ||
            ' CHECK (st_srid(' || quote_ident(column_name) ||
            ') = ' || new_srid::text || ')';
    ELSE
        -- We will use typmod to enforce if no srid constraints
        -- We are using postgis_type_name to lookup the new name
        -- (in case Paul changes his mind and flips geometry_columns to return old upper case name)
        EXECUTE 'ALTER TABLE ' || quote_ident(real_schema) || '.' || quote_ident(table_name) ||
        ' ALTER COLUMN ' || quote_ident(column_name) || ' TYPE  geometry(' || @extschema@.postgis_type_name(gtype, gcoord_dimension, true) || ', ' || new_srid::text || ') USING @extschema@.ST_SetSRID(' || quote_ident(column_name) || ',' || new_srid::text || ');' ;
    END IF;

	RETURN real_schema || '.' || table_name || '.' || column_name ||' SRID changed to ' || new_srid::text;

END;
$$
LANGUAGE 'plpgsql' VOLATILE;

CREATE OR REPLACE FUNCTION yukon_version() RETURNS text
	AS '$libdir/postgis-3'
	LANGUAGE 'c' IMMUTABLE
	COST 1;

CREATE OR REPLACE FUNCTION ST_GeomFromGML(text, int4)
	RETURNS geometry
	AS '$libdir/postgis-3','geom_from_gml'
	LANGUAGE 'c' IMMUTABLE  
	COST 10;

CREATE OR REPLACE FUNCTION ST_GeomFromGML(text)
	RETURNS geometry
	AS 'SELECT @extschema@._ST_GeomFromGML($1, 0)'
	LANGUAGE 'sql' IMMUTABLE  
	COST 10;

CREATE OR REPLACE FUNCTION ST_GeomFromKML(text)
	RETURNS geometry
	AS '$libdir/postgis-3','geom_from_kml'
	LANGUAGE 'c' IMMUTABLE 
	COST 10;

CREATE OR REPLACE FUNCTION ST_AsGML(version int4, geom geometry, maxdecimaldigits int4 DEFAULT 15, options int4 DEFAULT 0, nprefix text DEFAULT 'gml', id text DEFAULT '')
	RETURNS TEXT
	AS '$libdir/postgis-3','LWGEOM_asGML'
	LANGUAGE 'c' IMMUTABLE 
	COST 10;

CREATE OR REPLACE FUNCTION ST_AsKML(geom geometry, maxdecimaldigits int4 DEFAULT 15, nprefix TEXT default '')
	RETURNS TEXT
	AS '$libdir/postgis-3','LWGEOM_asKML'
	LANGUAGE 'c' IMMUTABLE  
	COST 10;

CREATE OR REPLACE FUNCTION ST_AsGML(version int4, geog geography, maxdecimaldigits int4 DEFAULT 15, options int4 DEFAULT 0, nprefix text DEFAULT 'gml', id text DEFAULT '')
	RETURNS text
	AS '$libdir/postgis-3','geography_as_gml'
	LANGUAGE 'c' IMMUTABLE  
	COST 10;

CREATE OR REPLACE FUNCTION ST_AsGML(geog geography, maxdecimaldigits int4 DEFAULT 15, options int4 DEFAULT 0, nprefix text DEFAULT 'gml', id text DEFAULT '')
	RETURNS text
	AS '$libdir/postgis-3','geography_as_gml'
	LANGUAGE 'c' IMMUTABLE  
	COST 10;

CREATE OR REPLACE FUNCTION ST_AsKML(geog geography, maxdecimaldigits int4 DEFAULT 15, nprefix text DEFAULT '')
	RETURNS text
	AS '$libdir/postgis-3','geography_as_kml'
	LANGUAGE 'c' IMMUTABLE  
	COST 10;