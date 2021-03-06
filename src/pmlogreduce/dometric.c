#include "pmlogreduce.h"

void
dometric(const char *name)
{
    int			sts;
    metric_t		*mp;

    if ((namelist = (char **)realloc(namelist, (numpmid+1)*sizeof(namelist[0]))) == NULL) {
	fprintf(stderr,
	    "%s: dometric: Error: cannot realloc space for %d names\n",
		pmProgname, numpmid+1);
	exit(1);
    }
    namelist[numpmid] = strdup(name);
    if ((pmidlist = (pmID *)realloc(pmidlist, (numpmid+1)*sizeof(pmidlist[0]))) == NULL) {
	fprintf(stderr,
	    "%s: dometric: Error: cannot realloc space for %d pmIDs\n",
		pmProgname, numpmid+1);
	exit(1);
    }
    if ((sts = pmLookupName(1, (char **)&name, &pmidlist[numpmid])) < 0) {
	fprintf(stderr,
	    "%s: dometric: Error: cannot lookup pmID for metric \"%s\": %s\n",
		pmProgname, name, pmErrStr(sts));
	exit(1);
    }
    if ((metriclist = (metric_t *)realloc(metriclist, (numpmid+1)*sizeof(metriclist[0]))) == NULL) {
	fprintf(stderr,
	    "%s: dometric: Error: cannot realloc space for %d metric_t's\n",
		pmProgname, numpmid+1);
	exit(1);
    }
    mp = &metriclist[numpmid];
    mp->first = NULL;
    if ((sts = pmLookupDesc(pmidlist[numpmid], &mp->idesc)) < 0) {
	fprintf(stderr,
	    "%s: dometric: Error: cannot lookup pmDesc for metric \"%s\": %s\n",
		pmProgname, name, pmErrStr(sts));
	exit(1);
    }
    mp->odesc = mp->idesc;	/* struct assignment */
    mp->mode = MODE_NORMAL;
    mp->idp = NULL;

    /*
     * some metrics cannot sensibly be processed ... skip these ones
     */
    if (mp->idesc.type == PM_TYPE_AGGREGATE ||
        mp->idesc.type == PM_TYPE_AGGREGATE_STATIC ||
        mp->idesc.type == PM_TYPE_EVENT) {
	fprintf(stderr,
	    "%s: %s: Warning: skipping %s metric\n",
		pmProgname, name, pmTypeStr(mp->idesc.type));
	mp->mode = MODE_SKIP;
	goto done;
    }

    /*
     * input -> output descriptor mapping ... has to be the same
     * logic as we apply to the pmResults later on.
     */
    switch (mp->idesc.sem) {
	case PM_SEM_COUNTER:
	    switch (mp->idesc.type) {
		case PM_TYPE_32:
		    mp->odesc.type = PM_TYPE_64;
		    mp->mode = MODE_REWRITE;
		    break;
		case PM_TYPE_U32:
		    mp->odesc.type = PM_TYPE_U64;
		    mp->mode = MODE_REWRITE;
		    break;
	    }
#if 0
	    mp->odesc.sem = PM_SEM_INSTANT;
	    if (mp->idesc.units.dimTime == 0) {
		/* rate convert */
		mp->odesc.units.dimTime = -1;
		mp->odesc.units.scaleTime = PM_TIME_SEC;
	    }
	    else if (mp->idesc.units.dimTime == 1) {
		/* becomes (time) utilization */
		mp->odesc.units.dimTime = 0;
		mp->odesc.units.scaleTime = 0;
	    }
	    else {
		fprintf(stderr, "Cannot rate convert \"%s\" yet,", namelist[numpmid]);
		__pmPrintDesc(stderr, &mp->idesc);
		exit(1);
	    }
	    break;
#endif
    }
#if PCP_DEBUG
    if (pmDebug & DBG_TRACE_APPL0) {
	fprintf(stderr, "metric: \"%s\" (%s)\n",
	    namelist[numpmid], pmIDStr(pmidlist[numpmid]));
	fprintf(stderr, "input descriptor:\n");
	__pmPrintDesc(stderr, &mp->idesc);
	fprintf(stderr, "output descriptor (added to archive):\n");
	__pmPrintDesc(stderr, &mp->odesc);
    }
#endif

    if ((sts = __pmLogPutDesc(&logctl, &mp->odesc, 1, &namelist[numpmid])) < 0) {
	fprintf(stderr,
	    "%s: Error: failed to add pmDesc for %s (%s): %s\n",
		pmProgname, namelist[numpmid], pmIDStr(pmidlist[numpmid]), pmErrStr(sts));
	exit(1);
    }

    /*
     * instance domain initialization
     */
    if (mp->idesc.indom != PM_INDOM_NULL) {
	/*
	 * has an instance domain, check to see if it has already been seen
	 */
	int		j;

	for (j = 0; j <= numpmid; j++) {
	    if (metriclist[j].idp != NULL && 
		metriclist[j].idp->indom == mp->idesc.indom) {
		mp->idp = metriclist[j].idp;
		break;
	    }
	}
	if (j > numpmid) {
	    /* first sighting, allocate a new one */
	    if ((mp->idp = (indom_t *)malloc(sizeof(indom_t))) == NULL) {
		fprintf(stderr,
		    "%s: dometric: Error: cannot malloc indom_t for %s\n",
		    pmProgname, pmInDomStr(mp->idesc.indom));
		exit(1);
	    }
	    mp->idp->indom = mp->idesc.indom;
	    mp->idp->numinst = 0;
	    mp->idp->inst = NULL;
	    mp->idp->name = NULL;
	}
    }

#if PCP_DEBUG
    if (pmDebug & DBG_TRACE_APPL0) {
	if (mp->idp != NULL)
	    fprintf(stderr, "    indom %s -> (" PRINTF_P_PFX "%p)\n", pmInDomStr(mp->idp->indom), mp->idp);
    }
#endif

done:

    numpmid++;
}

