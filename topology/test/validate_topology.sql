-- Validate topology
SELECT 'Topology validation errors follow:';
SELECT * from public.validatetopology('city_data');
SELECT 'End of topology validation errors';

