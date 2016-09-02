#include "interfaces/I3IcePickModule.h"
#include "interfaces/I3IceForkModule.h"
#include "icetray/I3IcePickInstaller.h"

#include "icepick/I3RunIdEventIdFilter.h"
I3_MODULE(I3IcePickModule<I3RunIdEventIdFilter>);
I3_MODULE(I3IceForkModule<I3RunIdEventIdFilter>);
I3_SERVICE_FACTORY(I3IcePickInstaller<I3RunIdEventIdFilter>);

#include "icepick/I3EventIdFilter.h"
I3_MODULE(I3IcePickModule<I3EventIdFilter>);
I3_MODULE(I3IceForkModule<I3EventIdFilter>);
I3_SERVICE_FACTORY(I3IcePickInstaller<I3EventIdFilter>);

#include "icepick/I3EventTimeFilter.h"
I3_MODULE(I3IcePickModule<I3EventTimeFilter>);
I3_MODULE(I3IceForkModule<I3EventTimeFilter>);
I3_SERVICE_FACTORY(I3IcePickInstaller<I3EventTimeFilter>);

#include "icepick/I3EventIdRangeFilter.h"
I3_MODULE(I3IcePickModule<I3EventIdRangeFilter>);
I3_MODULE(I3IceForkModule<I3EventIdRangeFilter>);
I3_SERVICE_FACTORY(I3IcePickInstaller<I3EventIdRangeFilter>);

#include "icepick/I3NHitChannelFilter.h"
I3_MODULE(I3IcePickModule<I3NHitChannelFilter>);
I3_MODULE(I3IceForkModule<I3NHitChannelFilter>);
I3_SERVICE_FACTORY(I3IcePickInstaller<I3NHitChannelFilter>);

#include "icepick/I3NLaunchFilter.h"
I3_MODULE(I3IcePickModule<I3NLaunchFilter>);
I3_MODULE(I3IceForkModule<I3NLaunchFilter>);
I3_SERVICE_FACTORY(I3IcePickInstaller<I3NLaunchFilter>);

#include "icepick/I3UnFilter.h"
I3_MODULE(I3IcePickModule<I3UnFilter>);
I3_MODULE(I3IceForkModule<I3UnFilter>);
I3_SERVICE_FACTORY(I3IcePickInstaller<I3UnFilter>);

#include "icepick/I3TriggerSatisfiedFilter.h"
I3_MODULE(I3IcePickModule<I3TriggerSatisfiedFilter>);
I3_MODULE(I3IceForkModule<I3TriggerSatisfiedFilter>);
I3_SERVICE_FACTORY(I3IcePickInstaller<I3TriggerSatisfiedFilter>);

#include "icepick/I3PickRawNHitEventFilter.h"
I3_MODULE(I3IcePickModule<I3PickRawNHitEventFilter>);
I3_MODULE(I3IceForkModule<I3PickRawNHitEventFilter>);
I3_SERVICE_FACTORY(I3IcePickInstaller<I3PickRawNHitEventFilter>);

#include "icepick/I3SkipNEventFilter.h"
I3_MODULE(I3IcePickModule<I3SkipNEventFilter>);
I3_MODULE(I3IceForkModule<I3SkipNEventFilter>);
I3_SERVICE_FACTORY(I3IcePickInstaller<I3SkipNEventFilter>);

#include "icepick/I3FitSucceededFilter.h"
I3_MODULE(I3IcePickModule<I3FitSucceededFilter>);
I3_MODULE(I3IceForkModule<I3FitSucceededFilter>);
I3_SERVICE_FACTORY(I3IcePickInstaller<I3FitSucceededFilter>);

#include "icepick/I3AzimuthWindowFilter.h"
I3_MODULE(I3IcePickModule<I3AzimuthWindowFilter>);
I3_MODULE(I3IceForkModule<I3AzimuthWindowFilter>);
I3_SERVICE_FACTORY(I3IcePickInstaller<I3AzimuthWindowFilter>);

#include "icepick/I3LDirFilter.h"
I3_MODULE(I3IcePickModule<I3LDirFilter>);
I3_MODULE(I3IceForkModule<I3LDirFilter>);
I3_SERVICE_FACTORY(I3IcePickInstaller<I3LDirFilter>);

#include "icepick/I3NDirFilter.h"
I3_MODULE(I3IcePickModule<I3NDirFilter>);
I3_MODULE(I3IceForkModule<I3NDirFilter>);
I3_SERVICE_FACTORY(I3IcePickInstaller<I3NDirFilter>);

#include "icepick/I3LogicalOrFilter.h"
I3_MODULE(I3IcePickModule<I3LogicalOrFilter>);
I3_MODULE(I3IceForkModule<I3LogicalOrFilter>);
I3_SERVICE_FACTORY(I3IcePickInstaller<I3LogicalOrFilter>);

#include "icepick/I3InverseFilter.h"
I3_MODULE(I3IcePickModule<I3InverseFilter>);
I3_MODULE(I3IceForkModule<I3InverseFilter>);
I3_SERVICE_FACTORY(I3IcePickInstaller<I3InverseFilter>);

#include "icepick/I3OrFilter.h"
I3_MODULE(I3IcePickModule<I3OrFilter>);
I3_MODULE(I3IceForkModule<I3OrFilter>);
I3_SERVICE_FACTORY(I3IcePickInstaller<I3OrFilter>);

#include "icepick/I3AndFilter.h"
I3_MODULE(I3IcePickModule<I3AndFilter>);
I3_MODULE(I3IceForkModule<I3AndFilter>);
I3_SERVICE_FACTORY(I3IcePickInstaller<I3AndFilter>);

#include "icepick/I3PrescaleFilter.h"
I3_MODULE(I3IcePickModule<I3PrescaleFilter>);
I3_MODULE(I3IceForkModule<I3PrescaleFilter>);
I3_SERVICE_FACTORY(I3IcePickInstaller<I3PrescaleFilter>);

#include "icepick/I3SimpleFilter.h"
I3_MODULE(I3IcePickModule<I3SimpleFilter>);
I3_MODULE(I3IceForkModule<I3SimpleFilter>);
I3_SERVICE_FACTORY(I3IcePickInstaller<I3SimpleFilter>);

#include "icepick/I3DoubleFilter.h"
I3_MODULE(I3IcePickModule<I3DoubleFilter>);
I3_MODULE(I3IceForkModule<I3DoubleFilter>);
I3_SERVICE_FACTORY(I3IcePickInstaller<I3DoubleFilter>);

#include "icepick/I3SoftwareTriggerFlagsFilter.h"
I3_MODULE(I3IcePickModule<I3SoftwareTriggerFlagsFilter>);
I3_MODULE(I3IceForkModule<I3SoftwareTriggerFlagsFilter>);
I3_SERVICE_FACTORY(I3IcePickInstaller<I3SoftwareTriggerFlagsFilter>);


