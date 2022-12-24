--complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION yukon_geomodel" to load this file. \quit


CREATE OR REPLACE FUNCTION UpdateGeoModelSRID(catalogn_name varchar,schema_name varchar,table_name varchar,column_name varchar,new_srid_in integer)
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
	IF ( schema_name !='') THEN
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

	-- Ensure that column_name is in geomodel_columns
	okay = false;
	
	EXECUTE 'SELECT count(*) FROM geomodel_columns WHERE f_table_schema ='|| quote_literal(real_schema) ||
	' and f_table_name = '|| quote_literal(table_name) ||'  and f_geomodel_column = '|| quote_literal(column_name)||';'
	INTO count;


	
	if(count) then
		EXECUTE 'SELECT type FROM geomodel_columns WHERE f_table_schema ='|| quote_literal(real_schema) ||
		' and f_table_name = '|| quote_literal(table_name) ||'  and f_geomodel_column = '|| quote_literal(column_name)||';'
		INTO gtype;
		
		EXECUTE 'SELECT coord_dimension FROM geomodel_columns WHERE f_table_schema ='|| quote_literal(real_schema) ||
		' and f_table_name = '|| quote_literal(table_name) ||'  and f_geomodel_column = '|| quote_literal(column_name)||';'
		INTO gcoord_dimension;
		okay := true;
	else
		okay := false;
	END IF;

	IF (NOT okay) THEN
		RAISE EXCEPTION 'column not found in geomodel_columns table';
		RETURN false;
	END IF;

	-- Ensure that new_srid is valid
	IF ( new_srid > 0 ) THEN
		IF ( SELECT count(*) = 0 from spatial_ref_sys where srid = new_srid ) THEN
			RAISE EXCEPTION 'invalid SRID: % not found in spatial_ref_sys', new_srid;
			RETURN false;
		END IF;
	ELSE
		new_srid = 0;
	END IF;
	
    EXECUTE 'ALTER TABLE ' || quote_ident(real_schema) || '.' || quote_ident(table_name) ||
        ' ALTER COLUMN ' || quote_ident(column_name) || ' TYPE  geomodel(' || gtype ||',' ||new_srid::text || ') USING ST_SetSRID(' || quote_ident(column_name) || ',' || new_srid::text || ');';

	RETURN real_schema || '.' || table_name || '.' || column_name ||' SRID changed to ' || new_srid::text;

END;
$$
LANGUAGE 'plpgsql' VOLATILE;



-----------------------------------------------------------------------
-- UpdateGeomodelSRID
--   <schema>, <table>, <column>, <srid>
-----------------------------------------------------------------------
CREATE OR REPLACE FUNCTION UpdateGeoModelSRID(varchar,varchar,varchar,integer)
	RETURNS text
	AS $$
DECLARE
	ret  text;
BEGIN
	SELECT UpdateGeoModelSRID('',$1,$2,$3,$4) into ret;
	RETURN ret;
END;
$$
LANGUAGE 'plpgsql' VOLATILE STRICT;

-----------------------------------------------------------------------
-- UpdateGeoModelSRID
--   <table>, <column>, <srid>
-----------------------------------------------------------------------
CREATE OR REPLACE FUNCTION UpdateGeoModelSRID(varchar,varchar,integer)
	RETURNS text
	AS $$
DECLARE
	ret  text;
BEGIN
	SELECT UpdateGeoModelSRID('','',$1,$2,$3) into ret;
	RETURN ret;
END;
$$
LANGUAGE 'plpgsql' VOLATILE STRICT;


drop function yukon_version();