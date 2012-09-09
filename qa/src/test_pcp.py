#!/usr/bin/python
##############################################################################
#
# test_pcp.py
#
# Copyright (C) 2012 Red Hat Inc.
#
# This file is part of pcp, the python extensions for SGI's Performance
# Co-Pilot.
#
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the
# Free Software Foundation; either version 2 of the License, or (at your
# option) any later version.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
# for more details.

"""Test for libpcp Wrapper module

Additional Information:

Performance Co-Pilot Web Site
http://oss.sgi.com/projects/pcp

Performance Co-Pilot Programmer's Guide
SGI Document 007-3434-005
http://techpubs.sgi.com
cf. Chapter 3. PMAPI - The Performance Metrics API
"""


##############################################################################
#
# imports
#

import unittest
import pmapi
import time
import sys
import argparse
from pcp import *
from ctypes import *

# Utilities

def dump_seq (name_p, seq_p):
    print (name_p)
    for t in seq_p:
        if type(t) == type(int()) or type(t) == type(long()):
            print (hex(t))
        else:
            print (t)
    print ()

def dump_array_ptrs (name_p, arr_p):
    print (name_p)
    for i in xrange(len(arr_p)):
        print (" ") if (i > 0) else "", arr_p[i].contents

def dump_array (name_p, arr_p):
    print (name_p)
    for i in xrange(len(arr_p)):
        print (" ") if (i > 0) else "", hex(arr_p[i])

archive = ""                    # For testing either localhost or archive

traverse_callback_count = 0     # callback for pmTraversePMNS

def traverse_callback (arg):
    global traverse_callback_count
    traverse_callback_count += 1

def test_pcp(self, context = 'local', path = ''):

    if (archive == ""):
        print 'Running as localhost'
        pm = pmContext(pmapi.PM_CONTEXT_HOST,"localhost")
        self.local_type = True
    else:
        print 'Running as archive', archive
        pm = pmContext(pmapi.PM_CONTEXT_ARCHIVE, archive)
        self.archive_type = True

    # pmGetContextHostName
    hostname = pm.pmGetContextHostName()
    print "pmGetContextHostName:", hostname
    self.assertTrue(len(hostname) >= 0)

    # pmParseMetricSpec
    (code, rsltp, errmsg) = pm.pmParseMetricSpec("kernel.all.load", 0, "localhost")
    print "pmParseMetricSpec:", rsltp.contents.source
    self.assertTrue(rsltp.contents.source == "localhost")

    # Get number of cpus
    # pmLookupName
    (code, self.ncpu_id) = pm.pmLookupName(("hinv.ncpu","kernel.all.load"))
    print "pmLookupName:", self.ncpu_id
    self.assertTrue(code >= 0)

    # pmIDStr
    print "pmIDStr:", pm.pmIDStr(self.ncpu_id[0])
    self.assertTrue(pm.pmIDStr(self.ncpu_id[0]).count(".") > 1)

    # pmLookupDesc
    (code, descs) = pm.pmLookupDesc(self.ncpu_id)
    dump_array_ptrs("pmLookupDesc", descs)
    self.assertTrue(code >= 0)

    # pmFetch
    (code, results) = pm.pmFetch(self.ncpu_id)
    print "pmFetch:", results
    self.assertTrue(code >= 0)

    # pmExtractValue
    (code, atom) = pm.pmExtractValue(results.contents.get_valfmt(0),
                                     results.contents.get_vlist(0, 0),
                                     descs[0].contents.type,
                                     pmapi.PM_TYPE_U32)
    self.assertTrue(code >= 0)
    self.assertTrue(atom.ul > 0)
    print ("pmExtractValue:", atom.ul)
    ncpu = atom.ul

    # pmGetChildren
    gc = pm.pmGetChildren("kernel")
    print "pmGetChildren:", gc
    self.assertTrue(len(gc) >=2)

    # pmGetChildrenStatus
    gc = pm.pmGetChildrenStatus("kernel")
    print "pmGetChildrenStatus:", gc
    self.assertTrue(len(gc[0]) == len(gc[1]))

    # pmGetPMNSLocation
    i = pm.pmGetPMNSLocation()
    print "pmGetPMNSLocation:", i
    self.assertTrue(any((i==PMNS_ARCHIVE,i==PMNS_LOCAL,i==PMNS_REMOTE)))

    # pmTraversePMNS
    global traverse_callback_count
    traverse_callback_count = 0
    i = pm.pmTraversePMNS("kernel", traverse_callback)
    print "pmTraversePMNS:", traverse_callback_count
    self.assertTrue(traverse_callback_count > 0)

    # pmLookupName
    try:
        (code, badid) = pm.pmLookupName("A_BAD_METRIC")
        self.assertTrue(False)
    except  pmErr as e:
        print "pmLookupName:", e
        self.assertTrue(True)
    metrics = ("kernel.all.load", "kernel.percpu.cpu.user", "kernel.percpu.cpu.sys", "mem.freemem")
    (code, self.metric_ids) = pm.pmLookupName(metrics)
    dump_array("pmLookupName", self.metric_ids)
    self.assertTrue(len(self.metric_ids) == 4)

    for i in xrange(len(metrics)):
        # pmNameAll
        x = pm.pmNameAll(self.metric_ids[i])
        print "pmNameAll:", x[0]
        self.assertTrue(x[0] == metrics[i])

        # pmNameID
        y = pm.pmNameID(self.metric_ids[i])
        print "pmNameID:",y 
        self.assertTrue(y == metrics[i])

        # pmLookupDesc
        (code, descs) = pm.pmLookupDesc(self.metric_ids[i])
        dump_array_ptrs("pmLookupDesc", descs)
        self.assertTrue(len(descs) == 1)

    (code, descs) = pm.pmLookupDesc(self.metric_ids)
    if self.local_type:
        # pmGetInDom
        (inst, name) = pm.pmGetInDom(descs[1])
        print "pmGetInDom:", name
        self.assertTrue(all((len(inst) >= 2, len(name) >= 2)))
    else:
        # pmGetInDomArchive
        (inst, name) = pm.pmGetInDomArchive(descs[1])
        print "pmGetInDomArchive:", name
        self.assertTrue(all((len(inst) >= 2, len(name) >= 2)))

    # pmInDomStr
    x = pm.pmInDomStr(descs[0])
    print "pmInDomStr:", x
    self.assertTrue(x.count(".") >= 1)

    # pmDelProfile
    code = pm.pmDelProfile(descs[0], None);
    print "pmDelProfile:"
    self.assertTrue(code >= 0)

    if self.local_type:
        # pmLookupInDom
        inst1 = pm.pmLookupInDom(descs[0], "1 minute")
        print "pmLookupInDom:", inst1
    else:
        # pmLookupInDomArchive
        inst1 = pm.pmLookupInDomArchive(descs[0], "1 minute")
        print "pmLookupInDomArchive:", inst1
    self.assertTrue(inst1 >= 0)
        
    if self.local_type:
        # pmNameInDom
        instname = pm.pmNameInDom(descs[0], inst1)
        print "pmNameInDom:", instname
    else:
        # pmNameInDomArchive
        instname = pm.pmNameInDomArchive(descs[0], inst1)
        print "pmNameInDomArchive:", instname
    self.assertTrue(instname == "1 minute")
        
    text = 0
    try:
        # pmLookupInDomText
        text = pm.pmLookupInDomText(descs[0])
        self.assertTrue(False)
    except pmErr as e:
        print "pmLookupInDomText:", e
        self.assertTrue(True)
        
    # pmAddProfile
    code = pm.pmAddProfile(descs[0], inst1);
    self.assertTrue(code >= 0)

    inst = 0
    try:
        # pmLookupInDom
        inst = pm.pmLookupInDom(descs[0], "gg minute")
        self.assertTrue(False)
    except  pmErr as e:
        print "pmLookupInDom:", e
        self.assertTrue(True)
        
    if self.local_type:
        # pmLookupInDom
        inst5 = pm.pmLookupInDom(descs[0], "5 minute")
        inst15 = pm.pmLookupInDom(descs[0], "15 minute")
        print "pmLookupInDom:", inst5, inst15
    else:
        # pmLookupInDomArchive
        inst5 = pm.pmLookupInDomArchive(descs[0], "5 minute")
        inst15 = pm.pmLookupInDomArchive(descs[0], "15 minute")
        print "pmLookupInDomArchive:", inst5, inst15
    self.assertTrue(inst15 >= 0)
        
    # pmAddProfile
    code = pm.pmAddProfile(descs[0], inst15);
    print "pmAddProfile:"
    self.assertTrue(code >= 0)

    # pmParseInterval
    (code, delta, errmsg) = pm.pmParseInterval("3 seconds")
    print "pmParseInterval:", delta
    self.assertTrue(code >= 0)

    try:
        # pmLoadNameSpace
        # TODO Test a real use, pmUnLoadNameSpace
        inst = pm.pmLoadNameSpace("NoSuchFile")
        self.assertTrue(False)
    except  pmErr as e:
        print "pmLoadNameSpace:", e
        self.assertTrue(True)

    # pmFetch
    (code, results) = pm.pmFetch(self.metric_ids)
    print "pmFetch:", results
    self.assertTrue(code >= 0)

    # pmSortInstances
    code = pm.pmSortInstances(results)
    print "pmSortInstances:", code
    self.assertTrue(code == 4)

    # pmStore
    try:
        code = pm.pmStore(results)
        self.assertTrue(False)
    except pmErr as e:
        print "pmStore: ", e
        self.assertTrue(True)

    for cpu in xrange(ncpu):
        for i in xrange(results.contents.numpmid):
            #  kernel.percpu.cpu.user ?
            if (results.contents.get_pmid(i) != self.metric_ids[1]):
                continue
            (code, atom) = pm.pmExtractValue(results.contents.get_valfmt(i),
                                             results.contents.get_vlist(i, cpu),
                                             descs[i].contents.type, pmapi.PM_TYPE_FLOAT)
            print "pmExtractValue", cpu, atom.f
            self.assertTrue(atom.f > 0)

    # pmExtractValue 
    for i in xrange(results.contents.numpmid):
        # mem.freemem ?
        if (results.contents.get_pmid(i) != self.metric_ids[3]):
            continue
        (code, tmpatom) = pm.pmExtractValue(results.contents.get_valfmt(i),
                                            results.contents.get_vlist(i, 0),
                                            descs[i].contents.type, pmapi.PM_TYPE_FLOAT)
        self.assertTrue(tmpatom.f > 0)

    # pmConvScale
    (code, atom) = pm.pmConvScale(pmapi.PM_TYPE_FLOAT, tmpatom, descs, 3, pmapi.PM_SPACE_MBYTE)
    print "pmConvScale", tmpatom.f, atom.f
    self.assertTrue(atom.f > 0)

    # pmAtomStr
    x = pm.pmAtomStr (atom, pmapi.PM_TYPE_FLOAT)
    print "pmAtomStr", x

    # pmFreeResult
    pm.pmFreeResult(results)
    print "pmFreeResult"

    code = pm.pmtimevalSleep(delta);
    print "pmtimevalSleep"

    # pmDupContext
    context = pm.pmDupContext()
    print "pmDupContext", context
    self.assertTrue(context >= 0)


    # pmWhichContext
    code = pm.pmWhichContext ()
    print "pmWhichContext", code
    self.assertTrue(code >= 0)

    # pmTypeStr
    x = pm.pmTypeStr (pmapi.PM_TYPE_FLOAT)
    print "pmTypeStr", x
    self.assertTrue (x  == "FLOAT")

    if self.archive_type:
        # pmSetMode
        code = pm.pmSetMode (pmapi.PM_MODE_INTERP, results.contents.timestamp, 0)
        print "pmSetMode", code
        self.assertTrue(code >= 0)

        # pmGetArchiveLabel
        (code, loglabel) = pm.pmGetArchiveLabel()
        print "pmGetArchiveLabel", code
        self.assertTrue (all((code >= 0, loglabel.pid_t > 0)))

    # pmPrintValue XXX
    print "pmPrintValue: "
    pm.pmPrintValue(sys.__stdout__, results, descs[0], 0, 0, 8)
    print

    # pmReconnectContext
    code = pm.pmReconnectContext ()
    print "pmReconnectContext", code
    self.assertTrue(code >= 0)

    del pm

class TestSequenceFunctions(unittest.TestCase):

    ncpu_id = []
    metric_ids = []
    archive_type = False
    local_type = False

    def test_context(self):
        test_pcp(self)


if __name__ == '__main__':

    HAVE_BITFIELDS_LTOR = False
    if (len(sys.argv) == 2):
        open(sys.argv[1] + '.index', mode='r')
        archive = sys.argv[1]
    elif (len(sys.argv) > 2):
        print "Usage: " + sys.argv[0] + " OptionalArchivePath"
        sys.exit()
    else:
        archive = ""
        
    sys.argv[1:] = ()

    unittest.main()
    sys.exit(main())

""" Not tested
pmNewContextZone
pmNewZone
pmUseZone
pmWhichZone
pmGetConfig
pmFetchArchive
pmGetArchiveEnd
pmSetMode
"""