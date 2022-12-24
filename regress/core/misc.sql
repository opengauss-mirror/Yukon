-- 这个文件用来测试原来在 PGIS 中暂未测试到的函数

-- ST_AsLatLonText
SELECT 'ST_AsLatLonText', (ST_AsLatLonText('POINT (-3.2342342 -2.32498)'));
SELECT 'ST_AsLatLonText', (ST_AsLatLonText('POINT (-3.2342342 -2.32498)', 'D	extdegree{}M''S.SSSC'));
SELECT 'ST_AsLatLonText', (ST_AsLatLonText('POINT (-3.2342342 -2.32498)', 'D degrees, M minutes, S seconds to the C'));
SELECT 'ST_AsLatLonText', (ST_AsLatLonText('POINT (-3.2342342 -2.32498)', 'D	extdegree{}M''S.SSS'));
SELECT 'ST_AsLatLonText', (ST_AsLatLonText('POINT (-3.2342342 -2.32498)', 'D.DDDD degrees C'));
SELECT 'ST_AsLatLonText', (ST_AsLatLonText('POINT (-302.2342342 -792.32498)'));