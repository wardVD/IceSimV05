class ValidationSection(list):

    """
    Section of a validation report.

    A section is a container for test results with a header.
    It can contain test results, messages, and other sections.
    """

    def __init__(self, parent, header="", verbosity=2):
        list.__init__(self)
        self.parent = parent
        self.header = header
        self.flags = set()
        self.verbosity = verbosity

    def counts(self):
        nFail = 0
        nSucc = 0
        for x in self:
            if isinstance(x, str):
                if x[0] == "F":
                    nFail += 1
                if x[0] == "S":
                    nSucc += 1
            else:
                nF, nS = x.counts()
                nFail += nF
                nSucc += nS
        return nFail, nSucc

    def __str__(self):
        from math import log10
        nFail, nSucc = self.counts()
        nTot = nFail + nSucc
        iSucc = int(log10(nSucc)) + 1 if nSucc > 0 else 1
        iTot = int(log10(nTot)) + 1 if nTot > 0 else 1

        def ColorText(msg, col):
            cmap = {
                "r": '\033[91m',
                "y": '\033[93m',
                "g": '\033[92m',
                "b": '\033[94m',
            }
            endcol = '\033[0m'
            return cmap[col] + msg + endcol

        prefix = ""
        lines = []

        def append(lines, iSucc, iTot, prefix, x):
            def wrap(iSucc, iTot, nSucc, nTot, prefix, msg):
                tag = " " * (4 + iSucc + iTot)
                spaces = tag[:]
                if nTot > 0:
                    form = " %%%ii/%%-%ii " % (iSucc, iTot)
                    if nSucc == nTot:
                        tag = ColorText("S", "g")
                    else:
                        tag = ColorText("F", "y" if nSucc > 0 else "r")
                    tag += form % (nSucc, nTot)

                tag += prefix
                spaces += prefix

                if "\n" in msg:  # is multi-line
                    parts = msg.split("\n")
                    lines = [ tag + parts[0] ] + \
                            [ spaces + x for x in parts[1:] ]
                    return "\n ".join(lines)

                charLimit = 100 - len(spaces)
                if len(msg) < charLimit:
                    return tag + msg

                words = msg.split()
                lines = []
                cur = []
                cursor = 0
                for iword, word in enumerate(words):
                    if cursor > charLimit:
                        if len(lines) == 0:
                            lines.append(tag + " ".join(cur))
                        else:
                            lines.append(spaces + " ".join(cur))
                        cursor = 0
                        cur = []
                    cur.append(word)
                    cursor += len(word) + 1
                if cur:
                    lines.append(spaces + " ".join(cur))
                return "\n ".join(lines)

            if isinstance(x, str):
                if len(x) > 1:
                    if x[0] == "W":
                        x = "W" + ColorText(x[1:], "y")
                    if self.verbosity >= 2 or \
                       x[0] == "F" and self.verbosity >= 0 or \
                       x[0] == "W" and self.verbosity >= 1:
                        lines.append(wrap(iSucc, iTot, 0, 0, prefix, x[1:]))
            else:
                nFail, nSucc = x.counts()
                nTot = nFail + nSucc

                if nFail > 0 or self.verbosity >= 2:
                    if isinstance(x, ValidationSection):
                        lines.append(wrap(iSucc, iTot, nSucc, nTot, prefix,
                                          x.header + " " + " ".join(x.flags)))
                        if self.verbosity >= 1:
                            for p in x:
                                append(lines, iSucc, iTot, prefix + "  ", p)

        append(lines, iSucc, iTot, prefix, self)

        return "\n".join(lines)

    def __bool__(self):
        nFail, nSucc = self.root.counts()
        return nFail == 0


class ValidationReport(ValidationSection):

    """
    A structured validation report.

    The report is convertable to str and bool. Conversion to str
    generates the text report. Conversion to bool returns True
    if all comparisons were successful and False otherwise.
    """

    def __init__(self, title, verbosity=2):
        ValidationSection.__init__(self, None,
                                   "[ValidationReport] " + title,
                                   verbosity)
        self.cur = self  # current node
        self.fail = False

    def __nonzero__(self):
        return bool(self.fail)

    def info(self, msg):
        self.cur.append("I" + msg)

    def warn(self, msg):
        self.cur.append("W" + msg)

    def success(self, msg=""):
        self.cur.append("S" + msg)

    def failure(self, msg=""):
        self.cur.append("F" + msg)
        self.fail = True

    def result(self, isOk, failureMsg="", successMsg=""):
        if isOk:
            self.success(successMsg)
        else:
            self.failure(failureMsg)

    def flag(self, msg):
        self.cur.flags.add(msg)

    def exception(self):
        import sys
        (exc_type, exc_value, traceback) = sys.exc_info()
        self.failure()
        msg = "<%s> %s" % (exc_type.__name__, str(exc_value))
        self.warn(msg)

    def section(self, header):
        node = ValidationSection(self.cur, header)
        self.cur.append(node)
        self.cur = node
        return self

    def __enter__(self):
        pass

    def __exit__(self, *args):
        self.cur = self.cur.parent


def CmpI3File(rep, fileNameCurrent, fileNameReference,
              restrictFrames=None, restrictKeys=None, ignoreKeys=None,
              strictOnLayout=False, keyBasedCmp=None):
    """
    Compare contents of two I3Files.

    Files are compared frame by frame. The set of keys to compare can be
    restricted (restrictKeys) or selected frame keys can be ignored (ignoreKeys).
    If strictOnLayout is False, curFrame may contain more keys than refFrame;
    otherwise the key sets have to be identical. Key-based comparison functions
    can be passed (keyBasedCmp) and will overwrite type-based comparison.
    """

    from icecube.dataio import I3File

    with rep.section("[File] %s <-> %s" % (fileNameCurrent, fileNameReference)):
        fileCur = I3File(fileNameCurrent)
        fileRef = I3File(fileNameReference)

        iFrame = -1
        while True:
            iFrame += 1
            frameCur = fileCur.pop_frame()
            frameRef = fileRef.pop_frame()

            if frameRef is None:
                break

            if restrictFrames and iFrame not in restrictFrames:
                continue

            with rep.section("[Frame] %i" % iFrame):
                if frameCur is None:
                    rep.failure("Frame missing LEFT")
                else:
                    CmpI3Frame(rep, frameCur, frameRef,
                               restrictKeys, ignoreKeys,
                               strictOnLayout, keyBasedCmp)


def CmpI3Frame(rep, curFrame, refFrame,
               restrictKeys=None, ignoreKeys=None,
               strictOnLayout=False,
               keyBasedCmp=None):
    """
    Compare contents of two I3Frames.

    Frames are compared key by key. The set of keys to compare can be
    restricted (restrictKeys) or selected frame keys can be ignored (ignoreKeys).
    If strictOnLayout is False, curFrame may contain more keys than refFrame;
    otherwise the key sets have to be identical. Key-based comparison functions
    can be passed (keyBasedCmp) and will overwrite type-based comparison.
    """

    keysToCompare = set()
    if restrictKeys:
        if isinstance(restrictKeys, str):
            restrictKeys = [restrictKeys]
        keysToCompare = set(restrictKeys)
    else:
        # get keys from reference
        keysToCompare.update(refFrame.keys())
    if ignoreKeys:
        if isinstance(ignoreKeys, str):
            ignoreKeys = [ignoreKeys]
        keysToCompare -= set(ignoreKeys)

    if strictOnLayout and not restrictKeys:
        # strictOnLayout: keys have to be the same in ref and cur
        curKeys = sorted(curFrame.keys())
        refKeys = sorted(refFrame.keys())
        if curKeys != refKeys:
            rep.failure("Keys differ LEFT %s RIGHT %s" %
                         (str(curKeys), str(refKeys)))

    for key in sorted(keysToCompare):
        with rep.section(key):
            if not key in refFrame:
                rep.failure("missing in RIGHT")
                continue
            if not key in curFrame:
                rep.failure("missing in LEFT")
                continue

            cur = curFrame[key]
            ref = refFrame[key]
            try:
                if keyBasedCmp and key in keyBasedCmp:
                    keyBasedCmp[key](rep, cur, ref)
                    continue

                Cmp(rep, cur, ref)
                continue
            except:
                rep.exception()


def CmpMap(rep, cur, ref):
    """Compare maps by size and element-wise."""
    if len(ref.keys()) != len(cur.keys()):
        rep.failure("lengths differ (%i, %i)" %
                    (len(ref.keys()), len(cur.keys())))
    for key in ref.keys():
        if key in cur:
            Cmp(rep, cur[key], ref[key])
        else:
            rep.failure(str(key) + " missing")
    else:  # if map is empty...
        rep.success()

def CmpSeries(rep, cur, ref):
    """Compare series by size and element-wise."""
    nref = len(ref)
    ncur = len(cur)
    if nref != ncur:
        rep.failure("lengths differ (%i, %i)" % (nref, ncur))
    for i in xrange(nref):
        if i < ncur:
            Cmp(rep, cur[i], ref[i])
        else:
            rep.failure("index %i missing" % i)
    else:  # if vector is empty...
        rep.success()


def CmpEqual(rep, cur, ref):
    """Compare equality. NaN == NaN yields true."""
    from math import isnan
    isEqual = cur == ref
    if isinstance(ref, float):
        isEqual |= isnan(ref) and isnan(cur)
    if isEqual:
        rep.success()
    else:
        msg = ""
        if hasattr(ref, "__str__"):
            msg = "%s LEFT %s RIGHT %s" % (
                type(ref).__name__, str(cur), str(ref))
        rep.failure(msg)


def CmpFields(rep, cur, ref, *fields):
    """Compare two objects based on their field values, using equality."""
    for field in fields:
        CmpEqual(rep, getattr(cur, field), getattr(ref, field))


def CmpI3Particle(rep, cur, ref):
    """Compare fields of I3Particle, excluding random ParticleID."""
    CmpFields(rep, cur, ref,
              "dir", "energy", "fit_status", "length",
              "location_type", "pdg_encoding", "pos", "shape",
              "speed", "time", "type")


def CmpI3EventHeader(rep, cur, ref):
    """Compare fields of I3EventHeader, excluding random SubEventID."""
    CmpFields(rep, cur, ref,
              "data_stream", "end_time", "event_id", "run_id",
              "start_time", "sub_event_stream", "sub_run_id")


def CmpIteritems(rep, cur, ref):
    """Compare trees by size and element-wise."""
    if len(ref) != len(cur):
        rep.failure("lengths differ (%i, %i)" % (len(ref), len(cur)))
    refIter = ref.iteritems()
    curIter = cur.iteritems()
    while True:
        try:
            refItem = refIter.next()
            curItem = curIter.next()
        except StopIteration:
            break
        rep.result(curItem[0] == refItem[0])
        Cmp(rep, curItem[1], refItem[1])

def CmpI3Tree(rep, cur, ref):
    """Compare trees by size and element-wise."""
    if len(ref) != len(cur):
        rep.failure("lengths differ (%i, %i)" % (len(ref), len(cur)))
        return
    refIter = ref.post_order_iter()
    curIter = cur.post_order_iter()
    while True:
        try:
            refItem = refIter.next()
            curItem = curIter.next()
        except StopIteration:
            break
        Cmp(rep, curItem, refItem)


class GeneralComparison(object):

    """
    General comparison class for validation.

    Comparison functions can be registered (see register) based on the identity of the 
    class object. Generic comparison based on duck typing and some heuristics is the
    default (see __call__).
    """

    def __init__(self):
        from icecube.dataclasses import I3Particle, I3EventHeader
        from icecube.icetray import I3Frame
        self._cmp = {}
        self.register(I3Particle, CmpI3Particle)
        self.register(I3EventHeader, CmpI3EventHeader)
        self.register(I3Frame, CmpI3Frame)

    def register(self, classObject, compareFunction):
        """
        Register a comparison function based on object type.
        """
        self._cmp[classObject] = compareFunction

    def __call__(self, rep, cur, ref):
        """General comparison: Handles common I3FrameObjects.

        This call first tries to do the comparison with a registered comparison
        function. If that fails, general comparison based on duck typing and
        heuristics is tried. It uses == operator, string representation, or 
        element-wise comparison for standard containers.
        """

        if cur.__class__ != ref.__class__:
            rep.failure("types differ LEFT <%s> RIGHT <%s>" %
                        (cur.__class__, ref.__class__))

        # first try registed type-based comparison
        if ref.__class__ in self._cmp:
            self._cmp[ref.__class__](rep, cur, ref)
            return

        # mark in current section that general comparison is used
        rep.flag("(G)")

        # has value member? try equality operator of value
        if hasattr(ref, "value"):
            CmpEqual(rep, cur.value, ref.value)
            return

        # does it have iteritems? compare item by item
        if hasattr(ref, "iteritems"):
            CmpIteritems(rep, cur, ref)
            return

        # looks like (I3)Map? compare key by key
        if hasattr(ref, "items") and hasattr(ref, "keys") and hasattr(ref, "values"):
            CmpMap(rep, cur, ref)
            return

        # looks like list? compare index by index
        if hasattr(ref, "__getitem__") and hasattr(ref, "__len__"):
            CmpSeries(rep, cur, ref)
            return

        # has equality operator? use that
        if hasattr(ref, "__eq__") or hasattr(ref, "__cmp__"):
            CmpEqual(rep, cur, ref)
            return

        # has str member? try comparing string representation
        if hasattr(ref, "__str__"):
            # check that __str__ is not just an alias for __repr__
            if ref.__str__ is not ref.__repr__:
                CmpEqual(rep, str(cur), str(ref))
                return

        rep.failure()
        rep.warn("Please register custom comparison for <%s>" % ref.__class__)


Cmp = GeneralComparison()  # use this for general comparison
