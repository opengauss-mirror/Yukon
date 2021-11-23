
TRUNCATE spatial_ref_sys;
INSERT INTO "spatial_ref_sys" ("srid","auth_name","auth_srid","srtext","proj4text") VALUES (4326,'EPSG',4326,'GEOGCS["WGS 84",DATUM["WGS_1984",SPHEROID["WGS 84",6378137,298.257223563,AUTHORITY["EPSG","7030"]],TOWGS84[0,0,0,0,0,0,0],AUTHORITY["EPSG","6326"]],PRIMEM["Greenwich",0,AUTHORITY["EPSG","8901"]],UNIT["degree",0.01745329251994328,AUTHORITY["EPSG","9122"]],AUTHORITY["EPSG","4326"]]','+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs ');


-- test geomodel
CREATE TABLE GEOMODEL_TEST(ID BIGINT,GEOCOL GEOMODEL);
INSERT INTO GEOMODEL_TEST VALUES(1,'0000000D63BCE84209BDE842D5FC1F4220FD1F4246A5C24ADCA5C24A20030000016831E4B496175D40E0C0B5519FFF43400000000000000000000000000000F03F000000000000F03F000000000000F03F0000000000000000000000000000000000000000000000002A1C01F025024B40243D8AD78AFB2F4012640220004055402B1802F025024BC07D78958858FB2FC068DFFCFFC2FE234000000000010000000000000000000000FFFFFFFF0000000001000000FEFFFFFFFFFFEF3F01000000C008ABBDFFFFFFFF531CB03D0000000000000000EFFFFFFF9F08AB3D010000000000F03F010000809452C1BD000000000000000004000000401CB0BDFFFFFFFF9552C13DFEFFFFFFFFFFEF3F000000000000000018278764B47F18BFFE75AEF175572A3F400103000000103E000000000000F03F040000001700000037365F303030303030303038343834313733305F47656F1700000037365F303030303030303038343834313834305F47656F1700000037365F303030303030303038343834313935305F47656F1700000037365F303030303030303038343834314136305F47656F040000003B0000005B505F5A303130303930322E4A50477C716B32326A7A30352E6A70675F4166666666666666665F4466666666666666665F53305F504631432D315D3C0000005B505F5A30313031303234332E6A70677C716B32326A7A30352E6A70675F4166666666666666665F4466666666666666665F53305F504633432D315D3C0000005B505F5A30313031303234372E6A70677C716B32326A7A30352E6A70675F4166666666666666665F4466666666666666665F53305F504633432D315D3C0000005B505F5A30313031303234382E6A70677C716B32326A7A30352E6A70675F4166666666666666665F4466666666666666665F53305F504633432D315D050000000C0000005A303130303930322E4A50470D0000005A30313031303234332E6A70670D0000005A30313031303234372E6A70670D0000005A30313031303234382E6A70670C000000716B32326A7A30352E6A7067');
SELECT * FROM GEOMODEL_TEST;
SELECT * FROM GEOMODEL_COLUMNS ;
SELECT ST_SETSRID('0000000DC0E3E242CB'::geomodel, 4326); 
SELECT ST_SRID('0010E60DC0E3E242CB'::geomodel);
DROP TABLE GEOMODEL_TEST;

-- test MODEL_ELEM
CREATE TABLE MODEL_ELEM_TEST(ID BIGINT,MODEL_ELEMCOL MODEL_ELEM);
INSERT INTO MODEL_ELEM_TEST VALUES(1,'03000000333333333333F33F77000000433A5C50726F6772616D2046696C65732028783836295C436F6D6D6F6E2046696C65735C4175746F6465736B205368617265645C4D6174657269616C735C54657874757265735C325C4D6174735C3130322E706E67436F6C6F72205B413D3235352C20523D3139322C20473D3139322C20423D3139325D000000006900450001000000000000000000000000000000000000000000010000000000000003000000010000000108000000010000803F00000000000000000000000000000000F03F000000000000000000000000000000000000000000000000FFFFFFFFFFFFFFFF0000000000000000C0C0C0FF000000000000000000010000000001000000000000010103000000000000000000000000000000010000000000000000000000000000000000000000000000000100000001000000300000000055000000433A5C50726F6772616D2046696C65732028783836295C436F6D6D6F6E2046696C65735C4175746F6465736B205368617265645C4D6174657269616C735C54657874757265735C325C4D6174735C3130322E706E670000000000000000000000000000000000000000020000000200000001000000000000000000F03F000000000000F03F00CDCDCDCD000000000000F03F0000000000000000000000000000000000000000000000000000000000000000000000000000F03F0000000000000000000000000000000000000000000000000000000000000000000000000000F03F0000000000000000000000000000000000000000000000000000000000000000000000000000F03F00000000');
SELECT * FROM MODEL_ELEM_TEST ;
DROP TABLE MODEL_ELEM_TEST ;

CREATE TABLE test_mesh (id serial, geog model_elem(mesh));
CREATE TABLE test_surface (id serial, geog model_elem(surface));
INSERT INTO test_mesh VALUEs (1,'0100000009000000736B656C65746F6E3077000000433A5C50726F6772616D2046696C65732028783836295C436F6D6D6F6E2046696C65735C4175746F6465736B205368617265645C4D6174657269616C735C54657874757265735C325C4D6174735C3130322E706E67436F6C6F72205B413D3235352C20523D3139322C20473D3139322C20423D3139325DBB490C020000D03F386744696666D63F38674469666616C0BB490C020000D0BF386744696666D6BF38674469666616C0000000000000F03F0000000000000000000000000000000000000000000000000000000000000000000000000000F03F0000000000000000000000000000000000000000000000000000000000000000000000000000F03F0000000000000000000000000000000000000000000000000000000000000000000000000000F03F070200000300040000000000BB490C020000D03F386744696666D63F38674469666616C0BB490C020000D03F386744696666D6BF38674469666616C0BB490C020000D0BF386744696666D6BF38674469666616C0BB490C020000D0BF386744696666D63F38674469666616C00400000000000000008000000080000080BF0000008000000080000080BF0000008000000080000080BF0000008000000080000080BF0000000001000000020004000000000048F9513F4CFB92BF48F9513F4CFB923F48F951BF4CFB923F48F951BF4CFB92BF01000000060000000000000001040000000200030000000000010002000100000077000000433A5C50726F6772616D2046696C65732028783836295C436F6D6D6F6E2046696C65735C4175746F6465736B205368617265645C4D6174657269616C735C54657874757265735C325C4D6174735C3130322E706E67436F6C6F72205B413D3235352C20523D3139322C20473D3139322C20423D3139325D');
INSERT INTO test_mesh VALUEs (1,'01000000180000003131395F303030303030303038373244364531305F47656F3B0000005B505F5A303130303233372E6A70677C716B31366A7A31312E6A70675F4166666666666666665F4466666666666666665F53305F504633432D315D00000040BF1357C00000008028094F40000000E0BE14294000000080EF385AC000000080DCEF4E400000000000002440000000000000F03F0000000000000000000000000000000000000000000000000000000000000000000000000000F03F0000000000000000000000000000000000000000000000000000000000000000000000000000F03F0000000000000000000000000000000000000000000000000000000000000000000000000000F03F0700000003000500000000007CC7D1C2E47E7742F7A54841FA9DB8C244497842F7A54841FA9DB8C24449784211113141FA9DB8C244497842000020417CC7D1C2E47E77420000204105000000000066AB80BCEAF77F3F0000008066AB80BCEAF77F3F0000008066AB80BCEAF77F3F0000008066AB80BCEAF77F3F0000008066AB80BCEAF77F3F0000008000000000020000000200050000000000324E1340B5D46A3F40208ABEB5D46A3F40208ABE6357C53E40208ABE9AF4023A324E13409AF4023A0200050000000000EC73043D8F9B463F71F5B73D8F9B463F71F5B73DDC54483F71F5B73D3C94493FEC73043D3C94493F0100000009000000000000000104000000000001000200000002000300000003000400010000003B0000005B505F5A303130303233372E6A70677C716B31366A7A31312E6A70675F4166666666666666665F4466666666666666665F53305F504633432D315D');
INSERT INTO test_surface VALUEs (1,'01000000180000003131395F303030303030303038373244364531305F47656F3B0000005B505F5A303130303233372E6A70677C716B31366A7A31312E6A70675F4166666666666666665F4466666666666666665F53305F504633432D315D00000040BF1357C00000008028094F40000000E0BE14294000000080EF385AC000000080DCEF4E400000000000002440000000000000F03F0000000000000000000000000000000000000000000000000000000000000000000000000000F03F0000000000000000000000000000000000000000000000000000000000000000000000000000F03F0000000000000000000000000000000000000000000000000000000000000000000000000000F03F0700000003000500000000007CC7D1C2E47E7742F7A54841FA9DB8C244497842F7A54841FA9DB8C24449784211113141FA9DB8C244497842000020417CC7D1C2E47E77420000204105000000000066AB80BCEAF77F3F0000008066AB80BCEAF77F3F0000008066AB80BCEAF77F3F0000008066AB80BCEAF77F3F0000008066AB80BCEAF77F3F0000008000000000020000000200050000000000324E1340B5D46A3F40208ABEB5D46A3F40208ABE6357C53E40208ABE9AF4023A324E13409AF4023A0200050000000000EC73043D8F9B463F71F5B73D8F9B463F71F5B73DDC54483F71F5B73D3C94493FEC73043D3C94493F0100000009000000000000000104000000000001000200000002000300000003000400010000003B0000005B505F5A303130303233372E6A70677C716B31366A7A31312E6A70675F4166666666666666665F4466666666666666665F53305F504633432D315D');
INSERT INTO test_surface VALUEs (1,'0100000009000000736B656C65746F6E3077000000433A5C50726F6772616D2046696C65732028783836295C436F6D6D6F6E2046696C65735C4175746F6465736B205368617265645C4D6174657269616C735C54657874757265735C325C4D6174735C3130322E706E67436F6C6F72205B413D3235352C20523D3139322C20473D3139322C20423D3139325DBB490C020000D03F386744696666D63F38674469666616C0BB490C020000D0BF386744696666D6BF38674469666616C0000000000000F03F0000000000000000000000000000000000000000000000000000000000000000000000000000F03F0000000000000000000000000000000000000000000000000000000000000000000000000000F03F0000000000000000000000000000000000000000000000000000000000000000000000000000F03F070200000300040000000000BB490C020000D03F386744696666D63F38674469666616C0BB490C020000D03F386744696666D6BF38674469666616C0BB490C020000D0BF386744696666D6BF38674469666616C0BB490C020000D0BF386744696666D63F38674469666616C00400000000000000008000000080000080BF0000008000000080000080BF0000008000000080000080BF0000008000000080000080BF0000000001000000020004000000000048F9513F4CFB92BF48F9513F4CFB923F48F951BF4CFB923F48F951BF4CFB92BF01000000060000000000000001040000000200030000000000010002000100000077000000433A5C50726F6772616D2046696C65732028783836295C436F6D6D6F6E2046696C65735C4175746F6465736B205368617265645C4D6174657269616C735C54657874757265735C325C4D6174735C3130322E706E67436F6C6F72205B413D3235352C20523D3139322C20473D3139322C20423D3139325D');
DROP TABLE test_mesh;
DROP TABLE test_surface;

--test geomoel_typmod_in
CREATE TABLE GEOMODEL_TEST(ID BIGINT,GEOCOL GEOMODEL('MESH'));
SELECT * FROM GEOMODEL_COLUMNS ;
DROP TABLE GEOMODEL_TEST ;

---- st_extent
CREATE TABLE geomodel_test(id serial);
SELECT AddGeoModelColumn('public','geomodel_test','geog',4326);
INSERT INTO geomodel_test values(1,'0000000DECE3E242ECE3E2425299B1415299B141ACA5C24AC0A5C24A20030000019E359B197B5C5C40F43AB644F83236400000000000001440000000000000F03F000000000000F03F000000000000F03F0000000000000000000000000000000000000000000000000000000000002E40000000000040554000000000008050400000000000002E4000000000004055400000000000004C4000000000010000000000000000000000FFFFFFFF00000000010000005D4DEAF7FFFFEF3FDED00754553011BF3A6C21F0950F253F0000000000000000292689A4C22D113FFE8490EFFFFFEF3F45935EEF43462F3F00000000000000009F75D7491C1025BF7B4B4A74E9452FBFD262C9E9FFFFEF3F0000000000000000153DD156A8138B40735905F367BC92C05439F3226B934540000000000000F03F01000000200000004275696C64696E675F484B35315F42555F42425F3037345F4C4F44322D315F30010000005B0000004D544C5F443A5CCAFDBEDDCFEE5C4D6178D4B4CAFDBEDD5CB2A9B8E75C484B35315C4C4F44325C6D6170735C484B35315F42555F43435F3032302E6A706766666666666666665F66666666666666665F305F302E3030303030305F010000003A000000443A5CCAFDBEDDCFEE5C4D6178D4B4CAFDBEDD5CB2A9B8E75C484B35315C4C4F44325C6D6170735C484B35315F42555F43435F3032302E6A7067'::geomodel);
SELECT ST_BOUNDARY((SELECT geog FROM geomodel_test WHERE id =1));
SELECT ST_CombineBBox(''::box3d,''::geomodel);
SELECT ST_CombineBBox('BOX3D(1 2 3,4 5 6)'::box3d,(SELECT geog FROM geomodel_test where id =1)) ;
SELECT ST_Extent(geomodel_test.GEOG) FROM geomodel_test;
SELECT ST_3DExtent(geomodel_test.GEOG) FROM geomodel_test;

SELECT Yukon_Version();
SELECT Yukon_Full_Version();
SELECT UpdateGeomodelSRID('public', 'geomodel_test','geog',4326);
SELECT UpdateGeomodelSRID('geomodel_test','geog',4326); 
SELECT count(*) FROM geomodel_test WHERE geog && ST_makeenvelope(113, 22, 113.5, 22.5);
CREATE INDEX test_gist ON geomodel_test USING GIST (geog);
DROP INDEX test_gist;
SELECT DropGeomodelColumn('public', 'geomodel_test');
SELECT AddGeoModelColumn('public','geomodel_test','geog',4326);
SELECT DropGeomodelTable('public','geomodel_test');
select ST_GeoModelType('01000000180000003131395F303030303030303038373244364531305F47656F3B0000005B505F5A303130303233372E6A70677C716B31366A7A31312E6A70675F4166666666666666665F4466666666666666665F53305F504633432D315D00000040BF1357C00000008028094F40000000E0BE14294000000080EF385AC000000080DCEF4E400000000000002440000000000000F03F0000000000000000000000000000000000000000000000000000000000000000000000000000F03F0000000000000000000000000000000000000000000000000000000000000000000000000000F03F0000000000000000000000000000000000000000000000000000000000000000000000000000F03F0700000003000500000000007CC7D1C2E47E7742F7A54841FA9DB8C244497842F7A54841FA9DB8C24449784211113141FA9DB8C244497842000020417CC7D1C2E47E77420000204105000000000066AB80BCEAF77F3F0000008066AB80BCEAF77F3F0000008066AB80BCEAF77F3F0000008066AB80BCEAF77F3F0000008066AB80BCEAF77F3F0000008000000000020000000200050000000000324E1340B5D46A3F40208ABEB5D46A3F40208ABE6357C53E40208ABE9AF4023A324E13409AF4023A0200050000000000EC73043D8F9B463F71F5B73D8F9B463F71F5B73DDC54483F71F5B73D3C94493FEC73043D3C94493F0100000009000000000000000104000000000001000200000002000300000003000400010000003B0000005B505F5A303130303233372E6A70677C716B31366A7A31312E6A70675F4166666666666666665F4466666666666666665F53305F504633432D315D'::geomodel);
SELECT ST_GeoModelType('0100000009000000736B656C65746F6E3077000000433A5C50726F6772616D2046696C65732028783836295C436F6D6D6F6E2046696C65735C4175746F6465736B205368617265645C4D6174657269616C735C54657874757265735C325C4D6174735C3130322E706E67436F6C6F72205B413D3235352C20523D3139322C20473D3139322C20423D3139325DBB490C020000D03F386744696666D63F38674469666616C0BB490C020000D0BF386744696666D6BF38674469666616C0000000000000F03F0000000000000000000000000000000000000000000000000000000000000000000000000000F03F0000000000000000000000000000000000000000000000000000000000000000000000000000F03F0000000000000000000000000000000000000000000000000000000000000000000000000000F03F070200000300040000000000BB490C020000D03F386744696666D63F38674469666616C0BB490C020000D03F386744696666D6BF38674469666616C0BB490C020000D0BF386744696666D6BF38674469666616C0BB490C020000D0BF386744696666D63F38674469666616C00400000000000000008000000080000080BF0000008000000080000080BF0000008000000080000080BF0000008000000080000080BF0000000001000000020004000000000048F9513F4CFB92BF48F9513F4CFB923F48F951BF4CFB923F48F951BF4CFB92BF01000000060000000000000001040000000200030000000000010002000100000077000000433A5C50726F6772616D2046696C65732028783836295C436F6D6D6F6E2046696C65735C4175746F6465736B205368617265645C4D6174657269616C735C54657874757265735C325C4D6174735C3130322E706E67436F6C6F72205B413D3235352C20523D3139322C20473D3139322C20423D3139325D'::geomodel);
SELECT ST_GeoModelType('03000000333333333333F33F77000000433A5C50726F6772616D2046696C65732028783836295C436F6D6D6F6E2046696C65735C4175746F6465736B205368617265645C4D6174657269616C735C54657874757265735C325C4D6174735C3130322E706E67436F6C6F72205B413D3235352C20523D3139322C20473D3139322C20423D3139325D000000006900450001000000000000000000000000000000000000000000010000000000000003000000010000000108000000010000803F00000000000000000000000000000000F03F000000000000000000000000000000000000000000000000FFFFFFFFFFFFFFFF0000000000000000C0C0C0FF000000000000000000010000000001000000000000010103000000000000000000000000000000010000000000000000000000000000000000000000000000000100000001000000300000000055000000433A5C50726F6772616D2046696C65732028783836295C436F6D6D6F6E2046696C65735C4175746F6465736B205368617265645C4D6174657269616C735C54657874757265735C325C4D6174735C3130322E706E670000000000000000000000000000000000000000020000000200000001000000000000000000F03F000000000000F03F00CDCDCDCD000000000000F03F0000000000000000000000000000000000000000000000000000000000000000000000000000F03F0000000000000000000000000000000000000000000000000000000000000000000000000000F03F0000000000000000000000000000000000000000000000000000000000000000000000000000F03F00000000'::geomodel);
