vector avg_smooth(int ptnum)
{
	int npts[];
	npts = neighbours(0, ptnum);

	vector avg = 0;
	foreach (int npt; npts)
	{
	        vector npos = point(0, 'P', npt);
	        avg += npos;
	}
	avg /= len(npts);
	return avg;
}