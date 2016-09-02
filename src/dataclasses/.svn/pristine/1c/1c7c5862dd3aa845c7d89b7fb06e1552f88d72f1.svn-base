#include <I3Test.h>
#include <dataclasses/physics/I3Trigger.h>
#include <dataclasses/status/I3DetectorStatus.h>
#include <dataclasses/status/I3TriggerStatus.h>
#include <dataclasses/payload/I3SuperDSTTrigger.h>



TEST_GROUP(I3SuperDSTTriggerTest);

TEST(i3objects_constructor)
{
  I3Trigger trigger = I3Trigger();
  I3DetectorStatus detectorstatus = I3DetectorStatus();
  detectorstatus.triggerStatus[trigger.GetTriggerKey()] = I3TriggerStatus();
  I3SuperDSTTrigger(trigger,detectorstatus);
}

TEST(i3objects_constructor_bad)
{
  I3Trigger trigger = I3Trigger();
  I3DetectorStatus detectorstatus = I3DetectorStatus();
  int caught = 0;
  try
  {
    I3SuperDSTTrigger(trigger,detectorstatus);
  }
  catch (const std::range_error& e)
  {
    caught = 1;
  }
  if (caught != 1)
    throw std::runtime_error("did not raise error like expected");
}

TEST(idx_constructor)
{
  uint32_t key_idx = 0;
  uint32_t startcode = 0;
  uint32_t lengthcode = 0;

  I3SuperDSTTrigger superdsttrigger(key_idx,startcode,lengthcode);
}
